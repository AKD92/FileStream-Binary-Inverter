
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#include "WinFileFind.h"

#define S_DOT "."
#define D_DOT ".."


UINT file_size(WIN32_FIND_DATA *f_data);

int search_files_file(const char *sFile, Queue *file_queue);
int search_files_dir(const char *sDir, Queue *file_queue);

int append_file_info(const char *sFile, const UINT *fSize, const UINT *fattr, Queue *file_queue);


int search_files(const char *objLoc, Queue *file_queue) {
	
	DWORD attr;
	
	attr = GetFileAttributes(objLoc);
	
	if (attr == INVALID_FILE_ATTRIBUTES) return -1;
	
	if (attr & FILE_ATTRIBUTE_DIRECTORY) {
		return search_files_dir(objLoc, file_queue);
	} else {
		return search_files_file(objLoc, file_queue);
	}
}

int search_files_file(const char *sFile, Queue *file_queue) {
	
	DWORD hFile;
	DWORD fSize;
	DWORD fAttr;
	int res;
	
	hFile = CreateFile(sFile, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) return -1;
	
	else {
		
		fSize = GetFileSize(hFile, NULL);
		CloseHandle(hFile);
		fAttr = GetFileAttributes(sFile);
		
		res = append_file_info(sFile, (UINT*)&fSize, (UINT*)&fAttr, file_queue);
		return res;
	}
}

int search_files_dir(const char *sDir, Queue *file_queue) {
	
	WIN32_FIND_DATA fdFile;
	HANDLE hFind;
	
	char sPath[2048];
	UINT fSize;	
	
	int valNextFile = 0;
	int file_count = 0;
	
	DWORD qual_val = 0;
	
/*	Specify a file mask. *.* = We want everything!*/
	sprintf(sPath, MAKE_PATH_MASK, sDir);
	
	hFind = FindFirstFile(sPath, &fdFile);
	
	if(hFind == INVALID_HANDLE_VALUE)
	{
		return -1;
	} else {
		
		do {
			if(strcmp(fdFile.cFileName, S_DOT)
				&& strcmp(fdFile.cFileName, D_DOT)) {
					
					sprintf(sPath, MAKE_PATH_ABSOLUTE, sDir, fdFile.cFileName);
					
					if (fdFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
/*						printf(PRNT_DIR, sPath);*/
						search_files_dir(sPath, file_queue);
					} else {
						
/*						isQualified(&(fdFile.dwFileAttributes), &qual_val);*/
/*						if (!qual_val) {*/
							
/*							printf(PRNT_FILE, sPath);*/
							
							fSize = file_size(&fdFile);
							append_file_info(sPath, &fSize, &fdFile.dwFileAttributes, file_queue);
							
							file_count++;
/*						}*/
					}
				}
			valNextFile = FindNextFile(hFind, &fdFile);
			
		} while (valNextFile);
	}
	FindClose(hFind);
	return file_count;
}

inline int append_file_info(const char *sFile, const UINT *fSize, const UINT *fattr, Queue *file_queue)
{
	UINT size_total;
	
	void *alloc_mem;
	UCHAR *f_name_loc;
	UINT *f_size_loc;
	UINT *f_attr_loc;
	
	size_total = (2 * sizeof(UINT)) + strlen(sFile) + 1;
	alloc_mem = malloc(size_total);
	if (alloc_mem == NULL) return -1;
	
	loc_file_size(alloc_mem, &f_size_loc);
	loc_file_attr(alloc_mem, &f_attr_loc);
	loc_file_name(alloc_mem, &f_name_loc);
	
	*f_size_loc = *fSize;
	*f_attr_loc = *fattr;
	strcpy((char*)f_name_loc, sFile);
	
/*	printf("%s [%u]\n", f_name_loc, *f_size_loc);*/
	
	queue_enqueue(file_queue, alloc_mem);
	
	return 0;
	
}


inline int issafe(UINT *attr) {
	
/*	static UINT i, j;*/
	
/*	i = *attr & FILE_ATTRIBUTE_SYSTEM;*/
/*	j = *attr & FILE_ATTRIBUTE_READONLY;*/
	
/*	*retval = i | j ;*/
	
	return *attr & (FILE_ATTRIBUTE_SYSTEM
		| FILE_ATTRIBUTE_READONLY);
}

inline UINT file_size(WIN32_FIND_DATA *f_data) {
	return (f_data->nFileSizeHigh * (MAXDWORD+1)) + f_data->nFileSizeLow;
}

inline void loc_file_size(void *mem, UINT **out) {
	*out = mem;
}

inline void loc_file_attr(void *mem, UINT **out) {
	*out = mem + sizeof(UINT);
}

inline void loc_file_name(void *mem, UCHAR **fname) {
	*fname = mem + (2*sizeof(UINT));
}
