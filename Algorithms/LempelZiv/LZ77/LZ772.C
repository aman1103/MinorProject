/*
	---- An LZ77 Compression Implementation ----

	Filename:     LZ772.C
	Decompressr:  lz772d.c
	Description:  **** Basic LZ77 (len,position) pair output ****
	Author:       Gerald Tamayo
	Date:         August 15, 2004

	The program here only serves as a working example of LZ77.
	Improving the search function is very important for
	LZ77; you can use a better search function like a hash
	search or a list search function (which I used here).

	The implementation uses a table of integer pointers, each one
	for a list of character/symbol to hold the positions in the
	window for that character. The search function can thus
	quickly "skip" to the next correct position in the window.

	The list is a DOUBLY-LINKED LIST to make deletion of
	nodes faster.

	Decompression is faster since you just have to extract the bytes
	from the window buffer using the pos and len variables.

	NOTE:

		This program uses a list search function. You can use
		brutef_search(): just "comment out" the code that uses the
		list nodes such as delete_lznode() and insert_lznode().

	ENCODING:

		A match length less than MIN_MATCH_LEN (the minimum match
		accepted for a longer match) gets a length of 1;

		The match lengths (MIN_MATCH_LEN..PAT_BUFSIZE) are mapped to
		codes (1..PAT_BUFSIZE-MIN_MATCH_LEN+1).

		When the match length is 1, no position code is
		emitted, but simply a byte.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utypes.h"
#include "gtbitio.c"
#include "lzlist.c"

/* the decompressor's must also equal these values. */
#define WIN_BUFSIZE   4096
#define NUM_POS_BITS    12
#define WIN_MASK      4095

#define PAT_BUFSIZE     16
#define NUM_LEN_BITS     4
#define MIN_MATCH_LEN    2

typedef struct {
	char algorithm[4];
	ulong file_size;
} file_stamp;

typedef struct {
	uint pos, len;
} dpos_t;

dpos_t dpos;
uchar win_buf[ WIN_BUFSIZE ];
uchar pattern[ PAT_BUFSIZE ];

uint win_cnt = 0, pat_cnt = 0, buf_cnt = 0;

void copyright( void );
dpos_t search ( uchar w[], uchar p[] );
dpos_t brutef_search ( uchar w[], uchar p[] );
void put_codes( dpos_t *dpos );

int main( int argc, char *argv[] )
{
	unsigned long in_file_len = 0, out_file_len = 0;
	float ratio = 0.0;
	unsigned int i;
	file_stamp fstamp;

	if ( argc != 3 ) {
		fprintf(stderr, "\n Usage: lz772 infile outfile");
		copyright();
		return 0;
	}

	if ( (gIN = fopen( argv[1], "rb" )) == NULL ) {
		fprintf(stderr, "\nError opening input file.");
		return 0;
	}
	if ( (pOUT = fopen( argv[2], "wb" )) == NULL ) {
		fprintf(stderr, "\nError opening output file.");
		return 0;
	}
	init_put_buffer();

	fprintf(stderr, "\n----[  A Lempel-Ziv (LZ77) Algorithm Implementation  ]----");

	fprintf(stderr, "\n\nName of input file : %s", argv[1] );

	/* display file length. */
	fseek( gIN, 0, SEEK_END );
	in_file_len = ftell( gIN );
	
	/* Write the FILE STAMP. */
	rewind( pOUT );
	strcpy( fstamp.algorithm, "LZ7" );
	fstamp.file_size = in_file_len;
	fwrite( &fstamp, sizeof(file_stamp), 1, pOUT );

	/* start Compressing to output file. */
	fprintf(stderr, "\t  Compressing...");

	/* initialize the table of pointers. */
	init_lzlist();

	/* initialize the sliding-window. */
	memset( win_buf, 32, WIN_BUFSIZE );

	/* initialize the search list. */
	for ( i = 0; i < WIN_BUFSIZE; i++ ) {
		lzprev[i] = -1;
		lznext[i] = -1;
		insert_lznode( win_buf[i], i );
	}

	/* make sure to rewind the input file */
	rewind(gIN);
	
 	/* fill the pattern buffer. */
	buf_cnt = fread( pattern, 1, PAT_BUFSIZE, gIN );

	/* initialize the input buffer. */
	init_get_buffer();

	/* compress */
	while( buf_cnt > 0 ) {	/* look-ahead buffer not empty? */
		dpos = search( win_buf, pattern );

		/* encode window position or len codes. */
		put_codes( &dpos );
	}
	flush_put_buffer();
	fprintf(stderr, "complete.");

	/* get outfile's size and get compression ratio. */
	out_file_len = ftell( pOUT );

	fprintf(stderr, "\nName of output file: %s", argv[2] );
	fprintf(stderr, "\n\nLength of input file     = %15lu bytes", in_file_len );
	fprintf(stderr, "\nLength of output file    = %15lu bytes", out_file_len );

	ratio = (((float) in_file_len - (float) out_file_len) /
		(float) in_file_len ) * (float) 100;
	fprintf(stderr, "\nCompression ratio:         %15.2f %%\n :: compgt\n", ratio );
	
	halt_prog:

	free_put_buffer();
	free_get_buffer();
	if ( gIN ) fclose( gIN );
	if ( pOUT ) fclose( pOUT );

	return 0;
}

void copyright( void )
{
	fprintf(stderr, "\n\n Written by: Gerald Tamayo, 2004\n");
}

dpos_t search( uchar w[], uchar p[] )
{
	register int i, j, k;
	dpos_t dpos = { 0, 1 };

	/* point to start of lzlist[c] */
	i = lzlist[ p[pat_cnt] ]; /* start window search here.. */
	if ( buf_cnt > 1 ) while( i != -1 ) {
		/* start search at the second characters. */
		if ( (j = pat_cnt+1) == PAT_BUFSIZE ) j = 0;
		k = 1;

		do {
			if ( p[j] != w[ (i+k) & WIN_MASK ] ) {
				break;
			}
			/* rotate if necessary. */
			if ( (++j) == PAT_BUFSIZE ) j = 0;
		} while( (++k) < buf_cnt );

		if ( k > dpos.len ) {
			/* if greater than previous length, record it. */
			dpos.pos = i;
			dpos.len = k;

			/* maximum match, end the search. */
			if ( k == buf_cnt ) break;
		}

		/* point to next occurrence of the first character. */
		i = lznext[i];
	}

	if ( dpos.len >= MIN_MATCH_LEN ) buf_cnt -= dpos.len;
	else buf_cnt -= 1;

	return dpos;
}

dpos_t brutef_search( uchar w[], uchar p[] )
{
	register int i, j, k;
	dpos_t dpos = { 0, 0 };

	/*
	brute-force search.
	start reading at i = 0.
	*/
	for ( i = 0; i < WIN_BUFSIZE; i++ ){
		j = pat_cnt;
		k = 0;
		do {
			if ( p[j] != w[ (i+k) & WIN_MASK ] ) {
				break;
			}
			/* rotate if necessary. */
			if ( (++j) == PAT_BUFSIZE ) j = 0;
		} while( (++k) < buf_cnt );

		if ( k > dpos.len ) {
			/* if greater than previous length, record it. */
			dpos.pos = i;
			dpos.len = k;

			/* maximum match, end the search. */
			if ( k == buf_cnt ) break;
		}
	}

	if ( dpos.len >= MIN_MATCH_LEN ) buf_cnt -= dpos.len;
	else buf_cnt -= 1;

	return dpos;
}

void put_codes( dpos_t *dpos )
{
	int i, j, k;

	/* a match length < MIN_MATCH_LEN gets a length of 1. */
	if ( dpos->len < MIN_MATCH_LEN ) {
		dpos->len = 1;
		k = 0;

		/* output length code. */
		put_nbits( k, NUM_LEN_BITS );

		/* do not encode window position, just the byte. */
		k = (unsigned char) pattern[pat_cnt];
		put_nbits( k, 8 );
	}
	else {
		k = dpos->len - MIN_MATCH_LEN + 1;

		/* output length code. */
		put_nbits( k, NUM_LEN_BITS );

		k = dpos->pos;
		put_nbits( k, NUM_POS_BITS );
	}

	/* if its a match, then move the buffer. */
	j = win_cnt;
	for ( i = 0; i < (dpos->len); i++, j++ ) {
		/* first remove this position from its character list. */
		delete_lznode( *(win_buf + (j & WIN_MASK)), j & WIN_MASK );

		/* write the character to the window buffer. */
		*(win_buf + (j & WIN_MASK)) =
			*(pattern + ((pat_cnt+i) % PAT_BUFSIZE));

		/* insert this position to the new character's list. */
		insert_lznode( *(win_buf + (j & WIN_MASK)), j & WIN_MASK );
	}

	/* get dpos.len bytes */
	for ( i = 0; i < (dpos->len); i++ ){
		if( (k=gfgetc()) != EOF ) {
			*(pattern + ((pat_cnt+i) % PAT_BUFSIZE)) =
				(uchar) k;
		}
		else { break; }
	}

	/* update counters. */
	buf_cnt += i;
	win_cnt = (win_cnt+dpos->len) & WIN_MASK;
	if ( (pat_cnt = (pat_cnt+dpos->len)) >= PAT_BUFSIZE ) {
		pat_cnt -= PAT_BUFSIZE;
	}
}
