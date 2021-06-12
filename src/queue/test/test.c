#include "../queue.h"
#include <stdio.h>

void main(){
	Queue *queue = queue_init();
	
	printf("queue is %s empty\n", queue_is_empty(queue) == 1? "":"not");
	
	queue_push_value(queue, "hello, mq!");
	queue_push_value(queue, "again!");
	
	printf("queue is %s empty\n", queue_is_empty(queue) == 1? "":"not");

	while(!queue_is_empty(queue)){
		printf("value = %s\n", queue_pop_value(queue));
	}
}