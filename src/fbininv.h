

/************************************************************************************
	Interface of FileStream Binary Inversion routines
	Author:             Ashis Kumar Das
	Email:              akd.bracu@gmail.com
*************************************************************************************/



#ifndef FILEBINARYINVERT_H
#define FILEBINARYINVERT_H




#define ERROR_READ					2
#define ERROR_WRITE					3
#define ERROR_OTHER					1



struct inv_stat_ {
	unsigned int totalFiles;
	unsigned int processedFiles;
	unsigned long int byteProcessed;
	unsigned long int byteEncountered;
};

typedef struct inv_stat_ InversionStat;





long int fbininv_setStartPosition(FILE *pFile, const unsigned int *fileSize, unsigned int skipPercent);

int fbininv_invertFileBits(char *buffer, unsigned int buf_size, FILE *pFile, InversionStat *invStat);



double stat_prctFailedFiles(const InversionStat *invStat);

double stat_prctByteProcessed(const InversionStat *invStat);



void util_alignFileSize(unsigned int fileSize, char *outFileSizeBuffer);

void util_alignStreamSize(unsigned long int strmSize, char *outStrmSizeBuffer);







#endif
