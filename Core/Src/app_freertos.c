/* USER CODE BEGIN Header */
/* *********************************************************************************** */
/* File name: seguidorLinha.c */
/* File description:
/*
/*
/*
/*
/* Author name: Ramiro; Djalma
/* Creation date:
/*
/* Revision date:
/* *********************************************************************************** */
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

//variáveis utilizadas pelo sensor ultrassônico
long lValor1;
long lValor2;
float fDiferenca, fDistancia;
uint8_t uiIsFirst = 1;

//variáveis utilizadas pelos motores
float fVelocidade = 0.030; // M/s
float fReducao = 0.5;
float fWAngularD, fWAngularE,fVE,fVD,fPIDVal_D, fPIDVal_E;

//variáveis utilizadas pela odometria
#define FUROS 20
#define RAIO 0.00325
#define COMPRIMENTO 0.12
#define LARGURA 0.028

float fTeta, fSD, fSE, fDistanciaO;

// encoder_Esquerdo
unsigned long ulPulsePerSecondE;
float fVelocidadeE;

// encoder_Direito
unsigned long ulPulsePerSecondD;
float fVelocidadeD;

//variáveis utilizadas pelo seguidor
uint32_t uiStart = 0;
uint32_t uiBloqueado = 0;
uint32_t uiCountSeguidor = 0;

//variáveis utilizadas para comunicação
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
	HAL_UART_Receive_IT(&huart1, (uint8_t *)cData, sizeof(cData));
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
	  HAL_UART_Receive(&huart1, (uint8_t *)&cData, sizeof(cData),100);
	  if(uiBloqueado){
		  int dist1 = (int)fDistancia;
		  int dist2 = (fDistancia-(int)fDistancia)*100;

		  sprintf(cMostrar,"Blockeado: %d.%02d\r \n ",dist1,dist2);
	  } else if(!uiStart){
		  sprintf(cMostrar,"Aguardando Start!! \r \n ");
	  } else{
		  if(cData == 'O' || cData == 'o'){
			  int valor1 = (int)fDistanciaO;
			  int valor2 = (fDistanciaO-(int)fDistanciaO)*100;

			  sprintf(cMostrar,"Distancia Percorrida: %d.%02dm \r \n ",valor1,valor2);
		  }else if(cData == 'E' || cData == 'e'){
			  float valE = ((float)ulPulsePerSecondE/FUROS)*10;
			  float valD = ((float)ulPulsePerSecondE/FUROS)*10;

			  int valor1 = (int)valE;
			  int valor2 = (valE-(int)valE)*100;

			  int valor3 = (int)valD;
			  int valor4 = (valD-(int)valD)*100;

			  sprintf(cMostrar,"EncoderE: %d.%02d RPS -- EncoderD: %d.%02d RPS \r \n ",valor1,valor2,valor3,valor4);
		  }else{
			  cData == 'M';
			  int valor1 = (int)fVE;
			  int valor2 = (fVE-(int)fVE)*100;

			  int valor3 = (int)fVD;
			  int valor4 = (fVD-(int)fVD)*100;

			  sprintf(cMostrar,"MotorE: %d.%02d m/s -- MotorD: %d.%02d m/s \r \n ",valor1,valor2,valor3,valor4);
		  }
	  }
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
    uint8_t uiS2 = HAL_GPIO_ReadPin(S2_GPIO_Port, S2_Pin);
    uint8_t uiS3 = HAL_GPIO_ReadPin(S3_GPIO_Port, S3_Pin);
    uint8_t uiS4 = HAL_GPIO_ReadPin(S4_GPIO_Port, S4_Pin);

    //uint8_t uiNEAR = HAL_GPIO_ReadPin(NEAR_GPIO_Port, NEAR_Pin);
    //uint8_t uiCLP = HAL_GPIO_ReadPin(CLP_GPIO_Port, CLP_Pin);
    uint8_t uiBTN = HAL_GPIO_ReadPin(BTN1_GPIO_Port, BTN1_Pin);

    osSemaphoreAcquire(SemaphoreMovimentaHandle, 200);

    if(uiBloqueado){
    	fWAngularD = 0;
		fWAngularE = 0;
		HAL_GPIO_WritePin(Buzzer_GPIO_Port, Buzzer_Pin, GPIO_PIN_SET);
		osDelay(500);
		HAL_GPIO_WritePin(Buzzer_GPIO_Port, Buzzer_Pin, GPIO_PIN_RESET);
    }else if(uiBTN){
    	uiStart = uiStart?0:1;
    	uiCountSeguidor = 0;
    	fWAngularD = 0;
    	fWAngularE = 0;
    	HAL_GPIO_WritePin(Buzzer_GPIO_Port, Buzzer_Pin, GPIO_PIN_SET);
    	osDelay(500);
    	HAL_GPIO_WritePin(Buzzer_GPIO_Port, Buzzer_Pin, GPIO_PIN_RESET);
    	osDelay(500);
    }else if(uiStart && uiCountSeguidor < 50){
    	if(uiS2 && uiS3 && !uiS4){
    		uiCountSeguidor = 0;
			fWAngularE = fVelocidade;
			fWAngularD = 0;
		}else if(!uiS2 && uiS3 && uiS4){
			uiCountSeguidor = 0;
			fWAngularE = 0;
			fWAngularD = fVelocidade;
		}else if(uiS2 && !uiS3 && uiS4){
			uiCountSeguidor = 0;
			fWAngularD = fVelocidade;
			fWAngularE = fVelocidade;
		}else if(!uiS2 && !uiS3 && !uiS4){
			uiCountSeguidor = 0;
			fWAngularD = 0;
			fWAngularE = 0;
			uiStart = 0;
			HAL_GPIO_WritePin(Buzzer_GPIO_Port, Buzzer_Pin, GPIO_PIN_SET);
			osDelay(100);
			HAL_GPIO_WritePin(Buzzer_GPIO_Port, Buzzer_Pin, GPIO_PIN_RESET);
			osDelay(100);
		}
    	if(uiS2 && uiS3 && uiS4){
    		uiCountSeguidor++;
		}
    }else{
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
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);

	sPID_D pid_D;
	sPID_E pid_E;

	pid_E.fKpE = 800;
	pid_E.fKiE = 2;
	pid_E.fKdE = 0;
	pid_E.fTsE = 200000;
	pid_E.fOutminE = 0;
	pid_E.fOutmaxE = 100;
	PID_init_E(&pid_E);

	pid_D.fKpD = 800;
	pid_D.fKiD = 2;
	pid_D.fKdD = 0;
	pid_D.fTsD = 200000;
	pid_D.fOutminD = 0;
	pid_D.fOutmaxD = 100;
	PID_init_D(&pid_D);

  /* Infinite loop */
  for(;;)
  {
	  osSemaphoreAcquire(SemaphoreMovimentaHandle, 200);
	  if(fWAngularD && fWAngularE){
		  HAL_GPIO_WritePin(IN1_GPIO_Port, IN1_Pin, 1);
		  HAL_GPIO_WritePin(IN2_GPIO_Port, IN2_Pin, 0);

		  HAL_GPIO_WritePin(IN3_GPIO_Port, IN3_Pin, 0);
		  HAL_GPIO_WritePin(IN4_GPIO_Port, IN4_Pin, 1);
	  }else if(!fWAngularD && !fWAngularE){
		  HAL_GPIO_WritePin(IN1_GPIO_Port, IN1_Pin, 0);
		  HAL_GPIO_WritePin(IN2_GPIO_Port, IN2_Pin, 0);

		  HAL_GPIO_WritePin(IN3_GPIO_Port, IN3_Pin, 0);
		  HAL_GPIO_WritePin(IN4_GPIO_Port, IN4_Pin, 0);
	  }/*else if(!fWAngularD && fWAngularE){
		  HAL_GPIO_WritePin(IN1_GPIO_Port, IN1_Pin, 1);
		  HAL_GPIO_WritePin(IN2_GPIO_Port, IN2_Pin, 0);

		  HAL_GPIO_WritePin(IN3_GPIO_Port, IN3_Pin, 0);
		  HAL_GPIO_WritePin(IN4_GPIO_Port, IN4_Pin, 0);
	  }else if(fWAngularD && !fWAngularE){
		  HAL_GPIO_WritePin(IN1_GPIO_Port, IN1_Pin, 0);
		  HAL_GPIO_WritePin(IN2_GPIO_Port, IN2_Pin, 0);

		  HAL_GPIO_WritePin(IN3_GPIO_Port, IN3_Pin, 0);
		  HAL_GPIO_WritePin(IN4_GPIO_Port, IN4_Pin, 1);
	  }*/

	  ulPulsePerSecondE = ulPulsePerSecondE*10;
	  ulPulsePerSecondD = ulPulsePerSecondD*10;

	  fVE = ((((float)ulPulsePerSecondE*2*3.14)/20)*RAIO);
	  fVD = ((((float)ulPulsePerSecondD*2*3.14)/20)*RAIO);

	  fPIDVal_D = PID_D(fVD, fWAngularD);
	  fPIDVal_E = PID_E(fVE, fWAngularE);

	  htim3.Instance->CCR1 = fPIDVal_D;
	  htim3.Instance->CCR2 = fPIDVal_E;

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
	  fTeta = fTeta + (((fVelocidadeD-fVelocidadeE)/(COMPRIMENTO+LARGURA))*1);

	  fSD = fSD + ((fVelocidadeD+fVelocidadeE)/2)*cos(fTeta);
	  fSE = fSE + ((fVelocidadeD+fVelocidadeE)/2)*sin(fTeta);

	  fDistanciaO = sqrt(pow(fSD,2) + pow(fSE,2));
	  osSemaphoreRelease(SemaphoreComunicaHandle);
	  osDelay(100);
  }
  /* USER CODE END FunctionOdometria */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {
	if(htim == &htim1){//ultrassonico
		if(uiIsFirst){
			lValor1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_3);
			uiIsFirst=0;
			__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_3, TIM_INPUTCHANNELPOLARITY_FALLING);
		}else{
			lValor2 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_3);
			uiIsFirst=1;

			fDiferenca = (float)((unsigned)lValor2-(unsigned)lValor1);
			fDistancia = ((fDiferenca/2)*0.0001)*340/2 < 100?((fDiferenca/2)*0.0001)*340/2 : fDistancia;

			uiBloqueado = (fDistancia>2 && fDistancia<20) ? 1 : 0;

			__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_3, TIM_INPUTCHANNELPOLARITY_RISING);
			__HAL_TIM_DISABLE_IT(htim, TIM_CHANNEL_3);
		}
	}
}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef * htim){
	if(htim == &htim6){
		ulPulsePerSecondE = __HAL_TIM_GET_COUNTER(&htim2);
		ulPulsePerSecondD = __HAL_TIM_GET_COUNTER(&htim5);

		fVelocidadeE = ((float)ulPulsePerSecondE/FUROS)*2*3.1415*RAIO;
		fVelocidadeD = ((float)ulPulsePerSecondD/FUROS)*2*3.1415*RAIO;

		__HAL_TIM_SET_COUNTER(&htim2,0);
		__HAL_TIM_SET_COUNTER(&htim5,0);
	}
}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if(huart == &huart1)
  {
	  //
  }
}

/* USER CODE END Application */

