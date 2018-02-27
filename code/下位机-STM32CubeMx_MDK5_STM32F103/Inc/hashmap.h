#ifndef __HASHMAP_H
#define __HASHMAP_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct _hashmap_node
{
    void* value;
    int key;
}hashmap_node;

typedef struct _hashmap_table
{
    int size;
    struct _hashmap_node** list;
}hashmap;

int HashForInt(hashmap* obj, int key);

hashmap* HashmapInit(int size);
void HashmapDestory(hashmap* obj);

void* HashmapPut(hashmap* obj, int key, void *value);
void* HashmapGet(hashmap* obj, int key);


#endif //__HASHMAP_H
