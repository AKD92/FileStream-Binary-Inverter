#ifndef WINFILEFIND_H
#define WINFILEFIND_H

#include <windows.h>
#include "DTypes.h"
#include "queue.h"

#define MAKE_PATH_MASK "%s\\*.*"
#define MAKE_PATH_ABSOLUTE "%s\\%s"
#define PRNT_DIR "Directory: %s\n"
#define PRNT_FILE "File: %s\n"

int search_files(const char *sDir, Queue *file_queue);

void loc_file_name(void*, UCHAR**);
void loc_file_size(void*, UINT**);
void loc_file_attr(void*, UINT**);

/* returns 0 if safe, 1 or more if unsafe file attribute */
int issafe(UINT*);

#endif
