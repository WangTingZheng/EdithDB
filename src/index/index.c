#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "index.h"
#include "../log/log.h"
#include "../sync/sync.h"

Index* hashtable;
off_t noff;

int persis_fd;
int log_fd;
int pos;

unsigned int hash(const char *key){
    unsigned int seed = 131; // 31 131 1313 13131 131313 etc..
    unsigned int hash = 0;
    while (*key){
        hash = hash * seed + (*key++);
    }
    return (hash & 0x7FFFFFFF);
}


void  index_create(char *filename){
    log_fd = log_set_filename(filename);
    hashtable = (Index *)malloc(sizeof(Index));
	memset(hashtable->array, 0, sizeof(Node *) * INDEX_ARRAY_SIZE);
}

VI* index_get(char* key){
	VI* vi = NULL;
	unsigned int pos = hash(key) % INDEX_ARRAY_SIZE;
	Node *node = hashtable->array[pos];
	while(node){
		if(strcmp(key, node->key) == 0){
			vi = node->info;
			break;
		}
		node = node->next;
	}

	if(!vi)
		log_append(log_fd, "index.c", "index_get", "There is no key in the hashtable.");
	return vi;
}

boolean index_contains(char *key){
	if(index_get(key))
		return true;
	return false;
}

void  index_put(char *key, VI *info){
	if(!index_contains(key)){
		unsigned int pos = hash(key) % INDEX_ARRAY_SIZE;
		Node *node = (Node *)malloc(sizeof(Node));

        node->key  = (char *)malloc(sizeof(char) * (strlen(key) + 1));
        node->info  = (VI *)malloc(sizeof(VI));

		memcpy(node->key, key, sizeof(key));
        memcpy(node->info, info, sizeof(VI));
		node->next = hashtable->array[pos];

        hashtable->array[pos] = node;
	}else{
		log_append(log_fd, "hashtable.c", "index_put", "key has already existed in hashtable.");
	}
}

void index_delete(char *key){
	unsigned int pos = hash(key) % INDEX_ARRAY_SIZE;
	Node *temp;
	Node *node = hashtable->array[pos];
	
	while(node){
		if(strcmp(node->key, key) == 0){
            hashtable->array[pos] = node->next;
			free(node->key);
			free(node->info);
			free(node);
		}else{
			break;
		}
		node = node->next;
	}
	if(node) {
        while(node->next){
            if(strcmp(node->next->key, key) == 0){
                temp = node->next;
                node->next = temp;
                free(temp->key);
                free(temp->info);
                free(temp);
            }
            node = node->next;
        }
	}
}

boolean index_isEmpty(){
	for(int i=0; i< INDEX_ARRAY_SIZE; i++){
		if(hashtable->array[pos])
			return false;
	}
	return true;
}

int index_size(){
	int ret = 0;
	Node *node;
	
	for(int i=0; i<INDEX_ARRAY_SIZE; i++){
		node = hashtable->array[i];
		while(node){
			ret++;
			node = node->next;
		}
	}
	
	return ret;
}

void index_print(){
	Node *node;
	for(int i=0; i< INDEX_ARRAY_SIZE; i++){
		node = hashtable->array[i];
		while(node){
			printf("key = %s size = %jd buf = %d offset = %jd  ", node->key, node->info->size, node->info->buf, node->info->offset);
			node = node->next;
		}
		printf("\n");
	}
}

void index_free(){
	Node *node;
	Node *temp;
	for(int i=0; i<INDEX_ARRAY_SIZE; i++){
		node = hashtable->array[i];
		while(node){
			temp = node->next;
			free(node->key);
			free(node->info);
			free(node);
			node = temp;
		}
	}
}


void index_set_path(char *path){
    persis_fd = sync_init(path);
	noff = 0;
	pos  = 0;
}

FNode *NodeToFNode(Node *node, int pos){
	FNode *fnode = (FNode *)malloc(sizeof(FNode));
	
	if(!node){
		log_append(log_fd,"index.c", "NodeToFNode", "node is null.");
	}else if(!(node->key)){
		log_append(log_fd,"index.c", "NodeToFNode", "node key is null.");
	}
	else{
		fnode->size   = node->info->size;
		fnode->pos    = pos;
		fnode->ksize  = strlen(node->key);
		fnode->offset = node->info->offset;
	}
	return fnode;
}

Node *FNodeToNode(FNode *fnode, char *key){
	Node *node = (Node *)malloc(sizeof(Node));
	VI *info   = (VI *)malloc(sizeof(VI));
	
	info->size   = fnode->size;
	info->buf    = NULL;
	info->offset = fnode->offset;
	
	memcpy(node->key, key, strlen(key));
	node->info = info;
	node->next = NULL;
	
	return node;
}

char *FNodeToString(FNode *fnode){
	char *string = (char *)malloc(sizeof(FNode));
	FNode *tfnode;
	
	tfnode  = (FNode *)string;
	*tfnode = *fnode;
	
	return string;
}

FNode *StringToFNode(char *string){
	return (FNode *)string;
}

void persis_node(Node *node, int pos){
	FNode *fnode;
	
	fnode = NodeToFNode(node, pos);
	
	if(!fnode){
		sync_write(persis_fd, FNodeToString(fnode), sizeof(FNode));
		sync_write(persis_fd, node->key, fnode->ksize);
	}
}

Node* load_node(off_t offset){
	char *string = (char *)malloc(sizeof(FNode));
	char *key    = NULL;
	off_t size;
	
	FNode *fnode;
	
	sync_read(persis_fd, string, sizeof(FNode), offset);
	fnode = StringToFNode(string);
	
	size = fnode->ksize;
	pos = fnode->pos;
	key = (char *)malloc(size);
	
	sync_read(persis_fd, key, size, offset + sizeof(FNode));
	noff = size + sizeof(FNode);
	
	return FNodeToNode(fnode, key);
}

void index_persis(){
	Node *node;
	
	for(int i=0; i< INDEX_ARRAY_SIZE; i++){
		node = hashtable->array[i];
		while(node){
			persis_node(node, i);
			node = node->next;
		}
	}
}
void index_load(Index *index){
	Node *node;
	Node *rnode;
	off_t noffset = sync_size(persis_fd);
	off_t offset;
	
	while(offset < noffset){
		rnode = load_node(offset);
		offset += noff;
		
		node = index->array[pos];
		if(!node){
			rnode->next = node;
			index->array[pos] = rnode;
		}else{
			index->array[pos] = node;
		}
	}
}


/*
	1.BKDR散列是如何设计的
	2.适合散列表的散列函数应该怎么设计
*/