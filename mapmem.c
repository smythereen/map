/* SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later */
/* Copyright (c) 2007-2026 John R. Smythe Jr. */
/*******************************************************************************
 * 
 *	Memory File Mapping Functions
 *		MapFile2Mem( )
 ******************************************************************************/
#include	"maplocal.h"

/* FUNCTION:	MapFile2Mem
 *
 *	DESCRIPTION:
 *		This subroutine will attach the file as a memory
 *		segment.	
 *
 *	INPUTS:
 *		file path pointer
 *		file length
 *		protection ( PROT_WRITE | 0 )
 *
 *	RETURNS:
 *		address the file is mapped to.
 *
 *	ON ERROR:
 *		This routine returns a NULL pointer.
 */

void	*	MapFile2Mem( char * file, off_t length, u32 prot )
{
	return( Map( file, 0, length, prot | MAP_SHRINK ) );
}
