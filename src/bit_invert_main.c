
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

#include "WinFileFind.h"
#include "Bit_Invert.h"
#include "list.h"



#define MODE_RW 								"r+b"
#define CHAR_DRECTORY_MARK						'\\'
#define FRMT_FILE_OPEN_FAILED_MSG 				"[ERROR_ON_ACCESS] %s\n"
#define FRMT_FILEINFO_MSG						"[FILE_OK] %s [%u]"
#define FRMT_FILE_UNSAFE_MSG					"[SKIP_NOTSAFE] %s"







int main(int argc, char ** argv);

static void util_convFileToDirectoryPath(char *strFilePath);

static int util_fileDataInversion(const char *strFilePath, unsigned int *pFileSize, STAT *pProgressStat);






/**************************** IMPLEMENTATION (DRIVER PROGRAM) *********************/


static int util_fileDataInversion(const char *strFilePath, unsigned int *pFileSize, STAT *pProgressStat) {
	
	int returnValue;
	static char data_buffer[768 * 1024];
	
	FILE *pFile = (FILE*) fopen(strFilePath, MODE_RW);
	if (pFile == NULL) return -2;
	
	position(pFile, pFileSize);
	returnValue = invert_bit((char *) data_buffer, 768 * 1024, pFile, pProgressStat);
	fclose(pFile);
	
	return returnValue;
}



static void util_convFileToDirectoryPath(char *strFilePath) {
	
	char *loc;
	
	/* Search backward using strrchar() */
	loc = strrchr(strFilePath, CHAR_DRECTORY_MARK);
	*(loc + 1) = 0;
	
	return;
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
		
		if (file_isFileWritable(fileData->fileAttribute) != 0) {
			sprintf(tempPathBuffer, FRMT_FILE_UNSAFE_MSG, fileData->strFilePath);
			puts(tempPathBuffer);
			goto LOOP_AGAIN;
		}
		
		op_status = util_fileDataInversion(fileData->strFilePath, &fileData->fileSize, &statProgress);
		
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
	
	f_failed_n = stat_countFailedFiles(&statProgress);
	f_failed_p = stat_percentageFailedFiles(&statProgress);
	
	sprintf(tempPathBuffer, "\nNo files: %u, Processed: %u, Failed %u [%u%%]",
		statProgress.totalFiles, statProgress.processedFiles, f_failed_n, f_failed_p);
	puts(tempPathBuffer);
	
	END:
	list_destroy(fileList);
	free(rootDirectoryPath);
	
	getche();
	
	return 0;
	
}
