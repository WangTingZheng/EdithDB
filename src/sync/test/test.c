#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../sync.h"
#include "../../until/until.h"

#define MAX_PATH_SIZE 100

void main(){
	int fd;
	char message[30] = "This is a text to write";
	char *output = (char *)malloc(30);

	fd = sync_init(until_get_full_path("SYNC"));
	sync_write(fd, message, strlen(message));
	sync_read(fd, output, strlen(message), 0);
	sync_exit(fd);
	
	printf("%s\n", output);
}
