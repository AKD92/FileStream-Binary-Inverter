

/************************************************************************************
	Interface of FileStream Bit Inversion routines
	Author:             Ashis Kumar Das
	Email:              akd.bracu@gmail.com
*************************************************************************************/



#ifndef FILEBITINVERT_H
#define FILEBITINVERT_H




#define ERROR_READ					2
#define ERROR_WRITE					3
#define ERROR_OTHER					1



struct stat_ {
	unsigned int totalFiles;
	unsigned int processedFiles;
	unsigned long long int byteProcessed;
	unsigned long long int byteEncountered;
};
typedef struct stat_ STAT;





long int fbitinv_setStartPosition(FILE *pFile, const unsigned int *fileSize, unsigned int skipPercent);

int fbitinv_invertFileBits(char *buffer, unsigned int buf_size, FILE *pFile, STAT *statProgress);

unsigned int stat_percentageFailedFiles(STAT *statProgress);







#endif
