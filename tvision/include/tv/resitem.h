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

#if defined( Uses_TResourceItem ) && !defined( __TResourceItem )
#define __TResourceItem

struct TResourceItem
{

    long pos;
    long size;
    char *key;
};

#endif  // Uses_TResourceItem

