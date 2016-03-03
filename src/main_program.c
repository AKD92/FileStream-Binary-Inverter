

/************************************************************************************
	Implementation of Main() function and utility routines
	Author:             Ashis Kumar Das
	Email:              akd.bracu@gmail.com
*************************************************************************************/



#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <list.h>
#include "filefind_win.h"
#include "fbitinv.h"






#define FILE_SKIP_PERCENTAGE					25				/* First 25% will be skipped */
#define INVERSION_BUFFER_SIZE					768 * 1024
#define FILEMODE_RW_BINARY						"r+b"
#define CHAR_DRECTORY_MARK						'\\'
#define FRMT_FILE_OPEN_FAILED_MSG 				"[ERR_ON_ACCESS] %s\n"
#define FRMT_FILEINFO_MSG						"[FILE_OK] %s [%u]"
#define FRMT_FILE_UNSAFE_MSG					"[SKIP_NOT_SAFE] %s"







int main(int argc, char ** argv);

static void util_convFileToDirectoryPath(char *strFilePath);

static int util_fileApplyInversion(const char *strFilePath, unsigned int *pFileSize, STAT *pProgressStat);






/**************************** IMPLEMENTATION (DRIVER PROGRAM) *********************/


static void util_convFileToDirectoryPath(char *strFilePath) {
	
	char *loc;
	
	/* Search backward using strrchar() */
	loc = strrchr(strFilePath, CHAR_DRECTORY_MARK);
	*(loc + 1) = 0;
	
	return;
}



static int util_fileApplyInversion(const char *strFilePath, unsigned int *pFileSize, STAT *pProgressStat) {
	
	FILE *pFile;
	int retValue;
	long int positionStart;
	static char inversionBuffer[INVERSION_BUFFER_SIZE];
	
	
	/* We are ready to start our inversion procedure on the specified file */
	/* So, first we open the file in [READ + WRITE + BINARY] mode */
	/* If the file cannot be opened, return with an error value -1 */
	pFile = (FILE*) fopen(strFilePath, FILEMODE_RW_BINARY);
	if (pFile == 0) {
		retValue = -1;
		goto INVERSION_END;
	}
	
	
	/* We want to start inverting bits of the specified file from 25% */
	/* That means, first 25% bits of the file will be skipped */
	/* Therefore, set start position (file pointer) at 25% */
	positionStart = fbitinv_setStartPosition(pFile, pFileSize, FILE_SKIP_PERCENTAGE);
	
	
	/* Start inversion procedure, use inversionBuffer as a temporary data buffer */
	retValue = fbitinv_invertFileBits((char *) inversionBuffer, INVERSION_BUFFER_SIZE, pFile, pProgressStat);
	
	
	/* Inversion complete, Now close the file */
	fclose(pFile);
	
	INVERSION_END:
	return retValue;
}




int main(int argc, char **argv) {
	
	int isListBuilt;
	
	ListElem *elem;
	List *fileList;
	STAT statProgress;
	FileData *fileData;
	unsigned int f_failed_p, f_failed_n;
	
	int op_status;
	char tempPathBuffer[2048], executableFilePath[2048], *rootDirectoryPath;
	
	elem = 0;
	fileList = 0;
	fileData = 0;
	memset((void *) &statProgress, 0, sizeof(STAT));
	
	
	/* Obtain full path of execuyable file containing this program */
	GetModuleFileName(NULL, executableFilePath, 2047);
	
	
	/* Allocate memory for storing path of starting directory (the first one) */
	rootDirectoryPath = (char *) malloc(sizeof(char) * 1024);
	
	
	/* If user didnot specify a path explicitly, use directory path of this program */
	if (argc < 2) {
		strcpy(rootDirectoryPath, executableFilePath);
		util_convFileToDirectoryPath(rootDirectoryPath);
	}
	else {
		strcpy(rootDirectoryPath, (const char *) argv + 1);
	}
	
	printf("\nExec Path: %s\n", executableFilePath);
	printf("Using Dir: %s\n\n", rootDirectoryPath);
	
	isListBuilt = file_buildFileList((const char *) rootDirectoryPath, &fileList);
	if (isListBuilt < 0) {
		puts("Could not find files or directories. Program will terminate.");
		goto END;
	}
	
	statProgress.totalFiles = list_size(fileList);
	
	elem = list_head(fileList);
	while (elem != 0) {
		fileData = (FileData *) list_data(elem);
		
		if (file_isFileWritable(fileData->fileAttribute) == 0) {
			sprintf(tempPathBuffer, FRMT_FILE_UNSAFE_MSG, fileData->strFilePath);
			puts(tempPathBuffer);
			goto LOOP_AGAIN;
		}
		
		op_status = util_fileApplyInversion(fileData->strFilePath, &fileData->fileSize, &statProgress);
		
		if (op_status != 0) {
			printf(FRMT_FILE_OPEN_FAILED_MSG, fileData->strFilePath);
		} else {
			sprintf(tempPathBuffer, FRMT_FILEINFO_MSG, fileData->strFilePath, fileData->fileSize);
			puts(tempPathBuffer);
			statProgress.processedFiles++;
			statProgress.byteEncountered += fileData->fileSize;
		}
		
		LOOP_AGAIN:
		elem = list_next(elem);
	}
	
	f_failed_n = statProgress.totalFiles - statProgress.processedFiles;
	f_failed_p = stat_percentageFailedFiles(&statProgress);
	
	sprintf(tempPathBuffer, "\nNo files: %u, Processed: %u, Failed %u [%u%%]",
		statProgress.totalFiles, statProgress.processedFiles, f_failed_n, f_failed_p);
	puts(tempPathBuffer);
	
	END:
	list_destroy(fileList);
	free((void *) rootDirectoryPath);
	
	
	printf("\nPress ANY KEY to EXIT\n");
	getche();
	
	return 0;
	
}
