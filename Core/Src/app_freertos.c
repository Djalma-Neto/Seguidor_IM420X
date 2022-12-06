/* USER CODE BEGIN Header */
/* ***********************************************************************************
*File name: seguidorLinha.c
*
*File description:  Projeto da disciplina IM420X com foco na utilização de Sistema Operacional em Tempo Real (RTOS)
* 					para gerenciamento de tarefas. O presente trabalho teve como finalidade desenvolver um veículo
* 					seguidor de linhas capaz de,atravez de sensores, evitar colisões.
* Author name:
*				Ramiro Romankevicius Costa.
*				Djalma Santana Malta Neto.
*
* Creation date: 10/11/2022.
*
* Revision date: 05/12/2022.
********************************************************************************** */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "usart.h"
#include "tim.h"
#include "stdio.h"
#include "stdlib.h"
#include <string.h>
#include "math.h"
#include "pid.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/*variáveis utilizadas pelo sensor ultrassônico*/
long lValor1;
long lValor2;
float fDiferenca, fDistancia;
uint8_t uiIsFirst = 1;

/*variáveis utilizadas pelos motores*/
float fVelocidade = 0.2; // M/s
float fReducao = 0.5;
float fWAngularD, fWAngularE,fPIDVal_D, fPIDVal_E, fVE,fVD;

/*variáveis utilizadas pela odometria*/
#define FUROS 20
#define RAIO 0.0325
#define COMPRIMENTO 0.12
#define LARGURA 0.028
#define PI 3.141516
#define fTs 0.1

float fTeta, fX, fY, fDistanciaO;

/*encoder_Esquerdo*/
unsigned long ulPulsesE;
float fWE;

/*encoder_Direito*/
unsigned long ulPulsesD;
float fWD;

/*variáveis utilizadas pelo seguidor*/
uint32_t uiStart = 0;
uint32_t uiBloqueado = 0;
uint32_t uiCountSeguidor = 0;

/*variáveis utilizadas para comunicação*/
char cMostrar[100];
char cData='M';

/* USER CODE END Variables */
/* Definitions for Utrassom */
osThreadId_t UtrassomHandle;
const osThreadAttr_t Utrassom_attributes = {
  .name = "Utrassom",
  .priority = (osPriority_t) osPriorityNormal2,
  .stack_size = 256 * 4
};
/* Definitions for Comunica */
osThreadId_t ComunicaHandle;
const osThreadAttr_t Comunica_attributes = {
  .name = "Comunica",
  .priority = (osPriority_t) osPriorityLow,
  .stack_size = 128 * 4
};
/* Definitions for Seguidor */
osThreadId_t SeguidorHandle;
const osThreadAttr_t Seguidor_attributes = {
  .name = "Seguidor",
  .priority = (osPriority_t) osPriorityBelowNormal1,
  .stack_size = 128 * 4
};
/* Definitions for Motores */
osThreadId_t MotoresHandle;
const osThreadAttr_t Motores_attributes = {
  .name = "Motores",
  .priority = (osPriority_t) osPriorityBelowNormal,
  .stack_size = 128 * 4
};
/* Definitions for Odometria */
osThreadId_t OdometriaHandle;
const osThreadAttr_t Odometria_attributes = {
  .name = "Odometria",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 128 * 4
};
/* Definitions for SemaphoreMovimenta */
osSemaphoreId_t SemaphoreMovimentaHandle;
const osSemaphoreAttr_t SemaphoreMovimenta_attributes = {
  .name = "SemaphoreMovimenta"
};
/* Definitions for SemaphoreComunica */
osSemaphoreId_t SemaphoreComunicaHandle;
const osSemaphoreAttr_t SemaphoreComunica_attributes = {
  .name = "SemaphoreComunica"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void FunctionUltrassom(void *argument);
void FunctionComunica(void *argument);
void FunctionSeguidor(void *argument);
void FunctionAtivarMotores(void *argument);
void FunctionOdometria(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* creation of SemaphoreMovimenta */
  SemaphoreMovimentaHandle = osSemaphoreNew(1, 1, &SemaphoreMovimenta_attributes);

  /* creation of SemaphoreComunica */
  SemaphoreComunicaHandle = osSemaphoreNew(1, 1, &SemaphoreComunica_attributes);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of Utrassom */
  UtrassomHandle = osThreadNew(FunctionUltrassom, NULL, &Utrassom_attributes);

  /* creation of Comunica */
  ComunicaHandle = osThreadNew(FunctionComunica, NULL, &Comunica_attributes);

  /* creation of Seguidor */
  SeguidorHandle = osThreadNew(FunctionSeguidor, NULL, &Seguidor_attributes);

  /* creation of Motores */
  MotoresHandle = osThreadNew(FunctionAtivarMotores, NULL, &Motores_attributes);

  /* creation of Odometria */
  OdometriaHandle = osThreadNew(FunctionOdometria, NULL, &Odometria_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_FunctionUltrassom */
/**
  * @brief  Function implementing the Utrassom thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_FunctionUltrassom */
void FunctionUltrassom(void *argument)
{
  /* USER CODE BEGIN FunctionUltrassom */
	HAL_TIM_IC_Start_IT(&htim1, TIM_CHANNEL_3);
  /* Infinite loop */
	for(;;)
	{
		__HAL_TIM_ENABLE_IT(&htim1, TIM_CHANNEL_3);
		HAL_GPIO_WritePin(Trig_GPIO_Port, Trig_Pin, 1);
		osDelay(10);
		HAL_GPIO_WritePin(Trig_GPIO_Port, Trig_Pin, 0);
		osDelay(500);
	}
  /* USER CODE END FunctionUltrassom */
}

/* USER CODE BEGIN Header_FunctionComunica */
/**
* @brief Function implementing the Comunica thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_FunctionComunica */
void FunctionComunica(void *argument)
{
  /* USER CODE BEGIN FunctionComunica */
  /* Infinite loop */
  for(;;)
  {
	  osSemaphoreAcquire(SemaphoreComunicaHandle, 200);

	  /*
	   * Captura de valor vindo do terminal remoto bluetooth podem=ndo ser "M", "O" ou "V"
	   * Sendo "M" RPS de cada roda, "O" dados da odometria e "V" deslocamento em m/s de cada roda
	   */
	  HAL_UART_Receive(&huart1, (uint8_t *)&cData, sizeof(cData),100);
	  if(uiBloqueado){
		  int dist1 = (int)fDistancia;
		  int dist2 = (fDistancia-(int)fDistancia)*100;

		  sprintf(cMostrar,"Blockeado: %d.%02d \r \n ",dist1,dist2);
	  } else if(!uiStart){
		  sprintf(cMostrar,"Aguardando Start!! \r \n ");
	  } else{
		  if(cData == 'O' || cData == 'o'){
			  int valor1 = (int)fDistanciaO;
			  int valor2 = (fDistanciaO-(int)fDistanciaO)*100;

			  sprintf(cMostrar,"Distancia Percorrida: %d.%02dm \r \n ",valor1,valor2);
		  }else if(cData == 'V' || cData == 'v'){
			  int valor1 = (int)fWE;
			  int valor2 = (fWE-(int)fWE)*100;

			  int valor3 = (int)fWD;
			  int valor4 = (fWD-(int)fWD)*100;

			  sprintf(cMostrar,"MotorE: %d.%02d m/s -- MotorD: %d.%02d m/s \r \n ",valor1,valor2,valor3,valor4);
		  }else{
			  cData = 'M';
			  float valE = ((float)ulPulsesE/FUROS)*10;
			  float valD = ((float)ulPulsesD/FUROS)*10;

			  int valor1 = (int)valE;
			  int valor2 = (valE-(int)valE)*100;

			  int valor3 = (int)valD;
			  int valor4 = (valD-(int)valD)*100;

			  sprintf(cMostrar,"RPS_E: %d.%02d -- RPS_D: %d.%02d \r \n ",valor1,valor2,valor3,valor4);
		  }
	  }

	  /*transmite a informação via UART e via BlueTooth*/
	  HAL_UART_Transmit(&huart1, (uint8_t*)cMostrar, sizeof(cMostrar), 100);
	  HAL_UART_Transmit(&hlpuart1, (uint8_t*)cMostrar, sizeof(cMostrar), 100);

	  osSemaphoreRelease(SemaphoreComunicaHandle);
	  osDelay(100);
  }
  /* USER CODE END FunctionComunica */
}

/* USER CODE BEGIN Header_FunctionSeguidor */
/**
* @brief Function implementing the Seguidor thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_FunctionSeguidor */
void FunctionSeguidor(void *argument)
{
  /* USER CODE BEGIN FunctionSeguidor */
  /* Infinite loop */
  for(;;)
  {
	  /*Faz a leitura dos dados dos sensores*/
    uint8_t uiS2 = HAL_GPIO_ReadPin(S2_GPIO_Port, S2_Pin);
    uint8_t uiS3 = HAL_GPIO_ReadPin(S3_GPIO_Port, S3_Pin);
    uint8_t uiS4 = HAL_GPIO_ReadPin(S4_GPIO_Port, S4_Pin);

    //uint8_t uiNEAR = HAL_GPIO_ReadPin(NEAR_GPIO_Port, NEAR_Pin);
    //uint8_t uiCLP = HAL_GPIO_ReadPin(CLP_GPIO_Port, CLP_Pin);
    uint8_t uiBTN = HAL_GPIO_ReadPin(BTN1_GPIO_Port, BTN1_Pin);

    osSemaphoreAcquire(SemaphoreMovimentaHandle, 200);


    if(uiBloqueado){/*caso blockeado...*/
    	fWAngularD = 0;
		fWAngularE = 0;
		HAL_GPIO_WritePin(Buzzer_GPIO_Port, Buzzer_Pin, GPIO_PIN_SET);
		osDelay(500);
		HAL_GPIO_WritePin(Buzzer_GPIO_Port, Buzzer_Pin, GPIO_PIN_RESET);
    }else if(uiBTN){/*caso BTN start pressionado...*/
    	uiStart = uiStart?0:1;
    	uiCountSeguidor = 0;
    	fWAngularD = 0;
    	fWAngularE = 0;
    	HAL_GPIO_WritePin(Buzzer_GPIO_Port, Buzzer_Pin, GPIO_PIN_SET);
    	osDelay(500);
    	HAL_GPIO_WritePin(Buzzer_GPIO_Port, Buzzer_Pin, GPIO_PIN_RESET);
    	osDelay(500);
    }else if(uiStart && uiCountSeguidor < 50){/*caso start pressionado e o veículo esteja na linha...*/
    	if(uiS2 && uiS3 && !uiS4){/*caso linha no sensor direito...*/
    		uiCountSeguidor = 0;
			fWAngularE = fVelocidade;
			fWAngularD = 0;
		}else if(!uiS2 && uiS3 && uiS4){/*caso linha no sensor esquerdo...*/
			uiCountSeguidor = 0;
			fWAngularE = 0;
			fWAngularD = fVelocidade;
		}else if(uiS2 && !uiS3 && uiS4){/*caso linha no sensor do meio...*/
			uiCountSeguidor = 0;
			fWAngularD = fVelocidade;
			fWAngularE = fVelocidade;
		}else if(!uiS2 && !uiS3 && !uiS4){/*caso linha nos tres sensores(Fim de curso)...*/
			uiCountSeguidor = 0;
			fWAngularD = 0;
			fWAngularE = 0;
			uiStart = 0;
			HAL_GPIO_WritePin(Buzzer_GPIO_Port, Buzzer_Pin, GPIO_PIN_SET);
			osDelay(100);
			HAL_GPIO_WritePin(Buzzer_GPIO_Port, Buzzer_Pin, GPIO_PIN_RESET);
			osDelay(100);
		}
    	/*
    	 * Caso o veículo saia da linha ele permanece fazendo a ultima manobra de correção, enquanto não encontrar  a linha
    	 * o contador "uiCountSeguidor" irá incrementar e quando exceder o valor 50 o veículo ira para o modo blockeado
    	 */
    	if(uiS2 && uiS3 && uiS4){/*caso sensores fora da linha(fora da trajetória)...*/
    		uiCountSeguidor++;
		}
    }else{
    	if(uiCountSeguidor >= 50){
    		//uiBloqueado = 1;
    	}
    	fWAngularD = 0;
		fWAngularE = 0;
    }
    osDelay(100);
    osSemaphoreRelease(SemaphoreMovimentaHandle);
  }
  /* USER CODE END FunctionSeguidor */
}

/* USER CODE BEGIN Header_FunctionAtivarMotores */
/**
* @brief Function implementing the Motores thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_FunctionAtivarMotores */
void FunctionAtivarMotores(void *argument)
{
  /* USER CODE BEGIN FunctionAtivarMotores */

	/*Iniciando o PWM*/
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);

	/*Iniciando o PID com os devidos ganhos*/
	sPID_D pid_D;
	sPID_E pid_E;

	pid_E.fKpE = 15;
	pid_E.fKiE = 0.8;
	pid_E.fKdE = 0.0001;
	pid_E.fTsE = 100;
	pid_E.fOutminE = 0;
	pid_E.fOutmaxE = 100;
	PID_init_E(&pid_E);

	pid_D.fKpD = 15;
	pid_D.fKiD = 0.8;
	pid_D.fKdD = 0.0001;
	pid_D.fTsD = 100;
	pid_D.fOutminD = 0;
	pid_D.fOutmaxD = 100;
	PID_init_D(&pid_D);

  /* Infinite loop */
  for(;;)
  {
	  osSemaphoreAcquire(SemaphoreMovimentaHandle, 200);

	  /*seta o IN1, IN2, IN3 e IN4 de forma que o veículo siga em frente*/
	  HAL_GPIO_WritePin(IN1_GPIO_Port, IN1_Pin, 1);
	  HAL_GPIO_WritePin(IN2_GPIO_Port, IN2_Pin, 0);

	  HAL_GPIO_WritePin(IN3_GPIO_Port, IN3_Pin, 0);
	  HAL_GPIO_WritePin(IN4_GPIO_Port, IN4_Pin, 1);

	  /*
	   * caso os valores referentes à velocidade dos dois motores sejam "0" o veículo deve parar imediatamente
	   * caso contrario deve prosseguir normalmente
	   */
	  if(!fWAngularD && !fWAngularE){
		  HAL_GPIO_WritePin(IN1_GPIO_Port, IN1_Pin, 0);
		  HAL_GPIO_WritePin(IN2_GPIO_Port, IN2_Pin, 0);

		  HAL_GPIO_WritePin(IN3_GPIO_Port, IN3_Pin, 0);
		  HAL_GPIO_WritePin(IN4_GPIO_Port, IN4_Pin, 0);

		  fPIDVal_E = 0;
		  fPIDVal_D = 0;
	  }else{
		  /*A função "PID_E" e "PID_D" retorna o valor PWM para cada motor de acordo com a realimentação vinda do encoder*/
		  fVE = fWE*RAIO;
		  fVD = fWD*RAIO;
		  fPIDVal_E = PID_E(fVE, fWAngularE);
		  fPIDVal_D = PID_D(fVD, fWAngularD);

	  }
	  htim3.Instance->CCR2 = fPIDVal_E;
	  htim3.Instance->CCR1 = fPIDVal_D;

	  osSemaphoreRelease(SemaphoreMovimentaHandle);
	  osDelay(100);
  }
  /* USER CODE END FunctionAtivarMotores */
}

/* USER CODE BEGIN Header_FunctionOdometria */
/**
* @brief Function implementing the Odometria thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_FunctionOdometria */
void FunctionOdometria(void *argument)
{
  /* USER CODE BEGIN FunctionOdometria */
	HAL_TIM_Base_Start_IT(&htim6);
	HAL_TIM_Base_Start(&htim2);
	HAL_TIM_Base_Start(&htim5);
  /* Infinite loop */
  for(;;)
  {
	  osSemaphoreAcquire(SemaphoreComunicaHandle, 100);

	  /* Primeiro é calculado a Velocidade de cada roda em m/s */
	  float fVE = fWE*RAIO;
	  float fVD = fWD*RAIO;

	  /*Posteriormente é calculado o valor Teta*/
	  fTeta = fTeta + ((fVD-fVE)/(COMPRIMENTO+LARGURA))*fTs;

	  /*Assim é calculado o valor de deslocamento em X e Y*/
	  fX = fX + ((fVD+fVE)/2)*cos(fTeta)*fTs;
	  fY = fY + ((fVD+fVE)/2)*sin(fTeta)*fTs;

	  /*a distancia é dada pela relação da raiz da doma dos quadrados de X e Y*/
	  fDistanciaO = (sqrt(pow(fX,2) + pow(fY,2)))/0.75;
	  osSemaphoreRelease(SemaphoreComunicaHandle);
	  osDelay(100);
  }
  /* USER CODE END FunctionOdometria */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/*interrupção para captura do ultrassônico*/
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {
	if(htim == &htim1){//ultrassonico
		/*a interrupção é disparada na borda de subida onde pega o primeiro valor do timer*/
		if(uiIsFirst){
			fDistancia = 0;
			lValor1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_3);
			/*
			 * Setando o "uiIsFirst" como "0" e forçando a captura da borda de descida, é esperado que
			 * na proxima interrupção seja pego o valor do timer referente ao tempo que o ECHO ficou em alta
			 */
			uiIsFirst=0;
			__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_3, TIM_INPUTCHANNELPOLARITY_FALLING);
		}else{
			/*pegando o segundo valor do timer*/
			lValor2 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_3);

			/*calculando a diferença entre a borda de subida e descida*/
			fDiferenca = (float)((unsigned)lValor2-(unsigned)lValor1);
			fDistancia = ((fDiferenca/2)*0.0001)*340/2 < 100?((fDiferenca/2)*0.0001)*340/2 : fDistancia;

			/*
			 * caso valor superior a 0 e inferior a 20 o veículo será blockeado
			 * o valor deve ser superior a 0 pois, caso não haja captura, o valo será "0" e
			 * não necessariamente deve ser blockeado
			 * */
			uiBloqueado = (fDistancia>0 && fDistancia<20) ? 1 : 0;
			uiBloqueado = 0;

			/*seta a captura para borda de subida e "uiIsFirst" para primeiro valor novamente*/
			uiIsFirst=1;
			__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_3, TIM_INPUTCHANNELPOLARITY_RISING);
			__HAL_TIM_DISABLE_IT(htim, TIM_CHANNEL_3);
		}
	}
}

/*interrupção a cada 100ms para calcular o valor do encoder que incrementa o contador do TIMER*/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef * htim){
	if(htim == &htim6){
		/*pegando os valores do contador do timer*/
		ulPulsesE = __HAL_TIM_GET_COUNTER(&htim2);
		ulPulsesD = __HAL_TIM_GET_COUNTER(&htim5);

		/*LED para verificar o correto funcionamento da interrupção*/
		HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);

		/*Calculando a velocidade em m/s de acordo ao "fTs" que é a taxa de amostragem*/
		fWE = (((float)ulPulsesE/FUROS)*2*PI)/fTs;
		fWD = (((float)ulPulsesD/FUROS)*2*PI)/fTs;

		/*resetando o contador dos timers*/
		__HAL_TIM_SET_COUNTER(&htim2,0);
		__HAL_TIM_SET_COUNTER(&htim5,0);
	}
}

/* USER CODE END Application */

