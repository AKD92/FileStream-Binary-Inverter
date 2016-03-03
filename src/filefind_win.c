

/************************************************************************************
	Implementation of File List Generation routines for Windows platform
	Author:             Ashis Kumar Das
	Email:              akd.bracu@gmail.com
*************************************************************************************/



#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "filefind_win.h"



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



int file_isFileWritable(unsigned int fileAttribute) {
	
	int isWritable;
	unsigned int attrbForbidden;
	
	attrbForbidden = (unsigned int) (FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_SYSTEM);
	
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
					
					file_createFileData(tempPathBuffer, &fileData);
					elem = list_tail(fList);
					list_ins_next(fList, elem, (const void *) fileData);
					fileCount++;
				}
			}
			
			isNextFileExist = FindNextFile(handleFile, &fileFindData);
		}   /* End of While loop */
	}
	
	FindClose(handleFile);
	return fileCount;
}

static int file_createFileData(const char *strFilePath, FileData **outFileData) {
	
	DWORD * handleFile;
	DWORD fileSize;
	DWORD fileAttribute;
	FileData *fileData;
	char *pFileFullPath;
	
	handleFile = CreateFile(strFilePath, GENERIC_READ,
								0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
								
	if (handleFile == INVALID_HANDLE_VALUE) {
		return -1;
	}
	else {
		
		fileSize = GetFileSize(handleFile, NULL);
		CloseHandle(handleFile);
		fileAttribute = GetFileAttributes(strFilePath);
		
		fileData = (FileData *) malloc(sizeof(FileData));
		pFileFullPath = (char *) malloc(strlen(strFilePath) + 1);

		fileData->fileSize = fileSize;
		fileData->fileAttribute = fileAttribute;
		fileData->strFilePath = pFileFullPath;

		strcpy(fileData->strFilePath, (const char *) strFilePath);
		*outFileData = fileData;
	}
	
	return 0;
	
}

