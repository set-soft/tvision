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

#define Uses_TResourceFile
#define Uses_TResourceItem
#define Uses_fpstream
#define Uses_TCollection
#define Uses_TStringCollection
#define Uses_TResourceCollection
#include <tv.h>

const long rStreamMagic = 0x52504246uL; // 'FBPR'

#pragma pack(1)

struct Count_type
{
    ushort lastCount __attribute__((packed));
    ushort pageCount __attribute__((packed));
};

struct Info_type
{
    ushort infoType __attribute__((packed));
    long infoSize __attribute__((packed));
};

struct THeader
{
    ushort signature __attribute__((packed));
    union
        {
        Count_type count __attribute__((packed));
        Info_type info __attribute__((packed));
        };
};

#pragma pack()

TResourceFile::TResourceFile( fpstream *aStream ) : TObject()
{
    THeader *header;
    int handle;
    int found;
    int repeat;
    long streamSize;

    stream = aStream;
    basePos = stream->tellp();
    handle = stream->rdbuf()->fd();
    streamSize = filelength(handle);
    header = new THeader;
    found = 0;
    do {
       repeat = 0;
       if ((unsigned long)basePos <= (streamSize - sizeof(THeader)))
           {
           stream->seekg(basePos, ios::beg);
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
        stream->seekg(basePos + sizeof(long) * 2, ios::beg);
        *stream >> indexPos;
        stream->seekg(basePos + indexPos, ios::beg);
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
    destroy( (TCollection *)index );
    delete(stream);
}

short TResourceFile::count()
{
    return index->getCount();
}

void TResourceFile::remove( const char *key )
{
    int i;

    if (index->search( (char *)key, i))
        {
        index->free(index->at(i));
        modified = True;
        }
}

void TResourceFile::flush()
{
    long lenRez;

    if (modified == True)
    {
        stream->seekg(basePos + indexPos, ios::beg);
        *stream << index;
        lenRez =  stream->tellp() - basePos -  sizeof(long) * 2;
        stream->seekg(basePos, ios::beg);
        *stream << rStreamMagic;
        *stream << lenRez;
        *stream << indexPos;
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
    stream->seekg(basePos + ((TResourceItem*)(index->at(i)))->pos, ios::beg);
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
    stream->seekp(basePos + indexPos, ios::beg);
    *stream << item;
    indexPos = stream->tellp() - basePos;
    p->size  = indexPos - p->pos;
    modified = True;
}

