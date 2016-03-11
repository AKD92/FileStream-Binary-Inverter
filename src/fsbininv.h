

/************************************************************************************
	Interface of FileStream Binary Inversion routines
	Author:             Ashis Kumar Das
	Email:              akd.bracu@gmail.com
*************************************************************************************/



#ifndef FILEBINARYINVERT_H
#define FILEBINARYINVERT_H



#include <stdint.h>


#define ERROR_READ					2
#define ERROR_WRITE					3
#define ERROR_OTHER					1



struct inv_stat_ {
	uint32_t totalFiles;
	uint32_t processedFiles;
	uint64_t byteProcessed;
	uint64_t byteEncountered;
};

typedef struct inv_stat_ InvStat;





int fsbininv_setStartPosition(FILE *pFile, uint64_t fileSize, unsigned int skipPercent);

int fsbininv_invertFileBits(char *buffer, unsigned int buf_size, FILE *pFile, InvStat *invStat);



double stat_pctgFailedFiles(const InvStat *invStat);

double stat_pctgByteProcessed(const InvStat *invStat);



void util_alignFileSize(uint64_t fileSize, char *outFileSizeBuffer);

void util_alignStreamSize(uint64_t strmSize, char *outStrmSizeBuffer);







#endif
