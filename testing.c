//testing.c
#include <stdio.h>
#include <stdlib.h> //malloc()
#include "kvs2.h"

int main(){
	printf("hello world");
	FILE* my_data = initialize("hashtable");
	int* val = malloc(sizeof(int)*5); //sample value for testing.
	for(int i = 0; i < 5; i++){
		val[i] = i+50;
	}
	int val_len = sizeof(int)*5;
	int* val_ptr = &val_len;
	insert(my_data, "nameb", val, val_len);
	read_int_array(my_data,"nameb", val_len);
	insert(my_data, "jason", "heron", sizeof("heron"));
	read_char_array(my_data, "jason", sizeof("heron"));

	//fetch(my_data, "nameb", val, val_ptr);
	fclose(my_data);
	free(val);
	return 0;
}