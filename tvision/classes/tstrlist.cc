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

#define Uses_TStrIndexRec
#define Uses_TStrListMaker
#define Uses_TStringList
#define Uses_opstream
#include <tv.h>

#include <string.h>

TStrListMaker::TStrListMaker( ushort aStrSize, ushort aIndexSize ) :
    strPos( 0 ),
    strSize( aStrSize ),
    strings( new char[aStrSize] ),
    indexPos( 0 ),
    indexSize( aIndexSize ),
    index( new TStrIndexRec[aIndexSize] )
{
}


TStrListMaker::~TStrListMaker()
{
    delete strings;
    int i;
    for (i=indexSize;i>0;i--) delete (index+(i-1));
}


void TStrListMaker::closeCurrent()
{
    if( cur.count != 0 )
        {
        index[indexPos++] = cur;
        cur.count = 0;
        }
}

void TStrListMaker::put( ushort key, char *str )
{
    if( cur.count == MAXKEYS || key != cur.key + cur.count )
        closeCurrent();
    if( cur.count == 0 )
        {
        cur.key = key;
        cur.offset = strPos;
        }
    int len = strlen( str );
    strings[strPos] = len;
    movmem( str, strings+strPos+1, len );
    strPos += len+1;
    cur.count++;
}

#if !defined( NO_STREAM )
void TStrListMaker::write( opstream& os )
{
    closeCurrent();
    os << strPos;
    os.writeBytes( strings, strPos );
    os << indexPos;
    os.writeBytes( index, indexPos * sizeof( TStrIndexRec ) );
}
#endif // NO_STREAM

