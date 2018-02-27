#ifndef __ENGINNER_H
#define __ENGINNER_H

#include "stm32f1xx_hal.h"
#include "packet_fsm.h"
#include "protocol_processor.h"
#include "motor.h"

enum EnginnerOrder
{
	MOTOR_SET_PWM,
	ORDER_MAX_SIZE
};

extern Fsm* packetReceiver;
extern ProtocolProcessor avgController;
extern UART_HandleTypeDef huart3;

void enginnerInit(void);
void enginnerProcess(void);
void HAL_UART_RxCpltCallback (UART_HandleTypeDef *huart);


#endif //__ENGINNER_H
