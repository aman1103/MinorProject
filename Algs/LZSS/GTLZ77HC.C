/*
	---- An LZ77 Coding + Adaptive Huffman Implementation ----

	Filename:               GTLZ77HC.C
	Decompression program:  gtlz77hd.c
	Date:                   August 15, 2004

	The program here only serves as a working example of LZ77.
	Improving the search function is very important for
	LZ77; you can use a better search function like a hash
	search or a list search function (which I used here).

	The implementation uses a table of integer pointers, each one
	for a *list* of character/symbol to hold the positions in the
	window for that character. The search function can thus quickly
	"skip" to the next correct position in the window.

	The list is a DOUBLY-LINKED LIST to make deletion of
	nodes faster.

	Encoding:

		literal byte:   2 bits + 8 bits
		match == 2  :   2 bits + position
		match  > 2  :   1 bit  + length + position

	The program maps the match length to the 4-bit codes: a 3 becomes
	0, a 4 becomes 1, and so on.  Thus the formula to map the length
	codes is k -= 3, where k is the length code. In the decompression
	phase, the decompressor will also adjust the value of the length
	codes: k += 3.

	Decompression in LZ77 is faster since you just have to extract the
	bytes from the window buffer using the pos and len variables.

	NOTES:

		-- This program uses a faster list search function
		though brutef_search() is included for reference; to use
		brutef_search(), you just have to "comment out" the code
		that uses the list nodes such as delete_lznode() and
		insert_lznode().

		-- The match length code is further compressed by an Adaptive
		Huffman function.

	To compile:   tcc   -w gtlz77hc gtbitio.c lzlist.c huf.c
	              bcc32 -w gtlz77hc gtbitio.c lzlist.c huf.c
	              g++  -O2 gtlz77hc.c gtbitio.c lzlist.c huf.c -s -o gtlz77hc
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utypes.h"
#include "gtbitio.h"
#include "lzlist.h"
#include "adhfgk.c"

/* the decompressor's must also equal these values. */
#define WIN_BUFSIZE   4096
#define NUM_POS_BITS    12
#define WIN_MASK      4095

#define PAT_BUFSIZE     18
#define NUM_LEN_BITS     4

typedef struct {
	char algorithm[4];
	ulong file_size;
} file_stamp;

typedef struct {
	uint pos, len;
} dpos_t;

dpos_t dpos;
uchar win_buf[ WIN_BUFSIZE ];      /* the "sliding" window buffer. */
uchar pattern[ PAT_BUFSIZE ];      /* the "look-ahead" buffer. */
uint win_cnt = 0, pat_cnt = 0, buf_cnt = 0;  /* some counters. */

dpos_t search( uchar w[], uchar p[] );          /* "linked-list" search. */
dpos_t brutef_search( uchar w[], uchar p[] );   /* "brute-force" search. */
void copyright( void );
void put_codes( dpos_t *dpos );

int main( int argc, char *argv[] )
{
	unsigned long in_file_len = 0, out_file_len = 0;
	float ratio = 0.0;
	unsigned int i;
	file_stamp fstamp;

	if ( argc != 3 ) {
		fprintf(stderr, "\n Usage: gtlz77hc infile outfile");
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
	init_buffer_sizes( 2048 );
	init_put_buffer();

	fprintf(stderr, "\n----[ A Lempel-Ziv (LZ77) Coding + Adaptive Huffman Implementation ]----\n");
	fprintf(stderr, "\nWindow Buffer size used  = %15lu bytes", (ulong) WIN_BUFSIZE );
	fprintf(stderr, "\nLook-Ahead Buffer size   = %15lu bytes", (ulong) PAT_BUFSIZE );

	fprintf(stderr, "\n\nName of input file : %s", argv[1] );

	/* display file length. */
	fseek( gIN, 0, SEEK_END );
	in_file_len = ftell( gIN );
	fprintf(stderr, "\nLength of input file     = %15lu bytes", in_file_len );

	/* Write the FILE STAMP. */
	rewind( pOUT );
	strcpy( fstamp.algorithm, "LZ7" );
	fstamp.file_size = in_file_len;
	fwrite( &fstamp, sizeof(file_stamp), 1, pOUT );

	/* start Compressing to output file. */
	fprintf(stderr, "\n\nCompressing...");

	/* initialize the table of pointers. */
	init_lzlist();

	/* set the sliding-window to all zero (0) values. */
	memset( win_buf, 0, WIN_BUFSIZE );

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

	/* ---- adaptive Huffman initializations. ---- */
	hmax_symbols = PAT_BUFSIZE-2;
	hsymbol_bit_size = NUM_LEN_BITS;
	hmin = 0;
	adh_init_first_node( hmin );

	/* compress */
	while( buf_cnt > 0 ) {	/* look-ahead buffer not empty? */
		dpos = search( win_buf, pattern );

		/* encode prefix bits. */
		if ( dpos.len > 2 ) {  /* more than TWO match? */
			put_ONE();           /* yes, send a 1 bit. */
		}
		else if ( dpos.len == 2 ) { /* exactly TWO matching characters? */
			put_ZERO();          /* yes, send a 0 bit. */
			put_ONE();           /* and a 1 bit. */
		}
		else {  /* no matching character. */
			put_ZERO();          /* send a 0 bit. */
			put_ZERO();          /* one more 0 bit to indicate a no match. */
		}

		/* encode window position or len codes. */
		put_codes( &dpos );
	}
	flush_put_buffer();
	fprintf(stderr, "complete.");

	/* get outfile's size and get compression ratio. */
	out_file_len = ftell( pOUT );

	fprintf(stderr, "\n\nName of output file: %s", argv[2] );
	fprintf(stderr, "\nLength of input file     = %15lu bytes", in_file_len );
	fprintf(stderr, "\nLength of output file    = %15lu bytes", out_file_len );

	ratio = (((float) in_file_len - (float) out_file_len) /
		(float) in_file_len ) * (float) 100;
	fprintf(stderr, "\nCompression ratio:         %15.2f %%\n", ratio );
	
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

/*
This function searches the sliding window buffer for the largest
"string" stored in the pattern buffer.

The function uses an "array of integer pointers" to doubly-linked
lists, which contain the various occurrences or "positions" of a
particular character in the sliding-window.

A match length of 0 is quickly set to a length code of 1
(we have to "slide" through the window buffer at least
one character at a time).
*/
dpos_t search ( uchar w[], uchar p[] )
{
	register int i, j, k;
	dpos_t dpos = { 0, 1 };	/* set match length to 1. */

	/* point to start of lzlist[c]. */
	i = lzlist[ p[pat_cnt] ]; /* start window search here. */

	if ( buf_cnt > 1 ) while ( i != LZ_NULL ) {
		/* start search at the second characters. */
		if ( (j = (pat_cnt+1)) == PAT_BUFSIZE ) j = 0;
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

	/* update buffer count. */
	buf_cnt -= dpos.len;

	return dpos;
}

/*
This function is a pattern-matching engine which employs a
"brute-force" approach to searching strings.
*/
dpos_t brutef_search ( uchar w[], uchar p[] )
{
	register int i, j, k;
	dpos_t dpos = { 0, 1 };

	/* brute-force search. */
	/* start reading at i = 0. */
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

			/*
				otherwise, do a thorough test for max match.
				but slower, of course.
			*/
		}
	}

	/* update buffer count. */
	buf_cnt -= dpos.len;

	return dpos;
}

/*
Transmits a length/position pair of codes according
to the match length received.

Due to the algorithm, we only encode the match length if it
is greater than 2. Next, a byte or a "position code" is transmitted.

Then this function properly performs the "sliding" part by
copying the matched characters to the window buffer; note that
the linked list is also updated.

Finally, it "gets" characters from the input file according
to the number of matching characters.
*/
void put_codes( dpos_t *dpos )
{
	int c, i, j, k;

	if ( dpos->len > 2 ) {	/* encode len code only if > 2. */
		k = (unsigned int) dpos->len;
		k -= 3;	/* map 3 to 0, 4 to 1, and so on. */
		/* ---- Huffman-encode the match length. ---- */
		adh_encode_symbol( k );
	}

	/* encode position for match len >= 2. */
	if ( dpos->len >= 2 ) {
		k = (unsigned int) dpos->pos;
		put_nbits( k, NUM_POS_BITS );
	}
	else {	/* but if len < 2, do not encode window position, */
		/* just the byte. */
		k = (unsigned char) pattern[pat_cnt];
		put_nbits( k, 8 );
	}

	/* if its a match, then move the buffer! */
	j = win_cnt & WIN_MASK;
	for ( i = 0; i < (dpos->len); i++, j++ ) {
		/* first remove this position from its character list. */
		delete_lznode( *(win_buf + (j & WIN_MASK)), (j & WIN_MASK) );

		/* write the character to the window buffer. */
		*(win_buf + (j & WIN_MASK)) =
			*(pattern + ((pat_cnt+i) % PAT_BUFSIZE));

		/* insert this position to the new character's list. */
		insert_lznode( *(win_buf + (j & WIN_MASK)), (j & WIN_MASK) );
	}

	/* get dpos.len bytes */
	for ( i = 0; i < (dpos->len); i++ ){
		if( (c = gfgetc()) != EOF ) {
			*(pattern + ((pat_cnt+i) % PAT_BUFSIZE)) =
				(uchar) c;
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
