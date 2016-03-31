

/************************************************************************************
	Implementation of Main() function and other Utility routines
	Author:             Ashis Kumar Das
	Email:              akd.bracu@gmail.com
*************************************************************************************/







#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <conio2.h>
#include <list.h>
#include "fsys_crawler_win.h"
#include "fsinvbin.h"





#define PROG_DELAY								600
#define FILE_SKIP_PERCENTAGE					25				/* First 25% will be skipped */
#define INVERSION_BUFFER_SIZE					512 * 1024		/* 512 kilo bytes */
#define FILEMODE_RW_BINARY						"r+b"

#define NEWLINE									'\n'






int main(int argc, char ** argv);

static void util_getParentDirectory(char *strFilePath);

static int util_commitInversion(const char *strFilePath, uint64_t fSize, InvStat *pInvStat);




void console_printProgramInfo();

void console_showWaitTimer(int type, int delay_ms);

void console_printInitDirectories(char *inputDir, char *execFilePath, int delay_ms);

void console_printStatistics(const InvStat *pInvStat, int refDelay);


void console_printActiveFile(char *fPath, uint64_t fSize);

void console_printInvertedFile(char *fPath, uint64_t fSize, int x, int y);

void console_printLockedFile(char *fPath, uint64_t fSize, int x, int y);

void console_printNotWritableFile(char *fPath, uint64_t fSize, int x, int y);








/**************************** IMPLEMENTATION (DRIVER PROGRAM) *********************/


static void util_getParentDirectory(char *strFilePath) {
	
	char *loc;
	char dirMark;
	
	dirMark = '\\';
	
	/* Search backward using strrchar() */
	loc = strrchr(strFilePath, dirMark);
	
	/* Replace the dirMark we just found with NUL terminator */
	*(loc) = 0;
	
	return;
}



static int util_commitInversion(const char *strFilePath, uint64_t fSize, InvStat *pInvStat) {
	
	FILE *pFile;
	int retValue;
	static char inversionBuffer[INVERSION_BUFFER_SIZE];
	
	
	/* We are ready to start our inversion procedure on the specified file */
	/* So, first we open the file in [READ + WRITE + BINARY] mode */
	/* If the file cannot be opened, return with an error value -1 */
	pFile = (FILE *) fopen64(strFilePath, FILEMODE_RW_BINARY);
	if (pFile == 0) {
		retValue = -1;
		goto END;
	}
	
	
	/* We want to start inverting bits of the specified file from 25% */
	/* That means, first 25% bits of the file will be skipped */
	/* Therefore, set start position (file pointer) at 25% */
	retValue = fsbininv_setStartPosition(pFile, fSize, FILE_SKIP_PERCENTAGE);
	if (retValue == -1)
		goto END;
	
	
	/* Start inversion procedure, use inversionBuffer as a temporary data buffer */
	retValue = fsbininv_invertFileBits((char *) inversionBuffer, INVERSION_BUFFER_SIZE, pFile, pInvStat);
	pInvStat->byteEncountered += (uint64_t) fSize;
	pInvStat->processedFiles += 1;
	
	/* Inversion complete, Now close the file */
	fclose(pFile);
	
	END:
	return retValue;
}




int main(int argc, char **argv) {
	
	
	ListElem *elem;
	List *listFiles;
	InvStat invStat;
	FileData *fData;
	
	int isListBuilt;
	int msg_x, msg_y;
	int invOpValue, isError;
	
	char *rootDirectoryPath, executableFilePath[2048];
	
	
	elem = 0;
	isError = 0;
	listFiles = 0;
	fData = 0;
	rootDirectoryPath = 0;
	memset((void *) &invStat, 0, sizeof(InvStat));
	
	
	/* Obtain full path of executable file containing this program */
	GetModuleFileName(NULL, executableFilePath, 2047);
	
	
	/* Allocate memory for storing path of starting directory (the first one) */
	rootDirectoryPath = (char *) malloc(sizeof(char) * 2048);
	if (rootDirectoryPath == 0)
			goto END;
	
	
	/* If user didnot specify a path explicitly, use directory path of this executable */
	if (argc < 2) {
		strcpy((char *) rootDirectoryPath, (const char *) executableFilePath);
		util_getParentDirectory(rootDirectoryPath);
	}
	else {
		strcpy((char *) rootDirectoryPath, (const char *) *(argv + 1));
	}
	
	delay(PROG_DELAY / 8);
	putch(NEWLINE);
	_setcursortype(_SOLIDCURSOR);
	inittextinfo();
	
	console_printProgramInfo();
	normvideo();
	putch(NEWLINE);
	delay(PROG_DELAY);
	
	console_printInitDirectories(rootDirectoryPath, executableFilePath, PROG_DELAY /2 );
	delay(PROG_DELAY / 2);
	
	textcolor(YELLOW);
	cputs("Traversing through the file system");
	
	/* Build the File and Directory Hierarchy as a list */
	isListBuilt = file_buildFileList((const char *) rootDirectoryPath, &listFiles);
	
	if (isListBuilt < 0) {
		
		isError = -1;
		console_showWaitTimer(2, PROG_DELAY / 4);
		normvideo();
		
		putch(NEWLINE);
		putch(NEWLINE);
		
		textcolor(LIGHTRED);
		cputs("Input directory or file path is invalid. Program will terminate.\n");
		goto END;
	}
	
	console_showWaitTimer(1, list_size(listFiles));
	normvideo();
	putch(NEWLINE);
	putch(NEWLINE);
	
	
	invStat.totalFiles = list_size(listFiles);
	elem = list_head(listFiles);
	
	while (elem != 0) {
		
		/* Get a FileData object from the Linked List */
		msg_x = wherex();
		msg_y = wherey();
		fData = (FileData *) list_data(elem);
		console_printActiveFile(fData->strFilePath, fData->fileSize);
		// delay(1000);
		
		/* Check if this file is Writable or not */
		if (file_isFileWritable(fData->fileAttribute) == 0) {
			
			console_printNotWritableFile(fData->strFilePath, fData->fileSize, msg_x, msg_y);
			delay(300);
			// flashbackground(BLUE, 300);
			goto LOOP_AGAIN;
		}
		
		/* Apply Inversion process to this File */
		invOpValue = util_commitInversion(fData->strFilePath, fData->fileSize, &invStat);
		
		/* Check if ERROR occured on file access */
		if (invOpValue != 0) {
			
			/* This file has attributes which forbids WRITE permission */
			/* Ex: READONLY, SYSTEM etc */
			/* We print an error message conveying this fact */
			console_printLockedFile(fData->strFilePath, fData->fileSize, msg_x, msg_y);
			delay(300);
			// flashbackground(BLUE, 300);
		}
		else {
			
			/* Inversion process has done successfully to this File */
			/* We print a message conveying our success */
			console_printInvertedFile(fData->strFilePath, fData->fileSize, msg_x, msg_y);
		}
		LOOP_AGAIN:
		elem = list_next(elem);
	}
	
	putch(NEWLINE);
	putch(NEWLINE);
	
	console_printStatistics(&invStat, PROG_DELAY);
	putch(NEWLINE);
	
	
	
	END:
	/* Clean up all these mess */
	
	free((void *) rootDirectoryPath);
	if (listFiles != 0) list_destroy(listFiles);
	
	if (isError == 0) {
		
		textcolor(YELLOW);
		cputs("\nPress ANY KEY to ");
		textcolor(LIGHTRED);
		cputs("EXIT\n\n");
		
		console_printProgramInfo();
		normvideo();
		
		clearkeybuf();
		getch();
		
	}
	else {
		normvideo();
		delay(PROG_DELAY);
	}
	
	_setcursortype(_NORMALCURSOR);
	delay(PROG_DELAY / 5);
	
	return isError;
	
}
