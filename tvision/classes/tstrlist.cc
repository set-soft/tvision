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
// SET: Moved the standard headers here because according to DJ
// they can inconditionally declare symbols like NULL
#define Uses_string

#define Uses_TStrIndexRec
#define Uses_TStrListMaker
#define Uses_TStringList
#define Uses_opstream
#include <tv.h>

TStrListMaker::TStrListMaker( ushort aStrSize, ushort aIndexSize )
{
    strPos=0;
    strSize=0;
    strings=new char[aStrSize];
    indexPos=0;
    indexSize=aIndexSize;
    index=new TStrIndexRec[aIndexSize];
}


TStrListMaker::~TStrListMaker()
{
    delete strings;
    delete[] index;
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
    memmove( strings+strPos+1, str, len );
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

