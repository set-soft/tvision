/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *

Modified by Robert H”hne to be used for RHIDE.
Modified by Salvador E. Tropea to add mfDontShowAgain, vsnprintf usage and
i18n. Added TValidator, etc.

 *
 *
 */
// SET: Moved the standard headers here because according to DJ
// they can inconditionally declare symbols like NULL
#include <stdarg.h>
#define Uses_string
#define Uses_stdio

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
#define Uses_AllocLocal
#define Uses_TValidator
#define Uses_snprintf
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

    dialog = new TDialog( rlocal, Titles[aOptions & 0x3] );

    stTVIntl *intlMessage=NULL;
    if (aOptions & mfDontTranslate)
       intlMessage=TVIntl::dontTranslateSt();
    else
       TVIntl::getText(msg,intlMessage);

    dialog->insert(
        new TStaticText(TRect(3, 2, dialog->size.x - 2, height - 3),
                        msg,intlMessage) );

    if (aOptions & mfDontShowAgain)
      {
       dsa=new TCheckBoxes(TRect(2,height-2,dialog->size.x-2,height-1),
                           new TSItem(__("Don't warn you next time"),0));
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

    TObject::CLY_destroy( dialog );
    
    return ccode;
}

ushort messageBoxRect( const TRect &r,
                       ushort aOptions,
                       const char *fmt,
                       ... )
{
    va_list argptr;

    char *intlFmt=TVIntl::getTextNew(fmt);

    va_start( argptr, fmt );
    int l=CLY_vsnprintf(NULL, (size_t)0, intlFmt, argptr);
    va_end( argptr );
    AllocLocalStr(msg,l+1);

    va_start( argptr, fmt );
    CLY_vsnprintf(msg, (size_t)l+1, intlFmt, argptr);
    va_end( argptr );

    DeleteArray(intlFmt);
    return messageBoxRect( r, msg, aOptions | mfDontTranslate );
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

    char *intlFmt=TVIntl::getTextNew(fmt);

    va_start( argptr, fmt );
    int l=CLY_vsnprintf(NULL, (size_t)0, intlFmt, argptr);
    va_end( argptr );
    AllocLocalStr(msg,l+1);

    va_start( argptr, fmt );
    CLY_vsnprintf(msg, (size_t)l+1, intlFmt, argptr);
    va_end( argptr );

    DeleteArray(intlFmt);
    return messageBoxRect( makeRect(), msg, aOptions | mfDontTranslate );
}

ushort inputBox( const char *Title, const char *aLabel, char *s, int limit,
                 TValidator *v )
{   // Use a size according to the label+limit and title
    int len;
    len = max( strlen(aLabel) + 8 + limit, strlen(Title) + 11 );
    len = min( len, 60 );
    len = max( len , 24 );
    TRect r(0, 0, len, 7);
    r.move((TProgram::deskTop->size.x - r.b.x) / 2,
           (TProgram::deskTop->size.y - r.b.y) / 2);
    return inputBoxRect(r, Title, aLabel, s, limit, v);
}

ushort inputBoxRect( const TRect &bounds,
                     const char *Title,
                     const char *aLabel,
                     char *s,
                     int limit,
                     TValidator *v )
{
    TDialog *dialog;
    TInputLine* control;
    TRect r;
    ushort c;

    dialog = new TDialog(bounds, Title);

    unsigned x = 4 + strlen( aLabel );
    r = TRect( x, 2, min(x + limit + 2, (unsigned int)dialog->size.x - 3), 3 );
    control = new TInputLine( r, limit );
    control->setValidator( v );
    dialog->insert( control );

    stTVIntl *intlLabel = NULL;
    r = TRect(2, 2, 3 + strlen(TVIntl::getText(aLabel,intlLabel)), 3);
    dialog->insert( new TLabel( r, aLabel, control, intlLabel ) );

    r = TRect( dialog->size.x / 2 - 11, dialog->size.y - 3,
               dialog->size.x / 2 - 1 , dialog->size.y - 1);
    dialog->insert( new TButton(r, __("~O~K"), cmOK, bfDefault));

    r.a.x += 12;
    r.b.x += 12;
    dialog->insert( new TButton(r, __("Cancel"), cmCancel, bfNormal));

    r.a.x += 12;
    r.b.x += 12;
    dialog->selectNext(False);
    dialog->setData(s);
    c = TProgram::deskTop->execView(dialog);
    if( c != cmCancel )
        dialog->getData(s);
    TObject::CLY_destroy( dialog );
    return c;
}

