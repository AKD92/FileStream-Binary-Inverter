

/************************************************************************************
	Implementation of Main() function and utility routines
	Author:             Ashis Kumar Das
	Email:              akd.bracu@gmail.com
*************************************************************************************/



#include <stdio.h>
#include <stdlib.h>
#include <conio2.h>
#include <list.h>
#include "filefind_win.h"
#include "fsbininv.h"





#define PROG_DELAY								600
#define FILE_SKIP_PERCENTAGE					25				/* First 25% will be skipped */
#define INVERSION_BUFFER_SIZE					768 * 1024
#define FILEMODE_RW_BINARY						"r+b"
#define CHAR_DRECTORY_MARK						'\\'

#define FRMT_FILE_OPEN_FAILED_MSG 				"[ERR_ON_ACCESS]\n%s\n"
// #define FRMT_FILEINFO_MSG						"[INVERSION_OK] %s [%s]\n"
#define FRMT_FILEINFO_MSG						"[INVERSION_OK] [%s]\n%s\n"
#define FRMT_FILE_UNSAFE_MSG					"[NON_WRITABLE]\n%s\n"
#define FRMT_NEWLINE							"\n"

#define PROG_NAME								"  FileStream Binary Inverter v2.0 (WIN)  \n"
#define PROG_AUTHOR								"  Programmed by AKD92 (akd.bracu@gmail.com)  \n"
#define PROG_WEB								"  https://github.com/AKD92  \n"







int main(int argc, char ** argv);

static void util_convFileToDirectoryPath(char *strFilePath);

static int util_fileApplyInversion(const char *strFilePath, unsigned int *pFileSize, InversionStat *pProgressStat);

static void util_printProgramInfo(int delay_ms);






/**************************** IMPLEMENTATION (DRIVER PROGRAM) *********************/


static void util_convFileToDirectoryPath(char *strFilePath) {
	
	char *loc;
	
	/* Search backward using strrchar() */
	loc = strrchr(strFilePath, CHAR_DRECTORY_MARK);
	*(loc + 1) = 0;
	
	return;
}



static void util_printProgramInfo(int delay_ms) {
	
	struct text_info txtInfo;
	
	textcolor(YELLOW);
	textbackground(LIGHTBLUE);
	
	gettextinfo(&txtInfo);
	cputsxy((txtInfo.screenwidth - strlen(PROG_NAME)), txtInfo.cury, PROG_NAME);
	
	if (delay_ms > 0) delay(delay_ms);
	gettextinfo(&txtInfo);
	cputsxy((txtInfo.screenwidth - strlen(PROG_AUTHOR)), txtInfo.cury, PROG_AUTHOR);
	
	if (delay_ms > 0) delay(delay_ms);
	gettextinfo(&txtInfo);
	cputsxy((txtInfo.screenwidth - strlen(PROG_WEB)), txtInfo.cury, PROG_WEB);
	
	return;
}



static int util_fileApplyInversion(const char *strFilePath, unsigned int *pFileSize, InversionStat *pStat) {
	
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
	positionStart = fsbininv_setStartPosition(pFile, pFileSize, FILE_SKIP_PERCENTAGE);
	
	
	/* Start inversion procedure, use inversionBuffer as a temporary data buffer */
	retValue = fsbininv_invertFileBits((char *) inversionBuffer, INVERSION_BUFFER_SIZE, pFile, pStat);
	pStat->byteEncountered += (unsigned long int) *pFileSize;
	
	/* Inversion complete, Now close the file */
	fclose(pFile);
	
	INVERSION_END:
	return retValue;
}




int main(int argc, char **argv) {
	
	int isListBuilt;
	
	ListElem *elem;
	List *fileList;
	InversionStat invStat;
	FileData *fileData;
	unsigned int f_failed_n;
	double f_failed_p, byte_ratio;
	
	int op_status;
	char tempPathBuffer[2048], executableFilePath[2048], *rootDirectoryPath;
	char tempFileSize[20], tempByteSize[20];
	
	elem = 0;
	fileList = 0;
	fileData = 0;
	memset((void *) &invStat, 0, sizeof(InversionStat));
	
	
	/* Obtain full path of executable file containing this program */
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
	
	inittextinfo();
	delay(PROG_DELAY / 5);
	printf(FRMT_NEWLINE);
	
	util_printProgramInfo(PROG_DELAY);
	normvideo();
	delay(PROG_DELAY);
	
	textcolor(YELLOW);
	printf("\n\nExec PATH: %s\n", executableFilePath);
	printf("Using DIR: %s\n\n\n", rootDirectoryPath);
	
	/* Build the File and Directory Hierarchy as a list */
	isListBuilt = file_buildFileList((const char *) rootDirectoryPath, &fileList);
	if (isListBuilt < 0) {
		
		textcolor(LIGHTRED);
		puts("Could not find files or directories. Program will terminate.");
		goto END;
	}
	
	invStat.totalFiles = list_size(fileList);
	
	elem = list_head(fileList);
	while (elem != 0) {
		
		fileData = (FileData *) list_data(elem);
		
		if (file_isFileWritable(fileData->fileAttribute) == 0) {
			
			textcolor(LIGHTRED);
			sprintf(tempPathBuffer, FRMT_FILE_UNSAFE_MSG, fileData->strFilePath);
			puts(tempPathBuffer);
			
			goto LOOP_AGAIN;
		}
		
		op_status = util_fileApplyInversion(fileData->strFilePath, &fileData->fileSize, &invStat);
		
		if (op_status != 0) {
			
			textcolor(LIGHTRED);
			sprintf(tempPathBuffer, FRMT_FILE_OPEN_FAILED_MSG, fileData->strFilePath);
			puts(tempPathBuffer);
			
		} else {
			
			textcolor(LIGHTGREEN);
			util_alignFileSize(fileData->fileSize, tempFileSize);
			sprintf(tempPathBuffer, FRMT_FILEINFO_MSG, tempFileSize, fileData->strFilePath);
			puts(tempPathBuffer);
			
			invStat.processedFiles += 1;
		}
		
		LOOP_AGAIN:
		elem = list_next(elem);
	}
	
	/* Gather statistical informations */
	f_failed_n = invStat.totalFiles - invStat.processedFiles;
	f_failed_p = stat_prctFailedFiles(&invStat);
	byte_ratio = stat_prctByteProcessed(&invStat);
	
	textcolor(YELLOW);
	puts("\n\nCompletion of inversion procedures on the binary stream of files.\n");
	
	/* Print some statistical data */
	sprintf(tempPathBuffer, "Total Files: %-7u Inverted: %-7u Failed: %-7u [Failure Ratio %5.2f%%]",
					invStat.totalFiles, invStat.processedFiles, f_failed_n, f_failed_p);
	puts(tempPathBuffer);
	
	/* Print data of total size encountered */
	util_alignStreamSize(invStat.byteProcessed, tempFileSize);
	util_alignStreamSize(invStat.byteEncountered, tempByteSize);
	sprintf(tempPathBuffer, "Inverted: %-14s Encountered: %-14s [Inversion Ratio %5.1f%%]",
									tempFileSize, tempByteSize, byte_ratio);
	puts(tempPathBuffer);
	
	
	/* Clean up */
	END:
	list_destroy(fileList);
	free((void *) rootDirectoryPath);
	
	textcolor(YELLOW);
	puts("\nPress ANY KEY to EXIT");
	
	util_printProgramInfo(0);
	normvideo();
	
	getche();
	
	return 0;
	
}
