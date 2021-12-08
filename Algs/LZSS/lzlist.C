/*
	---->>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

	Filename:   LZLIST.C
	Author:     Gerald Tamayo
	Date:       August 22, 2004
	
	The code uses a "Doubly-Linked" List.
	This is faster than brute-force (array) search.

	The *prev* pointer makes the virtual deletion of nodes faster,
	making it immediately "point" to the node to be deleted and
	*not* search for it sequentially.

	Inserting a node is also faster since we just insert the new node
	as the "head" of the character list.

	Most importantly, there are actually no *node objects* in this
	implementation; the "idea" of a node is realized only by lzprev[]
	and lznext[] arrays.

	---->>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
*/
#include <stdio.h>
#include <stdlib.h>
#include "lzlist.h"

/* this is the table of listheads indexed by 256 characters. */
int lzlist[ LZ_MAX_SYM ];

/*
	these arrays contain the "previous" and "next" pointers
	of the virtual nodes.
*/
int lzprev[ LZ_MAX_BUFSIZE ];
int lznext[ LZ_MAX_BUFSIZE ];

/*
	initialize the LZLIST heads to -1.
*/
void init_lzlist( void )
{
	unsigned int i;

	for ( i = 0; i < LZ_MAX_SYM; i++ ) {
		lzlist[ i ] = LZ_NULL;
	}
}

/* ---- inserts a node to the LZ list ---- */
/*
	If you think about it, we also perform a greedy search to all
	the nodes in the list indexed by the character, so it should
	not matter if we insert a node in sorted "ascending" order.

	Thus, simply insert at the *beginning* of the list
	(i.e., a "move-to-front" scheme).

	With this improvement, the insert function only uses
	four lines to insert the node!
*/
void insert_lznode( unsigned char c, int i )
{
	int k = lzlist[c];

	/* always insert at the beginning. */
	lzlist[c] = i;
	lzprev[i] = LZ_NULL;
	lznext[i] = k;
	if ( k != LZ_NULL ) lzprev[k] = i;
	/* that's it! */
}

/* ---- deletes an LZ node ---- */
/*
	This is faster because the deletion quickly points to the
	node to be deleted without traversing the list sequentially.
*/
void delete_lznode( unsigned char c, int i )
{
	if ( lzlist[c] == i ) { /* the head of the list? */
		/* the next node becomes the head of the list */
		lzlist[c] = lznext[i];
		if ( lzlist[c] != LZ_NULL )  /* 4/25/2008 */
			lzprev[ lzlist[c] ] = LZ_NULL;
	}
	else {
		lznext[ lzprev[i] ] = lznext[i];
		/* only if there is a node following node i, shall we assign to it. */
		if ( lznext[i] != LZ_NULL ) lzprev[ lznext[i] ]= lzprev[i];
	}
}
