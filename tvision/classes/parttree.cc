/**[txh]********************************************************************

  Copyright 2003 by Salvador Eduardo Tropea (SET)
  This file is covered by the GPL license.

  Module: Partition Tree
  Include: TVPartitionTree556
  Comments:
  This module provides something I call "partition tree" (I guess it have
another name but I don't know). This tree is used to implement searches done
entering with a 16 bits value and obtaining a 16 bits value. The seach is
really fast (3 indirections) and saves memory because only a fraction of
the memory is allocated.

***************************************************************************/

#define Uses_TVPartitionTree556
#include <tv.h>

//#define DEBUG
#ifdef DEBUG
 #define IncTables (tables++)
 #define IncBlocks (blocks++)
#else
 #define IncTables
 #define IncBlocks
#endif

TVPartitionTree556::TVPartitionTree556()
{
 base=new uint16 **[32];
 memset(base,0,sizeof(uint16 **)*32);
 #ifdef DEBUG
 tables=blocks=0;
 #endif
}

TVPartitionTree556::~TVPartitionTree556()
{
 int i,j;
 #ifdef DEBUG
 printf("About to release %d tables and %d blocks\n",tables,blocks);
 #endif
 for (i=0; i<32; i++)
    {
     uint16 **t=base[i];
     if (t)
       {
        for (j=0; j<32; j++)
            if (t[j]) delete[] t[j];
        delete[] t;
       }
    }
 delete[] base;
}

void TVPartitionTree556::add(unsigned unicode, uint16 code)
{
 int index1=unicode>>11;
 int index2=(unicode>>6) & 0x1F;
 uint16 **t=base[index1];
 uint16 *l;
 if (t)
   {
    l=t[index2];
    if (!l)
      {
       l=t[index2]=new uint16[64];
       memset(l,0xFF,128);
       IncBlocks;
      }
   }
 else
   {
    t=base[index1]=new uint16 *[32];
    memset(t,0,sizeof(uint16 *)*32);
    l=t[index2]=new uint16[64];
    memset(l,0xFF,128);
    IncTables;
    IncBlocks;
   }
 l[unicode & 0x3F]=code;
}

uint16 TVPartitionTree556::search(unsigned unicode)
{
 uint16 **t=base[unicode>>11];
 if (!t) return 0xFFFF;
 uint16 *l=t[(unicode>>6) & 0x1F];
 if (!l) return 0xFFFF;
 return l[unicode & 0x3F];
}

