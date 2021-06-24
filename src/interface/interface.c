#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "interface.h"
#include "../buffer/buffer.h"
#include "../index/index.h"
#include "../persis/persis.h"

HashTable hashtable;

void interface_init(Config config){
	index_init(&hashtable);
	
	persis_sync_init(config.index_path);
	persis_sync_load(&hashtable);
		
	buffer_init(config.buffer_pool_path, config.buffer_pool_size);
}

void interface_put(char *key, char *value){
	VI *value_info = (VI *)malloc(sizeof(VI));
	
	value_info->size = 0;
	value_info->buf  = NULL;
	value_info->offset = OFFSET_TYPE_SIZE;
	
	index_insert(key, value_info, &hashtable);
	buffer_put(value, value_info);
}

Queue *interface_get(char *key){
	QValue qvalue;
	
	Queue *queue;
	Queue *queue_new = queue_init();
	
	queue = index_lookup(key, &hashtable);
	
	while(!queue_is_empty(queue)){
		qvalue = queue_pop(queue);
		
		qvalue.value = (char *)malloc(qvalue.value_info->size);
		buffer_get(qvalue.value, qvalue.value_info);
		
		queue_push_value(queue_new, qvalue.value);
	}
	
	return queue_new;
}

char* interface_get_value(char *key){
	char *value = NULL;
	char *temp = NULL;
	
	Queue *queue;
	queue = interface_get(key);
	
	while(!queue_is_empty(queue)){
		temp = queue_pop_value(queue);
		if(value == NULL){
			value = temp;
		}else{
			strcat(value, ",");
			strcat(value, temp);
		}
	}
	
	return value;
}

void interface_delete(char *key){
	VI *value_info;
	
	Queue *queue;
	queue = index_lookup(key, &hashtable);

	while(!queue_is_empty(queue)){
		value_info = queue_pop_vi(queue);
		buffer_delete(value_info);
	}
	index_remove(key, &hashtable);
}

void interface_exit(){
	buffer_exit();
	persis_sync_save(&hashtable);
	index_free(&hashtable);
}	