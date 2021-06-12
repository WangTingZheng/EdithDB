#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../../interface.h"

#define MAX_PATH_SIZE 100

void main(){
	char *value = NULL;
	char *delete_value = NULL;
	
	char index[MAX_PATH_SIZE];
	char buffer[MAX_PATH_SIZE];
	
	getcwd(index, sizeof(index));
	getcwd(buffer, sizeof(index));
	
	strncat(index, "/../../../../../dbfile/index", sizeof("/../../../../../dbfile/index"));
	strncat(buffer, "/../../../../../dbfile/buffer", sizeof("/../../../../../dbfile/buffer"));
	
	Config config;
	config.index_path = index;
	config.buffer_pool_path = buffer;
	config.buffer_pool_size = 30;
	
	printf("-----------interface_int-----------\n");
	interface_init(config);

	printf("-----------interface_get-----------\n");
	value = interface_get_value("key");
	if(value != NULL)
		printf("get value = %s\n",  value);
	else
		printf("value is null\n");
	
	printf("-----------interface_delete-----------\n");
	interface_delete("key");

	printf("-----------interface_get-----------\n");
	delete_value = interface_get_value("key");
	if(delete_value != NULL)
		printf("get value = %s\n", delete_value);
	else
		printf("value is null\n");
}