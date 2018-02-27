#ifndef __AVG_QUEUE_H
#define __AVG_QUEUE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef void (*InfoDestory)(void *info);
typedef struct _avg_queue_node
{
	void* info;
	InfoDestory infoDestoryCallback; 
	struct _avg_queue_node* prev;
}AVG_QueueNode;

typedef struct _avg_queue
{
	AVG_QueueNode* head;
	AVG_QueueNode* tail;
	uint8_t size;
	uint8_t limit;
}AVG_Queue;

//���нڵ��ʼ���Լ�����
AVG_QueueNode* AVG_QueueNodeInit(void *info, void (*InfoDestory)(void *info));
void AVG_QueueNodeDestory(AVG_QueueNode* obj);

//���г�ʼ���Լ�����
AVG_Queue* AVG_QueueInit(uint8_t limit);
void AVG_QueueDestory(AVG_Queue* obj);

//����״̬
uint8_t AVG_QueueSize(AVG_Queue* obj);

//���нڵ����
void AVG_QueuePushInfo(AVG_Queue* obj, void* info, void (*InfoDestory)(void *info));
void* AVG_QueuePopInfo(AVG_Queue* obj);

#endif //__AVG_QUEUE_H
