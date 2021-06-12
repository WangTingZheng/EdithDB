#include <stddef.h>
#include <stdio.h>
#include "../persis.h"
#include "../../index/index.h"

void main(){
	HashTable hashtable;
	HashTable hashtable_new;
    VI value;
    
    persis_sync_init("/root/workspace/Finch/kv/dbfile/htsync.fd");
    
	index_init(&hashtable);
	index_init(&hashtable_new);
    
    value.size   = 1;
    value.buf    = NULL;
    value.offset = 4;
	
	index_insert("test", &value, &hashtable);
	index_insert("test1", &value, &hashtable);
	index_insert("test2", &value, &hashtable);
	index_insert("test3", &value, &hashtable);
	index_insert("test4", &value, &hashtable);
	index_insert("test5", &value, &hashtable);
	index_insert("test6", &value, &hashtable);

	persis_sync_save(&hashtable);
	index_print(&hashtable);
	
	printf("\n");
	
	persis_sync_load(&hashtable_new);
	index_print(&hashtable_new);
}