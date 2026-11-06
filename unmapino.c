/* SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later */
/* Copyright (c) 2007-2026 John R. Smythe Jr. */
/*******************************************************************************
 * 
 *	Mapped Memory File Map Functions
 *
 *	This file defines the shared memory function.
 *		UnmapFile( );
 ******************************************************************************/
#include	"maplocal.h"

/* FUNCTION:	UnmapInode
 *
 *	DESCRIPTION:
 *		This subroutine may relase shared memory
 *		data back to it associated file.
 *
 *	INPUTS:
 *		file =	File to unmap from process' address space
 *
 *	RETURNS:
 *		Nothing!
 *
 *	ON ERROR:
 *		This subroutine will log the error to the printer.
 */

void	UnmapInode( ino_t inode, int force_flag )
{
	int	i;
	map_t *	p;

	for( p = MapTbl, i = 0; i < MAX_MAP_ENTRIES; i++, p++ ) {
		if( p->adr == 0 ) continue;
		if( p->s.st_ino != inode ) continue;
		Unmap( p, force_flag );
		return;
	}
	ErrorLog(
		"UnmapInode( %u, %u ): Map Segment Not Found!",
		inode,
		force_flag
	);
	return;
}
