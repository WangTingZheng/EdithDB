#include "until.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_PATH_SIZE 100

char* until_get_path(){
	char *path = (char *)malloc(MAX_PATH_SIZE);
	getcwd(path, MAX_PATH_SIZE);
	
	return path;
}

char* until_get_full_path(char* filename){
	char *path = until_get_path();

	strcat(path, "/");
	strcat(path, filename);
	
	return path;
}