#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../sync/sync.h"
#include "persis.h"

int hashtable_fd;
OFFSET_TYPE flush_offset;
OFFSET_TYPE move_offset;
int pos;

FNode *NodeToFlushNode(Node *node, int pos, int key_size){
	FNode *fnode = (FNode *)malloc(sizeof(FNode)); //申请FNode空间
	
	//赋值数据块大小、数据磁盘偏移量、类型
	fnode->size    = node->value->size;
	fnode->pos	   = pos;
	fnode->key_size = key_size;
	fnode->offset  = node->value->offset;
	
	return fnode;
}

Node *FlushNodeToNode(FNode *fnode, char *key){
	Node *node = (Node *)malloc(sizeof(Node)); //申请Node空间
	VI *value_info = (VI*)malloc(sizeof(VI));  //申请数据信息空间
	
	//赋值数据块大小、数据块缓存池地址、数据块磁盘偏移量
	value_info->size   = fnode->size;
	value_info->buf    = NULL;
	value_info->offset = fnode->offset;

	//赋值节点的key指针、节点的数据信息头、节点的下一个节点
	node->key   = key;
	node->value = value_info;
	node->next  = NULL;
}

char *FlushNodeToChar(FNode *fnode){
	char *char_temp = (char *)malloc(FNode_Size);
	FNode *fnode_temp;
	
	fnode_temp = (FNode *)char_temp;
	*fnode_temp = *fnode;
	
	return char_temp;
}

FNode *CharToFlushNode(char *char_temp){
	return (FNode *)char_temp;
}

void hashtable_sync_save_offset(OFFSET_TYPE offset){
	char *char_temp = (char *)malloc(OFFSET_TYPE_SIZE);
	OFFSET_TYPE  *offset_type_ptr;
	offset_type_ptr = (OFFSET_TYPE *)char_temp;
	*offset_type_ptr = offset;
	
	sync_write(hashtable_fd, char_temp, OFFSET_TYPE_SIZE, 0);
}

OFFSET_TYPE hashtable_sync_load_offset(){
	char *char_temp = (char *)malloc(OFFSET_TYPE_SIZE);
	OFFSET_TYPE  *offset_type_ptr;
	
	sync_read(hashtable_fd, char_temp, OFFSET_TYPE_SIZE, 0);
	offset_type_ptr = (OFFSET_TYPE *)char_temp;
	if(*offset_type_ptr < OFFSET_TYPE_SIZE || *offset_type_ptr < 0)
		return OFFSET_TYPE_SIZE;
	return *offset_type_ptr;
}


void hashtable_sync_save_node(Node *node, int pos){
	FNode *fnode;
	int key_size;
	
	key_size = strlen(node->key); //计算key字符串长度
		
	if(node != NULL){ //如果节点不是空的
	    fnode = NodeToFlushNode(node, pos, key_size); //把节点转换为fnode
	}else
	{ //如果节点是空的
		fnode->size     = 0; //初始化节点
		fnode->key_size = 0;
		fnode->pos 	    = -1; //初始化位置
		fnode->offset   = -1; //初始化偏移量
	}
	
	//写入fnode，也就是node到数据库文件
	sync_write(hashtable_fd, FlushNodeToChar(fnode), FNode_Size, flush_offset);

	//向后移动偏移量
	flush_offset += FNode_Size;
	
	//如果节点不为空
	if(node != NULL){
		sync_write(hashtable_fd, node->key, key_size, flush_offset);
		flush_offset += key_size;
		hashtable_sync_save_offset(flush_offset);
	}
}

Node *hashtable_sync_load_node(OFFSET_TYPE offset){
	char *char_temp = (char *)malloc(FNode_Size); //申请保存FNode的空间
	char *key = NULL; //key的指针
			
	FNode *fnode;
	OFFSET_TYPE size; //
	
	sync_read(hashtable_fd, char_temp, FNode_Size, offset);//从数据文件读取FNode的字符
	fnode = CharToFlushNode(char_temp); //把char转换为FNode
	
	size = fnode->key_size;
	pos = fnode->pos;
	
	key = (char *)malloc(size);
	sync_read(hashtable_fd, key, size, offset + FNode_Size);
	move_offset = FNode_Size + size;
	return FlushNodeToNode(fnode, key);
}


void persis_sync_init(char *filename){
	hashtable_fd = sync_init(filename);
	flush_offset = OFFSET_TYPE_SIZE;
	move_offset  = 0;
	pos = 0;
}

void persis_sync_save(HashTable *hashtable){
	Node *node = NULL;
	
	for(int i = 0; i < HASHTABLE_ARRAY_SIZE; i++){ //遍历hashtable数组的每个元素，遍历每个链表
		node = hashtable->array[i]; //活动头节点
		while(node){ //当头节点存在时
			hashtable_sync_save_node(node, i); //保存当前节点
			if(node->next != NULL){ //如果当前节点有下个节点
				node = node->next; //移动到下一个节点
			}else{ //如果没有了，说明是最后一个
				break; //直接退出
			}
		}
	}
}

void persis_sync_load(HashTable *hashtable){
	Node *node; //读取的节点
	Node *list_node; //hashtable数组保存的节点
	OFFSET_TYPE offset = OFFSET_TYPE_SIZE; //hashtable节点持久化已读偏移量，规定hashtable节点数据从哪儿开始写
	OFFSET_TYPE already_offset = hashtable_sync_load_offset(); //hashtable节点写道哪里了
		
	//如果没读到头
	while(offset < already_offset){
		//从当前文件位置读取节点以及节点在数组中的位置
		node = hashtable_sync_load_node(offset);
		offset += move_offset; //向后移动偏移量
		
		list_node = hashtable->array[pos]; //读取节点所在hashtable数组元素，也就是链表的头节点
		if(list_node != NULL){ //如果头节点不为空
			node->next = list_node; //把新节点接到原有头节点的前面
			hashtable->array[pos] = node; //把新节点当成是头节点
		}else{ //如果头节点为空
			hashtable->array[pos] = node; //直接把新节点当成头节点
		}
	}
}

/******/
void test_offset(){
	OFFSET_TYPE offset;
	persis_sync_init("/root/workspace/Finch/kv/hashtable_sync/hashtable.fd");
	hashtable_sync_save_offset(1000);
	offset = hashtable_sync_load_offset();
	printf("%lld\n", offset);
}

void test_convert(){
	FNode *fnode = (FNode *)malloc(sizeof(FNode));
	FNode *fnode_temp;
	
	fnode->size = 100;
	fnode->offset = 1000;
	
	fnode_temp = CharToFlushNode(FlushNodeToChar(fnode));
	printf("size = %d offset = %lld\n", fnode_temp->size, fnode_temp->offset);
}

void test_node_sync(){
	Node *node = (Node *)malloc(sizeof(Node));
	Node *node_temp = NULL;
	VI *value_info = (VI*)malloc(sizeof(VI));
	int pos;
	
	OFFSET_TYPE offset = OFFSET_TYPE_SIZE;
	OFFSET_TYPE move_offset = 0;
	
	value_info->size = 12;
	value_info->buf  = NULL;
	value_info->offset = 1000;
	
	node->value = value_info;
	node->key = "test";
	node->next = NULL;
	
	persis_sync_init("/root/workspace/Finch/kv/hashtable_sync/hashtable.fd");
	hashtable_sync_save_node(node, 1);
	node_temp = hashtable_sync_load_node(OFFSET_TYPE_SIZE);
	
	printf("size = %d offset = %lld key = %s  pos = %d\n", node_temp->value->size, node_temp->value->offset, node_temp->key, pos);
}


/*
void main(){
	test_node_sync();
}*/