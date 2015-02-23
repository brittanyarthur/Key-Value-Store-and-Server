//kvs2.c
#include <stdio.h>
#include <stdlib.h> //malloc()
#include <unistd.h>
#include "kvs2.h"

#define table_size 300
#define table_length 100
	#define key_size 25
	#define value_size 75

struct kvpair{
	char* key;
	void* value;
}kvpair;

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

	fwrite(key, sizeof(key), 1, store);
	fwrite(value, sizeof(value), 1, store);
	return 0;
}

int fetch(FILE* store, char* key, void* value, int* length){
	//int index = probe(key);
	return 0;
}
/*
int fetch_probe(char* key){
	int index = hash(key)%table_size;

}*/

int h_read(FILE* store, char* key, int length){
	char buffer[table_length];
	int index = hash(key)%table_size;
	fseek(store, index*table_length, SEEK_SET);
	fread(buffer, length, 1, store);
	printf("%s\n",buffer);

	fread(buffer, length, 1, store);
	printf("%s\n",buffer);
	fclose(store);
	return 0;
}

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
	fwrite(&filler, sizeof(char), table_size, store);
}

int main(){
	//initialize hash table into file.
	FILE* store = initialize("hashtable");
	char* val = "Brit";
	insert(store, "nameb", val, sizeof(val));
	h_read(store,"nameb", sizeof(val));
}









