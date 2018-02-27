#ifndef __PACKET_FSM_H
#define __PACKET_FSM_H

#include "fsm.h"
#include <stdint.h>

//#define PACKET_FSM_USART1_DEBUG
#define PACKET_MAX_SIZE 255

typedef struct fsm_object Fsm;
typedef struct _Packet
{
	uint8_t length;
	uint8_t *data;
}Packet;

Packet* PacketInit(uint8_t length);
void PacketDestory(Packet* obj);

Fsm* PacketFsmInit(uint8_t packetBufSize);
void PacketFsmDestory(Fsm* obj);
void PacketFsmDriver(Fsm* obj, uint8_t event);

/**
* ���ص���Packet��ָ��,��ȡ��Packet�����ݺ�,�������PacketDestory()�ͷ��ڴ�,�����ڴ�й©
*/
Packet* PacketFsmPopPacket(Fsm* obj);
uint8_t PacketFsmPacketSize(Fsm* obj);

uint32_t PacketFsmGetCorrect(Fsm* obj);
uint32_t PacketFsmGetError(Fsm* obj);

#endif //__PACKET_FSM_H
