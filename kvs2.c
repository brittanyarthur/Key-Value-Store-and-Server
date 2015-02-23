//kvs2.c
#include <stdio.h>
#include <unistd.h>
#include "kvs2.h"


//size = # of entires, length = # size of each thing
FILE* initialize(char* name, int size, int length){
	FILE* store;
	if( access(name, W_OK ) != -1 )
		store = access_file(name); //file exists
	else
		store = create_file(name, size, length); //file does not exist
	return store;
}

FILE* create_file(char* name, int size, int length){
	FILE* store = fopen(name, "w+");
	//fill hash with fake data
	populate(store, size, length);
	return store;
}

FILE* access_file(char* name){
	FILE* store;
	store = fopen(name, "w");
	return store;
}

void populate(FILE* store, int size, int length){

}

int main(){
	int size = 10;
	int length = 50;
	//initialize hash table into file.
	initialize("hashtable", size, length);
}