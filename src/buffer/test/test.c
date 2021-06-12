#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "../buffer.h"

#define size strlen("helloworld")

void main(){
	VI *vi1 = (VI*)malloc(sizeof(VI));
	VI *vi2 = (VI*)malloc(sizeof(VI));
	VI *vi3 = (VI*)malloc(sizeof(VI));
	VI *vi4 = (VI*)malloc(sizeof(VI));
	VI *vi5 = (VI*)malloc(sizeof(VI));

	char *temp1 = (char *)malloc(size);
	char *temp2 = (char *)malloc(size);
	char *temp3 = (char *)malloc(size);
	char *temp4 = (char *)malloc(size);
	char *temp5 = (char *)malloc(size);

	buffer_init("/root/workspace/Finch/kv/buffer/test/db.fd", 30);

	buffer_put("wang", vi1);
	buffer_put("ting", vi2);
	buffer_put("zheng", vi3);
	buffer_put("value4", vi4);
	buffer_put("value5", vi5);

	buffer_get(temp1, vi1);
	buffer_get(temp2, vi2);
	buffer_get(temp3, vi3);
	buffer_get(temp4, vi4);
	buffer_get(temp5, vi5);

	printf("temp1 = %s, temp2 = %s, temp3 = %s, temp4 = %s, temp5 = %s\n", temp1, temp2, temp3, temp4, temp5);
}