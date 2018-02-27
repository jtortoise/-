#ifndef __PROTOCOL_PROCESSCOR_H
#define __PROTOCOL_PROCESSCOR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "packet_fsm.h"

typedef void* ProtocolProcessor;

ProtocolProcessor PP_Init(uint8_t orderSize);
void PP_Destory(ProtocolProcessor ppObj);

int PP_AddOrder(ProtocolProcessor ppObj, uint8_t order, void (*OrderProcessFun)(Packet* orderBuf));
int PP_ProcessOrder(ProtocolProcessor ppObj, Packet* orderBuf);

#endif //__PROTOCOL_PROCESSCOR_H
