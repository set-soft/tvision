/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *

Modified by Robert H”hne to be used for RHIDE.
Added options to customize the file sorting by Salvador E. Tropea.

 *
 *
 */
#include <tv/configtv.h>

#if defined(TVCompf_djgpp) || defined(TVComp_BCPP)
 #include <dos.h>
#endif
#define Uses_string
#define Uses_stdio
#define Uses_dir
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
 const char *nKey1=getName(key1),*nKey2=getName(key2);

 // .* files are also special
 if ((sortOptions & fcolDotsLast) && nKey1[0]!=nKey2[0])
   {
    if (nKey1[0]=='.' && strcmp(nKey1,"..")!=0) return 1;
    if (nKey2[0]=='.' && strcmp(nKey2,"..")!=0) return -1;
   }

 if (sort==fcolAlphabetical)
   {
    if (caseInSens)
       return strcasecmp(nKey1,nKey2);
    else
       return strcmp(nKey1,nKey2);
   }

 // If the names are equal don't swap
 if (caseInSens)
   {
    if (strcasecmp(nKey1,nKey2)==0)
       return 0;
   }
 else
   {
    if (strcmp(nKey1,nKey2)==0)
       return 0;
   }

 // .. is special
 if (strcmp(nKey1,"..")==0)
    return sortOptions & fcolParentLast ? 1 : -1;
 if (strcmp(nKey2,"..")==0)
    return sortOptions & fcolParentLast ? -1 : 1;

 // One is a directory, but not the other
 if ((attr(key1) & FA_DIREC) && (attr(key2) & FA_DIREC)==0)
    return sort==fcolDirsFirst ? -1 : 1;
 if ((attr(key2) & FA_DIREC) && (attr(key1) & FA_DIREC)==0)
    return sort==fcolDirsLast ? -1 : 1;

 // Both of the same type
 if (caseInSens)
    return strcasecmp(nKey1,nKey2);
 return strcmp(nKey1,nKey2);
}


#if !defined( NO_STREAM )
TStreamable *TFileCollection::build()
{
    return new TFileCollection( streamableInit );
}

void TFileCollection::writeItem( void *obj, opstream& os )
{
    TSearchRec *item = (TSearchRec *)obj;
    os << item->attr << (unsigned long)item->time << (unsigned long)item->size;
    os.writeString( item->name );
}

void *TFileCollection::readItem( ipstream& is )
{
    TSearchRec *item = new TSearchRec;
    unsigned long aux1, aux2;
    is >> item->attr >> aux1 >> aux2;
    is.readString( item->name, sizeof(item->name) );
    item->time = (time_t)aux1;
    item->size = (size_t)aux2;
    return item;
}
#endif // NO_STREAM

