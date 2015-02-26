
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

	int randNum, result, bytesRead, numMatch, numFail = 0;

	FILE* table = initialize("bigTestTable");
	assert(table != NULL);

	char keyBuff[10];



	for(int i=0; i< atoi(argv[1]); i++) {
		randNum = (int)random();
		result = 0;

		//make a string out of i
		sprintf(keyBuff, "%d", i);

		//insert
		insert(table, keyBuff, &randNum, sz);
		//printf("Wrote %d in %d bytes.\n", randNum, sz);

		//fetch
		fetch(table, &result, keyBuff, &bytesRead);
		//printf("Read  %d in %d bytes.\n", result, bytesRead);

		//interpert result
		if(randNum == result) {
			numMatch++;
		} else {
			numFail++;
			printf("FAIL: wrote %d, got %d back.\n", randNum, result);
		}
	}

	//print result
	printf("%d matched, %d failed.\n", numMatch, numFail);




	return EXIT_SUCCESS;
}





