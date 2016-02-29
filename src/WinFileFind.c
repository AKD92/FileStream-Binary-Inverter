



#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "WinFileFind.h"



#define S_DOT "."
#define D_DOT ".."



static void util_freeFileData(void *fData);

static int file_extractFromDirectory(const char *strDirectoryPath, List *fileList);

static int file_createFileData(const char *strFilePath, FileData **outFileData);





static void util_freeFileData(void *fData) {
	
	FileData *data;
	data = (FileData *) fData;
	free(data->strFilePath);
	free(fData);
	
	return;
}



int file_isFileWritable(unsigned int fileAttribute) {
	
/*	static unsigned int i, j;*/
	
/*	i = *fileAttribute & FILE_ATTRIBUTE_SYSTEM;*/
/*	j = *fileAttribute & FILE_ATTRIBUTE_READONLY;*/
	
/*	*returnValue = i | j ;*/

	unsigned int attrbForbidden;
	int isWritable;
	
	attrbForbidden = (unsigned int) (FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_SYSTEM);
	
	/* We expect that if a file is writable, then forbidden attributes */
	/* Will not be present in the attributes of the file */
	/* Perform a Bitwise AND operaton and check if the result is 0 */
	/* After ANDing, if result is 0, then accept, else reject */
	isWritable = (fileAttribute & attrbForbidden) == 0 ? 1 : 0;
	
	return isWritable;
}


int file_buildFileList(const char *strDirectoryPath, List **outFileList) {
	
	List *fileList;
	ListElem *elem;
	FileData *fileData;
	DWORD fileAttribute;
	DWORD isDirectory;
	int returnValue;
	
	fileAttribute = GetFileAttributes(strDirectoryPath);
	
	if (fileAttribute == INVALID_FILE_ATTRIBUTES) {
		returnValue = -1;
	}
	else {
		fileList = (List *) malloc(sizeof(List));
		list_init(fileList, util_freeFileData);
		
		isDirectory = fileAttribute & FILE_ATTRIBUTE_DIRECTORY;
		if (isDirectory != 0) {
			returnValue = file_extractFromDirectory(strDirectoryPath, fileList);
		} else {
			file_createFileData(strDirectoryPath, &fileData);
			elem = list_tail(fileList);
			list_ins_next(fileList, elem, (const void *) fileData);
			returnValue = 0;
		}
		
		*outFileList = fileList;
	}
	
	return returnValue;
}



static int file_extractFromDirectory(const char *strDirectoryPath, List *fileList) {
	
	ListElem *elem;
	char tempPathBuffer[2048];
	HANDLE handleFile;
	WIN32_FIND_DATA fileFindData;
	FileData *fileData;
	int valNextFile, fileCount, cmpResult;
	
	valNextFile = 0;
	fileCount = 0;
	
/*	Specify a file mask. *.* = We want everything!*/
	sprintf(tempPathBuffer, MAKE_PATH_MASK, strDirectoryPath);
	handleFile = FindFirstFile(tempPathBuffer, &fileFindData);
	
	if(handleFile == INVALID_HANDLE_VALUE) {
		return -1;
	}
	else {
		
		valNextFile = 1;
		while (valNextFile != 0) {
			cmpResult = strcmp(fileFindData.cFileName, S_DOT)
									&& strcmp(fileFindData.cFileName, D_DOT);
			if(cmpResult != 0) {
				sprintf(tempPathBuffer, MAKE_PATH_ABSOLUTE, strDirectoryPath, fileFindData.cFileName);
				
				if (fileFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
					file_extractFromDirectory((const char *) tempPathBuffer, fileList);
				} else {
					file_createFileData(tempPathBuffer, &fileData);
					elem = list_tail(fileList);
					list_ins_next(fileList, elem, (const void *) fileData);
					fileCount++;
				}
			}
			valNextFile = FindNextFile(handleFile, &fileFindData);
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

		strcpy(fileData->strFilePath, strFilePath);
		*outFileData = fileData;
	}
	
	return 0;
	
}

