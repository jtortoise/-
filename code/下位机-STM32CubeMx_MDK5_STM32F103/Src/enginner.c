#include "enginner.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Fsm* packetReceiver = NULL;
uint8_t packetFsmEvent = 0;

ProtocolProcessor avgController = NULL;

void task1(Packet* arg)
{
	int payload[3] = {0};
	int i, j, base=1;
	if(arg == NULL)
		return;
	
	for(i=0; i<3; i++)
	{
		for(j=24; j>=0; j=j-8)
		{
			payload[i] = payload[i] | (arg->data[base++] << j);
		}
	}
	
	MotorSetPWM(payload[0], payload[1], payload[2]);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, !HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_15));
}

void enginnerInit(void)
{
	/* 协议处理器初始化；
	   添加指令对应的处理函数；
	   负责处理接收到的指令。
	*/
	avgController = PP_Init(ORDER_MAX_SIZE+1);
	PP_AddOrder(avgController, (int)MOTOR_SET_PWM, task1);
	
	/* 数据包接收器初始化；
	   解决数据包传输过程中的粘包拆包的问题;；
	   能够完整的接收一个数据包。
	*/
	packetReceiver = PacketFsmInit(10);
	HAL_UART_Receive_IT(&huart3, &packetFsmEvent, 1);

	/* 电机PWM启动；
	   用来控制3个电机,占用了TIM1以及TIM3的CH1~CH3通道共6个PWM通道；
	   3个电机的PWM频率都是10KHz。
	*/
	MotorPWMStart();
	
}

void enginnerProcess(void)
{
	static uint8_t buf[16] = {0};
	static int len;
	static Packet* packetObj = NULL;
	
	
	if(PacketFsmPacketSize(packetReceiver))
	{
//		memset(buf, 0, 16);
//		len = snprintf((char*)buf, 16, "c:%u\te:%u\n", PacketFsmGetCorrect(packetReceiver), PacketFsmGetError(packetReceiver));
//		HAL_UART_Transmit_IT(&huart3, (uint8_t*)buf, len);
//		HAL_Delay(4);	
		packetObj = PacketFsmPopPacket(packetReceiver);
		PP_ProcessOrder(avgController, packetObj);
		PacketDestory(packetObj);
	}

}

void HAL_UART_RxCpltCallback (UART_HandleTypeDef *huart)
{
	if(huart->Instance == USART3)
	{
		PacketFsmDriver(packetReceiver, packetFsmEvent);
		HAL_UART_Receive_IT(huart, &packetFsmEvent, 1);
	}
}
