/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *

Modified by Robert H”hne to be used for RHIDE.
Modified to compile with gcc v3.x by Salvador E. Tropea, with the help of
Andris Pavenis.

 *
 *
 */

#define Uses_TResourceFile
#define Uses_TResourceItem
#define Uses_fpstream
#define Uses_filelength
#define Uses_TCollection
#define Uses_TStringCollection
#define Uses_TResourceCollection
#include <tv.h>

UsingNamespaceStd

const long rStreamMagic = 0x52504246uL; // 'FBPR'

#pragma pack(1)

struct Count_type
{
 ushort lastCount CLY_Packed;
 ushort pageCount CLY_Packed;
};

struct Info_type
{
 ushort infoType CLY_Packed;
 long   infoSize CLY_Packed;
};

struct THeader
{
 ushort signature  CLY_Packed;
 union
 {
  Count_type count;
  Info_type  info;
 } CLY_Packed;
};

#pragma pack()

TResourceFile::TResourceFile( fpstream *aStream ) : TObject()
{
    THeader *header;
    int found;
    int repeat;
    long streamSize;

    stream = aStream;
    basePos = stream->tellp();
    streamSize = stream->filelength();
    header = new THeader;
    found = 0;
    do {
       repeat = 0;
       if ((unsigned long)basePos <= (streamSize - sizeof(THeader)))
           {
           stream->seekg(basePos, CLY_IOSBeg);
           stream->readBytes(header, sizeof(THeader));
           if (header->signature == 0x5a4d)
               {
               basePos += ((header->count.pageCount * 512L) -
                          (-header->count.lastCount & 511));
               repeat = 1;
               }
           else if (header->signature == 0x4246)
               {
               if (header->info.infoType == 0x5250)
                   found = 1;
               else
                   {
                   basePos += 
                      header->info.infoSize + 16 - (header->info.infoSize)%16;
                   repeat = 1;
                   }
               }
           }
        } while (repeat);

    if (found)
    {
        stream->seekg(basePos + CLY_StreamPosT(sizeof(long) * 2), CLY_IOSBeg);
        long aux;
        *stream >> aux;
        indexPos=aux;
        stream->seekg(basePos + indexPos, CLY_IOSBeg);
        *stream >> index;
    }
    else
    {
        indexPos =  sizeof(long) * 3;
        index = new TResourceCollection(0, 8);
    }
    delete header;
}

TResourceFile::~TResourceFile()
{
    flush();
    CLY_destroy( (TCollection *)index );
    delete(stream);
}

short TResourceFile::count()
{
    return index->getCount();
}

// Avoid replacing free by MSS's macro
#include <tv/no_mss.h>

void TResourceFile::remove( const char *key )
{
    int i;

    if (index->search( (char *)key, i))
        {
        index->free(index->at(i));
        modified = True;
        }
}

#include <tv/yes_mss.h>

void TResourceFile::flush()
{
    long lenRez;

    if (modified == True)
    {
        stream->seekg(basePos + indexPos, CLY_IOSBeg);
        *stream << index;
        lenRez =  stream->tellp() - basePos -  CLY_StreamPosT(sizeof(long) * 2);
        stream->seekg(basePos, CLY_IOSBeg);
        *stream << rStreamMagic;
        *stream << lenRez;
        *stream << long(indexPos);
        stream->flush();
        modified = False;
    }
}

void *TResourceFile::get( const char *key)
{
    int i;
    void *p;

    if (! index->search((char *)key, i))
        return  0;
    stream->seekg(basePos + CLY_StreamPosT(((TResourceItem*)(index->at(i)))->pos),
                  CLY_IOSBeg);
    *stream >> p;
    return p;
}

const char *TResourceFile::keyAt(short i)
{
    return ((TResourceItem*)(index->at(i)))->key;
}

void TResourceFile::put(TStreamable *item, const char *key)
{
    int i;
    TResourceItem  *p;

    if (index->search( (char *)key, i))
        p = (TResourceItem*)(index->at(i));
    else
    {
        p = new TResourceItem;
        p->key = newStr(key);
        index->atInsert(i, p);
    }
    p->pos =  indexPos;
    stream->seekp(basePos + indexPos, CLY_IOSBeg);
    *stream << item;
    indexPos = stream->tellp() - basePos;
    p->size  = indexPos - CLY_StreamPosT(p->pos);
    modified = True;
}

