#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../sync.h"

#define MAX_PATH_SIZE 100

void main(){
	int fd;
	char pathname[MAX_PATH_SIZE];
	char message[30] = "This is a text to write";
	char *output = (char *)malloc(30);
	
    getcwd(pathname, sizeof(pathname));
    strncat(pathname, "/../../../../../dbfile/sync", sizeof("/../../../../../dbfile/sync"));

	fd = sync_init(pathname);
	sync_write(fd, message, strlen(message), 0);
	sync_read(fd, output, strlen(message), 0);
	sync_exit(fd);
	printf("%s\n", output);
}
