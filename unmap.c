/* SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later */
/* Copyright (c) 2007-2026 John R. Smythe Jr. */
/*******************************************************************************
 * 
 *	Mapped Memory File Map Functions
 *
 *	This file defines the mapped memory function.
 *		Unmap( );
 ******************************************************************************/
#include	"maplocal.h"

/* FUNCTION:	Unmap
 *
 *	DESCRIPTION:
 *		This subroutine may unmap memory associated with a file.
 *
 *	INPUTS:
 *		Pointer to map structure
 *
 *	RETURNS:
 *		Nothing!
 *
 *	ON ERROR:
 *		This subroutine will log the error to the printer.
 */

void	Unmap( map_t * p, int force_flag )
{
	if( p == NULL ) return;
	if( force_flag == 0 ) {
		gettimeofday( &p->stamp, NULL );
		Trace(	MAP_TRC_LVL,
			"Slot%4u, Inode%7u, Address 0x%08X, Size %10u - Logically  Unmapped File",
			( ( (u_t)p - (u_t)MapTbl ) / sizeof( map_t ) ),
			p->s.st_ino,
			p->adr,
			p->s.st_size
		);
		SemFree( p->s.st_ino );
		return;
	}
	if( p->adr != 0 && p->length != 0 &&
	    munmap( p->adr, p->length ) == -1 ) {
		ErrorLog(
			"Unmap( %u, %u ) munmap( 0x%08X, %u ): %s (errno=%u)",
			p->s.st_ino,
			force_flag,
			p->adr,
			p->length,
			strerror( errno ),
			errno
		);
		SemFree( p->s.st_ino );
		return;
	}
	if( TracePointer != NULL &&
	    (void *)TracePointer >= (void *)p->adr &&
	    (void *)TracePointer < (void *)&p->adr[ p->s.st_size ] ) {
		TracePointer = 0;
	}
	Trace(	MAP_TRC_LVL,
		"Slot%4u, Inode%7u, Address 0x%08X, Size %10u - Physically Unmapped File",
		( ( (u_t)p - (u_t)MapTbl ) / sizeof( map_t ) ),
		p->s.st_ino,
		p->adr,
		p->s.st_size
	);
	SemFree( p->s.st_ino );
	memset( p, 0, sizeof( map_t ) );
	return;
}
