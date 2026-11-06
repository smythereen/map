/* SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later */
/* Copyright (c) 2007-2026 John R. Smythe Jr. */
/*******************************************************************************
 * 
 *	Mapped Memory File Map Function
 *
 *	This file defines the mapped memory function.
 *		Unmap( );
 ******************************************************************************/
#include	"maplocal.h"
#include	<utime.h>

/* FUNCTION:	Unmap
 *
 *	DESCRIPTION:
 *		This subroutine may unmap memory associated with a file.
 *
 *	INPUTS:
 *		Pointer to map structure
 *
 *	RETURNS:
 *		-1 = error
 *		 0 = success
 *
 *	ON ERROR:
 *		This subroutine will log the error to the printer.
 */

int	MapFileSetTimes( char * file, time_t * access, time_t * modified )
{
	struct	utimbuf	ub;
	struct	stat	sb;
	char		mfile[ 256 ];

	if( file == NULL ) return( -1 );

	if( *file == '/' || *file == '.' ) {
		strncpy( mfile, file, sizeof( mfile ) );
	} else {
		File_Path( mfile, sizeof( mfile ), "MEMORY_PATH", file );
	}
	/*
	 *	File Existance Check
	 */
	if( stat( mfile, &sb ) == -1 ) return( -1 );
	if( access != NULL ) {
		ub.actime = *access;
	} else {
		ub.actime = sb.st_atime;
	}
	if( modified != NULL ) {
		ub.modtime = *modified;
	} else {
		ub.modtime = sb.st_mtime;
	}
	return( utime( mfile, &ub ) );
}
