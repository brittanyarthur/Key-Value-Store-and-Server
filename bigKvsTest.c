
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

	int randNum, result, bytesRead, numMatch, numFail, idxIns, idxFetch;
	randNum = result = bytesRead = numMatch = numFail = idxIns = idxFetch = 0;

	FILE* table = initialize("bigTestTable");
	assert(table != NULL);

	char keyBuff[10];



	for(int i=0; i< atoi(argv[1]); i++) {
		randNum = (int)random();
		result = 0;

		//make a string out of i
		sprintf(keyBuff, "%d", i);

		//insert
		idxIns = insert(table, keyBuff, &randNum, sz);
		//printf("Wrote %d in %d bytes to   slot %d.\n", randNum, sz, idxIns);

		//fetch
		idxFetch = fetch(table, &result, keyBuff, &bytesRead);
		//printf("Read  %d in %d bytes from slot %d.\n", result, bytesRead, idxFetch);

		//interpert result
		if(randNum == result && idxFetch == idxIns) {
			numMatch++;
		} else {
			numFail++;
			printf("FAIL: wrote %d to slot %d, got %d from slot %d.\n", randNum, idxIns, result, idxFetch);
		}

	}

	//print result
	printf("%d matched, %d failed.\n", numMatch, numFail);




	return EXIT_SUCCESS;
}





