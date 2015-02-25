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

#endif
