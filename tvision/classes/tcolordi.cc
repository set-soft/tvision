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
#define Uses_string

#define Uses_TKeys
#define Uses_TColorSelector
#define Uses_TMonoSelector
#define Uses_TColorDisplay
#define Uses_TColorItem
#define Uses_TColorItemList
#define Uses_TColorGroup
#define Uses_TColorGroupList
#define Uses_TColorDialog
#define Uses_TEvent
#define Uses_TDrawBuffer
#define Uses_TGroup
#define Uses_TScrollBar
#define Uses_TLabel
#define Uses_TButton
#define Uses_TRect
#define Uses_opstream
#define Uses_ipstream
#define Uses_TPalette
#define Uses_TScreen
// For the "Try" button:
#define Uses_TProgram
#define Uses_TDeskTop
#include <tv.h>

TColorDialog::TColorDialog( TPalette *aPalette, TColorGroup *aGroups ):
    TDialog( TRect( 0, 0, 61, 18 ), _("Colors") )
    , TWindowInit( &TColorDialog::initFrame )
{
    const char *tmp;
    options |= ofCentered;
    pal = aPalette;

    TScrollBar *sb = new TScrollBar( TRect( 18, 3, 19, 14 ) );
    insert( sb );

    groups = new TColorGroupList( TRect( 3, 3, 18, 14 ), sb, aGroups);
    insert( groups );
    tmp = _("~G~roup");
    insert( new TLabel( TRect( 2, 2, 3+cstrlen(tmp), 3 ), tmp, groups ) );

    sb = new TScrollBar( TRect( 41, 3, 42, 14 ) );
    insert( sb );

    TView *p = new TColorItemList( TRect( 21, 3, 41, 14 ), sb, aGroups->items );
    insert( p );
    tmp = _("~I~tem");
    insert( new TLabel( TRect( 20, 2, 21+cstrlen(tmp), 3 ), tmp, p ) );

    forSel = new TColorSelector( TRect( 45, 3, 57, 7 ),
                                 TColorSelector::csForeground );
    insert( forSel );
    tmp = _("~F~oreground");
    forLabel = new TLabel( TRect( 45, 2, 46+cstrlen(tmp), 3 ), tmp, forSel );
    insert( forLabel );

    int ib = TScreen::getBlinkState() ? 0 : 2;

    bakSel = new TColorSelector( TRect( 45, 9, 57, 11 + ib),
                                 TColorSelector::csBackground );
    insert( bakSel );
    tmp = _("~B~ackground");
    bakLabel = new TLabel( TRect( 45, 8, 46+cstrlen(tmp), 9 ), tmp, bakSel );
    insert( bakLabel );

    display = new TColorDisplay( TRect( 44, 12 + ib, 58, 14 + ib),
                                 _("Text ") );
    insert( display );

    monoSel = new TMonoSelector( TRect( 44, 3, 59, 7 ) );
    monoSel->hide();
    insert( monoSel );
    tmp = _("Color");
    monoLabel = new TLabel( TRect( 43, 2, 44+cstrlen(tmp), 3 ), tmp,
                            monoSel );
    monoLabel->hide();
    insert( monoLabel );

    if( aGroups != 0 && aGroups->items != 0 && pal)
        display->setColor( (uchar *)&pal->data[ aGroups->items->index ] );

    insert( new TButton( TRect( 24 - 10*ib, 15, 34 - 10*ib, 17 ),
                         _("~T~ry"), cmTryColors, bfNormal ) );
    insert( new TButton( TRect( 36 - 10*ib, 15, 46 - 10*ib, 17 ),
                         _("~O~K"), cmOK, bfDefault ) );
    insert( new TButton( TRect( 48 - 10*ib, 15, 58 - 10*ib, 17 ),
                         _("Cancel"),
                         cmCancel,
                         bfNormal ) );
    selectNext( False );
}

TColorDialog::~TColorDialog()
{ 
}  

void TColorDialog::handleEvent( TEvent& event )
{
    TDialog::handleEvent( event );
    if( event.what==evBroadcast && event.message.command==cmNewColorIndex )
        display->setColor( (uchar *)&pal->data[event.message.infoLong] );
    else
    // SET: Added this button to test the colors without pressing OK.
    if (event.what==evCommand && event.message.command==cmTryColors)
      { // That's a new broadcast, I used it for the editor because the
        // colors are cached.
       message(TProgram::deskTop,evBroadcast,cmUpdateColorsChanged,0);
       TProgram::application->Redraw();
      }
}

uint32 TColorDialog::dataSize()
{
    return *pal->data + 1;
}

void TColorDialog::getData( void *rec )
{
    memcpy( rec, pal->data, *pal->data+1 );
}

void TColorDialog::setData( void *rec)
{
    TPalette *p = (TPalette *)rec;

    memcpy( pal->data, p->data, *p->data+1 );
    display->setColor( (uchar *)&pal->data[1] );
    groups->focusItem( 0 );
    if( showMarkers )
        {
        forLabel->hide();
        forSel->hide();
        bakLabel->hide();
        bakSel->hide();
        monoLabel->show();
        monoSel->show();
        }
    groups->select();
}

#if !defined( NO_STREAM )
void TColorDialog::write( opstream& os )
{
    TDialog::write( os );
    os << display << groups << forLabel << forSel
       << bakLabel << bakSel << monoLabel << monoSel;
}

void *TColorDialog::read( ipstream& is )
{
    TDialog::read( is );
    is >> display >> groups >> forLabel >> forSel
       >> bakLabel >> bakSel >> monoLabel >> monoSel;
    pal = 0;
    return this;
}

TStreamable *TColorDialog::build()
{
    return new TColorDialog( streamableInit );
}

TColorDialog::TColorDialog( StreamableInit ) :
    TDialog( streamableInit )
    , TWindowInit( &TColorDialog::initFrame )
{
}

#endif // NO_STREAM

