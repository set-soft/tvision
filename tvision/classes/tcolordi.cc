/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *

Modified by Robert H”hne to be used for RHIDE.
Modified by Salvador E. Tropea: added i18n support, enlarged the dialog,
horizontal scroll bar in items, etc.

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
#define Uses_T1Label
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
    TWindowInit( &TColorDialog::initFrame )
    , TDialog( TRect( 0, 0, 77, 18 ), __("Colors") )
{
    options |= ofCentered;
    pal = aPalette;

    TScrollBar *sb = new TScrollBar( TRect( 31, 3, 32, 14 ) );
    insert( sb );

    groups = new TColorGroupList( TRect( 3, 3, 31, 14 ), sb, aGroups);
    insert( groups );
    insert( new T1Label( 2, 2, __("~G~roup"), groups ) );

    sb = new TScrollBar( TRect( 57, 3, 58, 13 ) );
    insert( sb );
    TScrollBar *sbH = new TScrollBar( TRect( 34, 13, 57, 14 ) );
    sbH->setParams(0,0,40,5,1);
    insert( sbH );

    TView *p = new TColorItemList( TRect( 34, 3, 57, 13 ), sb, aGroups->items, sbH );
    insert( p );
    insert( new T1Label( 33, 2, __("~I~tem"), p ) );

    forSel = new TColorSelector( TRect( 61, 3, 73, 7 ),
                                 TColorSelector::csForeground );
    insert( forSel );
    forLabel = new T1Label( 61, 2, __("~F~oreground"), forSel );
    insert( forLabel );

    int ib = TScreen::getBlinkState() ? 0 : 2;

    bakSel = new TColorSelector( TRect( 61, 9, 73, 11 + ib),
                                 TColorSelector::csBackground );
    insert( bakSel );
    bakLabel = new T1Label( 61, 8, __("~B~ackground"), bakSel );
    insert( bakLabel );

    display = new TColorDisplay( TRect( 60, 12 + ib, 74, 14 + ib),
                                 _("Text ") );
    insert( display );

    monoSel = new TMonoSelector( TRect( 60, 3, 75, 7 ) );
    monoSel->hide();
    insert( monoSel );
    monoLabel = new T1Label( 59, 2, __("Color"), monoSel );
    monoLabel->hide();
    insert( monoLabel );

    if( aGroups != 0 && aGroups->items != 0 && pal)
        display->setColor( (uchar *)&pal->data[ aGroups->items->index ] );

    insert( new TButton( TRect( 31 - 10*ib, 15, 44 - 10*ib, 17 ),
                         __("~T~ry"), cmTryColors, bfNormal ) );
    insert( new TButton( TRect( 46 - 10*ib, 15, 59 - 10*ib, 17 ),
                         __("~O~K"), cmOK, bfDefault ) );
    insert( new TButton( TRect( 61 - 10*ib, 15, 74 - 10*ib, 17 ),
                         __("Cancel"),
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
       TProgram::application->CLY_Redraw();
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
    TWindowInit( &TColorDialog::initFrame )
    , TDialog( streamableInit )
{
}

#endif // NO_STREAM

