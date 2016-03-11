

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
	unsigned long long int byteProcessed;
	unsigned long long int byteEncountered;
};

typedef struct inv_stat_ InversionStat;





long int fsbininv_setStartPosition(FILE *pFile, unsigned long long int fileSize, unsigned int skipPercent);

int fsbininv_invertFileBits(char *buffer, unsigned int buf_size, FILE *pFile, InversionStat *invStat);



double stat_pctgFailedFiles(const InversionStat *invStat);

double stat_pctgByteProcessed(const InversionStat *invStat);



void util_alignFileSize(unsigned long long int fileSize, char *outFileSizeBuffer);

void util_alignStreamSize(unsigned long long int strmSize, char *outStrmSizeBuffer);







#endif
