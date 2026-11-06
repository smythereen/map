/* SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later */
/* Copyright (c) 2007-2026 John R. Smythe Jr. */
/*******************************************************************************
 * 
 *	Memory Mapped File Mapping Function
 *		MapGrow( )
 ******************************************************************************/
#include	"maplocal.h"

/* FUNCTION:	MapGrow
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

void	*	MapAndGrowFile( char * file, int delta )
{
	return( Map( file, -1, delta, PROT_WRITE | ( delta < 0 ? MAP_SHRINK : 0 ) ) );
}
