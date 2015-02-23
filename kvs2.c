//kvs2.c
#include <stdio.h>
#include <unistd.h>
#include "hvs2.h"

void initialize(char* name, int size, int length){
	FILE* store;
	if( access(name, W_OK ) != -1 ) {
		//file exists
		store = access_file(name);
		
	}else{
		//file does not exist
		store = create_file(name, size, length);
	}
	return store;
}

FILE* create_file(char* name, int size, int length){
	FILE* store;

	return store;
}

FILE* access_file(char* name){

}

int main(){
	int size = 10;
	int length = 50;
	//initialize hash table into file.
	initialize("hashtable", size, length);
}