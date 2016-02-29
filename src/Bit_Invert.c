
#include <stdio.h>
#include <stdlib.h>
#include "Bit_Invert.h"

int						eof_val;
size_t					read_count;
unsigned long int		total_bytes_s;



long int position(FILE *f, const unsigned int *fileSize, unsigned int skip_p) {
	
	/* skip_p means skip_percentage, value must be 0 to 100 */
	
	long int filePosition;
	
	filePosition = ((long int) *fileSize * (long int) skip_p) / 100L;
	fseek(f, filePosition, SEEK_SET);
	
	return filePosition;
}



int invert_bit(char *buffer, unsigned int bufileSize, FILE *f, STAT *st) {
	
	register unsigned int j;
	size_t singleByte;
	long int readFrom;
	
	readFrom = 0;
	singleByte = 1;
	eof_val = 0;
	
	while (eof_val == 0) {
		
		read_count = fread((void *) buffer, singleByte, (size_t) bufileSize, f);
		
		eof_val = feof(f);
		readFrom = ftell(f);
		
		readFrom -= read_count;
		fseek(f, readFrom, SEEK_SET);
		
		for (j = 0; j < read_count; j++) {
			*(buffer + j) = ~(*(buffer + j));			/* Invert all bits in a byte */
		}
		
		fwrite((void *) buffer, singleByte, read_count, f);
		fflush(f);
		
		st->byteProcessed += read_count;
	}
	
	return 0;
}


unsigned int stat_countFailedFiles(STAT *st) {
	return st->totalFiles - st->processedFiles;
}

unsigned int stat_percentageFailedFiles(STAT *st) {

	unsigned int returnValue;
	returnValue = stat_countFailedFiles(st);
	return (unsigned int) ((double) returnValue/ (double)st->totalFiles * 100);
}
