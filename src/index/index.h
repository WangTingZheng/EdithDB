#ifndef __INDEX_H
#define __INDEX_H
#define INDEX_ARRAY_SIZE 10

typedef enum Boolean{
	true, false
}boolean;

typedef struct _VI{
	off_t size;
	char* buf;
	off_t offset;
}VI;

typedef struct _NODE Node;

struct _NODE{
	char *key;
	VI *info;
	Node *next;
};

typedef struct _INDEX{
	Node* array[INDEX_ARRAY_SIZE];
}Index;

typedef struct _FLUSH_NODE{
	off_t size;
	off_t ksize;
	off_t offset;
	int pos;
}FNode;

void     index_create();
void     index_put(char *key, VI *info);
VI*      index_get(char *key);
void     index_delete(char *key);
boolean  index_contains(char *key);
boolean  index_isEmpty();
int      index_size();

void     index_set_path(char *path);
void     index_persis();
void     index_load();
#endif