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

#if defined( Uses_TColorItem ) && !defined( __TColorItem )
#define __TColorItem

class TColorGroup;

class TColorItem
{

public:

    TColorItem( const char *nm, uchar idx, TColorItem *nxt = 0 );
    virtual ~TColorItem();
    const char *name;
    uchar index;
    TColorItem *next;
    friend TColorGroup& operator + ( TColorGroup&, TColorItem& );
    friend TColorItem& operator + ( TColorItem& i1, TColorItem& i2 );

};

#endif  // Uses_TColorItem

