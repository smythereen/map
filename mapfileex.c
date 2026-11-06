/* SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later */
/* Copyright (c) 2007-2026 John R. Smythe Jr. */
/*******************************************************************************
 * 
 *	Memory Mapped File Mapping Function
 *		MapFileEx( )
 ******************************************************************************/
#include	"maplocal.h"

/* FUNCTION:	MapFileEx
 *
 *	DESCRIPTION:
 *		This subroutine will attach the file as shared memory
 *		segment and set up a semaphore on the file.	
 *		The file must already exist!
 *
 *	INPUTS:
 *		file path pointer
 *
 *	RETURNS:
 *		address the file is mapped to.
 *
 *	ON ERROR:
 *		This routine returns a NULL pointer.
 */

void	*	MapFileEx( char * file )
{
	struct	stat	s;			/* Segment Descriptor Pointer */

	if( stat( file, &s ) == -1 ) {
		ErrorLog(
			"MapFileEx Stat: %s (errno=%u)",
			strerror( errno ),
			errno
		);
		return( NULL );
	}
	else {
		SemAllo( s.st_ino );
		return( MapFile( file ) );
	}
}
