#include "protocol_processor.h"
#include "hashmap.h"

typedef hashmap PP_HandleType;

ProtocolProcessor PP_Init(uint8_t orderSize)
{
	PP_HandleType* ppObj = HashmapInit(orderSize);
	
	return ppObj;
}

void PP_Destory(ProtocolProcessor ppObj)
{
	HashmapDestory((PP_HandleType*)ppObj);
}

int PP_AddOrder(ProtocolProcessor ppObj, uint8_t order, void (*OrderProcessFun)(Packet* orderBuf))
{
	HashmapPut((PP_HandleType*)ppObj, (int)order, (void*)OrderProcessFun);
	
	return 1;
}

int PP_ProcessOrder(ProtocolProcessor ppObj, Packet* packetObj)
{
	void (*OrderProcessFun)(Packet*);
	uint8_t cmd = packetObj->data[0];
	
	OrderProcessFun = (void(*)(Packet*))HashmapGet((PP_HandleType*)ppObj, (int)cmd);
	if(OrderProcessFun == NULL)
		return 0;
	
	OrderProcessFun(packetObj);
	return 1;
}


