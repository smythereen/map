/* SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later */
/* Copyright (c) 2007-2026 John R. Smythe Jr. */
/*******************************************************************************
 * 
 *	Map Memory Segments Header File
 *
 *	This file defines common structures, variables, and routines
 *	need for shared memory access.
 ******************************************************************************/

#ifndef	MAP_H
#define	MAP_H

#if	defined( __cplusplus )
extern	"C"	{
#endif

#include	<sys/types.h>
#include	<sys/mman.h>
#include	<common.h>

#define	MAP_TRC_LVL	0x08000000	/* MAP Unit's Trace Level */
#define	MAP_SHRINK	0x00010000	/* Memory Mapped File Can Shrink */

/*
 *	Global Data Prototypes
 */

extern	u32	MapFileSize;
extern	time_t	MapFileAccessTime;	/* File Access Time BEFORE Map Operation */
extern	time_t	MapFileModifiedTime;	/* File Modification Time BEFORE Map Operation */

/*
 *	Function Prototypes
 */

extern	void	*	Map( char * file, s64 offset, s32 length, u32 prot );
extern	void	*	MapFile( char * file );
extern	void	*	MapFileEx( char * file );
extern	void	*	MapFile2Mem( char * file, off_t len, u32 prot );
extern	int		MapFileSetTimes( char * file, time_t * new_access_time, time_t * new_modified_time );
extern	void		UnmapAddress( void * adr, int force_flag );
extern	void		UnmapAll( void );
extern	void		UnmapFile( char * file, int force_flag );
extern	void		UnmapInode( ino_t inode, int force_flag );
extern	void		UpdateMap( void * adr, off_t len );

#if	defined( __cplusplus )
}
#endif

#endif
