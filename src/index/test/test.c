#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "../../index/index.h"
#include  "../../log/log.h"

int index_fd;

VI *get_vi(off_t offset, off_t size){
    VI* info = (VI* )malloc(sizeof(VI));

    info->offset = offset;
    info->size   = size;
    info->buf    = NULL;

    return info;
}

void print_vi(VI *info){
    if(info){
        printf("offset = %jd size = %jd\n", (intmax_t)info->offset, (intmax_t)info->size);
    } else{
        log_append(index_fd, "index/test.c", "print_vi", "info is not exist!");
    }
}

void main(){
    index_fd = log_set_filename("TEST");
    index_create("LOG");

    printf("Put key-vi\n");

    index_put("10", get_vi(10, 10));
    index_put("20", get_vi(20, 20));
    index_put("30", get_vi(30, 30));

    printf("Print vi\n");

    print_vi(index_get("10"));
    print_vi(index_get("20"));
    print_vi(index_get("30"));

    printf("Delete index\n");

    index_delete("10");

    printf("Print vi after delete\n");

    print_vi(index_get("10"));
    print_vi(index_get("20"));
    print_vi(index_get("30"));

    printf("Index is %s empty\n", index_isEmpty()? "":"not");

    printf("Index size = %d", index_size());
}

