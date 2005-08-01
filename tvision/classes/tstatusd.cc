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

#define Uses_TStatusDef
#define Uses_TStatusItem
#include <tv.h>

CLY_EXPORT TStatusDef& operator + ( TStatusDef& s1, TStatusItem& s2 )
{
    TStatusDef *def = &s1;
    while( def->next != 0 )
        def = def->next;
    if( def->items == 0 )
        def->items = &s2;
    else
        {
        TStatusItem *cur = def->items;
        while( cur->next != 0 )
            cur = cur->next;
        cur->next = &s2;
        }
    return s1;
}

CLY_EXPORT TStatusDef& operator + ( TStatusDef& s1, TStatusDef& s2 )
{
    TStatusDef *cur = &s1;
    while( cur->next != 0 )
        cur = cur->next;
    cur->next = &s2;
    return s1;
}

