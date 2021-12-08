/*
	Filename:		lzhhx.c	(this is the eXtractor), 2006
	Encoder :		lzhh.c

	NOTES:

	-- Decompression in LZ77 is faster since you just have to
	extract the bytes from the window buffer using the pos and
	len variables.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
	note that "lzlist.h" is not needed here;
	we are only updating the window buffer.
*/
#include "utypes.h"
#include "gtbitio.c"
#include "huf.c"
#include "adhfgk.c"

#define NUM_POS_BITS    14
#define WIN_BUFSIZE     (1<<NUM_POS_BITS)
#define WIN_MASK        (WIN_BUFSIZE-1) 

#define NUM_LEN_BITS     8
#define PAT_BUFSIZE      ((1<<NUM_LEN_BITS)+MIN_LEN)
#define MIN_LEN          3

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
	unsigned long fsize=0;
	uint i, k;
	FILE *out;
	file_stamp fstamp;

	if ( argc != 3 ) {
		fprintf(stderr, "\n Usage: lzhhx infile outfile");
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

	/* adaptive huffman initializations. */
	hmin = 0;
	hmax_symbols = PAT_BUFSIZE-MIN_LEN;
	hsymbol_bit_size = NUM_LEN_BITS;
	adh_init_first_node( hmin );

	/* start decompression. */
	fsize = fstamp.file_size;
	while( fsize ) {
		if ( get_bit() == 1 ){	/* match len > MIN_LEN? */

			/* decode match len using Adaptive Huffman. */
			k = (unsigned char) adh_decode_symbol();

			/* map true values: 0 to (MIN_LEN+1), 1 to (MIN_LEN+2), and so on. */
			k += (MIN_LEN+1);
			dpos.len = k;

			/* get window position. */
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

			/* slide the window buffer. */
			win_buf[ win_cnt ] = (unsigned char) k;
			if ( (++win_cnt) == WIN_BUFSIZE ) win_cnt = 0;
			break;

			case 1:

			/* get window position. */
			dpos.pos = get_nbits( NUM_POS_BITS );
			dpos.len = MIN_LEN;    /* we know len == MIN_LEN. */
			
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
	fprintf(stderr, "complete.\n");

	halt_prog:

	free_get_buffer();
	if ( gIN ) fclose( gIN );
	if ( out ) fclose( out );

	return 0;
}

void copyright( void )
{
	fprintf(stderr, "\n\n Written by: Gerald Tamayo, 2006\n");
}
