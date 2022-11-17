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
uint8_t uiValor1;
uint8_t uiValor2;
uint8_t uiDiferenca;
uint32_t uiIs_First = 1;
uint8_t uiDistancia;

//motores
uint32_t uiIN1;
uint32_t uiIN2;
uint32_t uiIN3;
uint32_t uiIN4;
uint8_t uiVelocidadeRef;

//odometria
// encoder_1
uint8_t uiTime1;
uint32_t uiIsFistPulse1 = 0;

// encoder_2
uint8_t uiTime2;
uint32_t uiIsFistPulse2 = 0;

uint32_t uiStart = 0;
uint32_t uiBloqueado = 0;

char cMostrar[100];

/* USER CODE END Variables */
/* Definitions for Utrassom */
osThreadId_t UtrassomHandle;
const osThreadAttr_t Utrassom_attributes = {
  .name = "Utrassom",
  .priority = (osPriority_t) osPriorityNormal2,
  .stack_size = 128 * 4
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
  HAL_TIM_IC_Start_IT(&htim1, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
  /* Infinite loop */
  for(;;)
  {
    __HAL_TIM_ENABLE_IT(&htim1, TIM_IT_CC1);
	  htim2.Instance->CCR1 = 5;
	  osDelay(10);
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
    //sprintf(mostrar,"D: %d, V1: %d, V2: %d, DIF: %d \r \n ",(int)distancia,(int)valor1,(int)valor2,(int)diferenca);
	  sprintf(cMostrar,"S2: %d, S3: %d, S4: %d \r \n ",(int)uiIN1,(int)uiIN2,(int)uiIN3);
	  HAL_UART_Transmit(&hlpuart1, (uint8_t*)cMostrar, sizeof(cMostrar), 100);
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
    uint8_t uiNEAR = HAL_GPIO_ReadPin(NEAR_GPIO_Port, NEAR_Pin);
    uint8_t uiCLP = HAL_GPIO_ReadPin(CLP_GPIO_Port, CLP_Pin);

    osSemaphoreAcquire(SemaphoreMovimentaHandle, 200);

    if(uiCLP){
    	uiIN1 = 0;
      uiIN2 = 0;

      uiIN3 = 0;
      uiIN4 = 0;
      uiStart = uiStart?0:1;
    }else if(uiS2 && !uiS3 && uiS4 && uiStart){
    	uiIN1 = 1;
    	uiIN2 = 0;

    	uiIN3 = 0;
    	uiIN4 = 1;
    }else if(uiS2 && uiS3 && !uiS4 && uiStart){
    	uiIN1 = 1;
      uiIN2 = 0;

      uiIN3 = 0;
      uiIN4 = 0;
    }else if(!uiS2 && uiS3 && uiS4 && uiStart){
    	uiIN1 = 0;
      uiIN2 = 0;

      uiIN3 = 0;
      uiIN4 = 1;
    }else if(uiS2 && uiS3 && uiS4 && uiStart){
    	//uiIN1 = 0;
      //IN2 = 0;

      //IN3 = 0;
      //IN4 = 0;

      uiIN1 = 1;
      uiIN2 = 0;

      uiIN3 = 0;
      uiIN4 = 1;

    	HAL_GPIO_WritePin(Buzzer_GPIO_Port, Buzzer_Pin, GPIO_PIN_SET);
    	osDelay(500);
    	HAL_GPIO_WritePin(Buzzer_GPIO_Port, Buzzer_Pin, GPIO_PIN_RESET);
    	osDelay(500);
    } else if(!uiS2 && !uiS3 && !uiS4 && uiStart){
    	HAL_GPIO_WritePin(Buzzer_GPIO_Port, Buzzer_Pin, GPIO_PIN_SET);
    	osDelay(500);
    	HAL_GPIO_WritePin(Buzzer_GPIO_Port, Buzzer_Pin, GPIO_PIN_RESET);
    	osDelay(500);
    }
    uiVelocidadeRef = 80;

    osSemaphoreRelease(SemaphoreMovimentaHandle);
    osDelay(200);
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

	  HAL_GPIO_WritePin(IN1_GPIO_Port, IN1_Pin, uiIN1);
	  HAL_GPIO_WritePin(IN2_GPIO_Port, IN2_Pin, uiIN2);

	  HAL_GPIO_WritePin(IN3_GPIO_Port, IN3_Pin, uiIN3);
	  HAL_GPIO_WritePin(IN4_GPIO_Port, IN4_Pin, uiIN4);
	  htim3.Instance->CCR1 = uiVelocidadeRef;
	  htim3.Instance->CCR2 = uiVelocidadeRef+30;

	  //htim3.Instance->CCR1 = 85;
	  //htim3.Instance->CCR2 = 85;
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
	HAL_TIM_IC_Start_IT(&htim5, TIM_CHANNEL_2);
	HAL_TIM_IC_Start_IT(&htim4, TIM_CHANNEL_1);
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
	if (htim == &htim1) {
		if (uiIs_First) {
			uiValor1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
			uiIs_First = 0;
			__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1,TIM_INPUTCHANNELPOLARITY_FALLING);
		} else if(!uiIs_First){
			uiValor2 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
			__HAL_TIM_SET_COUNTER(htim, 0);

			uiDiferenca = uiValor2>uiValor1?uiValor2-uiValor1:uiValor1-uiValor2;

			uiDistancia = uiDiferenca * .034/2;

			uiIs_First = 1;
			__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1,TIM_INPUTCHANNELPOLARITY_RISING);
			__HAL_TIM_DISABLE_IT(htim, TIM_IT_CC1);
		}
	} else if(htim == &htim5){
    if(uiIsFistPulse1){
      __HAL_TIM_SET_COUNTER(htim, 0);
      uiIsFistPulse1 = 0;
    }else{
      uiTime1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
      uiIsFistPulse1 = 1;
    }
  }

}

/* USER CODE END Application */

