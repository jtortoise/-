#include "hashmap.h"

int HashForInt(hashmap* obj, int key)
{
    if(key < 0)
        return ((-key)%(obj->size));

    return (key%(obj->size));
}

hashmap* HashmapInit(int size)
{
    struct _hashmap_table* obj = (struct _hashmap_table*)malloc(sizeof(struct _hashmap_table));
    if(obj == NULL)
        return NULL;

    obj->size = size;
    obj->list = (struct _hashmap_node**)calloc(size, sizeof(struct _hashmap_node*));

    return obj;
}

void HashmapDestory(hashmap* obj)
{
    int i=0;
    //循环释放各对键值
    for(i=0; i<obj->size; i++)
    {
        if(obj->list[i] != NULL)
            free(obj->list[i]);
    }

    free(obj->list);
    free(obj);
}

void* HashmapPut(hashmap* obj, int key, void *value)
{
    int hash=0;
    struct _hashmap_node* node=NULL;

    if(obj == NULL)
        return NULL;

    hash = HashForInt(obj, key);
    if(obj->list[hash] != NULL)
        return NULL;

    node = (struct _hashmap_node*)malloc(sizeof(struct _hashmap_node));
    if(node == NULL)
        return NULL;

    node->key = key;
    node->value = value;

    obj->list[hash] = node;
    return (obj->list[hash]);
}

void* HashmapGet(hashmap* obj, int key)
{
    int hash = HashForInt(obj, key);
    struct _hashmap_node* node = obj->list[hash];
    if(node == NULL || node->key != key)
        return NULL;

    return node->value;
}
