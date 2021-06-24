#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../../interface.h"
#include "../../../until/until.h"

void main(){
	char *value = NULL;
	char *delete_value = NULL;
	
	Config config;
	config.index_path = until_get_full_path(5, "hashtable");
	config.buffer_pool_path = until_get_full_path(5, "buffer");
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