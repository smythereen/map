/* SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later */
/* Copyright (c) 2007-2026 John R. Smythe Jr. */
/*******************************************************************************
 * 
 *	Mapped Memory File Map Functions
 *
 *	This file defines the shared memory function.
 *		UnmapAddress( );
 ******************************************************************************/
#include	"maplocal.h"

/* FUNCTION:	UnmapAddress
 *
 *	DESCRIPTION:
 *		This subroutine may relase shared memory
 *		data back to it associated file.
 *
 *	INPUTS:
 *		adr =	Address to unmap from process' address space
 *
 *	RETURNS:
 *		Nothing!
 *
 *	ON ERROR:
 *		This subroutine will log the error to the printer.
 */

void	UnmapAddress( void * adr, int force_flag )
{
	int	i;
	map_t *	p;

	/*
	 *	Find Address In Map Table
	 */
	for( p = MapTbl, i = 0; i < MAX_MAP_ENTRIES; i++, p++ ) {
		if( p->adr == 0 ) continue;
		if( adr != p->adr ) continue;
		Unmap( p, force_flag );
		return;
	}
	ErrorLog(
		"UnmapAddress( 0x%08X, %u ): Map Segment Not Found!",
		adr,
		force_flag
	);
	return;
}
