/* SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later */
/* Copyright (c) 2007-2026 John R. Smythe Jr. */

#include	"maplocal.h"

int	main( int ac, char ** av )
{
	char	*	p;
	u32		i;
	u32		j;
	s32		length = 0;
	s64		offset = 0;

	TraceLevel = MAP_TRC_LVL;
	LocalName = av[ 0 ];
	switch( ac ) {
	case 4:	length = strtol( av[ 3 ], 0, 10 );
	case 3:	offset = strtoll( av[ 2 ], 0, 10 );
	case 2:	p = Map( av[ 1 ], offset, length, PROT_WRITE | MAP_SHRINK );
		if( p != NULL ) break;
	default:
		fprintf( stderr, "\nUsage:\n\t%s\tfile [ offset [ size ] ]\n\n", av[ 0 ] );
		fflush( stderr );
		exit( 1 );
	}
	printf(	"\n0x%08X = Map( \"%s\", %01lld, %01d, 0x%X );\n",
		p, av[ 1 ], offset, length, PROT_WRITE | MAP_SHRINK
	);
	for( i = 0; i < MapFileSize - 8; ) {
		j = sprintf( p, "A%06u\n", i );
		p += j;
		i += j;
	}
	p += sprintf( p, "A%06u", i );
	*p = '\n';
	offset = -1;
	length = MapFileSize;
	sleep( 30 );
	printf(	"0x%08X = Map( \"%s\", %01lld, %01d, 0x%X );\n",
		p, av[ 1 ], offset, length, PROT_WRITE | MAP_SHRINK
	);
	p = Map( av[ 1 ], offset, length, PROT_WRITE | MAP_SHRINK );
	printf(	"0x%08X = Map( \"%s\", %01lld, %01d, 0x%X );\n",
		p, av[ 1 ], offset, length, PROT_WRITE | MAP_SHRINK
	);
	for( i = length, p += length; i < MapFileSize - 8; ) {
		j = sprintf( p, "B%06u\n", i );
		p += j;
		i += j;
	}
	p += sprintf( p, "B%06u", i );
	*p = '\n';
	sleep( 60 );
	exit( 0 );
}
