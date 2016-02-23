
#include <stdio.h>
#include <stdlib.h>
#include "DTypes.h"
#include "Bit_Invert.h"

int						eof_val;
size_t					read_count;
unsigned long int		total_bytes_s;
fpos_t					prev_pos;
fpos_t					first_pos;

void changeval(UCHAR *loc);


inline void position(FILE *f, const UINT *f_size) {
	first_pos = *f_size / 4;
	fsetpos(f, &first_pos);
	
/*	printf("Size %d, size/4 = %d\n", *f_size, first_pos);*/
}

int invert_bit(UCHAR *buffer, UINT buf_size, FILE *f, STAT *st) {
	
	register UINT j;
	eof_val = 0;
	prev_pos = 0;
	read_count = 0;
/*	int write_items = 0;*/
	
	
/*	while(!eof_val) {*/
	while (!eof_val) {
		
		read_count = fread(buffer, 1, buf_size, f);
		
		eof_val = feof(f);
/*		printf("\treads %d, end-of-file %d\n", read_count, eof_val);*/
		
		fgetpos(f, &prev_pos);
/*		printf("\tcurrent pos %d\n", prev_pos);*/
		
		prev_pos -= read_count;
		fsetpos(f, &prev_pos);
/*		printf("\tnew pos %d\n", prev_pos);*/
		
		for (j=0; j < read_count; j++) {
			changeval(buffer + j);
		}
		fwrite(buffer, 1, read_count, f);
		fflush(f);
		
		st->t_size_processed += read_count;
	}
	
	return 0;
}

inline void changeval(UCHAR *loc) {
	*loc = ~(*loc);
}

void stat_init(STAT *st) {
	st->no_files = 0;
	st->no_processed_files = 0;
	st->t_size_processed = 0;
	st->t_size_encountered = 0;
}

void stat_files_failed_n(STAT *st, UINT *out) {
	*out = st->no_files - st->no_processed_files;
}

void stat_files_failed_p(STAT*st, UINT *out) {
	stat_files_failed_n(st, out);
	*out = (UINT) ((double)*out/ (double)st->no_files * 100);
}
