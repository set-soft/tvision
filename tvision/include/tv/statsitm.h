/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *

Modified by Robert H”hne to be used for RHIDE.
Modified by Salvador E. Tropea to allow dynamic status bars and i18n support.

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
    ~TStatusItem() { DeleteArray((char *)text); TVIntl::freeSt(intlText); };

    TStatusItem *next;
    const char *text;
    stTVIntl *intlText;
    ushort keyCode;
    ushort command;

};

inline TStatusItem::TStatusItem( const char *aText,
                                 ushort key,
                                 ushort cmd,
                                 TStatusItem *aNext
                                ) :
    next( aNext ), keyCode( key ), command( cmd )
{
 // SET: If we don't allocate a copy is imposible to have dynamic status
 // lines loaded from disk files or other similar stuff. Or if we does it
 // we leak.
 text=newStr(aText);
 intlText=NULL;
}

#endif  // Uses_TStatusItem

