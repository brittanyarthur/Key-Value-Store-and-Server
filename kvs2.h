/*
* Brittany Arthur (bharthur)
* Jason Heron (jheron)
* Peter Froud (pfroud)
*/

#ifndef KVS2_H_
#define KVS2_H_

FILE* initialize(char* name);

int fetch(FILE* store, void* result, char* key, int* length);
int insert(FILE* store, char* key, void* value, int length);
unsigned long hash(char *str); //REMOVE THIS FROM HEADER
int delete(FILE* store, char* key);


#define table_size         360
#define table_length       512
#define key_size           25
#define flag_size          4
#define max_value_size     table_length - key_size - flag_size
#define TABLE_ENTRY_COUNT  "table_entry_count"
#define TABLE_ENTRY_LENGTH "table_entry_length"

#endif
