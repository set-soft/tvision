/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *

Modified by Robert H”hne to be used for RHIDE.
Modified by Salvador E. Tropea for i18n support.

 *
 *
 */

#if defined( Uses_TColorGroup ) && !defined( __TColorGroup )
#define __TColorGroup

class TColorItem;

class CLY_EXPORT TColorGroup
{

public:

    TColorGroup( const char *nm, TColorItem *itm = 0, TColorGroup *nxt = 0 );
    virtual ~TColorGroup();
    const char *name;
    TColorItem *items;
    TColorGroup *next;
    friend TColorGroup& operator + ( TColorGroup&, TColorItem& );
    friend TColorGroup& operator + ( TColorGroup& g1, TColorGroup& g2 );

protected:

    stTVIntl *cacheName;

    friend class TColorGroupList;
};

#endif  // Uses_TColorGroup

