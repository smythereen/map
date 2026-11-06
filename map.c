/* SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later */
/* Copyright (c) 2010-2026 John R. Smythe Jr. */
/*******************************************************************************
 * 
 *	File Memory Mapping Function
 *		Map( )
 ******************************************************************************/
#include	"maplocal.h"

	map_t	MapTbl[ MAX_MAP_ENTRIES ];

	u32	MapFileSize;		/* Mapped Region Size In Bytes	*/
	u64	MapFileOffset;		/* Offset In File To Start The Mapped Region */
	u64	TotalMapFileSize;	/* Current File Length In Bytes	*/
	u64	TotalMapMemorySize;	/* Current Memory Used In Bytes	*/
	u64	MaximumMapMemorySize;	/* Maximum Allowed Memory Usage	*/
	time_t	MapFileAccessTime;	/* Time File Was Last Accessed	*/
	time_t	MapFileModifiedTime;	/* Time File Was Last Modified	*/

static	char	prefix[ 512 ];

static	int	Create_File( char * file, s64 length )
{
	int	fd;

	if( MkDir( file, 0775 ) == -1 ) {
		ErrorLog(
			"%s Error mkdir(%s,0775) %s (errno=%u)",
			prefix,
			file,
			strerror( errno ),
			errno
		);
		return( -1 );
	}
	fd = open( file, ( O_RDWR | O_CREAT ), 0666 ); 
	if( fd == -1 ) {
		ErrorLog(
			"%s Error Open(%s,0x%X,0%o) %s (errno=%u)",
			prefix,
			file,
			( O_RDWR | O_CREAT ),
			0666,
			strerror( errno ),
			errno
		);
		return( -1 );
	}
	if( ftruncate( fd, length ) == -1 ) {
		ErrorLog(
			"%s Ftruncate(%d, %llu): %s (errno=%u)",
			prefix,
			fd,
			length,
			strerror( errno ),
			errno
		);
		close( fd );
		return( -1 );
	}
	return( fd );
}

/* FUNCTION:	Map
 *
 *	DESCRIPTION:
 *		This subroutine will attach a file or a piece of a file
 *		as a shared memory segment.	
 *
 *	INPUTS:
 *		file path pointer
 *		starting offset in file
 *		segment length in bytes
 *		protection ( PROT_WRITE | 0 )
 *
 *	RETURNS:
 *		address the file segment is mapped to.
 *
 *	ON ERROR:
 *		This routine returns a NULL pointer.
 */

void	*	Map( char * file, s64 offset, s32 length, u32 prot )
{
	int		fd;
	int		i;
	void	*	ret;
	map_t	*	p;	/* Current Cache Slot */
	map_t	*	e;	/* First Empty Cache Slot Found */
	map_t	*	o;	/* Oldest Inactive Cache Slot Found */
	struct	stat	sb;
	char		mfile[ 256 ];

	sprintf( prefix, "Map(%s,offset=%lld,length=%d,prot=0x%X)", file, offset, length, prot );
	Trace( MAP_TRC_LVL, prefix );

	fd = -1;
	MapFileSize = 0;
	MapFileOffset = 0;
	MapFileAccessTime = 0;
	MapFileModifiedTime = 0;
	TotalMapFileSize = 0;

	if( file == NULL ) return( NULL );

	if( *file == '/' || *file == '.' ) {
		strncpy( mfile, file, sizeof( mfile ) );
	} else {
		File_Path( mfile, sizeof( mfile ), "MEMORY_PATH", file );
	}
	/*
	 *	File Existance Check
	 */
	while( fd == -1 && stat( mfile, &sb ) == -1 ) {
		switch( errno ) {
		case	ENOTDIR:
		case	ENOENT:
			if( ( prot & PROT_WRITE ) != 0 ) {
				if( offset == 0 && length == 0 ) return( NULL );
				if( offset < 0 || length < 0 ) {
					ErrorLog(
						"%s Cannot Create File! Bad Offset Or Length.",
						prefix
					);
					return( NULL );
				}
				/*
				 *	Try To Create File If It Does Not Exist!
				 */
				if( ( fd = Create_File( mfile, offset + length ) ) != -1 ) break;
				return( NULL );
			}
		default:
			ErrorLog(
				"%s Stat Error: %s (errno=%u)",
				prefix,
				strerror( errno ),
				errno
			);
			return( NULL );
		}
	}
	/*
	 *	If offset & length are zero ?
	 *	Then use existing file size.
	 */
	if( offset == 0 && length == 0 ) {
		length = sb.st_size;
	}
	/*
	 *	If offset is negative ?
	 *	Then length is a file size delta.
	 */
	if( offset < 0 ) {
		length += sb.st_size;
		offset = 0;
	}
	if( offset < 0 || length < 0 ) {
		ErrorLog(
			"%s Bad Offset (%lld) Or Length (%d).",
			prefix,
			offset,
			length
		);
		if( fd != -1 ) close( fd );
		return( NULL );
	}
	/*
	 *	Check The Map Table Cache
	 */
	o = NULL;
	e = NULL;
	for( p = MapTbl, i = 0; i < MAX_MAP_ENTRIES; i++, p++ ) {
		if( p->adr == 0 ) {
			if( e == NULL ) e = p;	/* Mark First Empty Cache Slot */
			continue;
		}
		if( p->stamp.tv_sec != 0 ) {
			if(	o == NULL ||
				( (u32)o->stamp.tv_sec > (u32)p->stamp.tv_sec ) ||
				( ( (u32)o->stamp.tv_sec == (u32)p->stamp.tv_sec  ) &&
				  ( (u32)o->stamp.tv_usec > (u32)p->stamp.tv_usec ) )
			  ) o = p;	/* Mark Oldest Inactive Cache Slot */
		}
		if( p->s.st_ino   != sb.st_ino ) continue;
		if( p->s.st_dev   != sb.st_dev ) continue;
		if( p->s.st_size  != sb.st_size ||
		    p->length     != (u32)length ||
		    p->protection != (u32)prot ||
		    p->offset     != (u64)offset ) {
			Unmap( p, 1 );
			if( e == NULL ) e = p;	/* Mark First Empty Cache Slot */
			continue;
		}
		p->stamp.tv_sec  = 0;		/* Mark As Used */
		p->stamp.tv_usec = 0;		/* Mark As Used */
		Trace(	MAP_TRC_LVL,
			"Slot%4u, Inode%7u, Address 0x%08X, Size %10u - Remapped File \"%s\" ",
			i,
			p->s.st_ino,
			p->adr,
			p->s.st_size,
			mfile
		);
		MapFileSize		= p->length;
		MapFileOffset		= p->offset;
		MapFileAccessTime	= p->s.st_atime;
		MapFileModifiedTime	= p->s.st_mtime;
		TotalMapFileSize	= p->s.st_size;
		if( fd != -1 ) close( fd );
		return( p->adr );
	}
	/*
	 *	Assign Cache Slot
	 */
	if( e != NULL ) {
		p = e;
	} else if( o != NULL ) {
		Unmap( o, 1 );
		p = o;
	} else {
		ErrorLog(
			"%s No Free Map Table Entries!!!",
			prefix,
			mfile
		);
		if( fd != -1 ) close( fd );
		return( NULL );
	}
	if( fd == -1 ) {
		/*
		 *	Always Automatically Grow File
		 *	Shrink File ONLY On Request
		 */
		while( ( ( offset + length ) > sb.st_size ) ||
		       ( ( prot & MAP_SHRINK ) != 0 && ( offset + length ) != sb.st_size ) ) {
			if( truncate( mfile, ( offset + length ) ) == -1 ) {
				ErrorLog(
					"%s Error Truncate(%s, %llu): %s (errno=%u)",
					prefix,
					mfile,
					(u64)( offset + length ),
					strerror( errno ),
					errno
				);
				return( NULL );
			}
			if( stat( mfile, &sb ) == -1 ) {
				ErrorLog(
					"%s Error Stat(%s): %s (errno=%u)",
					prefix,
					mfile,
					strerror( errno ),
					errno
				);
				return( NULL );
			}
		}
		if( length == 0 ) return( NULL );
		/*
		 *	Open File To Map
		 */
		fd = open( mfile, ( prot & PROT_WRITE ? O_RDWR : O_RDONLY ) ); 
		if( fd == -1 ) {
			ErrorLog(
				"%s Error Open(%s,0x%X) %s (errno=%u)",
				prefix,
				mfile,
				( prot & PROT_WRITE ? O_RDWR : O_RDONLY ),
				strerror( errno ),
				errno
			);
			return( NULL );
		}
	}
	/*
	 *	Map The File To A Memory Address
	 */
	ret = mmap(	NULL,				/* Address Suggestion	*/
			length,				/* Length		*/
			PROT_READ|(prot & PROT_WRITE),	/* Protection		*/
			MAP_SHARED,			/* Flags		*/
			fd,
			offset				/* File Offset		*/
	);
	close( fd );
	if( ret == (void *)-1 ) {
		ErrorLog(
			"%s Error Mmap(%s,l=%u,p=0x%X,f=0x%X,o=%llu): %s (errno=%u)",
			prefix,
			mfile,
			length,
			PROT_READ|(prot & PROT_WRITE),	/* Protection		*/
			MAP_SHARED,			/* Flags		*/
			offset,				/* File Offset		*/
			strerror( errno ),
			errno
		);
		return( NULL );
	}
	/*
	 *	Store Data In Cache Slot
	 */
	MapFileSize = length;
	MapFileOffset = offset;
	memcpy( &p->s, &sb, sizeof( p->s ) );
	MapFileAccessTime	= p->s.st_atime;
	MapFileModifiedTime	= p->s.st_mtime;
	p->stamp.tv_sec		= 0;		/* Mark As Used */
	p->stamp.tv_usec	= 0;		/* Mark As Used */
	p->adr			= ret;
	p->offset		= offset;
	p->length		= length;
	p->protection		= prot;
	TotalMapFileSize	= sb.st_size;
	Trace(	MAP_TRC_LVL,
		"Slot%4u, Inode%7u, Address 0x%08X, Size %10u - Mapped File \"%s\" ",
		( ( (u_t)p - (u_t)&MapTbl[0] ) / sizeof( map_t ) ),
		sb.st_ino,
		ret,
		length,
		mfile
	);
	return( ret );
}
