#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

//all these are example values
#define DEFAULT_LEN_KEY 10
#define DEFAULT_LEN_VALUE 40
#define ENTRY_OVERHEAD 3
#define DEFAULT_LEN_ENTRY (DEFAULT_LEN_KEY + DEFAULT_LEN_VALUE + ENTRY_OVERHEAD)


//written for Ubuntu, might be wonky in FreeBSD

FILE* store;
fpos_t hashTableStart;
int lenFile, numEntries, lenEntry, lenKey, lenValue;

/**
Make a store from a new file.
Helper function to initialize().

length: total length of the file.
size: number of entries in the hash table.

Returns: 0 on success, -1 on error.
*/
int initNewFile(int length, int size){

	if(	fprintf(store, "lenFile=%d\n", length) <= 0 ||
		fprintf(store, "numEntries=%d\n", size) <= 0 ||
		fprintf(store, "lenEntry=%d\n", DEFAULT_LEN_ENTRY) <= 0 ||
		fprintf(store, "lenKey=%d\n", DEFAULT_LEN_KEY) <= 0 ||
		fprintf(store, "lenValue=%d\n", DEFAULT_LEN_VALUE) <= 0
	) {
		printf("Couldn't write header.\n");
		return -1;
	}
	
	lenFile = length;
	numEntries = size;
	lenEntry = DEFAULT_LEN_ENTRY;
	lenKey = DEFAULT_LEN_KEY;
	lenValue = DEFAULT_LEN_VALUE;
	
	return 0;
}

/**
Load an existing store from a file.
Helper function to initialize().

length: total length of the file.
size: number of entries in the hash table.

Returns: 0 on success, -1 on error.
*/
int initExistingFile(int length, int size){


	if(	fscanf(store, "lenFile=%d\n", &lenFile) <= 0 ||
		fscanf(store, "numEntries=%d\n", &numEntries) <= 0 ||
		fscanf(store, "lenEntry=%d\n", &lenEntry) <= 0 ||
		fscanf(store, "lenKey=%d\n", &lenKey) <= 0 ||
		fscanf(store, "lenValue=%d\n", &lenValue) <= 0
	) {
		fprintf(stderr, "Couldn't read header.\n");
		return -1;	
	}
	
	if(length != lenFile) {
		fprintf(stderr, "Specefied length was %d, but length of file is %d.\n", length, lenFile);
		return -1;
	}
	
	if(size != numEntries){
		fprintf(stderr, "Specefied size was %d, but size of file is %d.\n", size, numEntries);
		return -1;
	}
	
	if(lenKey + lenValue + ENTRY_OVERHEAD != lenEntry){
		fprintf(stderr, "Length of entry does not add up.\n");
		return -1;
	}

	return 0;
}

/**
Open or create a file that will contain the hash table.

length: total length of the file.
size: number of entries in the hash table.

Returns: a file descriptor, or -1 for error.
*/
int initialize(char* file, int length, int size) {

	/* apparently there is no open option which does all of these:
	1. open for reading and writing
	2. create if does not exist
	3. if does exist, do NOT truncate
	*/
	
	//Q note: another way to check if file exists is with if( access( "testfile", W_OK ) != -1 ) {//exists}else{//does not exist} location: #include <unistd.h> 
	//open for reading and writing, without creation               
	if((store = fopen(file, "r+")) == NULL) {

		/*
		If doesn't exist, re-do open with creation.
		This option would also truncate the file if it does exist,
		so we can't do it in the first step.
		*/
		if((store = fopen(file, "w+")) == NULL) {
			perror("Could not open file");
			return -1;
		} else {
			//created a file
			if((initNewFile(length, size)) == -1) {
				fprintf(stderr, "Couldn't init new file.\n");
				return -1;
			}
		}
	} else {
		//opened existing file
		if((initExistingFile(length, size)) == -1) {
			fprintf(stderr, "Couldn't init existing file.\n");
			return -1;
		}
	}
	
	//save where the end of the header is
	if((fgetpos(store, &hashTableStart)) == -1){        //Q: will this work on both cases? Esp we should check the one after we have appended data to a file already?
		perror("Couldn't get file position)");
	}
	
	//print result to terminal
	printf("lenFile=%d\n", lenFile);
	printf("numEntries=%d\n", numEntries);
	printf("lenEntry=%d\n", lenEntry);
	printf("lenKey=%d\n", lenKey);
	printf("lenValue=%d\n", lenValue);
	
	return fileno(store);
}



int main() {
	lenFile = numEntries = lenEntry = lenKey = lenValue = -1;
	store = NULL;
	//how clear hashTableStart? fpos_t is a struct or something.

	//31 and 12 are example numbers
	if(initialize("example.store", 31, 12) == -1) {
		fprintf(stderr, "Couldn't initialize.\n");
		return EXIT_FAILURE;
	}

	if(fclose(store) == EOF){
		perror("Couldn't close file.\n");
		return EXIT_FAILURE;
	}
	
	return EXIT_SUCCESS;
}

/*
int insert(char* key, void* value, int length) {
	return 0;
}

int fetch(char* key, void* value, int* length) {
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
