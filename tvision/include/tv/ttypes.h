/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *

Modified by Robert H”hne to be used for RHIDE.

 *
 *
 */

#if !defined( __TTYPES_H )
#define __TTYPES_H

#ifdef __GNUC__
#undef Boolean
#undef False
#undef True
#define Boolean bool
#define True true
#define False false
#else
enum Boolean { False, True };
#endif

typedef short int16;
typedef int int32;
typedef unsigned short uint16;
typedef unsigned int uint32;

typedef unsigned short ushort;
typedef unsigned char uchar;

const char EOS = '\0';

enum StreamableInit { streamableInit };

class ipstream;
class opstream;
class TStreamable;
class TStreamableTypes;

typedef int32 ccIndex;
typedef Boolean (*ccTestFunc)( void *, void * );
typedef void (*ccAppFunc)( void *, void * );

const int ccNotFound = -1;

extern uchar specialChars[];
extern uchar o_specialChars[];


#endif	// __TTYPES_H
