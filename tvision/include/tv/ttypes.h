/*
 Copyright (c) 2000-2001 by Salvador E. Tropea.
 This file was heavily reestructured.
*/

#if !defined( __TTYPES_H )
#define __TTYPES_H

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

#endif	// __TTYPES_H
