/* SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later */
/* Copyright (c) 2007-2026 John R. Smythe Jr. */
/*******************************************************************************
 * 
 *	Mapped Memory File Map Functions
 *
 *	This file defines the shared memory function.
 *		UpdateMap( );
 ******************************************************************************/
#include	"maplocal.h"

/* FUNCTION:	UpdateMap
 *
 *	DESCRIPTION:
 *		This subroutine will flush the modified shared memory
 *		data back to it associated file.
 *
 *	INPUTS:
 *		adr =	address of shared segment
 *		len =	length of modified memory in bytes.
 *
 *	RETURNS:
 *		Nothing!
 *
 *	ON ERROR:
 *		This subroutine will log the error to the printer.
 */

void	UpdateMap( void * a, off_t len )
{
	int	i;
	map_t *	p;
	uchar * adr;

	for( adr = (uchar *)a, p = MapTbl, i = 0; i < MAX_MAP_ENTRIES; i++, p++ ) {
		if( p->adr          == 0 ) continue;
		if( p->stamp.tv_sec != 0 ) continue;
		if( adr < p->adr || &adr[ len ] > &p->adr[ p->s.st_size ] ) continue;
		if( msync( adr, len, ( MS_ASYNC | MS_INVALIDATE ) ) == -1 ) {
			ErrorLog(
				"UpdateMap( 0x%08X, %u ) msync: %s (errno=%u)",
				adr,
				len,
				strerror( errno ),
				errno
			);
			return;
		}
		return;
	}
	ErrorLog(
		"UpdateMap( 0x%08X, %u ): Map Segment Not Found!",
		adr,
		len
	);
	return;
}
