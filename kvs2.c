//kvs2.c
#include <stdio.h>
#include <stdlib.h> //malloc()
#include <unistd.h>
#include "kvs2.h"

#define table_size 10
#define table_length 50

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
//	index*
	return index;
}

FILE* create_file(char* name){
	FILE* store = fopen(name, "w+");
	//fill hash with fake data
	populate(store);
	return store;
}

FILE* access_file(char* name){
	FILE* store;
	store = fopen(name, "w");
	return store;
}

void populate(FILE* store){
	//The argument size will be the number of entries in your hash table
	void* value = malloc(table_length);
	fwrite(value, sizeof(value), table_size, store);
}

int main(){
	//initialize hash table into file.
	FILE* store = initialize("hashtable");
	char* val = "Jason Heron!!!";
	insert(store, "name", val, sizeof(val));
}




