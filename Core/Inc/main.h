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
#define Echo_Pin GPIO_PIN_2
#define Echo_GPIO_Port GPIOC
#define Encoder_E_Pin GPIO_PIN_0
#define Encoder_E_GPIO_Port GPIOA
#define LED1_Pin GPIO_PIN_5
#define LED1_GPIO_Port GPIOA
#define HC05_TX_Pin GPIO_PIN_4
#define HC05_TX_GPIO_Port GPIOC
#define S3_Pin GPIO_PIN_5
#define S3_GPIO_Port GPIOC
#define Encoder_D_Pin GPIO_PIN_12
#define Encoder_D_GPIO_Port GPIOB
#define S4_Pin GPIO_PIN_6
#define S4_GPIO_Port GPIOC
#define IN3_Pin GPIO_PIN_7
#define IN3_GPIO_Port GPIOC
#define NEAR_Pin GPIO_PIN_9
#define NEAR_GPIO_Port GPIOC
#define IN1_Pin GPIO_PIN_8
#define IN1_GPIO_Port GPIOA
#define IN2_Pin GPIO_PIN_9
#define IN2_GPIO_Port GPIOA
#define HC05_RX_Pin GPIO_PIN_10
#define HC05_RX_GPIO_Port GPIOA
#define S2_Pin GPIO_PIN_12
#define S2_GPIO_Port GPIOA
#define Trig_Pin GPIO_PIN_15
#define Trig_GPIO_Port GPIOA
#define Buzzer_Pin GPIO_PIN_2
#define Buzzer_GPIO_Port GPIOD
#define HC05_STATE_Pin GPIO_PIN_3
#define HC05_STATE_GPIO_Port GPIOB
#define PWM_1_Pin GPIO_PIN_4
#define PWM_1_GPIO_Port GPIOB
#define PWM_2_Pin GPIO_PIN_5
#define PWM_2_GPIO_Port GPIOB
#define IN4_Pin GPIO_PIN_6
#define IN4_GPIO_Port GPIOB
#define HC05_EN_Pin GPIO_PIN_8
#define HC05_EN_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
