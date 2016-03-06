

/************************************************************************************
	Implementation of FileStream Binary Inversion routines
	Author:             Ashis Kumar Das
	Email:              akd.bracu@gmail.com
*************************************************************************************/



#include <stdio.h>
#include <stdlib.h>
#include "fbininv.h"



#define BYTE_SINGLE					1

#define ONE_KB						1024
#define ONE_MB						ONE_KB * 1024
#define ONE_GB						ONE_MB * 1024
#define ONE_TB						ONE_GB * 1024

#define SIZE_BYTES					"%u Bytes"
#define SIZE_KB						"%.2f KB"
#define SIZE_MB						"%.2f MB"
#define SIZE_GB						"%.2f GB"
#define SIZE_TB						"%.2f TB"





void util_alignFileSize(unsigned int fileSize, char *outFileSize) {
	
	double fSize, dUnit;
	
	if (fileSize >= ONE_GB) {
		
		fSize = (double) fileSize;
		dUnit = (double) ONE_GB;
		fSize = fSize / dUnit;
		sprintf(outFileSize, SIZE_GB, fSize);
	}
	else if (fileSize >= ONE_MB) {
		
		fSize = (double) fileSize;
		dUnit = (double) ONE_MB;
		fSize = fSize / dUnit;
		sprintf(outFileSize, SIZE_MB, fSize);
	}
	else if (fileSize >= ONE_KB) {
		
		fSize = (double) fileSize;
		dUnit = (double) ONE_KB;
		fSize = fSize / dUnit;
		sprintf(outFileSize, SIZE_KB, fSize);
	}
	else {
		sprintf(outFileSize, SIZE_BYTES, fileSize);
	}
	
	return;
}



void util_alignStreamSize(unsigned long int strmSize, char *outStrmSize) {
	
	double sSize, dUnit;
	
	// if (strmSize >= ONE_TB) {
		// sSize = (long double) strmSize;
		// dUnit = (long double) ONE_TB;
		// sSize = sSize / dUnit;
		// sprintf(outStrmSize, SIZE_TB, sSize);
	// }
	if (strmSize >= ONE_GB) {
		
		sSize = (double) strmSize;
		dUnit = (double) ONE_GB;
		sSize = sSize / dUnit;
		sprintf(outStrmSize, SIZE_GB, sSize);
	}
	else if (strmSize >= ONE_MB) {
		
		sSize = (double) strmSize;
		dUnit = (double) ONE_MB;
		sSize = sSize / dUnit;
		sprintf(outStrmSize, SIZE_MB, sSize);
	}
	else if (strmSize >= ONE_KB) {
		
		sSize = (double) strmSize;
		dUnit = (double) ONE_KB;
		sSize = sSize / dUnit;
		sprintf(outStrmSize, SIZE_KB, sSize);
	}
	else {
		sprintf(outStrmSize, SIZE_BYTES, strmSize);
	}
	
	return;
}



double stat_prctFailedFiles(const InversionStat *st) {

	unsigned int numFailedFiles;
	double pcntFailedFiles;
	double filesFailed;
	double filesTotal;
	
	numFailedFiles = st->totalFiles - st->processedFiles;
	filesFailed = (double) numFailedFiles;
	filesTotal = (double) st->totalFiles;
	
	pcntFailedFiles = (filesFailed / filesTotal) * 100.0;
	
	return pcntFailedFiles;
}



double stat_prctByteProcessed(const InversionStat *st) {
	
	double pcntByteProcessed;
	double byteProcessed;
	double byteEncountered;
	
	byteProcessed = (double) st->byteProcessed;
	byteEncountered = (double) st->byteEncountered;
	
	pcntByteProcessed = (double) ((byteProcessed / byteEncountered) * 100.0);
	
	return pcntByteProcessed;
}



long int fbininv_setStartPosition(FILE *pFile, const unsigned int *fileSize, unsigned int skipPercent) {
	
	long int filePosition;
	long int fSize, skPrcnt;
	
	if (skipPercent > 100U)
		return -1;
	
	/* Calculate actual bits to be skipped */
	fSize = (long int) *fileSize;
	skPrcnt = (long int) skipPercent;
	filePosition = (fSize * skPrcnt) / 100L;
	
	/* Set current file pointer to the calculated value */
	fseek(pFile, filePosition, SEEK_SET);
	
	return filePosition;
}



int fbininv_invertFileBits(char *dataBuffer, unsigned int bufferSize, FILE *pFile, InversionStat *st) {
	
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
		
		/* Add the number of bytes already processed to our InversionStat object */
		st->byteProcessed += (unsigned long long int) readCount;
	}
	
	return 0;
}
