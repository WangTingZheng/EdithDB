#include <stdio.h>
#include "../until.h"

void main(){
	printf("current path: %s\n", until_get_path());
	printf("path: %s", until_get_full_path(4, "index"));
}