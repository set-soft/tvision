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

#define Uses_TSubMenu
#define Uses_TMenuItem
#define Uses_TMenu
#include <tv.h>

TSubMenu::TSubMenu( const char *nm, ushort key, ushort helpCtx ) : 
    TMenuItem( nm, 0, key, helpCtx )
{
}

CLY_EXPORT TSubMenu& operator + ( TSubMenu& s, TMenuItem& i )
{
    TSubMenu *sub = &s;
    while( sub->next != 0 )
        sub = (TSubMenu *)(sub->next);

    if( sub->subMenu == 0 )
        sub->subMenu = new TMenu( i );
    else
        {
        TMenuItem *cur = sub->subMenu->items;
        while( cur->next != 0 )
            cur = cur->next;
        cur->next = &i;
        }
    return s;
}

CLY_EXPORT TSubMenu& operator + ( TSubMenu& s1, TSubMenu& s2 )
{
    TMenuItem *cur = &s1;
    while( cur->next != 0 )
        cur = cur->next;
    cur->next = &s2;
    return s1;
}

