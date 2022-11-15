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

/*
uint8_t direcao;
uint8_t oldDirecao;

//ultrassônico
uint32_t valor1 = 0;
uint32_t valor2 = 0;
uint32_t diferenca = 0;
uint8_t Is_First = 1;
uint8_t distancia = 0;

//encoders
uint8_t countEncoderD = 0;
uint8_t countEncoderE = 0;

char mostrar[100];
*/

/* USER CODE END Variables */
/* Definitions for movimenta */
osThreadId_t movimentaHandle;
const osThreadAttr_t movimenta_attributes = {
  .name = "movimenta",
  .priority = (osPriority_t) osPriorityNormal1,
  .stack_size = 128 * 4
};
/* Definitions for readUart */
osThreadId_t readUartHandle;
const osThreadAttr_t readUart_attributes = {
  .name = "readUart",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 128 * 4
};
/* Definitions for Triger */
osThreadId_t TrigerHandle;
const osThreadAttr_t Triger_attributes = {
  .name = "Triger",
  .priority = (osPriority_t) osPriorityNormal2,
  .stack_size = 128 * 4
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartMovimenta(void *argument);
void StartReadUart(void *argument);
void StartTriger(void *argument);

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
  /* creation of movimenta */
  movimentaHandle = osThreadNew(StartMovimenta, NULL, &movimenta_attributes);

  /* creation of readUart */
  readUartHandle = osThreadNew(StartReadUart, NULL, &readUart_attributes);

  /* creation of Triger */
  TrigerHandle = osThreadNew(StartTriger, NULL, &Triger_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
	/* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
	/* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartMovimenta */
/**
 * @brief  Function implementing the movimenta thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartMovimenta */
void StartMovimenta(void *argument)
{
  /* USER CODE BEGIN StartMovimenta */
	/* Infinite loop */
	/*
	 int vel = 1;

	 int IN1 = 0;
	 int IN2 = 0;

	 int IN3 = 0;
	 int IN4 = 0;
	 for (;;) {
		 if(HAL_UART_Receive(&hlpuart1, &direcao, 1, 200) == HAL_OK){
			 if(direcao == 'w'){
				 switch(oldDirecao){
					 case 's':IN1=0;IN2=0;IN3=0;IN4=0;break;
					 case 'd':IN1=vel;IN2=0;IN3=vel/2;IN4=0;break;
					 case 'a':IN1=vel/2;IN2=0;IN3=vel;IN4=0;break;
					 default:IN1=vel;IN2=0;IN3=vel;IN4=0;break;
				 }
			 }else if(direcao == 's'){
				 switch(oldDirecao){
					 case 'w':IN1=0;IN2=0;IN3=0;IN4=0;break;
					 case 'd':IN1=0;IN2=vel/2;IN3=0;IN4=vel;break;
					 case 'a':IN1=0;IN2=vel;IN3=0;IN4=vel/2;break;
					 default:IN1=0;IN2=vel;IN3=0;IN4=vel;break;
				 }
			 }else if(direcao == 'd'){
				 switch(oldDirecao){
					 case 'w':IN1=vel;IN2=0;IN3=vel/2;IN4=0;break;
					 case 's':IN1=0;IN2=vel/2;IN3=0;IN4=vel;break;
					 case 'd':IN1=vel;IN2=0;IN3=0;IN4=vel;break;
					 case 'a':IN1=0;IN2=0;IN3=0;IN4=0;break;
					 default:IN1=vel;IN2=0;IN3=0;IN4=0;break;
				 }
			 }else if(direcao == 'a'){
				 switch(oldDirecao){
					 case 'w':IN1=vel/2;IN2=0;IN3=vel;IN4=0;break;
					 case 's':IN1=0;IN2=vel;IN3=0;IN4=vel/2;break;
					 case 'a':IN1=0;IN2=vel;IN3=vel;IN4=0;break;
					 case 'd':IN1=0;IN2=0;IN3=0;IN4=0;break;
					 default:IN1=0;IN2=0;IN3=vel;IN4=0;break;
				 }
			 }else{
				 IN1=0;
				 IN2=0;
				 IN3=0;
				 IN4=0;
			 }
			 oldDirecao = IN1+IN2+IN3+IN4==0?0:direcao;
		 }
		 HAL_GPIO_WritePin(IN1_GPIO_Port, IN1_Pin, IN1);
		 HAL_GPIO_WritePin(IN2_GPIO_Port, IN2_Pin, IN2);

		 HAL_GPIO_WritePin(IN3_GPIO_Port, IN3_Pin, IN3);
		 HAL_GPIO_WritePin(IN4_GPIO_Port, IN4_Pin, IN4);

		 osDelay(200);
	 }*/
  /* USER CODE END StartMovimenta */
}

/* USER CODE BEGIN Header_StartReadUart */
/**
 * @brief Function implementing the readUart thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartReadUart */
void StartReadUart(void *argument)
{
  /* USER CODE BEGIN StartReadUart */
	/* Infinite loop */
	for (;;) {
		//unsigned char dist[4];
		//itoa(oldDirecao,(char *)dist,10);
		//HAL_UART_Transmit(&hlpuart1, dist, sizeof(oldDirecao), 200);

		//sprintf(mostrar,"D: %d, V1: %d, V2: %d \r \n ",distancia,(int)valor1,(int)valor2);
		//HAL_UART_Transmit(&hlpuart1, (uint8_t*)mostrar, sizeof(mostrar), 100);
	}
  /* USER CODE END StartReadUart */
}

/* USER CODE BEGIN Header_StartTriger */
/**
 * @brief Function implementing the Triger thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartTriger */
void StartTriger(void *argument)
{
  /* USER CODE BEGIN StartTriger */

	/* Infinite loop */
	for (;;) {
		//HAL_TIM_IC_Start_IT(&htim1, TIM_CHANNEL_1);
		//__HAL_TIM_ENABLE_IT(&htim1, TIM_IT_CC1);

		//HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
		//htim2.Instance->CCR1 = 10;
		osDelay(500);
		//HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);
	}
  /* USER CODE END StartTriger */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {
	/*
	if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_4) {
		if (Is_First) {
			valor1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_4);
			Is_First = 0;
			__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_4,TIM_INPUTCHANNELPOLARITY_FALLING);
		} else {
			valor2 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_4);
			__HAL_TIM_SET_COUNTER(htim, 0);

			diferenca = valor2 > valor1 ? valor2 - valor1 : valor1 - valor2;

			distancia = (diferenca * 0.034) / 2;

			Is_First = 1;
			__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_4,TIM_INPUTCHANNELPOLARITY_RISING);
			__HAL_TIM_DISABLE_IT(htim, TIM_IT_CC4);
		}
	}*/
}

/* USER CODE END Application */

