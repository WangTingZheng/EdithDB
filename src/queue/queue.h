#ifndef __QUEUE_H
#define __QUEUE_H

#include "../index/index.h"

Queue* queue_init();
int queue_is_empty(Queue *queue);
void queue_push(Queue *queue, QValue value);
void queue_push_vi(Queue *queue, VI *vi);
void queue_push_value(Queue *queue, char *value);
QValue queue_pop(Queue *queue);
VI *queue_pop_vi(Queue *queue);
char *queue_pop_value(Queue *queue);

#endif