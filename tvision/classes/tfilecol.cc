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
#include <string.h>
#ifdef __DJGPP__
#include <dos.h>
#include <dir.h>
#endif
#include <stdio.h>

#define Uses_TFileCollection
#define Uses_TSearchRec
#include <tv.h>

// SET: Added to customize the sorting. These values are the values that make
// the class behave like the Turbo Vision help describes. Note that selecting
// fcolCaseSensitive and not using fcolAlphabetical breaks the incremental
// search trick that looks for Shift to look-up directories.
unsigned TFileCollection::sortOptions=fcolParentLast | fcolDirsLast |
         fcolCaseInsensitive;

inline const char *getName( void *k )
{
    return ((TSearchRec *)k)->name;
}

inline int attr( void *k )
{
    return ((TSearchRec *)k)->attr;
}

// SET: I remade it with 6 possible combinations
int TFileCollection::compare(void *key1, void *key2)
{
 unsigned sort=sortOptions & fcolTypeMask;
 unsigned caseInSens=sortOptions & fcolCaseInsensitive;

 if (!sort)
   {
    if (caseInSens)
       return strcasecmp(getName(key1),getName(key2));
    else
       return strcmp(getName(key1),getName(key2));
   }

 // If the names are equal don't swap
 if (caseInSens)
   {
    if (strcasecmp(getName(key1),getName(key2))==0)
       return 0;
   }
 else
   {
    if (strcmp(getName(key1),getName(key2))==0)
       return 0;
   }

 // .. is special
 if (strcmp(getName(key1),"..")==0)
    return sortOptions & fcolParentLast ? 1 : -1;
 if (strcmp(getName(key2),"..")==0)
    return sortOptions & fcolParentLast ? -1 : 1;

 // One is a directory, but not the other
 if ((attr(key1) & FA_DIREC) && (attr(key2) & FA_DIREC)==0)
    return sort==fcolDirsFirst ? -1 : 1;
 if ((attr(key2) & FA_DIREC) && (attr(key1) & FA_DIREC)==0)
    return sort==fcolDirsLast ? -1 : 1;

 // Both of the same type
 if (caseInSens)
    return strcasecmp(getName(key1),getName(key2));
 return strcmp(getName(key1),getName(key2));
}


#if !defined( NO_STREAM )
TStreamable *TFileCollection::build()
{
    return new TFileCollection( streamableInit );
}

void TFileCollection::writeItem( void *obj, opstream& os )
{
    TSearchRec *item = (TSearchRec *)obj;
    os << item->attr << item->time << item->size;
    os.writeString( item->name );
}

void *TFileCollection::readItem( ipstream& is )
{
    TSearchRec *item = new TSearchRec;
    is >> item->attr >> item->time >> item->size;
    is.readString( item->name, sizeof(item->name) );
    return item;
}
#endif // NO_STREAM

