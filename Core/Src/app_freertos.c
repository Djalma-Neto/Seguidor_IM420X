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

//seguidor
uint8_t S2;
uint8_t S3;
uint8_t S4;
uint8_t NEAR;
uint8_t CLP;

//ultrassônico
uint32_t valor1 = 0;
uint32_t valor2 = 0;
uint32_t diferenca = 0;
uint8_t Is_First = 1;
uint8_t distancia = 0;

//motores
uint32_t IN1;
uint32_t IN2;
uint32_t IN3;
uint32_t IN4;
uint32_t velocidade;

char mostrar[100];

/* USER CODE END Variables */
/* Definitions for triger */
osThreadId_t trigerHandle;
const osThreadAttr_t triger_attributes = {
  .name = "triger",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 128 * 4
};
/* Definitions for comunicacao */
osThreadId_t comunicacaoHandle;
const osThreadAttr_t comunicacao_attributes = {
  .name = "comunicacao",
  .priority = (osPriority_t) osPriorityLow,
  .stack_size = 128 * 4
};
/* Definitions for seguidor */
osThreadId_t seguidorHandle;
const osThreadAttr_t seguidor_attributes = {
  .name = "seguidor",
  .priority = (osPriority_t) osPriorityLow7,
  .stack_size = 128 * 4
};
/* Definitions for motores */
osThreadId_t motoresHandle;
const osThreadAttr_t motores_attributes = {
  .name = "motores",
  .priority = (osPriority_t) osPriorityLow6,
  .stack_size = 128 * 4
};
/* Definitions for Semaphore */
osSemaphoreId_t SemaphoreHandle;
const osSemaphoreAttr_t Semaphore_attributes = {
  .name = "Semaphore"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartTriger(void *argument);
void StartComunicacao(void *argument);
void StartSeguidor(void *argument);
void StartMotores(void *argument);

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
  /* creation of Semaphore */
  SemaphoreHandle = osSemaphoreNew(1, 1, &Semaphore_attributes);

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
  /* creation of triger */
  trigerHandle = osThreadNew(StartTriger, NULL, &triger_attributes);

  /* creation of comunicacao */
  comunicacaoHandle = osThreadNew(StartComunicacao, NULL, &comunicacao_attributes);

  /* creation of seguidor */
  seguidorHandle = osThreadNew(StartSeguidor, NULL, &seguidor_attributes);

  /* creation of motores */
  motoresHandle = osThreadNew(StartMotores, NULL, &motores_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartTriger */
/**
  * @brief  Function implementing the triger thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartTriger */
void StartTriger(void *argument)
{
  /* USER CODE BEGIN StartTriger */
	HAL_TIM_IC_Start_IT(&htim1, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
  /* Infinite loop */
  for(;;)
  {
	  __HAL_TIM_ENABLE_IT(&htim1, TIM_IT_CC1);
	  htim2.Instance->CCR1 = 5;
	  osDelay(10);
  }
  /* USER CODE END StartTriger */
}

/* USER CODE BEGIN Header_StartComunicacao */
/**
* @brief Function implementing the comunicacao thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartComunicacao */
void StartComunicacao(void *argument)
{
  /* USER CODE BEGIN StartComunicacao */
  /* Infinite loop */
  for(;;)
  {
	  //sprintf(mostrar,"D: %d, V1: %d, V2: %d, DIF: %d \r \n ",(int)distancia,(int)valor1,(int)valor2,(int)diferenca);
	  sprintf(mostrar,"S2: %d, S3: %d, S4: %d \r \n ",(int)S2,(int)S3,(int)S4);
	  HAL_UART_Transmit(&hlpuart1, (uint8_t*)mostrar, sizeof(mostrar), 100);
	  osDelay(100);
  }
  /* USER CODE END StartComunicacao */
}

/* USER CODE BEGIN Header_StartSeguidor */
/**
* @brief Function implementing the seguidor thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartSeguidor */
void StartSeguidor(void *argument)
{
  /* USER CODE BEGIN StartSeguidor */
  /* Infinite loop */
  for(;;)
  {
    S2 = HAL_GPIO_ReadPin(S2_GPIO_Port, S2_Pin);
    S3 = HAL_GPIO_ReadPin(S3_GPIO_Port, S3_Pin);
    S4 = HAL_GPIO_ReadPin(S4_GPIO_Port, S4_Pin);
    NEAR = HAL_GPIO_ReadPin(NEAR_GPIO_Port, NEAR_Pin);
    CLP = HAL_GPIO_ReadPin(CLP_GPIO_Port, CLP_Pin);

    osSemaphoreAcquire(SemaphoreHandle, 200);


    if(!CLP){
    	IN1 = 0;
		IN2 = 0;

		IN3 = 0;
		IN4 = 0;
    }else if(S2 && !S3 && S4){
    	IN1 = 1;
    	IN2 = 0;

    	IN3 = 0;
    	IN4 = 1;
    }else if(S2 && S3 && !S4){
    	IN1 = 1;
		IN2 = 0;

		IN3 = 0;
		IN4 = 0;
    }else if(!S2 && S3 && S4){
    	IN1 = 0;
		IN2 = 0;

		IN3 = 0;
		IN4 = 1;
    }else if(!S2 && !S3 && !S4){
    	IN1 = 0;
		IN2 = 0;

		IN3 = 0;
		IN4 = 0;

		osDelay(500);
    	HAL_GPIO_WritePin(Buzzer_GPIO_Port, Buzzer_Pin, GPIO_PIN_SET);
    	osDelay(500);
    	HAL_GPIO_WritePin(Buzzer_GPIO_Port, Buzzer_Pin, GPIO_PIN_RESET);
    	osDelay(500);
    	HAL_GPIO_WritePin(Buzzer_GPIO_Port, Buzzer_Pin, GPIO_PIN_SET);
		osDelay(500);
		HAL_GPIO_WritePin(Buzzer_GPIO_Port, Buzzer_Pin, GPIO_PIN_RESET);
    }
    velocidade = 10;

    osSemaphoreRelease(SemaphoreHandle);
    osDelay(200);
  }
  /* USER CODE END StartSeguidor */
}

/* USER CODE BEGIN Header_StartMotores */
/**
* @brief Function implementing the motores thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartMotores */
void StartMotores(void *argument)
{
  /* USER CODE BEGIN StartMotores */
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);

  /* Infinite loop */
  for(;;)
  {
	  osSemaphoreAcquire(SemaphoreHandle, 200);

	  HAL_GPIO_WritePin(IN1_GPIO_Port, IN1_Pin, IN1);
	  HAL_GPIO_WritePin(IN2_GPIO_Port, IN2_Pin, IN2);

	  HAL_GPIO_WritePin(IN3_GPIO_Port, IN3_Pin, IN3);
	  HAL_GPIO_WritePin(IN4_GPIO_Port, IN4_Pin, IN4);
	  //htim3.Instance->CCR1 = velocidade;
	  //htim3.Instance->CCR2 = velocidade;

	  htim3.Instance->CCR1 = 10;
	  htim3.Instance->CCR2 = 255;
	  osSemaphoreRelease(SemaphoreHandle);
	  osDelay(200);
  }
  /* USER CODE END StartMotores */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {
	if (htim == &htim1) {
		if (Is_First) {
			valor1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
			Is_First = 0;
			__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1,TIM_INPUTCHANNELPOLARITY_FALLING);
		} else if(!Is_First){
			valor2 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
			__HAL_TIM_SET_COUNTER(htim, 0);

			diferenca = valor2>valor1?valor2-valor1:valor1-valor2;

			distancia = diferenca * .034/2;

			Is_First = 1;
			__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1,TIM_INPUTCHANNELPOLARITY_RISING);
			__HAL_TIM_DISABLE_IT(htim, TIM_IT_CC1);
		}
	}
}

/* USER CODE END Application */

