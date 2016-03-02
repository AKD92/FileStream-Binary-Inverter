

/************************************************************************************
	Implementation of FileStream Bit Inversion routines
	Author:             Ashis Kumar Das
	Email:              akd.bracu@gmail.com
*************************************************************************************/



#include <stdio.h>
#include <stdlib.h>
#include "fbitinv.h"



#define BYTE_SINGLE					1






unsigned int stat_percentageFailedFiles(STAT *st) {

	unsigned int numFailedFiles;
	unsigned int pcntFailedFiles;
	
	numFailedFiles = st->totalFiles - st->processedFiles;
	pcntFailedFiles = (unsigned int) ((double) numFailedFiles / (double) st->totalFiles * 100);
	
	return pcntFailedFiles;
}



long int fbitinv_setStartPosition(FILE *f, const unsigned int *fileSize, unsigned int skipPercent) {
	
	long int filePosition;
	
	/* Calculate actual bits to be skipped */
	filePosition = ((long int) *fileSize * (long int) skipPercent) / 100L;
	
	/* Set current file pointer to the calculated value */
	fseek(f, filePosition, SEEK_SET);
	
	return filePosition;
}



int fbitinv_invertFileBits(char *dataBuffer, unsigned int bufferSize, FILE *pFile, STAT *st) {
	
	register size_t readCount;
	register int isEOF;
	register unsigned int index;
	register long int readPointer;
	
	isEOF = 0;
	readPointer = 0;
	
	while (isEOF == 0) {
		
		/* READ some bytes and store those to the dataBuffer */
		readCount = fread((void *) dataBuffer, BYTE_SINGLE, (size_t) bufferSize, pFile);
		
		/* Check if End Of File (EOF) has occured */
		isEOF = feof(pFile);
		
		/* Obtain the current file pointer after a READ */
		readPointer = ftell(pFile);
		
		/* Set file pointer to the previus position (before this READ) */
		readPointer = readPointer - readCount;
		fseek(pFile, readPointer, SEEK_SET);
		
		
		/* Invert all bits those reside on the dataBuffer */
		index = 0;
		while (index < readCount) {
			*(dataBuffer + index) = ~(*(dataBuffer + index));
			index = index + 1;
		}
		
		/* WRITE the mutated (inverted) bytes from dataBuffer to File */
		fwrite((void *) dataBuffer, BYTE_SINGLE, readCount, pFile);
		
		/* Flush the file for writing into actual output device (hardware access) */
		fflush(pFile);
		
		/* Add the number of bytes already processed to our STAT object */
		st->byteProcessed += (unsigned long long int) readCount;
	}
	
	return 0;
}
