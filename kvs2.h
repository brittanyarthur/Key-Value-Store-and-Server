/*
* Brittany Arthur (bharthur)
* Jason Heron (jheron)
* Peter Froud (pfroud)
*/

#ifndef KVS2_H_
#define KVS2_H_

/**
Open or create a hashtable.

name: the name of the hashtable.

Returns: pointer to file stream of the hashtable.
*/
FILE* initialize(char* name, int size, int length);


/*
Open a pre-existing file

name: the name of the hashtable.

Returns: pointer to file stream of the hashtable.
*/
FILE* get_hashfile(char* name);

/**
Get a key-value pair from the hashtable.

store: pointer to file stream of the hashtable.
key: the key of the entry to find.
value: poitner where value should be written to.
length: pointer to where the number of bytes should be written to.

Returns: the index the entry was found.
*/
int fetch(FILE* store, void* result, char* key, int* length);

/**
Add a key,value pair to the hashtable.

store: pointer to file stream of the hashtable.
key: string of the key.
value: any data of the value.
length: length, in bytes, of the value.

Returns: the index it was inserted in, or -1 if error.
*/
int insert(FILE* store, char* key, void* value, int length);

/**
Remove a key-value pair from the hashtable.

store: pointer to file stream of the hashtable.
key: string of the key to delete.

Returns: 0 on success.
*/
int delete(FILE* store, char* key);


#define table_size         360
#define table_length       512
#define key_size           25
#define flag_size          4
#define max_value_size     table_length - key_size - flag_size
#define TABLE_ENTRY_COUNT  "table_entry_count"
#define TABLE_ENTRY_LENGTH "table_entry_length"

#endif
