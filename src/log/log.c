#include <stdio.h>
#include <time.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "../sync/sync.h"
#include "../until/until.h"

char* get_time(){
	unsigned long long a = 0;
    char *timeStr = (char *)malloc(14);

    time_t timer;
    struct tm *tblock;
    time(&timer);
    tblock = gmtime(&timer);
 
    a = (tblock->tm_year+1900)*100;
    a = (a+tblock->tm_mon+1)*100;
    a = (a+tblock->tm_mday)*100;
    a = (a+tblock->tm_hour+8)*100;
    a = (a+tblock->tm_min)*100;
    a = (a+tblock->tm_sec);
    sprintf(timeStr, "%llu", a);
    
    return timeStr;
}

int log_set_path(char *path){
	return sync_init(path);
}

int log_set_filename(char *filename){
    return sync_init(until_get_full_path(filename));
}

void log_append(int fd, char *source, char *function, char *message){
	char *str = (char *)malloc(1);
	char *time = get_time();
	sprintf(str, "%s%s%s%s%s%s%s%s%s", "[", time, "]", source, ":", function, ":", message, "\n");
	sync_write(fd, str, strlen(str));
}