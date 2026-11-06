/* SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later */
/* Copyright (c) 2007-2026 John R. Smythe Jr. */
/*******************************************************************************
 * 
 *	Mapped Memory File Map Functions
 *
 *	This file defines the shared memory function.
 *		UnmapAll( );
 ******************************************************************************/
#include	"maplocal.h"

/* FUNCTION:	UnmapAll
 *
 *	DESCRIPTION:
 *		This subroutine will physically release ALL
 *		logically unmapped files.
 *
 *	INPUTS:
 *		None
 *
 *	RETURNS:
 *		Nothing!
 *
 *	ON ERROR:
 *		This subroutine will log the error to the printer.
 */

void	UnmapAll( void )
{
	int	i;
	map_t *	p;

	for( p = MapTbl, i = 0; i < MAX_MAP_ENTRIES; i++, p++ ) {
		if( p->adr   == 0 ) continue;	/* Slot Not Used */
		if( p->stamp.tv_sec == 0 ) continue;	/* Memory Still Used */
		Unmap( p, 1 );
	}
	return;
}
