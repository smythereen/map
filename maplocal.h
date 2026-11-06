/* SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later */
/* Copyright (c) 2007-2026 John R. Smythe Jr. */
/*******************************************************************************
 * 
 *	Mapped Memory Local Header File
 *
 *	This file defines private structures, variables, and routines
 *	required for shared memory functions.
 ******************************************************************************/
#ifndef	MAPLOCAL_H
#define	MAPLOCAL_H

#if	defined( __cplusplus )
extern	"C"	{
#endif

#include	<fcntl.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<sys/fcntl.h>
#include	<sys/mman.h>
#include	<sys/stat.h>
#include	<sys/types.h>
#include	<sys/time.h>
#include	<unistd.h>

#include	<sem.h>
#include	<map.h>

#define		MAX_MAP_ENTRIES		200

/*
 *	The Mapped Memory To File Structure
 */

typedef	struct	timeval	tv_t;

typedef	struct	map_s	{
	struct	stat	s;	/* File Status */
	uchar	*	adr;	/* A Zero Means Physically Unmapped */
	tv_t		stamp;	/* A non-Zero Means Logically Unmapped */
	u64		offset;
	u_t		length;
	u_t		protection;
}	map_t;

extern	map_t	MapTbl[ ];

/*
 *	Map Unit Private Functions
 */

extern	void	Unmap( map_t * p, int force_flag );

#if	defined( __cplusplus )
}
#endif

#endif
