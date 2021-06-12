#ifndef  __HASHTABLE_H
#define	 __HASHTABLE_H

#include "../type/type.h"

typedef struct _NODE Node;

typedef struct _Value_Info{
	int size;
	char *buf;
	OFFSET_TYPE offset;
}VI;

struct _NODE{
	char *key;
	VI *value;
	Node *next;
};

typedef struct _HASHTABLE{
	Node* array[HASHTABLE_ARRAY_SIZE];
}HashTable;


typedef struct _QUEUE_VALUE QValue;

struct _QUEUE_VALUE{
	char *value;
	VI *value_info;
};

typedef struct _QUEUE_NODE QNode;
 struct _QUEUE_NODE{
	QValue value;
	QNode *next;
};

typedef struct _QUEUE{
	QNode *front;
	QNode *rear;
}Queue;

void index_init(HashTable *hashtable);
void index_insert(const char *key, VI *value, HashTable *hashtable);
void index_remove(char *key, HashTable *hashtable);
Queue* index_lookup(const char *key, HashTable *hashtable);
void index_print(HashTable *hashtable);
void index_free(HashTable *hashtable);

#endif

