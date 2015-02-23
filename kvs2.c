//kvs2.c
#include <stdio.h>
#include <unistd.h>

void initialize(char* name, int size, int length);
void create_file(char* name, int size, int length);

void initialize(char* name, int size, int length){
	if( access(name, W_OK ) != -1 ) {
		//file exists
		create_file(name, size, length);
	}else{
		//file does not exist
	}
}

void create_file(char* name, int size, int length){
	FILE* file = fopen(name, "w+");
}

int main(){
	int size = 10;
	int length = 50;
	//initialize hash table into file.
	initialize("hashtable", size, length);
}