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
// SET: Moved the standard headers here because according to DJ
// they can inconditionally declare symbols like NULL
#include <stdarg.h>
#include <stdio.h>
#define Uses_string

#define Uses_MsgBox
#define Uses_TObject
#define Uses_TDialog
#define Uses_TStaticText
#define Uses_TRect
#define Uses_TButton
#define Uses_TProgram
#define Uses_TInputLine
#define Uses_TDeskTop
#define Uses_TLabel
#define Uses_TCheckBoxes
#define Uses_TSItem
#define Uses_TScreen
#include <tv.h>

static const char *buttonName[] =
{
    __("~Y~es"),
    __("~N~o"),
    __("~O~K"),
    __("Cancel")
};

static ushort commands[] =
{
    cmYes,
    cmNo,
    cmOK,
    cmCancel
};

static const char *Titles[] =
{
    __("Warning"),
    __("Error"),
    __("Information"),
    __("Confirm")
};

ushort messageBoxRect( const TRect &r, const char *msg, ushort aOptions )
{
    TDialog *dialog;
    short i, x, buttonCount;
    TView* buttonList[5];
    ushort ccode;
    int height=r.b.y-r.a.y;
    TCheckBoxes *dsa=0;
    TRect rlocal=r;

    if (aOptions & mfDontShowAgain)
      {
       rlocal.a.y-=2;
       rlocal.b.y++;
      }

    dialog = new TDialog( rlocal, _(Titles[aOptions & 0x3]) );

    dialog->insert(
        new TStaticText(TRect(3, 2, dialog->size.x - 2, height - 3),
                        msg) );

    if (aOptions & mfDontShowAgain)
      {
       dsa=new TCheckBoxes(TRect(2,height-2,dialog->size.x-2,height-1),
                           new TSItem(_("Don't warn you next time"),0));
       dialog->insert(dsa);
      }

    for( i = 0, x = -2, buttonCount = 0; i < 4; i++ )
        {
        if( (aOptions & (0x0100 << i)) != 0)
            {
            buttonList[buttonCount] =
                new TButton( TRect(0, 0, 10, 2), _(buttonName[i]), commands[i], bfNormal );
            x += buttonList[buttonCount++]->size.x + 2;
            }
        }

    x = (dialog->size.x - x) / 2;

    for( i = 0; i < buttonCount; i++ )
        {
        dialog->insert(buttonList[i]);
        buttonList[i]->moveTo(x, dialog->size.y - 3);
        x += buttonList[i]->size.x + 2;
        }

    dialog->selectNext(False);

    Boolean oldBusy=TScreen::showBusyState(False);
    ccode = TProgram::deskTop->execView(dialog);
    TScreen::showBusyState(oldBusy);
    if (aOptions & mfDontShowAgain)
      {
       ushort val;
       dsa->getData(&val);
       if (val)
          ccode|=0x8000; // Not so clean but cmOK,Yes,etc are low values
      }

    TObject::destroy( dialog );
    
    return ccode;
}

ushort messageBoxRect( const TRect &r,
                       ushort aOptions,
                       const char *fmt,
                       ... )
{
    va_list argptr;
    va_start( argptr, fmt );
    char msg[256];
    vsprintf( msg, fmt, argptr );
    va_end( argptr );
    return messageBoxRect( r, msg, aOptions );
}

static TRect makeRect()
{
    TRect r( 0, 0, 40, 9 );
    r.move((TProgram::deskTop->size.x - r.b.x) / 2,
           (TProgram::deskTop->size.y - r.b.y) / 2);
    return r;
}

ushort messageBox( const char *msg, ushort aOptions )
{
    return messageBoxRect( makeRect(), msg, aOptions );
}

ushort messageBox( ushort aOptions, const char *fmt, ... )
{
    va_list argptr;
#if 0
    va_start( argptr, aOptions );
#else
    va_start( argptr, fmt );
#endif

    char msg[256];
    vsprintf( msg, fmt, argptr );
#if 1
    va_end( argptr );
#endif
    return messageBoxRect( makeRect(), msg, aOptions );
}

ushort inputBox( const char *Title, const char *aLabel, char *s, int limit )
{
    TRect r(0, 0, 60, 8);
    r.move((TProgram::deskTop->size.x - r.b.x) / 2,
           (TProgram::deskTop->size.y - r.b.y) / 2);
    return inputBoxRect(r, Title, aLabel, s, limit);
}

ushort inputBoxRect( const TRect &bounds,
                     const char *Title,
                     const char *aLabel,
                     char *s,
                     int limit )
{
    TDialog *dialog;
    TView* control;
    TRect r;
    ushort c;

    dialog = new TDialog(bounds, Title);

    r = TRect( 4 + strlen(aLabel), 2, dialog->size.x - 3, 3 );
    control = new TInputLine( r, limit );
    dialog->insert( control );

    r = TRect(2, 2, 3 + strlen(aLabel), 3);
    dialog->insert( new TLabel( r, aLabel, control ) );

    r = TRect( dialog->size.x - 24, dialog->size.y - 4,
               dialog->size.x - 14, dialog->size.y - 2);
    dialog->insert( new TButton(r, _("~O~K"), cmOK, bfDefault));

    r.a.x += 12;
    r.b.x += 12;
    dialog->insert( new TButton(r, _("Cancel"), cmCancel, bfNormal));

    r.a.x += 12;
    r.b.x += 12;
    dialog->selectNext(False);
    dialog->setData(s);
    c = TProgram::deskTop->execView(dialog);
    if( c != cmCancel )
        dialog->getData(s);
    TObject::destroy( dialog );
    return c;
}

