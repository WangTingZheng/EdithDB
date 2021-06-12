#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "index.h"
#include "../queue/queue.h"

HashTable hashtable;

void index_init(HashTable *hashtable){
	memset(hashtable->array, 0, sizeof(Node*)*HASHTABLE_ARRAY_SIZE);
}

//BKDR hash function from <The C Programming Language>
unsigned int hashtable_hash_str(const char *key)
{
    unsigned int seed = 131; // 31 131 1313 13131 131313 etc..
    unsigned int hash = 0;
    while (*key)
    {
        hash = hash * seed + (*key++);
    }
    return (hash & 0x7FFFFFFF);
}

void index_insert(const char *key, VI *value, HashTable *hashtable){
	unsigned int pos = hashtable_hash_str(key) % HASHTABLE_ARRAY_SIZE;

	Node *pNewNode = (Node*)malloc(sizeof(Node));
	memset(pNewNode, 0, sizeof(Node));
	pNewNode->key = (char*)malloc(sizeof(char)*(strlen(key)+1));
	strcpy(pNewNode->key, key);
	pNewNode->value = value;

	pNewNode->next = hashtable->array[pos];
	hashtable->array[pos] = pNewNode;
}

void index_remove(char *key, HashTable *hashtable){
	unsigned int pos = hashtable_hash_str(key) % HASHTABLE_ARRAY_SIZE;
	if(hashtable->array[pos]){
		Node *pHead = hashtable->array[pos];
		Node *pNow;
		Node *pRemove = NULL;
		
		while(strcmp(key, pHead->key) == 0){
			hashtable->array[pos] = pHead->next;
			free(pHead->key);
			//free(pHead);
			pHead = hashtable->array[pos];
			if(pHead == NULL)
				return;
		}
		
		pNow = pHead;
		
		while(pNow->next){
			if(strcmp(key, pNow->key) == 0){
				pRemove = pNow->next;
				pNow->next = pRemove->next;
				free(pRemove->key);
				free(pRemove->value);	
				free(pRemove);
			}
		}
		return;
	}
}

Queue *index_lookup(const char *key, HashTable *hashtable){
	unsigned int pos = hashtable_hash_str(key) % HASHTABLE_ARRAY_SIZE;
	Queue *queue = queue_init();
	
	if(hashtable->array[pos]){
		Node *pHead = hashtable->array[pos];
		while(pHead){
			if(strcmp(key, pHead->key) == 0){
				queue_push_vi(queue, pHead->value);
			}
			pHead = pHead->next;
		}
	}
	return queue;
}

void index_print(HashTable *hashtable){
	for(int i=0; i< HASHTABLE_ARRAY_SIZE;i++){
		Node *pHead = (*hashtable).array[i];
		printf("%d=>", i);
		while(pHead){
			printf(" key: %s, value info: ", pHead->key);
			printf("size = %d, buf = %p, offset = %lld", pHead->value->size, &(pHead->value->buf), pHead->value->offset);
			pHead = pHead->next;
		}
		printf("\n");
	}
}


void index_free(HashTable *hashtable){
	Node *pHead;
	Node *pTemp;
	for(int i=0; i< HASHTABLE_ARRAY_SIZE ;i++){
		if(hashtable->array[i]){
			pHead = hashtable->array[i];
			while(pHead){
				pTemp = pHead;
				pHead = pHead->next;			
				
				if(pTemp){
					free(pTemp->key);
					free(pTemp->value);
					free(pTemp);
				}
			}
			hashtable->array[i] = NULL;
		}
	}
}