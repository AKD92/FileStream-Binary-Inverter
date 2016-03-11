

/************************************************************************************
	Implementation of FileStream Binary Inversion routines
	Author:             Ashis Kumar Das
	Email:              akd.bracu@gmail.com
*************************************************************************************/



#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include "fsbininv.h"



#define BYTE_SINGLE						1

#define ONE_KB							1024
#define ONE_MB							ONE_KB * 1024
#define ONE_GB							ONE_MB * 1024
#define ONE_TB							ONE_GB * 1024

#define FILESIZE_BYTES					"%4" PRIu64 " Bytes"
#define FILESIZE_KB						"%7.2f KB"
#define FILESIZE_MB						"%7.2f MB"
#define FILESIZE_GB						"%7.2f GB"
#define FILESIZE_TB						"%7.2f TB"

#define STREAMSIZE_BYTES				"%" PRIu64 " Bytes"
#define STREAMSIZE_KB					"%.2f KB"
#define STREAMSIZE_MB					"%.2f MB"
#define STREAMSIZE_GB					"%.2f GB"
#define STREAMSIZE_TB					"%.2f TB"





void util_alignFileSize(uint64_t fileSize, char *outFileSizeBuffer) {
	
	double fSize, dUnit;
	
	if (fileSize >= ONE_GB) {
		
		fSize = (double) fileSize;
		dUnit = (double) ONE_GB;
		fSize = fSize / dUnit;
		sprintf(outFileSizeBuffer, FILESIZE_GB, fSize);
	}
	else if (fileSize >= ONE_MB) {
		
		fSize = (double) fileSize;
		dUnit = (double) ONE_MB;
		fSize = fSize / dUnit;
		sprintf(outFileSizeBuffer, FILESIZE_MB, fSize);
	}
	else if (fileSize >= ONE_KB) {
		
		fSize = (double) fileSize;
		dUnit = (double) ONE_KB;
		fSize = fSize / dUnit;
		sprintf(outFileSizeBuffer, FILESIZE_KB, fSize);
	}
	else {
		sprintf(outFileSizeBuffer, FILESIZE_BYTES, fileSize);
	}
	
	return;
}



void util_alignStreamSize(uint64_t strmSize, char *outStrmBuffer) {
	
	double sSize, dUnit;
	
	if (strmSize >= ONE_GB) {
		
		sSize = (double) strmSize;
		dUnit = (double) ONE_GB;
		sSize = sSize / dUnit;
		sprintf(outStrmBuffer, STREAMSIZE_GB, sSize);
	}
	else if (strmSize >= ONE_MB) {
		
		sSize = (double) strmSize;
		dUnit = (double) ONE_MB;
		sSize = sSize / dUnit;
		sprintf(outStrmBuffer, STREAMSIZE_MB, sSize);
	}
	else if (strmSize >= ONE_KB) {
		
		sSize = (double) strmSize;
		dUnit = (double) ONE_KB;
		sSize = sSize / dUnit;
		sprintf(outStrmBuffer, STREAMSIZE_KB, sSize);
	}
	else {
		sprintf(outStrmBuffer, STREAMSIZE_BYTES, strmSize);
	}
	
	return;
}



double stat_pctgFailedFiles(const InvStat *st) {

	uint32_t numFailedFiles;
	double pcntFailedFiles;
	double filesFailed;
	double filesTotal;
	
	numFailedFiles = st->totalFiles - st->processedFiles;
	filesFailed = (double) numFailedFiles;
	filesTotal = (double) st->totalFiles;
	
	pcntFailedFiles = (filesFailed / filesTotal) * 100.0;
	
	return pcntFailedFiles;
}



double stat_pctgByteProcessed(const InvStat *st) {
	
	double pcntByteProcessed;
	double byteProcessed;
	double byteEncountered;
	
	byteProcessed = (double) st->byteProcessed;
	byteEncountered = (double) st->byteEncountered;
	
	pcntByteProcessed = (double) ((byteProcessed / byteEncountered) * 100.0);
	
	return pcntByteProcessed;
}



int fsbininv_setStartPosition(FILE *pFile, uint64_t fileSize, unsigned int skipPercent) {
	
	off64_t filePosition;
	
	if (skipPercent >= 100U)
		return -1;
	
	/* Calculate actual bits to be skipped */
	filePosition = (off64_t) (fileSize * skipPercent) / UINT64_C(100);
	
	/* Set current file pointer to the calculated value */
	fseeko64(pFile, filePosition, SEEK_SET);
	
	return 0;
}



int fsbininv_invertFileBits(char *dataBuffer, unsigned int bufferSize, FILE *pFile, InvStat *st) {
	
	register size_t readCount;
	register int isEOF;
	register unsigned int index;
	register off64_t readPointer;
	
	isEOF = 0;
	readPointer = 0;
	
	while (isEOF == 0) {
		
		/* READ some bytes and store those to the dataBuffer */
		readCount = fread((void *) dataBuffer, BYTE_SINGLE, (size_t) bufferSize, pFile);
		
		/* Check if End Of File (EOF) has occured */
		isEOF = feof(pFile);
		
		/* Obtain the current file pointer after a READ */
		readPointer = ftello64(pFile);
		
		/* Set file pointer to the previus position (before this READ) */
		readPointer = readPointer - (off64_t) readCount;
		fseeko64(pFile, readPointer, SEEK_SET);
		
		
		/* Invert all bits those reside on the dataBuffer */
		index = 0;
		while (index < readCount) {
			*(dataBuffer + index) = ~(*(dataBuffer + index));
			index = index + 1;
		}
		
		/* WRITE the mutated (inverted) bytes from dataBuffer to File */
		fwrite((void *) dataBuffer, BYTE_SINGLE, readCount, pFile);
		
		/* Flush the file for synchronizing with output device (hardware access) */
		fflush(pFile);
		
		/* Add the number of bytes already processed to our InvStat object */
		st->byteProcessed += (uint64_t) readCount;
	}
	
	return 0;
}
