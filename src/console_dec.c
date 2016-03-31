

/************************************************************************************
	Implementation of Access to Console Functions for Windows platform
	Author:             Ashis Kumar Das
	Email:              akd.bracu@gmail.com
*************************************************************************************/



#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <stdlib.h>
#include <conio2.h>
#include "fsinvbin.h"





#define PROG_SPACE								"                                             "
#define PROG_NAME								"   FileStream Inverter Binary v2.0 (WIN32)   "
#define PROG_AUTHOR								"  Programmed by AKD92 (akd.bracu@gmail.com)  "
#define PROG_WEB								"          https://github.com/AKD92           "



#define WAITTIMER_TYPE_FULL						1
#define WAITTIMER_TYPE_ERROR					2

#define FULL_MESSAGE1							"Accessibility checking"
#define FULL_MESSAGE2							"Starting inversion process"
#define ERROR_MESSAGE							"Directory list could not be generated"


#define FILE_ACTIVE_MSG							"[  WORKING_ON   ]"
#define FILE_INV_DONE							"[ INVERSION_OK  ]"
#define FILE_LOCKED_MSG 						"[ ERR_ON_ACCESS ]"
#define FILE_NOACCESS_MSG						"[ ACCESS_DENIED ]"

#define FRMT_FILESIZE							" [%10s]\n"
#define FRMT_FILEPATH							"%s\n"
#define NEWLINE									'\n'





static char tempFileSizeBuffer[20];
static char tempLineBuffer[2048];



void console_printProgramInfo();

void console_showWaitTimer(int type, int delay_ms);

void console_printInitDirectories(char *inputDir, char *execFilePath, int delay_ms);

void console_printStatistics(const InvStat *pInvStat, int refDelay);


void console_printActiveFile(char *fPath, uint64_t fSize);

void console_printInvertedFile(char *fPath, uint64_t fSize, int x, int y);

void console_printLockedFile(char *fPath, uint64_t fSize, int x, int y);

void console_printNotWritableFile(char *fPath, uint64_t fSize, int x, int y);



void util_alignFileSize(uint64_t fileSize, char *outFileSizeBuffer);




void console_printProgramInfo(void) {
	
	struct text_info txtInfo;
	unsigned short attr;
		
	textcolor(WHITE);
	textbackground(BLUE);
	
	gettextinfo(&txtInfo);
	attr = txtInfo.attribute;
	
	cputsxy((txtInfo.screenwidth - strlen(PROG_SPACE))/2, txtInfo.cury, PROG_SPACE);
	normvideo();
	putch(NEWLINE);
	
	textattr(attr);
	
	gettextinfo(&txtInfo);
	cputsxy((txtInfo.screenwidth  - strlen(PROG_NAME))/2, txtInfo.cury, PROG_NAME);
	normvideo();
	putch(NEWLINE);
	
	textattr(attr);
	
	gettextinfo(&txtInfo);
	cputsxy((txtInfo.screenwidth  - strlen(PROG_AUTHOR))/2, txtInfo.cury, PROG_AUTHOR);
	normvideo();
	putch(NEWLINE);
	
	textattr(attr);
	
	gettextinfo(&txtInfo);
	cputsxy((txtInfo.screenwidth  - strlen(PROG_WEB))/2, txtInfo.cury, PROG_WEB);
	normvideo();
	putch(NEWLINE);
	
	textattr(attr);
	
	gettextinfo(&txtInfo);
	cputsxy((txtInfo.screenwidth  - strlen(PROG_SPACE))/2, txtInfo.cury, PROG_SPACE);
	normvideo();
	putch(NEWLINE);
	
	return;
}



void console_printStatistics(const InvStat *pInvStat, int refDelay) {
	
	char waitLetter;
	double pctgFailed, invRatio;
	uint32_t countFailed;
	unsigned int iCount;
	char tmpBuf_1[20], tmpBuf_2[20], tmpBuf_3[20], tmpBuf_4[20], tmpBuf_5[20];
	
	
	waitLetter = '.';
	invRatio = stat_pctgByteProcessed(pInvStat);
	pctgFailed = stat_pctgFailedFiles(pInvStat);
	countFailed = pInvStat->totalFiles - pInvStat->processedFiles;
	
	
	textcolor(YELLOW);
	cputs("Inversion complete. Showing results");
	
	textbackground(LIGHTBLUE);
	
	iCount = 0;
	while (iCount < 9) {
		
		delay(refDelay / 3);
		putch(waitLetter);
		iCount += 1;
	}
	
	normvideo();
	delay(refDelay / 2);
	
	putch(NEWLINE);
	putch(NEWLINE);
	
	
	/* Print some statistical data */
	
	sprintf(tmpBuf_1, "%-7" PRIu32, pInvStat->totalFiles);
	sprintf(tmpBuf_2, "%-7" PRIu32, pInvStat->processedFiles);
	sprintf(tmpBuf_3, "%-6" PRIu32, countFailed);
	sprintf(tmpBuf_4, "%5.2f", pctgFailed);
	
	textcolor(YELLOW);
	cputs("Total Files: ");
	textcolor(LIGHTGREEN);
	cputs(tmpBuf_1);
	
	textcolor(YELLOW);
	cputs(" Succeeded: ");
	textcolor(LIGHTGREEN);
	cputs(tmpBuf_2);
	
	textcolor(YELLOW);
	cputs(" Failed: ");
	textcolor(LIGHTRED);
	cputs(tmpBuf_3);
	
	textcolor(YELLOW);
	cputs(" [Failure Ratio ");
	textcolor(LIGHTMAGENTA);
	cputs(tmpBuf_4);
	cputs("%]\n");
	
	
	/* Print data of total size encountered */
	
	util_alignStreamSize(pInvStat->byteProcessed, tmpBuf_1);
	util_alignStreamSize(pInvStat->byteEncountered, tmpBuf_2);
	
	sprintf(tmpBuf_3, "%-14s", tmpBuf_1);
	sprintf(tmpBuf_4, "%-14s", tmpBuf_2);
	sprintf(tmpBuf_5, "%5.1f", invRatio);
	
	textcolor(YELLOW);
	cputs("Inverted: ");
	textcolor(LIGHTGREEN);
	cputs(tmpBuf_3);
	
	textcolor(YELLOW);
	cputs(" Encountered: ");
	textcolor(LIGHTGREEN);
	cputs(tmpBuf_4);
	
	textcolor(YELLOW);
	cputs(" [Inversion Ratio ");
	textcolor(LIGHTMAGENTA);
	cputs(tmpBuf_5);
	cputs("%]");
	
	return;
}



void console_printActiveFile(char *fPath, uint64_t fSize) {
	
	util_alignFileSize(fSize, tempFileSizeBuffer);
	
	textcolor(LIGHTMAGENTA);
	cputs(FILE_ACTIVE_MSG);
	
	textcolor(YELLOW);
	sprintf(tempLineBuffer, FRMT_FILESIZE, tempFileSizeBuffer);
	cputs(tempLineBuffer);
	
	textcolor(LIGHTMAGENTA);
	sprintf(tempLineBuffer, FRMT_FILEPATH, fPath);
	cputs(tempLineBuffer);
	
	return;
}



void console_printInvertedFile(char *fPath, uint64_t fSize, int x, int y) {
	
	util_alignFileSize(fSize, tempFileSizeBuffer);
	
	gotoxy(x, y);
	textcolor(LIGHTGREEN);
	cputs(FILE_INV_DONE);
	
	textcolor(YELLOW);
	sprintf(tempLineBuffer, FRMT_FILESIZE, tempFileSizeBuffer);
	cputs(tempLineBuffer);
	
	textcolor(LIGHTGREEN);
	sprintf(tempLineBuffer, FRMT_FILEPATH, fPath);
	cputs(tempLineBuffer);
	
	return;
}



void console_printLockedFile(char *fPath, uint64_t fSize, int x, int y) {
	
	util_alignFileSize(fSize, tempFileSizeBuffer);
	
	gotoxy(x, y);
	textcolor(LIGHTRED);
	cputs(FILE_LOCKED_MSG);
	
	textcolor(LIGHTMAGENTA);
	sprintf(tempLineBuffer, FRMT_FILESIZE, tempFileSizeBuffer);
	cputs(tempLineBuffer);
	
	textcolor(LIGHTRED);
	sprintf(tempLineBuffer, FRMT_FILEPATH, fPath);
	cputs(tempLineBuffer);
	
	return;
}



void console_printNotWritableFile(char *fPath, uint64_t fSize, int x, int y) {
	
	util_alignFileSize(fSize, tempFileSizeBuffer);
	
	gotoxy(x, y);
	textcolor(LIGHTRED);
	cputs(FILE_NOACCESS_MSG);
	
	textcolor(LIGHTMAGENTA);
	sprintf(tempLineBuffer, FRMT_FILESIZE, tempFileSizeBuffer);
	cputs(tempLineBuffer);
	
	textcolor(LIGHTRED);
	sprintf(tempLineBuffer, FRMT_FILEPATH, fPath);
	cputs(tempLineBuffer);
	
	return;
}



void console_printInitDirectories(char *inputDir, char *execFilePath, int delay_ms) {
	
	textcolor(YELLOW);
	cputs("\n\nInput Directory: ");
	textcolor(LIGHTBLUE);
	cprintf("%s\n", inputDir);
	
	delay(delay_ms);
	
	textcolor(YELLOW);
	cputs("\nExecutable File: ");
	textcolor(LIGHTBLUE);
	cprintf("%s\n\n", execFilePath);
	
	return;
}



void console_showWaitTimer(int type, int delay_ms) {
	
	char cdot;
	int progBar_x, progBar_y;
	int centerPointX;
	register int index;
	int loopDelay, loopCount, maxDot;
	int barLeft, barRight, barTop, barBottom;
	
	struct text_info txtInfo;
	struct char_info *pCharBuffer;
	
	cdot = '-';
	
	putch(NEWLINE);
	textcolor(YELLOW);
	
	gettextinfo(&txtInfo);
	centerPointX = (txtInfo.screenwidth / 2);
	loopCount = centerPointX - (centerPointX / 4) + (centerPointX / 10);
	maxDot = loopCount * 2 + 1;
	
	pCharBuffer = (struct char_info *) malloc(maxDot * sizeof(struct char_info));
	
	putchxy(centerPointX, txtInfo.cury, cdot);
	if (type == WAITTIMER_TYPE_FULL) {
		loopDelay = delay_ms;
	}
	else {
		loopDelay = delay_ms / 5;
	}
	
	delay(loopDelay);
	index = 1;
	while (index < loopCount) {
		
		putchxy(centerPointX - index, txtInfo.cury, cdot);
		putchxy(centerPointX + index, txtInfo.cury, cdot);
		
		delay(loopDelay);
		index = index + 1;
	}
	delay(delay_ms * 2);
	
	
	progBar_x = centerPointX - loopCount + 1;
	// progBar_x = (txtInfo.screenwidth - maxDot) / 2;
	progBar_y = wherey();
	
	
	if (type == WAITTIMER_TYPE_FULL) {
		gotoxy(1, progBar_y - 1);
		clreol();
		
		textcolor(LIGHTRED);
		cputs(FULL_MESSAGE1);
		delay(delay_ms * 12);
		
		gotoxy(1, progBar_y - 1);
		clreol();
		
		textcolor(YELLOW);
		cputs(FULL_MESSAGE2);
		delay(250);
		
		textcolor(LIGHTGREEN);
	}
	else {
		gotoxy(1, progBar_y - 1);
		clreol();
		
		textcolor(LIGHTRED);
		cputs(ERROR_MESSAGE);
		delay(delay_ms * 10);
	}
	
	barTop = progBar_y;
	barBottom = progBar_y;
	barLeft = progBar_x;
	barRight = barLeft + maxDot;
	
	/* gettext parameters: left, top, right, bottom, char_buffer */
	gotoxy(progBar_x, progBar_y);
	_conio_gettext(barLeft, barTop, barRight, barBottom, pCharBuffer);
	
	
	/* Retreive current text color from textInfo and */
	/* Apply this color as an attribute to our char buffer */
	/* Current color can be RED or GREEN, depending on error conditions */
	gettextinfo(&txtInfo);
	for (index = 0; index < maxDot; index++) {
		(pCharBuffer + index)->attr = txtInfo.attribute;
	}
	
	
	/* puttext parameters: left, top, right, bottom, char_buffer */
	puttext(barLeft, barTop, barRight, barBottom, pCharBuffer);
	free((void *) pCharBuffer);
	
	if (type == WAITTIMER_TYPE_FULL) {
		delay(800);
		clreol();
	}
	else {
		delay(3000);
	}
	
	return;
}

