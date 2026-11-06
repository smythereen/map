/* SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later */
/* Copyright (c) 2007-2026 John R. Smythe Jr. */
/*******************************************************************************
 * 
 *	Mapped Memory File Map Functions
 *
 *	This file defines the mapped memory function.
 *		UnmapFile( );
 ******************************************************************************/
#include	"maplocal.h"

/* FUNCTION:	UnmapFile
 *
 *	DESCRIPTION:
 *		This subroutine may relase mapped memory associated a file.
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

void	UnmapFile( char * file, int force_flag )
{
	struct	stat	s;			/* Segment Descriptor Pointer */
	char	mfile[ 256 ];

	if( *file == '/' ) {
		strncpy( mfile, file, sizeof( mfile ) );
	} else {
		File_Path( mfile, sizeof( mfile ), "MEMORY_PATH", file );
	}
	/*
	 *	Get File Inode Number
	 */
	if( stat( mfile, &s ) == -1 ) {
		ErrorLog(
			"UnmapFile(%s) Stat: %s (errno=%u)",
			mfile,
			strerror( errno ),
			errno
		);
		return;
	}
	UnmapInode( s.st_ino, force_flag );
	return;
}
