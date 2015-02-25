#ifndef KVS2_H_
#define KVS2_H_

FILE* initialize(char* name);
FILE* create_file(char* name);
FILE* access_file(char* name);
int fetch(FILE* store, void* result, char* key, int* length);
void read_int_array(FILE* store, char* key, int length);
void populate(FILE* store);
int insert(FILE* store, char* key, void* value, int length);
void read_char_array(FILE* store, char* key, int length);
int fetch_probe(FILE* store, char* key);
int insert_probe(FILE* store, char* key);
int delete(char* key);
unsigned long hash(char *str);

#define table_size        360
#define table_length      512
#define key_size          25
#define flag_size         4
#define max_value_size    table_length - key_size - flag_size

#endif
