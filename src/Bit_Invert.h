#ifndef FBI_H
#define FBI_H

#include "DTypes.h"

#define ERROR_READ 2
#define ERROR_WRITE 3
#define ERROR_OTHER 1



struct stat_ {
	UINT no_files;
	UINT no_processed_files;
	UINT_L t_size_processed;
	UINT_L t_size_encountered;
};

typedef struct stat_ STAT;

void position(FILE* , const UINT* );
int invert_bit(UCHAR* , UINT , FILE* , STAT*);

void stat_init(STAT*);
void stat_files_failed_n(STAT*, UINT*);
void stat_files_failed_p(STAT*, UINT*);


#endif
