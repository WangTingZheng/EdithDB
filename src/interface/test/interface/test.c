#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../../interface.h"
#include "../../../until/until.h"

void main(){
/*					 测试put和get功能            */
	char *value = NULL;

	Config config;
	config.index_path = until_get_full_path("hashtable");
	config.buffer_pool_path = until_get_full_path( "buffer");
	config.buffer_pool_size = 30;
	
	printf("-----------interface_int-----------\n");
	interface_init(config);
	
	printf("-----------interface_put-----------\n");
	interface_put("key", "wang");
	
	printf("-----------interface_get-----------\n");
	value = interface_get_value("key");

	if(value != NULL)
		printf("get value = %s\n", value);
	else
		printf("value is null");
			
	printf("----------interface_remove---------\n");
	interface_delete("key");
	interface_put("key", "ting");
	interface_put("key", "zheng");
	
	
	printf("-----------interface_get-----------\n");
	value = interface_get_value("key");
	if(value != NULL)
		printf("get value = %s\n", value);
	else
		printf("value is null");

	printf("-----------interface_exit-----------\n");
	interface_exit();	
	
	printf("------------------------------------\n");
}