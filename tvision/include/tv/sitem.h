/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *

Modified by Robert H”hne to be used for RHIDE.
Modified by Salvador E. Tropea: added operators and append.

 *
 *
 */

#if defined( Uses_TSItem ) && !defined( __TSItem )
#define __TSItem

class TSItem
{

public:

    TSItem( const char *aValue, TSItem *aNext )
        { value = newStr(aValue); next = aNext; }
    ~TSItem() { DeleteArray((char *)value); }
    void append( TSItem *aNext ); // SET: I put it in the same source as TCluster

    const char *value;
    TSItem *next;
};

// by SET
inline TSItem& operator + ( TSItem& s, TSItem& i )
{
    s.append(&i);
    return s;
}

// SET: From TVTools
inline void operator += ( TSItem& i1, TSItem& i2 )
{
    i1 = i1 + i2;
}

#endif  // Uses_TSItem

