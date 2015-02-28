#ifndef KVS2_H_
#define KVS2_H_

FILE* initialize(char* name, int table_size, int table_length);
int fetch(FILE* store, void* result, char* key, int* length);
int insert(FILE* store, char* key, void* value, int length);
int get_table_max_value(FILE* store);
unsigned long hash(char *str); //REMOVE THIS FROM HEADER 

int delete(char* key);


#define key_size           25
#define flag_size          4
#define TABLE_ENTRY_COUNT  "table_entry_count"
#define TABLE_ENTRY_LENGTH "table_entry_length"

#endif
