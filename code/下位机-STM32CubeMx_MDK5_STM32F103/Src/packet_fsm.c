#include "packet_fsm.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stm32f1xx_hal.h"
#include "avg_queue.h"

//#define PACKET_FSM_USART1_DEBUG

typedef struct _PacketFsmHandleType
{
	Packet* packetObj;
	AVG_Queue* packetQueue;
	uint32_t correctCount;
	uint32_t errorCount;
	uint32_t lastTimepoint;
	uint8_t packetLength;
	uint8_t packetCounter;
	uint8_t packetBcc;

}PacketFsmHandleType;

PacketFsmHandleType* PacketFsmHandleTypeInit(uint8_t queueLimit)
{
	PacketFsmHandleType* hpacket = (PacketFsmHandleType*)malloc(sizeof(PacketFsmHandleType));
	hpacket->packetQueue = AVG_QueueInit(queueLimit);
	hpacket->lastTimepoint = 0;
	hpacket->packetBcc = 0;
	hpacket->packetCounter = 0;
	hpacket->packetLength = 0;
	hpacket->correctCount = 0;
	hpacket->errorCount = 0;
	hpacket->packetObj = NULL;
	
	return hpacket;
}

void PacketFsmHandleTypeDestory(PacketFsmHandleType* obj)
{
	AVG_QueueDestory(obj->packetQueue);
	free(obj);
}

void PacketFsmHandleTypeReset(PacketFsmHandleType* obj)
{
	obj->packetBcc = 0;
	obj->packetCounter = 0;
	obj->packetLength = 0;
	obj->packetObj = NULL;
}

Packet* PacketInit(uint8_t length)
{
	Packet* obj = (Packet*)malloc(sizeof(Packet));
	
	obj->length = length;
	obj->data = (uint8_t*)malloc(length);
	return obj;	
}

void PacketDestory(Packet* obj)
{
	if(obj == NULL)
		return;
	free(obj->data);
	free(obj);
}

void PacketDestoryCallback(void* obj)
{
	PacketDestory((Packet*)obj);
}

#ifdef PACKET_FSM_USART1_DEBUG
extern UART_HandleTypeDef huart3;
static const int DEBUG_BUFFER_SIZE = 256;
char debugBuffer[DEBUG_BUFFER_SIZE] = {0};
int debugBufSize = 0;
#endif //PACKET_FSM_USART1_DEBUG

//前向声明
void BeginStateCallback(Fsm* obj, int parNum, void **arg);
void LengthStateCallback(Fsm* obj, int parNum, void **arg);
void DataStateCallback(Fsm* obj, int parNum, void **arg);
void BccStateCallback(Fsm* obj, int parNum, void **arg);

Fsm* PacketFsmInit(uint8_t packetBufSize)
{
	//初始化PacketFsm对象
	Fsm* packetFsm = (Fsm*)malloc(sizeof(Fsm));
	fsm_init(packetFsm);
	
	//初始化Fsm转换过程中共享的变量
	PacketFsmHandleType* hpacket = PacketFsmHandleTypeInit(packetBufSize);
	packetFsm->fsm_handle = (void*)hpacket;
	
	//向PacketFsm添加状态
	fsm_default(packetFsm, BeginStateCallback);
	fsm_add(packetFsm, "LengthState", LengthStateCallback);
	fsm_add(packetFsm, "DataState", DataStateCallback);
	fsm_add(packetFsm, "BccState", BccStateCallback);
	
	return packetFsm;
}

void PacketFsmDestory(Fsm* obj)
{
	if(obj == NULL)
		return;
	//释放状态机内部所有状态节点
	fsm_terminate(obj);
	//释放状态机Callback共享的资源
	PacketFsmHandleTypeDestory((PacketFsmHandleType*)(obj->fsm_handle));
	//释放状态机
	free(obj);
}

void PacketFsmDriver(Fsm* obj, uint8_t event)
{
	obj->fsm_arg_num = 1;
	obj->fsm_arg_value = (void*)&event;
	fsm_next_state(obj);
}

Packet* PacketFsmPopPacket(Fsm* obj)
{
	return (Packet*)AVG_QueuePopInfo(((PacketFsmHandleType*)(obj->fsm_handle))->packetQueue);
}

uint8_t PacketFsmPacketSize(Fsm* obj)
{
	return AVG_QueueSize(((PacketFsmHandleType*)(obj->fsm_handle))->packetQueue);
}

uint32_t PacketFsmGetCorrect(Fsm* obj)
{
	return ((PacketFsmHandleType*)(obj->fsm_handle))->correctCount;
}
uint32_t PacketFsmGetError(Fsm* obj)
{
	return ((PacketFsmHandleType*)(obj->fsm_handle))->errorCount;
}

static const uint32_t TIME_OUT = 100000;
int PacketFsmStateTimeout(Fsm *obj)
{
	uint32_t interval = HAL_GetTick() - ((PacketFsmHandleType*)(obj->fsm_handle))->lastTimepoint;
	if(interval >= TIME_OUT)
		return 1;
	return 0;
}

void PacketFsmStateTpUpdate(Fsm *obj)
{
	((PacketFsmHandleType*)(obj->fsm_handle))->lastTimepoint = HAL_GetTick();
}

static const uint8_t PACKET_START_SYMBOL = 0x55;
void BeginStateCallback(Fsm* obj, int parNum, void **arg)
{
	uint8_t event = *((uint8_t*)arg);
	
	//数据包起始标志判断正确
	if(event == PACKET_START_SYMBOL)
	{
		//初始化状态机接收数据包过程中需要的变量
		PacketFsmHandleTypeReset((PacketFsmHandleType*)obj->fsm_handle);
		//转到下一个状态
		fsm_to_state(obj, "LengthState", 0, NULL);
		PacketFsmStateTpUpdate(obj);
	}
	
#ifdef PACKET_FSM_USART1_DEBUG
	memset((void*)debugBuffer, 0, DEBUG_BUFFER_SIZE);
	debugBufSize = snprintf(debugBuffer, DEBUG_BUFFER_SIZE, "BeginState:0x%.2X\n", event);
	//HAL_UART_Transmit_IT(&huart3, (uint8_t*)debugBuffer, debugBufSize);
	HAL_UART_Transmit(&huart3, (uint8_t*)debugBuffer, debugBufSize, 4);
#endif //PACKET_FSM_USART1_DEBUG
}

void LengthStateCallback(Fsm* obj, int parNum, void **arg)
{
	uint8_t event = *((uint8_t*)arg);
	
	if(PacketFsmStateTimeout(obj))
	{
		((PacketFsmHandleType*)(obj->fsm_handle))->errorCount++;
		fsm_to_state(obj, "default", 0, NULL);
		PacketFsmDriver(obj, event);
		return;
	}
	
	((PacketFsmHandleType*)(obj->fsm_handle))->packetLength = event;
	((PacketFsmHandleType*)(obj->fsm_handle))->packetObj = PacketInit(event);
	
	fsm_to_state(obj, "DataState", 0, NULL);
	PacketFsmStateTpUpdate(obj);
	
#ifdef PACKET_FSM_USART1_DEBUG
	memset((void*)debugBuffer, 0, DEBUG_BUFFER_SIZE);
	debugBufSize = snprintf(debugBuffer, DEBUG_BUFFER_SIZE, "LengthState:0x%.2X\n", event);
	//HAL_UART_Transmit_IT(&huart3, (uint8_t*)debugBuffer, debugBufSize);
	HAL_UART_Transmit(&huart3, (uint8_t*)debugBuffer, debugBufSize, 4);
#endif //PACKET_FSM_USART1_DEBUG
}

void DataStateCallback(Fsm* obj, int parNum, void **arg)
{
	uint8_t event = *((uint8_t*)arg);
	PacketFsmHandleType* tempHPacket = (PacketFsmHandleType*)(obj->fsm_handle);
	
	if(PacketFsmStateTimeout(obj))
	{
		tempHPacket->errorCount++;
		PacketDestory(tempHPacket->packetObj);
		fsm_to_state(obj, "default", 0, NULL);
		PacketFsmDriver(obj, event);
		return;
	}
	
	tempHPacket->packetBcc ^= event;
	tempHPacket->packetObj->data[tempHPacket->packetCounter] = event;
	tempHPacket->packetCounter++;
	
	if(tempHPacket->packetLength == tempHPacket->packetCounter)
	{
		fsm_to_state(obj, "BccState", 0, NULL);
	}
	PacketFsmStateTpUpdate(obj);
	
#ifdef PACKET_FSM_USART1_DEBUG
	memset((void*)debugBuffer, 0, DEBUG_BUFFER_SIZE);
	debugBufSize = snprintf(debugBuffer, DEBUG_BUFFER_SIZE, "DataState:0x%.2X(%u,%u)\n", event, tempHPacket->packetCounter, tempHPacket->packetLength);
	//HAL_UART_Transmit_IT(&huart3, (uint8_t*)debugBuffer, debugBufSize);
	HAL_UART_Transmit(&huart3, (uint8_t*)debugBuffer, debugBufSize, 4);
#endif //PACKET_FSM_USART1_DEBUG
}

void BccStateCallback(Fsm* obj, int parNum, void **arg)
{
	uint8_t event = *((uint8_t*)arg);
	PacketFsmHandleType* tempHPacket = (PacketFsmHandleType*)(obj->fsm_handle);
	
	if(PacketFsmStateTimeout(obj))
	{
		tempHPacket->errorCount++;
		PacketDestory(tempHPacket->packetObj);
		fsm_to_state(obj, "default", 0, NULL);
		PacketFsmDriver(obj, event);
		return;
	}
	
	if(tempHPacket->packetBcc == event)
	{
		//添加数据包到队列中
		AVG_QueuePushInfo(tempHPacket->packetQueue, (void*)(tempHPacket->packetObj), PacketDestoryCallback);
		tempHPacket->correctCount++;
	}
	else
	{
		//释放无效的包
		PacketDestory(tempHPacket->packetObj);
		tempHPacket->packetObj = NULL;
		tempHPacket->errorCount++;
	}
	
	fsm_to_state(obj, "default", 0, NULL);
	
#ifdef PACKET_FSM_USART1_DEBUG
	if(tempHPacket->packetObj != NULL)
	{
		memset((void*)debugBuffer, 0, DEBUG_BUFFER_SIZE);
		debugBufSize = snprintf(debugBuffer, DEBUG_BUFFER_SIZE, "PacketFsm Get Packet(%u,%u):0x%.2X 0x%.2X\n",
								AVG_QueueSize(tempHPacket->packetQueue), tempHPacket->packetObj->length, tempHPacket->packetObj->data[0], tempHPacket->packetObj->data[1]);
		//HAL_UART_Transmit_IT(&huart3, (uint8_t*)debugBuffer, debugBufSize);
		HAL_UART_Transmit(&huart3, (uint8_t*)debugBuffer, debugBufSize, 4);
	}
	else
	{
		memset((void*)debugBuffer, 0, DEBUG_BUFFER_SIZE);
		debugBufSize = snprintf(debugBuffer, DEBUG_BUFFER_SIZE, "BccState:0x%.2X(0x%.2X)\n", event, tempHPacket->packetBcc);
		//HAL_UART_Transmit_IT(&huart3, (uint8_t*)debugBuffer, debugBufSize);
		HAL_UART_Transmit(&huart3, (uint8_t*)debugBuffer, debugBufSize, 4);
	}
#endif //PACKET_FSM_USART1_DEBUG
}
