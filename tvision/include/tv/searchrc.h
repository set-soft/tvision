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

#if defined( Uses_TSearchRec ) && !defined( __TSearchRec )
#define __TSearchRec

struct TSearchRec
{
    uchar attr;
    time_t time;
    size_t size;
    short ff_fille;
    char name[PATH_MAX];
};

#endif  // Uses_TSearchRec

