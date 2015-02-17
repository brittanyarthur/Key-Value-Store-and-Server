#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

//written for Ubuntu, might be wonky in OS X and/or FreeBSD

int SIZE;   //number of entries in hash table
int LENGTH; //we don't know what this is
int fd;


int initialize(char* file, int length, int size) {
	
	//open for reading and writing, create file if needed
	if((fd = open(file, O_RDWR | O_CREAT, 0666)) == -1) {
		perror("Could not open file");
		return -1;
	}
	
	SIZE = size;
	LENGTH = length;
	


	return fd;
}

/*
int insert(char* key, void* value, int length) {
	return 0;
}

int fetch(char* key, void* value, int* length) { //int* may be typo on assigment
        return 0;
}

int probe(char* key) {
        return 0;
}

int delete(char* key) {
        return 0;
}
*/

/**
* djb2 hash algorithm by Dan Bernstein
* from http://www.cse.yorku.ca/~oz/hash.html
*/
unsigned long hash(unsigned char *str) {
	unsigned long hash = 5381;
	int c;

	while ((c = *str++)) 
		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
	return hash;
}


int main() {
	SIZE = LENGTH = fd = 0;

	initialize("store.test", 1, 1);
	printf("Hello world from kvs\n");
	close(fd);
	return EXIT_SUCCESS;
}
