/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32g4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define BTN1_Pin GPIO_PIN_13
#define BTN1_GPIO_Port GPIOC
#define ECHO_Pin GPIO_PIN_0
#define ECHO_GPIO_Port GPIOC
#define LED1_Pin GPIO_PIN_5
#define LED1_GPIO_Port GPIOA
#define S3_Pin GPIO_PIN_5
#define S3_GPIO_Port GPIOC
#define S4_Pin GPIO_PIN_6
#define S4_GPIO_Port GPIOC
#define IN3_Pin GPIO_PIN_7
#define IN3_GPIO_Port GPIOC
#define CLP_Pin GPIO_PIN_8
#define CLP_GPIO_Port GPIOC
#define NEAR_Pin GPIO_PIN_9
#define NEAR_GPIO_Port GPIOC
#define IN1_Pin GPIO_PIN_8
#define IN1_GPIO_Port GPIOA
#define IN2_Pin GPIO_PIN_9
#define IN2_GPIO_Port GPIOA
#define Encoder_E_Pin GPIO_PIN_11
#define Encoder_E_GPIO_Port GPIOA
#define S2_Pin GPIO_PIN_12
#define S2_GPIO_Port GPIOA
#define TRIG_Pin GPIO_PIN_15
#define TRIG_GPIO_Port GPIOA
#define Encoder_D_Pin GPIO_PIN_12
#define Encoder_D_GPIO_Port GPIOC
#define Buzzer_Pin GPIO_PIN_2
#define Buzzer_GPIO_Port GPIOD
#define ENA_Pin GPIO_PIN_4
#define ENA_GPIO_Port GPIOB
#define ENB_Pin GPIO_PIN_5
#define ENB_GPIO_Port GPIOB
#define IN4_Pin GPIO_PIN_6
#define IN4_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
