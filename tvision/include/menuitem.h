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

#if defined( Uses_TMenuItem ) && !defined( __TMenuItem )
#define __TMenuItem

class TMenu;

class TMenuItem
{

public:

    TMenuItem( const char *aName,
               ushort aCommand,
               ushort aKeyCode,
               ushort aHelpCtx = hcNoContext,
               char *p = 0,
               TMenuItem *aNext = 0
             );
    TMenuItem( const char *aName,
               ushort aKeyCode,
               TMenu *aSubMenu,
               ushort aHelpCtx = hcNoContext,
               TMenuItem *aNext = 0
             );

    ~TMenuItem();

    void append( TMenuItem *aNext );

    TMenuItem *next;
    const char *name;
    ushort command;
    Boolean disabled;
    ushort keyCode;
    ushort helpCtx;
    union
        {
        const char *param;
        TMenu *subMenu;
        };
};

inline void TMenuItem::append( TMenuItem *aNext )
{
    next = aNext;
}

inline TMenuItem &newLine()
{
    return *new TMenuItem( 0, 0, 0, hcNoContext, 0, 0 );
}

#endif  // Uses_TMenuItem

