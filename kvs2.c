//kvs2.c
//size = # of entires, length = # size of each thing
#include <stdio.h>
#include <stdlib.h> //malloc()
#include <unistd.h>
#include <string.h>
#include "kvs2.h"

#define table_size 360
#define table_length 512
	#define key_size 25
	#define value_size 75
const int MAGIC_NUM = 0xdeadd00d;

struct kvpair{
	char* key;
	void* value;
}kvpair;

//this looks good
FILE* initialize(char* name){
	FILE* store;
	if( access(name, W_OK ) != -1 )
		//file exists
		store = access_file(name); 
	else
		//file does not exist
		store = create_file(name); 
	return store;
}

//this looks good
int insert(FILE* store, char* key, void* value, int length){
	if(key == NULL || value == NULL){
		printf("Error: Cannot insert null values into hashtable.\n");
		return -1;
	}
	//CONFIRM THIS CHECK IS DONE PROPERLY
	if(sizeof(char)*strlen(key)+length > table_length){ 
		printf("Error: Data is too large.\n");
		return -1;
	}
	int index = hash(key)%table_size;
	fseek(store, index*table_length, SEEK_SET);

	int m = MAGIC_NUM;
	int* magic = &m;
	fwrite(magic, sizeof(MAGIC_NUM), 1, store);
	fwrite(key, sizeof(key), 1, store);
	fwrite(value, sizeof(value), 1, store);
	return 0;
}

//return the slot location of where the value is stored. or else -1.
int fetch(FILE* store, char* key, void* value, int* length){
	(void)value;
	(void)length;
	int index = hash(key)%table_size;
	fseek(store, index*table_length, SEEK_SET);

	int magic_candidate = 0;
	char buffer[table_length];
	do{
	    fread(&magic_candidate, sizeof(int)*4, 1, store);
	    printf("Magic number is %d\n",MAGIC_NUM);
		printf("Magic number could be %d\n", magic_candidate);
	    if(magic_candidate == MAGIC_NUM){
	    	printf("found magic num\n");
	    	//check if this key is equal to the one I have
		    	return index;
	    }
	    fseek(store, (++index)*table_length, SEEK_SET);
	}while(magic_candidate != MAGIC_NUM);

	//int index = probe(key);
	return 0;
}

int main(){
	//initialize hash table into file.
	FILE* store = initialize("hashtable");
	char* val = "Brit"; //sample value for testing.
	int val_len = sizeof(val);
	int* val_ptr = &val_len;
	insert(store, "nameb", val, sizeof(val));
	//h_read(store,"nameb", sizeof(val));
	fetch(store, "nameb", val, val_ptr);
	fclose(store);
}




//HELPER FUNCTIONS
FILE* create_file(char* name){
	FILE* store = fopen(name, "w+");
	populate(store);
	return store;
}

FILE* access_file(char* name){
	FILE* store;
	store = fopen(name, "a+");
	return store;
}

//continue point: Assume this works. Assume structures put null ptrs at end.
void populate(FILE* store){
	//The argument size will be the number of entries in your hash table
	//fwrite(value, sizeof(value), table_size, store);
	char filler = 0;
	int multiply = table_length/sizeof(char);
	fwrite(&filler, sizeof(char)*multiply, table_size, store);
}






//SECOND CLASS AND TODO FUNCTIONS
/*
int fetch_probe(char* key){
	int index = hash(key)%table_size;

}*/

int h_read(FILE* store, char* key, int length){
	char buffer[table_length];
	int index = hash(key)%table_size;
	fseek(store, (index*table_length)+4, SEEK_SET);

	fread(buffer, length, 1, store);
	printf("1 %s\n",buffer);

	fread(buffer, length, 1, store);
	printf("2 %s\n",buffer);
/*
	fread(buffer, length, 1, store);
	printf("3 %s\n",buffer);*/
	
	return 0;
}



