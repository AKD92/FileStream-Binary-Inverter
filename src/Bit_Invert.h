#ifndef FBI_H
#define FBI_H

#define ERROR_READ 2
#define ERROR_WRITE 3
#define ERROR_OTHER 1



struct stat_ {
	unsigned int totalFiles;
	unsigned int processedFiles;
	unsigned long long int byteProcessed;
	unsigned long long int byteEncountered;
};

typedef struct stat_ STAT;

long int position(FILE *f, const unsigned int *fileSize, unsigned int skip_p);
int invert_bit(char *buffer, unsigned int buf_size, FILE *f, STAT *statProgress);

unsigned int stat_countFailedFiles(STAT *statProgress);
unsigned int stat_percentageFailedFiles(STAT *statProgress);


#endif
