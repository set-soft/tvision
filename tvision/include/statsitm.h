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

#if defined( Uses_TStatusItem ) && !defined( __TStatusItem )
#define __TStatusItem

class TStatusItem
{

public:

    TStatusItem( const char *aText,
                 ushort key,
                 ushort cmd,
                 TStatusItem *aNext = 0
                );

    TStatusItem *next;
    const char *text;
    ushort keyCode;
    ushort command;

};

inline TStatusItem::TStatusItem( const char *aText,
                                 ushort key,
                                 ushort cmd,
                                 TStatusItem *aNext
                                ) :
    next( aNext ), text( aText ), keyCode( key ), command( cmd )
{
}

#endif  // Uses_TStatusItem

