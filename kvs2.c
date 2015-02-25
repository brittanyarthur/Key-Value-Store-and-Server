//kvs2.c
//size = # of entires, length = # size of each thing
#include <stdio.h>
#include <stdlib.h> //malloc()
#include <unistd.h>
#include <string.h>
#include "kvs2.h"

#define table_size   360
#define table_length 512
#define key_size     25
	
//MAGIC NUMBER FLAGS
const int TOMBSTONE = 0xdeadd00d;
const int VALID     = 0xad00000b;
const int INVALID   = 0xda00000b;

//this looks good
FILE* initialize(char* name){
	FILE* store;
	if(access(name, W_OK ) != -1)
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
	if(key_size+length > table_length){ 
		printf("Error: Data is too large.\n");
		return -1;
	}
	//need to probe here using insert_probe
	int index = hash(key)%table_size;
	fseek(store, index*table_length, SEEK_SET);

	//need to insert valid here
	int m = VALID;
	int* magic = &m;
	fwrite(magic, sizeof(VALID), 1, store); //insert magic
	fwrite(key, key_size, 1, store); //insert key
									//insert length
	fwrite(value, length, 1, store); //insert valud
	return 0;
}

//read needs to use probe to find key in table
//return the slot location of where the value is stored. or else -1.
int fetch(FILE* store, void* result, char* key, int length){
	char key_buffer[table_length]; //needs to be fixed size of key
	//need to probe here
	int index = hash(key)%table_size;

	//skip the magic number (this +4 is horrible)
	fseek(store, (index*table_length)+4, SEEK_SET);
	//read in key
	fread(key_buffer, key_size, 1, store);
	printf("1 key in fetch: %s\n",key_buffer);
	//check if read fails?
	fread(result, length, 1, store);
	return 0;
}

//find the slot number that matches the key. or else, return -1.
int probe(FILE* store, char* key){
	int index = hash(key)%table_size;
	char buffer[table_length];
	int* magic = malloc(sizeof(int));
	fseek(store, index*table_length, SEEK_SET);

	do{
		*magic = 0;
	    fread(magic, sizeof(TOMBSTONE), 1, store); //read in possible magic number
	    if(*magic == TOMBSTONE){
	    	fseek(store, (index*table_length)+4, SEEK_SET); //offset magic number
	    	fread(buffer, table_length, 1, store); 
	    	if(strcmp(buffer, key)==0){ //check for a key match at this index
	    		printf("found a match!\n");
		    	return index;
	    	}
	    }
	    ++index;
	    fseek(store, index*table_length, SEEK_SET); //find location of next magic number
	}while(*magic == TOMBSTONE); //search until there is a "null" slot

	free(magic);
	return -1;
}

int main(){
	//initialize hash table into file.
	FILE* store = initialize("hashtable");

	int* val = malloc(sizeof(int)*5); //sample value for testing.
	for(int i = 0; i < 5; i++){
		val[i] = i+50;
	}
	int val_len = sizeof(int)*5;
	int* val_ptr = &val_len;
	insert(store, "nameb", val, val_len);
	read_int_array(store,"nameb", val_len);
	//fetch(store, "nameb", val, val_ptr);
	fclose(store);
	free(val);
	return 0;
}


/*------------------------HELPER FUNCTIONS--------------------------------*/

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
//TODO: Confirm or improve.
void populate(FILE* store){
	char filler = 0;
	int multiply = table_length;
	fwrite(&filler, sizeof(char)*multiply, table_size, store);
}


/*-----------------SECOND CLASS AND TODO FUNCTIONS---------------------------*/

int delete(char* key){
	return 0;
}

void read_int_array(FILE* store, char* key, int length){
	int result[5];
	for(int k = 0; k < 5; k++){
		result[k] = 0; //fill result with zeros
	}
	fetch(store, result, key, length);
	printf("2 value: \n");
	for(int i = 0; i < 5; i++){
		printf("result[%d] = %d\n",i,result[i]);
	}
}



