#ifndef __INTERFACE_H
#define __INTERFACE_H

#include "../queue/queue.h"

typedef struct _CONFIG{
	char *index_path;
	char *buffer_pool_path;
	int buffer_pool_size;
}Config;

void interface_init(Config config);
void interface_put(char *key, char *value);
Queue *interface_get(char *key);
char* interface_get_value(char *key);
void interface_delete(char *key);
void interface_exit();

#endif

