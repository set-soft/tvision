/***************************************************************************

  Copyright 2003 by Salvador Eduardo Tropea (SET)
  This file is covered by the GPL license.
  For more information read parttree.cc

***************************************************************************/

#if defined(Uses_TVPartitionTree556) && !defined(TVPartitionTree556_Included)
#define  TVPartitionTree556_Included

class TVPartitionTree556
{
public:
 TVPartitionTree556();
 ~TVPartitionTree556();
 void   add(unsigned unicode, uint16 code);
 uint16 search(unsigned unicode);
 uint16 isearch(unsigned unicode);

protected:
 uint16 ***base;
 #ifdef DEBUG
 int    tables, blocks;
 #endif
};

// This is an inline version for code that really needs speed
inline
uint16 TVPartitionTree556::isearch(unsigned unicode)
{
 uint16 **t=base[unicode>>11];
 if (!t) return 0xFFFF;
 uint16 *l=t[(unicode>>6) & 0x1F];
 if (!l) return 0xFFFF;
 return l[unicode & 0x3F];
}

#endif
