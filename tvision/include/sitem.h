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

#if defined( Uses_TSItem ) && !defined( __TSItem )
#define __TSItem

class TSItem
{

public:

    TSItem( const char *aValue, TSItem *aNext )
        { value = newStr(aValue); next = aNext; }
    ~TSItem() { delete[] value; }

    const char *value;
    TSItem *next;
};

#endif  // Uses_TSItem

