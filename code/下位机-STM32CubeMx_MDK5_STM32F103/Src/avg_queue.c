#include "avg_queue.h"

//���нڵ��ʼ��������
AVG_QueueNode* AVG_QueueNodeInit(void *info, void (*InfoDestory)(void *info))
{
	AVG_QueueNode* obj = (AVG_QueueNode*)malloc(sizeof(AVG_QueueNode));
	
	//���нڵ������ڴ�ʧ��
	if(obj == NULL)
		return NULL;
	
	obj->info = info;
	obj->infoDestoryCallback = InfoDestory;
	obj->prev = NULL;
	
	return obj;
}

void AVG_QueueNodeDestory(AVG_QueueNode* obj)
{
	free(obj);
}

//���г�ʼ��������
AVG_Queue* AVG_QueueInit(uint8_t limit)
{
	AVG_Queue* obj = (AVG_Queue*)malloc(sizeof(AVG_Queue));
	
	//�ڴ�����ʧ��
	if(obj == NULL)
		return NULL;
	
	//��Ա������ʼ��
	obj->limit = limit;
	obj->size = 0;
	obj->head = NULL;
	obj->tail = NULL;
	
	return obj;
}
void AVG_QueueDestory(AVG_Queue* obj)
{	
	//ѭ���ͷŵ������ڲ������нڵ��ڴ�
	while(AVG_QueuePopInfo(obj) != NULL);
	//�ͷŶ����ڴ�
	free(obj);
	
}

//����״̬
uint8_t AVG_QueueSize(AVG_Queue* obj)
{
	return obj->size;
}

AVG_QueueNode* AVG_QueueGetHead(AVG_Queue* obj)
{
	return obj->head;
}

//���в���
void AVG_QueuePushInfo(AVG_Queue* obj, void* info, void (*InfoDestory)(void* info))
{
	//���������Ч����Ϣ
	if(obj==NULL || info==NULL)
		return;
	
	AVG_QueueNode* node = AVG_QueueNodeInit(info, InfoDestory);
	AVG_QueueNode* temp;
	
	if(obj->size >= obj->limit)
	{
		//���д�С��������,����������һ���ڵ�
		temp = AVG_QueueGetHead(obj);
		if(temp->infoDestoryCallback != NULL)
			temp->infoDestoryCallback(temp->info);
		AVG_QueuePopInfo(obj);
	}
	
	//����½ڵ�
	if(obj->size == 0)
	{
		//����Ϊ��
		obj->head = node;
		obj->tail = node;
	}
	else
	{
		//���в�Ϊ��
		obj->tail->prev = node;
		obj->tail = node;
	}
	
	obj->size++;
}

void* AVG_QueuePopInfo(AVG_Queue* obj)
{
	AVG_QueueNode* node = NULL;
	void* info;
	
	//��Ч�Ķ��л�������޿��õĽڵ�
	if(obj==NULL || obj->size==0)
		return NULL;
	//ȡ�����е�ͷ�ڵ�
	node = AVG_QueueGetHead(obj);
	obj->head = obj->head->prev;
	obj->size--;
	//����ڵ㸺�ص���Ϣ
	info = node->info;
	//�ͷŽڵ�,�����ڴ�й©
	AVG_QueueNodeDestory(node);
	
	return info;
}
