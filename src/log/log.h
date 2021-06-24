#ifndef __LOG_H
#define __LOG_H

int log_set_path(char *path);
int log_set_filename(char *filename);
void log_append(int fd, char *source, char *function, char *message);
#endif