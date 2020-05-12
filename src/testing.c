/* 
 * File:   testing.c
 * Author: Javier Marquez Ruiz <89javier98@uma.es>
 *
 * Created on 7 of May of 2020
 * Used to test the libraries given in VC and 
 * practise some caching
 */

//Javier Marquez Ruiz

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include "tableDB.h"
#include "tableCache.h"

int main(){
	//Getting used with types & variables
	TC_tableEntry_t tableEntry1, tableEntry2;
	tableEntry1.id = 0;
	tableEntry1.used = 0;
	tableEntry2.id = 1;
	tableEntry2.used = 0;

	printf("Size of the table entry: %lu bytes\n", sizeof(TC_tableEntry_t));
	printf("Size of text in table entry: %lu bytes\n", sizeof(tableEntry1.text));
	
	TC_cacheBlock_t cacheBlock1;
	cacheBlock1.entries[0] = tableEntry1;
	cacheBlock1.entries[1] = tableEntry2;
	printf("Size of the cache block: %lu bytes\n", sizeof(TC_cacheBlock_t));
	//printf("Table entries per block: %lu\n", sizeof(TC_cacheBlock_t)/sizeof(TC_tableEntry_t));
	printf("Actual table entries per block: %i\n", TC_BLOCK_ENTRIES);
	
	//What can I do with so_dbtable.dat?
	FILE * f = fopen("so_dbtable.dat", "r");
	fclose(f);

	return 	EXIT_SUCCESS;
}
