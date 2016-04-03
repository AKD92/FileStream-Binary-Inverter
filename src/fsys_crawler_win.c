

/************************************************************************************
	Implementation of File List Generation routines for Windows platform
	Author:             Ashis Kumar Das
	Email:              akd.bracu@gmail.com
	GitHub:             https://github.com/AKD92
*************************************************************************************/






#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <windows.h>
#include "fsys_crawler_win.h"



#define DIR_CURRENT					"."
#define DIR_PARENT					".."



static void util_freeFileData(void *fData);

static int file_extractFromDirectory(const char *strDirectoryPath, List *fileList);

static int file_createFileData(const char *strFilePath, FileData **outFileData);





static void util_freeFileData(void *fData) {
	
	FileData *data;
	data = (FileData *) fData;
	free((void *) data->strFilePath);
	free((void *) fData);
	
	return;
}



int file_isFileWritable(uint32_t fileAttribute) {
	
	int isWritable;
	uint32_t attrbForbidden;
	
	attrbForbidden = (uint32_t) (FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_SYSTEM);
	
	/* We expect that if a file is writable, then forbidden attributes */
	/* Will not be present in the attributes of the file */
	/* Perform a Bitwise AND operaton and check if the result is 0 */
	/* After ANDing, if result is 0, then accept, else reject */
	isWritable = (fileAttribute & attrbForbidden) == 0 ? 1 : 0;
	
	return isWritable;
}


int file_buildFileList(const char *strDirectoryPath, List **outFileList) {
	
	List *fList;
	ListElem *elem;
	FileData *fileData;
	DWORD fileAttribute;
	DWORD isDirectory;
	int retValue;
	
	fileAttribute = GetFileAttributes(strDirectoryPath);
	
	if (fileAttribute == INVALID_FILE_ATTRIBUTES) {
		retValue = -1;
	}
	else {
		fList = (List *) malloc(sizeof(List));
		list_init(fList, util_freeFileData);
		
		isDirectory = fileAttribute & FILE_ATTRIBUTE_DIRECTORY;
		
		if (isDirectory != 0) {
			retValue = file_extractFromDirectory(strDirectoryPath, fList);
		} else {
			file_createFileData(strDirectoryPath, &fileData);
			elem = list_tail(fList);
			list_ins_next(fList, elem, (const void *) fileData);
			retValue = 0;
		}
		
		*outFileList = fList;
	}
	
	return retValue;
}



static int file_extractFromDirectory(const char *strDirectoryPath, List *fList) {
	
	ListElem *elem;
	char tempPathBuffer[2048];
	HANDLE handleFile;
	WIN32_FIND_DATA fileFindData;
	FileData *fileData;
	int isDir;
	int isCurrentDir, isParentDir, isRegularFile;
	register int isNextFileExist, fileCount;
	
	fileCount = 0;
	isNextFileExist = 0;
	
/*	Specify a file mask. *.* = We want everything!*/
	sprintf(tempPathBuffer, MAKE_PATH_MASK, strDirectoryPath);
	handleFile = FindFirstFile(tempPathBuffer, &fileFindData);
	
	if(handleFile == INVALID_HANDLE_VALUE) {
		return -1;
	}
	else {
		
		isNextFileExist = 1;
		while (isNextFileExist != 0) {
			
			isCurrentDir = strcmp((const char *) fileFindData.cFileName, (const char *) DIR_CURRENT);
			isParentDir = strcmp((const char *) fileFindData.cFileName, (const char *) DIR_PARENT);
			isRegularFile = isCurrentDir & isParentDir;
			
			if(isRegularFile != 0) {
				
				isDir = fileFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
				sprintf(tempPathBuffer, MAKE_PATH_ABSOLUTE, strDirectoryPath, fileFindData.cFileName);
				
				if (isDir != 0) {
					file_extractFromDirectory((const char *) tempPathBuffer, fList);
					
				} else {
					
					file_createFileData((const char *) tempPathBuffer, &fileData);
					elem = list_tail(fList);
					list_ins_next(fList, elem, (const void *) fileData);
					fileCount++;
				}
			}
			/* End of IF isRegularFile */
			
			isNextFileExist = FindNextFile(handleFile, &fileFindData);
		}   /* End of While loop */
	}
	
	FindClose(handleFile);
	return fileCount;
}

static int file_createFileData(const char *strFilePath, FileData **outFileData) {
	
	DWORD fileAttribute;
	FileData *fileData;
	char *pFileFullPath;
	static struct stat64 fStat;
	
	stat64(strFilePath, &fStat);
	fileAttribute = GetFileAttributes(strFilePath);
	
	fileData = (FileData *) malloc(sizeof(FileData));
	pFileFullPath = (char *) malloc(strlen(strFilePath) + 1);

	fileData->fileSize = (uint64_t) fStat.st_size;
	fileData->fileAttribute = (uint32_t) fileAttribute;
	fileData->strFilePath = pFileFullPath;

	strcpy(fileData->strFilePath, (const char *) strFilePath);
	*outFileData = fileData;

	
	return 0;
	
}

