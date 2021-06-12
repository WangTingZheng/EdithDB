#ifndef __BUFFER_H
#define	__BUFFER_H

#include "../index/index.h"

enum State {unavail, flushed, not_flush};

typedef struct HEAD{
	VI *info;
	enum State state;
}Head;

void buffer_init(char *filename, int buffer_size);
void buffer_put(char *value, VI *value_info);
void buffer_get(char *value, VI* value_info);
void buffer_delete(VI *value_info);
void buffer_exit();

#endif
