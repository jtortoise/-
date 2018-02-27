#ifndef __MOTOR_H
#define __MOTOR_H

#include "stm32f1xx_hal.h"

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim3;

#define MOTOR_PWM_MAX_WIDTH __HAL_TIM_GET_AUTORELOAD(&htim1)

void MotorPWMStart(void);
void MotorSetPWM(int motorA, int motorB, int motorC);

#endif //__MOTOR_H
