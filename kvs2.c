//kvs2.c
#include <stdio.h>
#include <stdlib.h> //malloc()
#include <unistd.h>
#include "kvs2.h"

#define table_size 10
#define table_length 100

//size = # of entires, length = # size of each thing
FILE* initialize(char* name){
	FILE* store;
	if( access(name, W_OK ) != -1 )
		store = access_file(name); //file exists
	else
		store = create_file(name); //file does not exist
	return store;
}

int insert(FILE* store, char* key, void* value, int length){
	int index = hash(key)%table_size;
	printf("value to insert is: %s\n",value);
	fseek(store, index*table_length, SEEK_SET);
	fwrite(value, sizeof(value), 1, store);
	return 0;
}

int h_read(FILE* store, char* key, int length){
	char buffer[table_length];
	int index = hash(key)%table_size;
	fseek(store, index*table_length, SEEK_SET);
	fread(buffer, length, 1, store);
	printf("%s\n",buffer);
	fclose(store);
	return 0;
}

FILE* create_file(char* name){
	FILE* store = fopen(name, "w+");
	//fill hash with fake data
	populate(store);
	return store;
}

FILE* access_file(char* name){
	FILE* store;
	store = fopen(name, "a+");
	return store;
}

void populate(FILE* store){
	//The argument size will be the number of entries in your hash table
	void* value = malloc(table_length);
	//fwrite(value, sizeof(value), table_size, store);
	fwrite(value, sizeof(value), table_size*8, store);
}

int main(){
	//initialize hash table into file.
	FILE* store = initialize("hashtable");
	char* val = "Brit";
	insert(store, "nameb", val, sizeof(val));
	h_read(store,"nameb", sizeof(val));
}









