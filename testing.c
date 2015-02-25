//testing.c
#include <stdio.h>
#include <stdlib.h> //malloc()
#include <string.h> //strcmp
#include "kvs2.h"

int test1(FILE* file);

int main(){
	FILE* my_data = initialize("hashtable");
	if(test1(my_data)){
		// expected result: the 
		printf("TEST PASS: collision on the same key.\n");
	}else{
		printf("TEST FAIL: collision on the same key\n");
	}
	/*int* val = malloc(sizeof(int)*5); //sample value for testing.
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
	free(val);*/
	fclose(my_data);
	return 0;
}

//collision on same key
int test1(FILE* file){
	char* key = "brit";
	char first_result[max_value_size]; //not sure what size, so i made it max size of value
	int length;
	int* len = &length;
	insert(file, key, "arthur", sizeof("arthur"));
	fetch(file, first_result, key, len);
	//printf("first result is: %s\n", first_result);

	char second_result[max_value_size];
	insert(file, key, "hope", sizeof("hope"));
	fetch(file, second_result, key, len);
	//printf("second result is: %s\n", second_result);

	if(strcmp(first_result, second_result) != 0 && 
	   strcmp(first_result, "arthur") == 0 &&
	   strcmp("hope", second_result) == 0){
		return 1;
	}else{
		return -1;
	}
}

