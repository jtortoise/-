#include "avg_queue.h"

//队列节点初始化和销毁
AVG_QueueNode* AVG_QueueNodeInit(void *info, void (*InfoDestory)(void *info))
{
	AVG_QueueNode* obj = (AVG_QueueNode*)malloc(sizeof(AVG_QueueNode));
	
	//队列节点申请内存失败
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

//队列初始化和销毁
AVG_Queue* AVG_QueueInit(uint8_t limit)
{
	AVG_Queue* obj = (AVG_Queue*)malloc(sizeof(AVG_Queue));
	
	//内存申请失败
	if(obj == NULL)
		return NULL;
	
	//成员变量初始化
	obj->limit = limit;
	obj->size = 0;
	obj->head = NULL;
	obj->tail = NULL;
	
	return obj;
}
void AVG_QueueDestory(AVG_Queue* obj)
{	
	//循环释放掉队列内部的所有节点内存
	while(AVG_QueuePopInfo(obj) != NULL);
	//释放队列内存
	free(obj);
	
}

//队列状态
uint8_t AVG_QueueSize(AVG_Queue* obj)
{
	return obj->size;
}

AVG_QueueNode* AVG_QueueGetHead(AVG_Queue* obj)
{
	return obj->head;
}

//队列操作
void AVG_QueuePushInfo(AVG_Queue* obj, void* info, void (*InfoDestory)(void* info))
{
	//避免添加无效的信息
	if(obj==NULL || info==NULL)
		return;
	
	AVG_QueueNode* node = AVG_QueueNodeInit(info, InfoDestory);
	AVG_QueueNode* temp;
	
	if(obj->size >= obj->limit)
	{
		//队列大小超过限制,弃掉队列中一个节点
		temp = AVG_QueueGetHead(obj);
		if(temp->infoDestoryCallback != NULL)
			temp->infoDestoryCallback(temp->info);
		AVG_QueuePopInfo(obj);
	}
	
	//添加新节点
	if(obj->size == 0)
	{
		//队列为空
		obj->head = node;
		obj->tail = node;
	}
	else
	{
		//队列不为空
		obj->tail->prev = node;
		obj->tail = node;
	}
	
	obj->size++;
}

void* AVG_QueuePopInfo(AVG_Queue* obj)
{
	AVG_QueueNode* node = NULL;
	void* info;
	
	//无效的队列或队列中无可用的节点
	if(obj==NULL || obj->size==0)
		return NULL;
	//取出队列的头节点
	node = AVG_QueueGetHead(obj);
	obj->head = obj->head->prev;
	obj->size--;
	//保存节点负载的信息
	info = node->info;
	//释放节点,避免内存泄漏
	AVG_QueueNodeDestory(node);
	
	return info;
}
