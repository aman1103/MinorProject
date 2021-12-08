#include <stdio.h>
#include <stdlib.h>

#if !defined(LZLIST_H)
	#define LZLIST_H

#define LZ_NULL           -1
#define LZ_MAX_SYM       256
#define LZ_MAX_BUFSIZE  8192

extern int lzprev[];
extern int lznext[];

/* ---- this is the table of listheads
				 indexed by 256 characters. ---- */
extern int lzlist[ ];

/* ---- function prototypes. ---- */
void init_lzlist( void );
void insert_lznode( unsigned char c, int i );
void delete_lznode( unsigned char c, int i );

#endif
