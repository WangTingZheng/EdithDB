#define	_FILE_OFFSET_BITS	64
#define _LARGEFILE_SOURCE
#define _LARGEFILE64_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "sync.h"

int sync_init(char *pathname){
	int fd = open(pathname, O_RDWR | O_LARGEFILE | O_CREAT , S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);

	if(fd <= 0){
		perror("sync.c:sync_init");
		return -1;
	}
	return fd;
}

int sync_read(int fd, char *mem, int size, long long offset){
	int ret;
	int n = 0;
	int toread;
	int empty_size;
	char *ptr;

	if(fd <= 0){
		perror("sync.c:sync_read:fd");
		return -1;
	}

	ret = lseek64(fd, offset, SEEK_SET);

	if(ret < 0){
		perror("sync.c:sync_read:lseek64");
		return -1;
	}

	toread = size;
	ptr = mem;

	while(toread > 0){
		n = read(fd, ptr, size);
		if(n == -1){
			perror("sync.c:sync_read:read");
			return -1;
		}else if(n == 0){
			printf("sync.c:sync_read:read:disk zone is empty\n");
			return -1;
		}
		toread -= n;
		ptr += n;
	}

	return 0;
}

int sync_write(int fd, char *mem, int size, long long offset){
	int ret;
	char *ptr;

	ptr = mem;

	if(fd <= 0){
		perror("sync.c:sync_write:fd");
		return -1;
	}

	ret = lseek64(fd, offset, SEEK_SET);

	if(ret < 0){
		perror("sync.c:sync_write:lseek64");
		return -1;
	}


	if(write(fd, ptr, size)  == -1){
		perror("sync.c:sync_write:write");
		return -1;
	}
	
	return 0;
}

int sync_exit(int fd){
	close(fd);
	return 0;
}