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

//pass
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

//pass
int insert(FILE* store, char* key, void* value, int length){
	if(key == NULL || value == NULL){
		printf("Error: Cannot insert null values into hashtable.\n");
		return -1;
	}
	if(key_size+length+sizeof(TOMBSTONE) > table_length){ 
		printf("Error: Data is too large.\n");
		return -1;
	}
	int index = insert_probe(store, key);

	fseek(store, index*table_length, SEEK_SET);

	int m = VALID;
	int* magic = &m;
	int *len_ptr = &length;

	fwrite(magic, sizeof(VALID), 1, store); //insert magic
	fwrite(key, key_size, 1, store); //insert key
	fwrite(len_ptr, sizeof(int), 1, store); //insert length
	fwrite(value, length, 1, store); //insert value
	return 0;
}

//pass //note: while condition can be a condition to check for a full hash table
//find the location to insert the new pair. if the key is encountered, return that index.
//or else, find the closes null index to the index that was found through the hash
int insert_probe(FILE* store, char* key){
	//found index for new key
	int index = hash(key)%table_size;
	char key_buffer[key_size];
	int* flag = malloc(sizeof(int));
	fseek(store, index*table_length, SEEK_SET);

	do{
		*flag = 0;
	    fread(flag, sizeof(TOMBSTONE), 1, store); //read in possible flag number
	    if(*flag == VALID){
	    	//printf("found valid data!\n");
	    	fseek(store, (index*table_length)+4, SEEK_SET); //offset flag number
	    	fread(key_buffer, key_size, 1, store); 
	    	// only return valid if I find a key match exists at that location
	    	if(strcmp(key_buffer, key)==0){ 
	    		//printf("found a match!\n");
		    	return index; // return the index to insert and replace 
	    	}
	    	// if not the key - just keep going, keep looking for a place to insert
	    }else if(*flag == INVALID){
	    	//printf("found invalid data!\n");
	    	return index; // return the index to insert
	    }else if(*flag == TOMBSTONE){
	    	//printf("found tombstone!\n");
	    	return index; // return the index to insert
	    }
	    ++index;
	    index = index > table_size ? 0 : index;
	    fseek(store, index*table_length, SEEK_SET); //find location of next flag number
	}while(*flag == TOMBSTONE || *flag == VALID); //search until there is a "null" slot

	free(flag);
	return -1; // this should never be reached if hashtable isn't 100% full.
}

//pass
//read needs to use probe to find key in table
//return the slot location of where the value is stored. or else -1.
int fetch(FILE* store, void* result, char* key, int* length){
	char key_buffer[table_length]; //needs to be fixed size of key
	//TODO: need to probe here
	int index = fetch_probe(store, key);

	fseek(store, (index*table_length)+sizeof(TOMBSTONE), SEEK_SET);
	//read in key
	fread(key_buffer, key_size, 1, store);

	fread(length, sizeof(int), 1, store);
	//check if read fails?
	fread(result, *length, 1, store);
	return 0;
}

//pass
//find the slot number that matches the key. or else, return -1.
int fetch_probe(FILE* store, char* key){
	int index = hash(key)%table_size;
	char key_buffer[key_size];
	int* magic = malloc(sizeof(int));
	fseek(store, index*table_length, SEEK_SET);

	do{
		*magic = 0;
	    fread(magic, sizeof(TOMBSTONE), 1, store); //read in possible magic number
	    if(*magic == VALID){
	    	//printf("found valid data!\n");
	    	fseek(store, (index*table_length)+4, SEEK_SET); //offset magic number
	    	fread(key_buffer, key_size, 1, store); 
	    	if(strcmp(key_buffer, key)==0){ //check for a key match at this index
	    		//printf("found a match!\n");
		    	return index;
	    	}
	    }else if(*magic == INVALID){
	    	//printf("found invalid data!\n");
	    	return -1;
	    }else if(*magic == TOMBSTONE){
	    	//printf("found tombstone!\n");
	    }
	    ++index;
	    index = index > table_size ? 0 : index;
	    fseek(store, index*table_length, SEEK_SET); //find location of next magic number
	}while(*magic == TOMBSTONE || *magic == VALID); //search until there is a "null" slot

	free(magic);
	return -1;
}
/*
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
	insert(store, "jason", "heron", sizeof("heron"));
	read_char_array(store, "jason", sizeof("heron"));

	//fetch(store, "nameb", val, val_ptr);
	fclose(store);
	free(val);
	return 0;
}*/


/*------------------------HELPER FUNCTIONS--------------------------------*/

FILE* create_file(char* name){
	FILE* store = fopen(name, "w+");
	populate(store);
	return store;
}

FILE* access_file(char* name){
	FILE* store;
	store = fopen(name, "r+");
	return store;
}

//pass
//continue point: Assume this works. Assume structures put null ptrs at end.
//TODO: Confirm or improve.
void populate(FILE* store){
	char filler = 0;
	int multiply = table_length - sizeof(INVALID);
	int invalid_value = INVALID;
	int* invalid = &invalid_value;

	for(int i = 0; i < table_size; i++){
		//every slot will begin with "invalid" at initialize
 		fwrite(invalid, sizeof(INVALID), 1, store);
		fwrite(&filler, sizeof(char)*multiply, 1, store);
	}
}


/*-----------------SECOND CLASS AND TODO FUNCTIONS---------------------------*/

int delete(char* key){
	(void)key;
	return 0;
}

//pass
void read_int_array(FILE* store, char* key, int length){
	int result[5];
	for(int k = 0; k < 5; k++){
		result[k] = 0; //fill result with zeros
	}
	int len_storage = 0;
	int *len_ptr = &len_storage;
	fetch(store, result, key, len_ptr);
	printf("2 value: \n");
	for(int i = 0; i < 5; i++){
		printf("result[%d] = %d\n",i,result[i]);
	}
}

//pass
void read_char_array(FILE* store, char* key, int length){
	char result[100];
	int len_storage = 0;
	int *len_ptr = &len_storage;
	for(int k = 0; k < 5; k++){
		result[k] = '\0'; //fill result with zeros
	}
	fetch(store, result, key, len_ptr);
	printf("2 value: %s\n",result);

}

/**
* djb2 hash algorithm by Dan Bernstein
* from http://www.cse.yorku.ca/~oz/hash.html
*/
unsigned long hash(char *str) {
	unsigned long hash = 5381;
	int c;

	while ((c = *str++))
		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
	return hash;
}




