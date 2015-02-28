//testing.c
#include <stdio.h>
#include <stdlib.h> //malloc()
#include <string.h> //strcmp
#include "kvs2.h"

int insert_test1(FILE* file);
int insert_test2(FILE* file);

int print_matching_index(FILE* file);
void rand_str(char *dest, size_t length);

int main(){
	FILE* my_data = initialize("hashtable", 100, 100); 
	if(insert_test1(my_data)){
		// expected result: the
		printf("TEST 1 PASS: collision on the same key.\n");
	}else{
		printf("TEST 1 FAIL: collision on the same key\n");
	}

	if(insert_test2(my_data)){
		// expected result: the
		printf("TEST 2 PASS: collision: allowing multiple entries on same index but different keys.\n");
	}else{
		printf("TEST 2 FAIL: collision: allowing multiple entries on same index but different keys.\n");
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
int insert_test1(FILE* file){
	char* key = "brit"; 
	char first_result[max_value_size]; //not sure what size, so i made it max size of value
	char second_result[max_value_size];
	int length;
	int* len = &length;
 
	//insert <brittany, arthur>
	insert(file, key, "arthur", sizeof("arthur"));
	//get brit's last name
	fetch(file, first_result, key, len);
	//printf("first result is: %s\n", first_result);

	/*Now user updates her last name*/

	//insert <brittany, hope>
	insert(file, key, "hope", sizeof("hope"));
	//get brit's last name; expected: new last name will have replaced old last name
	fetch(file, second_result, key, len);
	//printf("second result is: %s\n", second_result);

	if(strcmp(first_result, second_result) != 0 &&
	   strcmp(first_result, "arthur") == 0 &&
	   strcmp(second_result, "hope") == 0){
		return 1;
	}else{
		return -1;
	}
}

//collision: allowing multiple entries on same index but different keys
int insert_test2(FILE* file){
	char mango_value[max_value_size]; //not sure what size, so i made it max size of value
	char key2_value[max_value_size];
	int length;
	int* len = &length;

	//insert mango - it has in index of 351
	char* key = "mango";
	insert(file, key, "fruit", sizeof("fruit")); 
	//insert key2 - it also has in index of 351
	char* key2 = "YD4U8LRvpX8MNX9ygULp5sqJ";
	insert(file, key2, "key2value", sizeof("key2value"));

	//get the value of mango - expected: value exists
	fetch(file, mango_value, key, len);
	//printf("first result is: %s\n", mango_value);

	//get the value of key2
	fetch(file, key2_value, key2, len);
	//printf("second result is: %s\n", key2_value);

	if(strcmp(mango_value, key2_value) != 0 &&
	   strcmp(mango_value, "fruit") == 0 &&
	   strcmp(key2_value, "key2value") == 0 &&
	   hash(mango_value)%table_size == hash(key2_value)%table_size){
		return 1;
	}else{
		return -1;
	}
}

//---------------------- TEST HELPER FUNCTIONS --------------------

int print_matching_index(FILE* file){
	(void)file; 	//prevent unused parameter warning
	char* key = "mango";
	unsigned int index = hash(key)%table_size;
	char key_matching_index[key_size];
	for(;;){
		rand_str(key_matching_index, key_size-1);
		if ((hash(key_matching_index)%table_size)==index){
			printf("key 1: %s\n", key);
			printf("key 2: %s\n", key_matching_index);
			printf("index: %d\n", index);
			return index;
		}
	}
}

//http://stackoverflow.com/questions/15767691/whats-the-c-library-function-to-generate-random-string
void rand_str(char *dest, size_t length) {
    char charset[] = "0123456789"
                     "abcdefghijklmnopqrstuvwxyz"
                     "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    while (length-- > 0) {
        size_t index = (double) rand() / RAND_MAX * (sizeof charset - 1);
        *dest++ = charset[index];
    }
    *dest = '\0';
}








