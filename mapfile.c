/* SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later */
/* Copyright (c) 2007-2026 John R. Smythe Jr. */
/*******************************************************************************
 * 
 *	Memory Mapped File Mapping Function
 *		MapFile( )
 ******************************************************************************/
#include	"maplocal.h"

/* FUNCTION:	MapFile
 *
 *	DESCRIPTION:
 *		This subroutine will attach the file as shared memory
 *		segment.	
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

void	*	MapFile( char * file )
{
	return( Map( file, 0, 0, PROT_WRITE ) );
}
