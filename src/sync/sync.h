#ifndef __SYNC_H
#define __SYNC_H

int sync_init(char *pathname);
int sync_read(int fd, char *mem, int size, long long offset);
off_t sync_size(int fd);
int sync_write(int fd, char *mem, int size);
int sync_exit(int fd);

#endif