/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *

Modified by Robert H”hne to be used for RHIDE.
Modified by Salvador E. Tropea.

 *
 *
 */

#define Uses_string
#include <tv.h>

#include <stdlib.h>

// Avoid replacing new by MSS's macro
#include <tv/no_mss.h>

// SET: MSVC 7.1 is even more stupid than previous versions and we must
// disable this misleading warning.
#ifdef TVComp_MSC
 #pragma warning( disable : 4291 )
#endif

class HistRec
{
    
public:

    HistRec( uchar nId, const char *nStr );

    void *operator new( size_t );
    void *operator new( size_t, HistRec * );
    // SET: This class can't be "deleted", is just some kind of "cast" to
    // interpret a buffer as a list of elements, some kind of "heap helper".
    // MSVC compiler worries about the fact that it oveloads new but
    // doesn't overload delete. So we just make clear that delete isn't
    // valid.
    void  operator delete( void * ) { abort(); };

    uchar id;
    uchar len;
    char str[1];

};

void *HistRec::operator new( size_t, HistRec *hr )
{
    return hr;
}

void NEVER_RETURNS *HistRec::operator new( size_t )
{
    abort();
    RETURN_WHEN_NEVER_RETURNS;
}

inline HistRec::HistRec( uchar nId, const char *nStr ) :
    id( nId ),
    len( strlen( nStr ) + 3 )
{
    strcpy( str, nStr );
}


inline HistRec *advance( HistRec *ptr, size_t s )
{
    return (HistRec *)((char *)ptr + s);
}

inline HistRec *backup( HistRec *ptr, size_t s )
{
    return (HistRec *)((char *)ptr - s);
}

inline HistRec *next( HistRec *ptr )
{
    return ::advance( ptr, ptr->len );
}

inline HistRec *prev( HistRec *ptr )
{
    return backup( ptr, ptr->len );
}

ushort historySize = 4000;  // initial size of history block

static uchar curId;
static HistRec *curRec;
static HistRec *historyBlock;
static HistRec *lastRec;

void advanceStringPointer()
{
    curRec = next( curRec );
    while( curRec < lastRec && curRec->id != curId )
        curRec = next( curRec );
    if( curRec >= lastRec )
        curRec = 0;
}

void deleteString()
{
    size_t len = curRec->len;

    // BUG FIX - EFW - Mon 10/30/95
    // This insures that if n = lastRec, no bytes are copied and
    // a GPF is prevented.
    HistRec *n = next(curRec);
    CLY_memcpy(curRec, n, size_t((char *)lastRec - (char *)n));

    lastRec = backup( lastRec, len );
}

void insertString( uchar id, const char *str )
{
    ushort len = strlen( str ) + 3;
    while( len > historySize - ( (char *)lastRec - (char *)historyBlock ) )
        {
        ushort firstLen = historyBlock->len;
        HistRec *dst = historyBlock;
        HistRec *src = next( historyBlock );
        memmove( dst, src,  size_t( (char *)lastRec - (char *)src ) );
        lastRec = backup( lastRec, firstLen );
	}
    new( lastRec ) HistRec( id, str );
    lastRec = next( lastRec );
}

void startId( uchar id )
{
    curId = id;
    curRec = historyBlock;
}

ushort historyCount( uchar id )
{
    startId( id );
    ushort count =  0;
    advanceStringPointer();
    while( curRec != 0 )
        {
        count++;
        advanceStringPointer();
        }
    return count;
}

void historyAdd( uchar id, const char *str )
{
    if( str[0] == EOS )
	return;
    startId( id );
    advanceStringPointer();
    while( curRec != 0 )
        {
        if( strcmp( str, curRec->str ) == 0 )
            deleteString();
        advanceStringPointer();
        }
    insertString( id, str );
}

const char *historyStr( uchar id, int index )
{
#if 0
    startId( id );
    for( short i = 0; i <= index; i++ )
        advanceStringPointer();
    if( curRec != 0 )
        return curRec->str;
    else
        return 0;
#else // To get the history in reverse order
    int count = historyCount(id);
    startId( id );
    int _index = (count - index) - 1;
    for (int i=0;i<=_index;i++)
        advanceStringPointer();
    if( curRec != 0 )
        return curRec->str;
    else
        return 0;
#endif
}

void clearHistory()
{
    new (historyBlock) HistRec( 0, "" );
    lastRec = next( historyBlock );
}

#include <tv/yes_mss.h>

void initHistory()
{
    historyBlock = (HistRec *) new char[historySize];
    clearHistory();
}

void doneHistory()
{
    delete[] historyBlock;
}
