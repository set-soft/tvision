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

#define Uses_TStringList
#define Uses_TStrIndexRec
#define Uses_ipstream
#include <tv.h>

TStringList::~TStringList()
{
    int i;
    for (i=indexSize;i>0;i--) delete (index+(i-1));
/*
    delete [indexSize] index;
*/
}


void TStringList::get( char *dest, ushort key )
{
    if( indexSize == 0 )
        {
        *dest = EOS;
        return;
        }

    TStrIndexRec *cur = index;
    while ((cur->key + cur->count - 1 < key) && (cur - index < indexSize))
        cur++;
    if ((cur->key + cur->count - 1 < key) || (cur->key > key))
    {
        *dest = EOS;
        return;
    }

    ip->seekg( basePos + cur->offset );
    int count = key - cur->key;
    do  {
        uchar sz = ip->readByte();
        ip->readBytes( dest, sz );
        dest[sz] = EOS;
        } while( count-- > 0 );
}

#if !defined( NO_STREAM )
void *TStringList::read( ipstream& is )
{
    ip = &is;

    ushort strSize;
    is >> strSize;

    basePos = is.tellg();
    is.seekg( basePos + strSize );
    is >> indexSize;
    index = new TStrIndexRec[indexSize];
    is.readBytes( index, indexSize * sizeof( TStrIndexRec ) );
    return this;
}

TStringList::TStringList( StreamableInit )
{
   basePos=0;
   indexSize=0;
   index=0;
}

TStreamable *TStringList::build()
{
    return new TStringList( streamableInit );
}
#endif // NO_STREAM

