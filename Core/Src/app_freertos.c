/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : app_freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
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

//ultrassônico
long lValor1;
long lValor2;
float fDiferenca;
uint8_t uiIs_First = 1;
float fDistancia;

//motores
long lVelocidadeRefD = 0;
long lVelocidadeRefE = 0;
unsigned int uiVelocidade = 255;
float fCalibragemE=0.0;
float fCalibragemD=0.2;
float fReducao=0.4;


//odometria
uint8_t uiFuros = 20;
// encoder_1
unsigned long ulPulsePerSecondE;
float fRpmE;
uint8_t uiFistWind1 = 1;

// encoder_2
unsigned long ulPulsePerSecondD;
float fRpmD;
uint8_t uiFistWind2 = 1;

uint32_t uiStart = 0;
uint32_t uiBloqueado = 0;

char cMostrar[100];

char cData[100];

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

int PID(int valocidade, int valor){
	int pwm=0;
	//lVelocidadeRefE => SETPOINT
	//erro => lVelocidadeRefE-fRpmE
	//Kp = 53/720
	//pwm = erro*Kp
	pwm = (valocidade-valor)*(0.01/valocidade);

	return pwm;
}

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
		osDelay(200);
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
	HAL_UART_Receive_IT(&huart1, (uint8_t *)cData, 100);
	HAL_GPIO_WritePin(HC05_EN_GPIO_Port, HC05_EN_Pin, 1);
  /* Infinite loop */
  for(;;)
  {
	  osSemaphoreAcquire(SemaphoreComunicaHandle, 200);

	  int valor1 = (int)fDistancia;
	  int valor2 = (fDistancia-(int)fDistancia)*100;
	  sprintf(cMostrar,"Dis: %d.%02d \r \n ",valor1,valor2);

	  HAL_GPIO_WritePin(HC05_STATE_GPIO_Port, HC05_STATE_Pin, 0);
	  osDelay(500);
	  HAL_GPIO_WritePin(HC05_STATE_GPIO_Port, HC05_STATE_Pin, 1);

	  HAL_UART_Transmit(&huart1, (uint8_t*)cMostrar, sizeof(cMostrar), 100);

	  osSemaphoreRelease(SemaphoreComunicaHandle);
	  osDelay(500);
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

    if(uiBTN){
    	lVelocidadeRefD = 1;
    	lVelocidadeRefE = 1;
    	uiStart = uiStart?0:1;
    	HAL_GPIO_WritePin(Buzzer_GPIO_Port, Buzzer_Pin, GPIO_PIN_SET);
    	osDelay(500);
    	HAL_GPIO_WritePin(Buzzer_GPIO_Port, Buzzer_Pin, GPIO_PIN_RESET);
    	osDelay(500);
    }else if(uiS2 && !uiS3 && uiS4 && uiStart){
    	lVelocidadeRefD = uiVelocidade;
    	lVelocidadeRefE = uiVelocidade;
    }else if(uiS2 && uiS3 && !uiS4 && uiStart){
    	lVelocidadeRefE = uiVelocidade;
    	lVelocidadeRefD = uiVelocidade-(uiVelocidade*fReducao);
    }else if(!uiS2 && uiS3 && uiS4 && uiStart){
    	lVelocidadeRefE = uiVelocidade-(uiVelocidade*fReducao);
    	lVelocidadeRefD = uiVelocidade;
    }else if(((uiS2 && !uiS3 && uiS4)) && uiStart){
    	lVelocidadeRefD = 1;
    	lVelocidadeRefE = 1;
    } else if(!uiS2 && !uiS3 && !uiS4 && uiStart){
    	lVelocidadeRefD = 1;
		lVelocidadeRefE = 1;
		uiStart = 0;
    	HAL_GPIO_WritePin(Buzzer_GPIO_Port, Buzzer_Pin, GPIO_PIN_SET);
    	osDelay(100);
    	HAL_GPIO_WritePin(Buzzer_GPIO_Port, Buzzer_Pin, GPIO_PIN_RESET);
    	osDelay(100);
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
  /* Infinite loop */
  for(;;)
  {
    osSemaphoreAcquire(SemaphoreMovimentaHandle, 200);

	  HAL_GPIO_WritePin(IN1_GPIO_Port, IN1_Pin, 1);
	  HAL_GPIO_WritePin(IN2_GPIO_Port, IN2_Pin, 0);

	  HAL_GPIO_WritePin(IN3_GPIO_Port, IN3_Pin, 0);
	  HAL_GPIO_WritePin(IN4_GPIO_Port, IN4_Pin, 1);

	  htim3.Instance->CCR1 = lVelocidadeRefE+(lVelocidadeRefE*fCalibragemE);
	  htim3.Instance->CCR2 = lVelocidadeRefD+(lVelocidadeRefD*fCalibragemD);

	  osSemaphoreRelease(SemaphoreMovimentaHandle);
	  osDelay(200);
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
	HAL_TIM_IC_Start_IT(&htim4, TIM_CHANNEL_1);
	HAL_TIM_IC_Start_IT(&htim4, TIM_CHANNEL_2);
	HAL_TIM_Base_Start_IT(&htim6);
  /* Infinite loop */
  for(;;)
  {
	  osDelay(1);
  }
  /* USER CODE END FunctionOdometria */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {
	if(htim == &htim1){//ultrassonico
		if(uiIs_First){
			lValor1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_3);
			uiIs_First=0;
			__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_3, TIM_INPUTCHANNELPOLARITY_FALLING);
		}else{
			lValor2 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_3);
			uiIs_First=1;

			fDiferenca = (float)((unsigned)lValor2-(unsigned)lValor1);
			fDiferenca = (fDiferenca/2)*0.0001;
			fDistancia = (fDiferenca*340)/2>100?fDistancia:(fDiferenca*340)/2;

			__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_3, TIM_INPUTCHANNELPOLARITY_RISING);
			__HAL_TIM_DISABLE_IT(htim, TIM_CHANNEL_3);
		}
	}
	if(htim == &htim4 && htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1){//encoder Esquerdo
		ulPulsePerSecondE++;
	}
	if(htim == &htim4 && htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2){//encoder Direito
		ulPulsePerSecondD++;
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef * htim){
	if(htim == &htim6){
		fRpmE = ulPulsePerSecondE;
		fRpmD = ulPulsePerSecondD;
		//HAL_GPIO_TogglePin(Buzzer_GPIO_Port, Buzzer_Pin);

		ulPulsePerSecondE = 0;
		ulPulsePerSecondD = 0;
	}
}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if(huart == &huart1)
  {
	  HAL_UART_Receive(&huart1, (uint8_t *)cData, 100, 100);
	  HAL_GPIO_WritePin(HC05_STATE_GPIO_Port, HC05_STATE_Pin, 0);
	  osDelay(500);
	  HAL_GPIO_WritePin(HC05_STATE_GPIO_Port, HC05_STATE_Pin, 1);
  }
}

/* USER CODE END Application */

