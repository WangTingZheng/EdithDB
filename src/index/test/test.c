#include <stdio.h>
#include <stdlib.h>

#include "../index.h"
#include "../../queue/queue.h"

void main(){
        HashTable hashtable;
        Node *node;

        VI *value = (VI *)malloc(sizeof(VI));
        VI *value_temp;
        char *buf;

        value->size   = 1000;
        value->buf    = buf;
        value->offset = 1002;

        index_init(&hashtable);

        printf("---------- Test Insert ----------\n");
        index_insert("test1", value, &hashtable);
        index_print(&hashtable);

        printf("---------- Test LookUp ----------\n");
        Queue *queue = index_lookup("test1", &hashtable);
        while(!queue_is_empty(queue)){
			value_temp = queue_pop_vi(queue);
			printf("size = %d\n", value_temp->size);
        }
     

        printf("---------- Test Remove ----------\n");
        index_remove("test1", &hashtable);
        index_print(&hashtable);
        
        Queue *queue_re = index_lookup("test1", &hashtable);
        while(!queue_is_empty(queue_re)){
			value_temp = queue_pop_vi(queue_re);
			printf("size = %d\n", value_temp->size);
        }

        printf("---------- Test Free ----------\n");
        index_insert("test1", value, &hashtable);
        index_print(&hashtable);
        printf("\n");
        index_free(&hashtable);
        index_print(&hashtable);
}
