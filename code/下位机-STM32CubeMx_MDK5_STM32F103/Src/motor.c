#include "motor.h"

void MotorPWMStart()
{
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);
}

void MotorSetPWM(int motorA, int motorB, int motorC)
{
	if(motorA >= 0)
	{
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, MOTOR_PWM_MAX_WIDTH);
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, MOTOR_PWM_MAX_WIDTH - motorA);
	}
	else
	{
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, MOTOR_PWM_MAX_WIDTH);
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, MOTOR_PWM_MAX_WIDTH + motorA);
	}
	
	if(motorB >= 0)
	{
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, MOTOR_PWM_MAX_WIDTH);
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, MOTOR_PWM_MAX_WIDTH - motorB);
	}
	else
	{
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, MOTOR_PWM_MAX_WIDTH);
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, MOTOR_PWM_MAX_WIDTH + motorB);
	}
	
	if(motorC >= 0)
	{
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, MOTOR_PWM_MAX_WIDTH);
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, MOTOR_PWM_MAX_WIDTH - motorC);
	}
	else
	{
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, MOTOR_PWM_MAX_WIDTH);
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, MOTOR_PWM_MAX_WIDTH + motorC);
	}
}
