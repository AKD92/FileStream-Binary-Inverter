
#include <stdio.h>
#include <stdlib.h>
#include "Bit_Invert.h"

int						eof_val;
size_t					read_count;
unsigned long int		total_bytes_s;
fpos_t					prev_pos;
fpos_t					first_pos;



void position(FILE *f, const unsigned int *fileSize) {
	
	first_pos = *fileSize / 4;
	fsetpos(f, &first_pos);
	
/*	printf("Size %d, size/4 = %d\n", *fileSize, first_pos);*/
	return;
}



int invert_bit(char *buffer, unsigned int bufileSize, FILE *f, STAT *st) {
	
	register unsigned int j;
	eof_val = 0;
	prev_pos = 0;
	read_count = 0;
/*	int write_items = 0;*/
	
	
/*	while(!eof_val) {*/
	while (!eof_val) {
		
		read_count = fread(buffer, 1, bufileSize, f);
		
		eof_val = feof(f);
/*		printf("\treads %d, end-of-file %d\n", read_count, eof_val);*/
		
		fgetpos(f, &prev_pos);
/*		printf("\tcurrent pos %d\n", prev_pos);*/
		
		prev_pos -= read_count;
		fsetpos(f, &prev_pos);
/*		printf("\tnew pos %d\n", prev_pos);*/
		
		for (j=0; j < read_count; j++) {
			*(buffer + j) = ~(*(buffer + j));
		}
		fwrite(buffer, 1, read_count, f);
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
