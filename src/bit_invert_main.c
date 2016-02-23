
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

#include "WinFileFind.h"
#include "Bit_Invert.h"
#include "queue.h"

#define MAX_FILES 					32 * 1024		/* 2 MB ; 5,24,288 files total*/
#define BUF_SIZE 					768 * 1024		/* 768 KB */

#define MODE_RW 					"r+b"
#define OPEN_FAILED 				"Error on file access:\n%s\n"
#define FRMT_FILE_PRINT				"%s [%u]"
#define FRMT_FILE_UNSAFE			"** SKIP Unsafe file attributes\n%s"


void str_f_dir(char *sFile);

int apply_invert_op(const UCHAR *b_file, UINT *file_size, STAT*);

UCHAR *data_buffer;

/**************************** IMPLEMENTATION (DRIVER PROGRAM) *********************/


int main(int argc, char **argv) {
	
	UINT n;
	int status;
	
	Queue file_queue;
	STAT f_stat;
	UINT f_failed_p, f_failed_n;
	
	UCHAR *working_fname;
	UINT *working_fsize, *working_fattr;
	void *f_info_mem;
	
	int op_status;
	
	char address_buf[2048];
	char exec_path[2048];
	char *root_dir;
	
	GetModuleFileName(NULL, exec_path, 2047);
	
	if (argc < 2) {
		root_dir = malloc(sizeof(char) * 1024);
		memcpy(root_dir, exec_path, strlen(exec_path));
		str_f_dir(root_dir);
	} else {
		root_dir = *(argv + 1);
	}
	
	printf("\nUsing Dir: %s\n", root_dir);
	printf("Exec Path: %s\n\n", exec_path);
	
	queue_init(&file_queue, free);
	
	stat_init(&f_stat);
	
	status = search_files(root_dir, &file_queue);
	
	if (status < 0) {
		
		puts("Could not find files or directories. Program will terminate.");
		
		queue_destroy(&file_queue);
		
		getche();
		return -1;
		
	}
	
	f_stat.no_files = file_queue.size;
	data_buffer = (UCHAR*) malloc(BUF_SIZE);
	
	while (queue_size(&file_queue) > 0) {
		
		queue_dequeue(&file_queue, &f_info_mem);
		
		loc_file_attr(f_info_mem, &working_fattr);
		loc_file_name(f_info_mem, &working_fname);
		loc_file_size(f_info_mem, &working_fsize);
		
		if (issafe(working_fattr) > 0) {
			sprintf(address_buf, FRMT_FILE_UNSAFE, working_fname);
			puts(address_buf);
			continue;
		}
		
		sprintf(address_buf, FRMT_FILE_PRINT, working_fname, *working_fsize);
		puts(address_buf);
		
		op_status = apply_invert_op(working_fname, working_fsize, &f_stat);
		
		if (op_status < 0) {
			printf(OPEN_FAILED, working_fname);
		} else {
			f_stat.no_processed_files++;
			f_stat.t_size_encountered += working_fsize;
		}
		
	}
	
	stat_files_failed_p(&f_stat, &f_failed_p);
	stat_files_failed_n(&f_stat, &f_failed_n);
	
	sprintf(address_buf, "\nNo files: %u, Processed: %u, Failed %u [%u%%]",
		f_stat.no_files, f_stat.no_processed_files, f_failed_n, f_failed_p);
	puts(address_buf);
	
	queue_destroy(&file_queue);
	free(data_buffer);
	
	if (argc < 2) free(root_dir);
	
	getche();
	
	return 0;
	
}

int apply_invert_op(const UCHAR *b_file, UINT *file_size, STAT *st) {
	
	int retval;
	
	FILE *f = (FILE*) fopen64(b_file, MODE_RW);
	if (f == NULL) return -2;
	
	position(f, file_size);
	retval = invert_bit(data_buffer, BUF_SIZE, f, st);
	fclose(f);
	
	return retval;
}

inline void str_f_dir(char *sFile) {
	char *loc;
	loc = strrchr(sFile, 92);
	*(loc + 1) = 0;
}
