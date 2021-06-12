#include <stdlib.h>
#include "queue.h"

Queue* queue_init(){
	Queue *queue = (Queue *)malloc(sizeof(Queue));
	queue->front = queue->rear = NULL;
	return queue;
}

int queue_is_empty(Queue *queue){
    return queue->front == NULL;
}

void queue_push(Queue *queue, QValue value){
	QNode *node = (QNode *)malloc(sizeof(QNode));
	
	node->value = value;
	node->next = NULL;
	
	if(queue->rear == NULL){
		queue->front = node;
		queue->rear = node;
	}else{
		queue->rear->next = node;
		queue->rear = node;
	}
}

void queue_push_vi(Queue *queue, VI *vi){
	QValue *qvalue = (QValue*)malloc(sizeof(QValue));
	qvalue->value_info = vi;
	queue_push(queue, *qvalue);
}

void queue_push_value(Queue *queue, char *value){
	QValue *qvalue = (QValue*)malloc(sizeof(QValue));
	qvalue->value = value;
	queue_push(queue, *qvalue);
}

QValue queue_pop(Queue *queue){
	QValue value;
	if(queue->front != NULL){
		value = queue->front->value;
		queue->front = queue->front->next;
	}
	return value;
}

VI *queue_pop_vi(Queue *queue){
	return queue_pop(queue).value_info;
}

char *queue_pop_value(Queue *queue){
	return queue_pop(queue).value;
}