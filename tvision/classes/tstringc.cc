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

#define Uses_TStringCollection
#define Uses_opstream
#define Uses_ipstream
#include <tv.h>

TStringCollection::TStringCollection( ccIndex aLimit, ccIndex aDelta ) :
    TSortedCollection(aLimit, aDelta)
{
}

int TStringCollection::compare( void *key1, void *key2 )
{
    return strcmp( (char *)key1, (char *)key2 );
}

void TStringCollection::freeItem( void* item )
{
    delete[] (char *)item;
}

#if !defined( NO_STREAM )
TStreamable *TStringCollection::build()
{
    return new TStringCollection( streamableInit );
}

void TStringCollection::writeItem( void *obj, opstream& os )
{
    os.writeString( (const char *)obj );
}

void *TStringCollection::readItem( ipstream& is )
{
    return is.readString();
}
#endif // NO_STREAM

TStringCollection & TStringCollection::operator = (const TStringCollection & pl)
{
  int i;
  duplicates = pl.duplicates;
  freeAll();
  for (i=0;i<pl.count;i++)
  {
    insert(strdup((char *)pl.items[i]));
  }
  return *this;
}

