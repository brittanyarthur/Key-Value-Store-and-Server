
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "kvs2.h"


#define sz ((int)sizeof(int))


int main(int argc, char** argv){

	if(argc != 2) {
		printf("usage: bigTest numInsertions\n");
		return EXIT_FAILURE;
	}

	FILE* table = initialize("bigTestTable");
	assert(table != NULL);

	char keyBuff[10];

	int randNum, result, bytesRead;
	randNum = result, bytesRead = -1;

	for(int i=0; i< atoi(argv[1]); i++) {
		randNum = (int)random();
		result = -1;

		sprintf(keyBuff, "%d", i); //make a string out of i


		insert(table, keyBuff, &randNum, sz);
		printf("Wrote %d in %d bytes.\n", randNum, sz);


		fetch(table, &result, keyBuff, &bytesRead);
		printf("Read %d in %d bytes.\n", result, bytesRead);
	}




	return EXIT_SUCCESS;
}





