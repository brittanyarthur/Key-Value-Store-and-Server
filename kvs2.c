/*
* Brittany Arthur (bharthur)
* Jason Heron (jheron)
* Peter Froud (pfroud)
*/
//size = # of entires, length = # size of each thing
#include <stdio.h>
#include <stdlib.h> //malloc()
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include "kvs2.h"

/** FLAGS */
const int TOMBSTONE = 0xdeadd00d;
const int VALID     = 0xad00000b;
const int INVALID   = 0xda00000b;
const int METADATA  = 0xdaa0000b;

/** FUNCTION PROTOTYPES */
FILE* create_file(char* name, int size, int length);
FILE* access_file(char* name);
void populate(FILE* store, int table_entry_count, int table_entry_size);
int fetch_probe(FILE* store, char* key);
int insert_probe(FILE* store, char* key);
void insert_meta_data(FILE* store, char* key, int value, int table_entry_length, int index);
int get_table_entry_length(FILE* store);
int get_table_entry_count(FILE* store, int entry_length);
unsigned long hash(char *str);

/**
Open or create a hashtable.
Helper functions: create_file() and access_file().

name: the name of the hashtable.

Returns: pointer to file stream of the hashtable.
*/
FILE* initialize(char* name) {
	FILE* store;
	if(access(name, W_OK ) != -1)
		//file exists
		store = access_file(name);
	else
		//file does not exist
		store = create_file(name, 0, 0);
	return store;
}


/**
Add a key,value pair to the hashtable.
Helper functions: insert_probe() and get_table_entry_length().

store: pointer to file stream of the hashtable.
key: string of the key.
value: any data of the value.
length: length, in bytes, of the value.

Returns: the index it was inserted in, or -1 if error.
*/
int insert(FILE* store, char* key, void* value, int length) {
	//get the table length using metadata that is stored in the hash table
	int entry_length = get_table_entry_length(store);
	//printf("entry_length is : %d \n\n\n", entry_length);
	//int entry_count = get_table_entry_count(store, entry_length);
	//printf("entry_count is : %d \n\n\n", entry_count);

	if(key == NULL || value == NULL) {
		printf("Error: Cannot insert null values into hashtable.\n");
		return -1;
	}
	if(key_size+length+sizeof(TOMBSTONE) > (unsigned long)entry_length) {
		printf("Error: Data is too large.\n");
		return -1;
	}
	int index = insert_probe(store, key);
	printf("INSERTING INTO: %d\n", index);

	fseek(store, index*entry_length, SEEK_SET);

	int m = VALID;
	int* magic = &m;
	int *len_ptr = &length;

	fwrite(magic, sizeof(VALID), 1, store); //insert magic
	fwrite(key, key_size, 1, store); //insert key
	fwrite(len_ptr, sizeof(int), 1, store); //insert length
	fwrite(value, length, 1, store); //insert value
	return index;
}

//note: while condition can be a condition to check for a full hash table
/**
Helper function to insert().
Looks through a hashtable for the closest empty index for insertion and returns the index.
Helper functions: get_table_entry_length() and get_table_entry_count().

store: pointer to file stream of the hashtable.
key: string of the key.

Returns: index to insert the key into.
*/
int insert_probe(FILE* store, char* key) {
	//found index for new key
	int entry_length = get_table_entry_length(store);
	int entry_count = get_table_entry_count(store, entry_length);

	int index = hash(key)%entry_count;
	char key_buffer[key_size];
	int* flag = malloc(sizeof(int));
	assert(flag != NULL);
	fseek(store, index*entry_length, SEEK_SET);

	do {
		*flag = 0;
		fread(flag, sizeof(TOMBSTONE), 1, store); //read in possible flag number
		if(*flag == VALID) {
			//printf("found valid data!\n");
			fseek(store, (index*entry_length)+4, SEEK_SET); //offset flag number
			fread(key_buffer, key_size, 1, store);
			// only return valid if I find a key match exists at that location
			if(strcmp(key_buffer, key)==0) {
				//printf("found a match!\n");
				return index; // return the index to insert and replace
			}
			// if not the key - just keep going, keep looking for a place to insert
		} else if(*flag == INVALID) {
			//printf("found invalid data!\n");
			return index; // return the index to insert
		} else if(*flag == TOMBSTONE) {
			//printf("found tombstone!\n");
			return index; // return the index to insert
		}
		++index;
		index = index > entry_count ? 0 : index;
		fseek(store, index*entry_length, SEEK_SET); //find location of next flag number
	} while(*flag == TOMBSTONE || *flag == VALID); //search until there is a "null" slot

	free(flag);
	return -1; // this should never be reached if hashtable isn't 100% full.
}


/**
Get a key-value pair from the hashtable.
Helper functions: fetch_probe() and get_table_entry_length().

store: pointer to file stream of the hashtable.
key: the key of the entry to find.
value: pointer where value should be written to.
length: pointer to where the number of bytes should be written to.

Returns: the index the entry was found.
*/
int fetch(FILE* store, void* result, char* key, int* length) {
	int entry_length = get_table_entry_length(store);

	char key_buffer[entry_length]; //needs to be fixed size of key
	//TODO: need to probe here
	int index = fetch_probe(store, key);
	printf("FETCH PROBE RETURNED in fetch! index %d\n", index);

	fseek(store, (index*entry_length)+sizeof(TOMBSTONE), SEEK_SET);
	//read in key
	fread(key_buffer, key_size, 1, store);

	fread(length, sizeof(int), 1, store);
	//check if read fails?
	fread(result, *length, 1, store);
	return index;
}

/**
Helper function to fetch().
Looks through a hashtable for the key and returns the index.
Helper functions: get_table_entry_length() and get_table_entry_count().

store: pointer to file stream of the hashtable.
key: string of the key.

Returns: index of the entry with key, or -1 if not found.
*/
int fetch_probe(FILE* store, char* key) {
	int entry_length = get_table_entry_length(store);
	int entry_count = get_table_entry_count(store, entry_length);

	int index = hash(key)%entry_count;
	char key_buffer[key_size];
	int* magic = malloc(sizeof(int));
	assert(magic != NULL);
	fseek(store, index*entry_length, SEEK_SET);

	do {
		*magic = 0;
		fread(magic, sizeof(TOMBSTONE), 1, store); //read in possible magic number
		if(*magic == VALID) {
			//printf("found valid data!\n");
			fseek(store, (index*entry_length)+4, SEEK_SET); //offset magic number
			fread(key_buffer, key_size, 1, store);
			if(strcmp(key_buffer, key)==0) { //check for a key match at this index
				//printf("found a match!\n");
				return index;
			}
		} else if(*magic == INVALID) {
			//printf("found invalid data!\n");
			return -1;
		} else if(*magic == TOMBSTONE) {
			//printf("found tombstone!\n");
		}
		++index;
		index = index > entry_count ? 0 : index;
		fseek(store, index*entry_length, SEEK_SET); //find location of next magic number
	} while(*magic == TOMBSTONE || *magic == VALID); //search until there is a "null" slot

	free(magic);
	printf("DID NOT FIND %s\n",key);
	return -1;
}


/**------------------------HELPER FUNCTIONS--------------------------------*/

/**
Helper function to initialize().
Creates a file to hold the hashtable.
Helper function: populate().

name: filename of the table to create.

Returns: pointer to file stream of the hashtable.
*/
FILE* create_file(char* name, int size, int length) {
	FILE* store = fopen(name, "w+");
	populate(store, table_size, table_length);
	return store;
}

/**
Helper function to initialize().
Opens a file holding the hashtable.

name: filename of the table to open.

Returns: pointer to file stream of the hashtable.
*/
FILE* access_file(char* name) {
	FILE* store;
	store = fopen(name, "r+");
	return store;
}


/**
Helper function to create_file().
Fills a newly-created hashtable with 0s.
Helper function: insert_meta_data().

store: pointer to file stream of the hashtable.
table_entry_count: number of entries in the table.
table_entry_length: length of each entry.

Returns: nothing.
*/
void populate(FILE* store, int table_entry_count, int table_entry_length) {
	char filler = 0;
	int remaining_line_size = table_entry_length - sizeof(INVALID);
	int invalid_value = INVALID;
	int* invalid = &invalid_value;

	for(int i = 0; i < table_entry_count; i++) {
		//every slot will begin with "invalid" at initialize
		fwrite(invalid, sizeof(INVALID), 1, store);
		for(int k = 0; k < remaining_line_size; k++) {
			fwrite(&filler, sizeof(char), 1, store);
		}
		//BEFORE THE LOOP. TBD: WHY ISNT THIS EQUIVALENT TO ABOVE?
		//fwrite(&filler, sizeof(char)*multiply, 1, store);
	}

	insert_meta_data(store, TABLE_ENTRY_LENGTH, table_entry_length, table_entry_length, 0);
	insert_meta_data(store, TABLE_ENTRY_COUNT, table_entry_count, table_entry_length, 1);
}

/**
Helper function to populate().
Writes the entry length as the 0th entry in the table, and the number of entries as the 1st entry.

store: pointer to file stream of the hashtable.
key: either TABLE_ENTRY_LENGTH or TABLE_ENTRY_COUNT.
value:  either the length of each entry or the number of entries in the table.
table_entry_length: length of each entry, used for seeking.
index: either 0 or 1.

Returns: nothing.
*/
void insert_meta_data(FILE* store, char* key, int value, int table_entry_length, int index) {
	//storing at index 0 and at index 1
	//first storing entry length
	fseek(store, index*table_entry_length, SEEK_SET);
	//insert metaflag
	int meta_value_flag = METADATA;
	int* meta_value_flag_ptr = &meta_value_flag;
	fwrite(meta_value_flag_ptr, sizeof(METADATA), 1, store);
	//insert key
	int keysize = (strlen(key)+1)*sizeof(char);
	fwrite(key, keysize, 1, store);
	//insert length of value
	int val_len = sizeof(int);
	int* val_len_ptr = &val_len;
	fwrite(val_len_ptr, sizeof(int), 1, store);
	//insert value
	int meta_value = value;
	int* meta_value_ptr = &meta_value;
	fwrite(meta_value_ptr, sizeof(int), 1, store);
}

/**
Helper function to insert_probe(), fetch_probe(), insert(), and fetch().
Reads the entry length from metadata.

store: pointer to file stream of the hashtable.

Returns: the entry length of the hashtable.
*/
int get_table_entry_length(FILE* store) {
	//offset = sizeof entry_key + sizeof flag + sizeof value
	int offset = sizeof(METADATA) + (strlen(TABLE_ENTRY_LENGTH)+1)*sizeof(char) + sizeof(int);
	fseek(store, offset, SEEK_SET);
	int entry_len = 0;
	int* entry_len_ptr = &entry_len;
	fread(entry_len_ptr, sizeof(int), 1, store);
	return entry_len;
}

/**
Helper function to insert_probe() and fetch_probe().
Reads the number of entries from metadata.

store: pointer to file stream of the hashtable.
entry_length: the length of each entry.

Returns: the number of entries in the hashtable.
*/
int get_table_entry_count(FILE* store, int entry_length) {
	//offset = sizeof entry_key + sizeof flag + sizeof value + skip the old entry
	int offset = sizeof(METADATA) + (strlen(TABLE_ENTRY_COUNT)+1)*sizeof(char) + sizeof(int) + entry_length;
	fseek(store, offset, SEEK_SET);
	int entry_ct = 0;
	int* entry_ct_ptr = &entry_ct;
	fread(entry_ct_ptr, sizeof(int), 1, store);
	return entry_ct;
}


/*-----------------SECOND CLASS AND TODO FUNCTIONS---------------------------*/

/**
Removed a file from the hashtable.

store: pointer to file stream of the hashtable.
key: string of key for entry to delete.

Returns: 0 on success.
*/
int delete(FILE* store, char* key) {
	int entry_length = get_table_entry_length(store);
	/*probe for valid entry with matching key*/
	int index = fetch_probe(store, key);
	printf("FETCH PROBE FOUND! index %d\n", index);
	printf("DELETING %s\n",key);

	/*mark magic number as DEADD00D*/
	fseek(store, index*entry_length, SEEK_SET);
	int tombstone_flag = TOMBSTONE;
	int* tombstone_flag_ptr = &tombstone_flag;
	fwrite(tombstone_flag_ptr, sizeof(TOMBSTONE), 1, store);

	return 0;
}


/**
djb2 hash algorithm by Dan Bernstein, from http://www.cse.yorku.ca/~oz/hash.html.

str: string to generate hash of.

Returns: hash of the input string.
*/
unsigned long hash(char *str) {
	unsigned long hash = 5381;
	int c;

	while ((c = *str++))
		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
	return hash;
}
