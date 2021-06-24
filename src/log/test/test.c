#include <stdio.h>
#include "../log.h"
#include "../../until/until.h"

void main(){
	int a = 1;
	log_set_path(until_get_full_path("LOG"));
	log_append("test.c", "main", "This is a test, value is %d", a);
}