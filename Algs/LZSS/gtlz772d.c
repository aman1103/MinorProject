#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
	Filename:		gtlz772d.c (August 15, 2004)

	Decompression in LZ77 is faster since you just have to extract
	the bytes from the window buffer using the pos and len variables.
*/
#include "utypes.h"
#include "gtbitio.h"

#define WIN_BUFSIZE    4096
#define NUM_POS_BITS     12
#define WIN_MASK       4095

#define PAT_BUFSIZE      18
#define NUM_LEN_BITS      4

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
uint win_cnt = 0;

void copyright( void );

int main( int argc, char *argv[] )
{
	unsigned long fsize = 0;
	uint i, k;
	FILE *out;
	file_stamp fstamp;

	if ( argc != 3 ) {
		fprintf(stderr, "\n Usage: gtlz772d infile outfile");
		copyright();
		return 0;
	}

	if ( (gIN = fopen( argv[1], "rb" )) == NULL ) {
		fprintf(stderr, "\nError opening input file.");
		return 0;
	}
	fread( &fstamp, sizeof(file_stamp), 1, gIN );
	init_get_buffer();

	if ( (out = fopen( argv[2], "wb" )) == NULL ) {
		fprintf(stderr, "\nError opening output file.");
		goto halt_prog;
	}

	fprintf(stderr, "\n Name of input  file : %s", argv[1] );
	fprintf(stderr, "\n Name of output file : %s", argv[2] );

	fprintf(stderr, "\n\n Decompressing...");

	/* initialize sliding-window. */
	memset( win_buf, 0, WIN_BUFSIZE );

	fsize = fstamp.file_size;
	while( fsize ) {
		if ( get_bit() == 1 ){
			/* get length. */
			dpos.len = get_nbits( NUM_LEN_BITS );

			/* map true values: 0 to 3, 1 to 4, and so on. */
			dpos.len += 3;

			/* get position. */
			dpos.pos = get_nbits( NUM_POS_BITS );

			/* if its a match, then "slide" the window buffer. */
			i = dpos.len;
			while ( i-- ) {
				/* copy byte. */
				pattern[i] = win_buf[ (dpos.pos+i) & WIN_MASK ];
			}
			i = dpos.len;
			while ( i-- ) {
				win_buf[ (win_cnt+i) & WIN_MASK ] = pattern[ i ];
			}
			
			/* output string. */
			fwrite( pattern, dpos.len, 1, out );
			fsize -= dpos.len;
			win_cnt = (win_cnt + dpos.len) & WIN_MASK;
		}
		else {
			switch( get_bit() ){
			case 0:

			/* get byte and output it. */
			k = get_nbits( 8 );
			fputc( (unsigned char) k, out );
			fsize--;

			win_buf[ win_cnt ] = (unsigned char) k;
			if ( (++win_cnt) == WIN_BUFSIZE ) win_cnt = 0;

			break;

			case 1:

			/* get position. */
			dpos.pos = get_nbits( NUM_POS_BITS );
			dpos.len = 2;

			/* if its a match, then "slide" the window buffer. */
			i = dpos.len;
			while ( i-- ) {
				/* copy byte. */
				pattern[i] = win_buf[ (dpos.pos+i) & WIN_MASK ];
			}
			i = dpos.len;
			while ( i-- ) {
				win_buf[ (win_cnt+i) & WIN_MASK ] = pattern[ i ];
			}
			
			/* output string. */
			fwrite( pattern, dpos.len, 1, out );
			fsize -= dpos.len;
			win_cnt = (win_cnt + dpos.len) & WIN_MASK;
			
			break;
			}	/* end switch */
		}
	}
	fprintf(stderr, "complete.");
	copyright();
	
	halt_prog:

	free_get_buffer();
	if ( gIN ) fclose( gIN );
	if ( out ) fclose( out );

	return 0;
}

void copyright( void )
{
	fprintf(stderr, "\n\n Written by: Gerald Tamayo, 2004\n");
}
