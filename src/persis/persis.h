#ifndef __PERSIS_H
#define __PERSIS_H
#include "../type/type.h"
#include "../index/index.h"
#include "../persis/persis.h"

#define FNode_Size sizeof(FNode) 

typedef struct _FLUSH_NODE{
	int size;
	int key_size;
	int pos;
	OFFSET_TYPE offset;
}FNode;

void persis_sync_init(char *filename);
void persis_sync_save(HashTable *hashtable);
void persis_sync_load(HashTable *hashtable);
#endif