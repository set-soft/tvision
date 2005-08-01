/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *

Modified by Robert H”hne to be used for RHIDE.
Modified by Salvador E. Tropea (release CPU and other stuff)
Modified by Salvador E. Tropea to compile for 64 bits architectures.
Modified by Salvador E. Tropea to disable Alt+N stuff.

 *
 *
 */

#define Uses_TKeys
#define Uses_TProgram
#define Uses_TEvent
#define Uses_TScreen
#define Uses_TStatusLine
#define Uses_TMenu
#define Uses_TMenuItem
#define Uses_TGroup
#define Uses_TDeskTop
#define Uses_TEventQueue
#define Uses_TMenuBar
#define Uses_TStatusDef
#define Uses_TStatusItem
#define Uses_TPalette
#define Uses_TGKey
#define Uses_TVOSClipboard
#include <tv.h>

#include <compatlayer.h>

// Public variables

TStatusLine * TProgram::statusLine = 0;
TMenuBar * TProgram::menuBar = 0;
TDeskTop * TProgram::deskTop = 0;
TProgram * TProgram::application = 0;
int TProgram::appPalette = apColor;
TEvent TProgram::pending;
clock_t TProgram::lastIdleClock = 0;
clock_t TProgram::inIdleTime = 0;
Boolean TProgram::inIdle = False;
char    TProgram::doNotReleaseCPU = 0;
char    TProgram::doNotHandleAltNumber = 0;

extern TPoint shadowSize;

TProgram::TProgram() :
    TProgInit( &TProgram::initStatusLine,
                  &TProgram::initMenuBar,
                  &TProgram::initDeskTop
                ),
    TGroup( TRect( 0,0,TScreen::screenWidth,TScreen::screenHeight ) )
{
    application = this;
    initScreen();
    state = sfVisible | sfSelected | sfFocused | sfModal | sfExposed;
    options = 0;
    syncScreenBuffer();

    if( TScreen::noUserScreen() )
        disableCommand( cmCallShell );

    if( createStatusLine != 0 &&
        (statusLine = createStatusLine( getExtent() )) != 0
      )
        insert(statusLine);

    if( createMenuBar != 0 &&
        (menuBar = createMenuBar( getExtent() )) != 0
      )
        insert(menuBar);

    if( createDeskTop != 0 &&
        (deskTop = createDeskTop( getExtent() )) != 0
      )
        insert(deskTop);

}

TProgram::~TProgram()
{
    application = 0;
    if (TVOSClipboard::destroy)
       TVOSClipboard::destroy();
}

void TProgram::shutDown()
{
    statusLine = 0;
    menuBar = 0;
    deskTop = 0;
    TGroup::shutDown();
}

inline Boolean hasMouse( TView *p, void *s )
{
    return Boolean( (p->state & sfVisible) != 0 &&
                     p->mouseInView( ((TEvent *)s)->mouse.where ));
}

#ifndef TVOS_UNIX
inline
clock_t Clock(void)
{
 return clock();
}
#else
/*
  In Linux clock returns time consumed by our program, so if ourprogram
sleeps it doesn't count so the screen saver and clock aren'tupdated.
  Instead we must use gettimeofday.
*/

#include <sys/time.h>

clock_t Clock(void)
{
 static int firstCall=1;
 static struct timeval ref;
 struct timeval cur;
 clock_t ret;

 // If that's the first time take the value as reference
 if (firstCall)
   {
    gettimeofday(&ref,0);
    firstCall=0;
   }
 // Current time
 gettimeofday(&cur,0);
 // Substract the reference
 cur.tv_sec-=ref.tv_sec;
 if (cur.tv_usec<ref.tv_usec)
   {
    cur.tv_sec--;
    cur.tv_usec=ref.tv_usec-cur.tv_usec;
   }
 else
    cur.tv_usec-=ref.tv_usec;

 // Convert to 1/100 s
 ret=clock_t(cur.tv_usec/10000+cur.tv_sec*100);

 return ret;
}
#endif

void TProgram::getEvent(TEvent& event)
{
    if( pending.what != evNothing )
        {
        event = pending;
        pending.what = evNothing;
        inIdle=False;
        }
    else
        {
        event.getMouseEvent();
        if( event.what == evNothing )
            {
            event.getKeyEvent();
            if( event.what == evNothing )
                {
                if( inIdle )
                    {
                    clock_t t=Clock();
                    inIdleTime+=t-lastIdleClock;
                    lastIdleClock=t;
                    }
                else
                    {
                    inIdleTime=0;
                    lastIdleClock=Clock();
                    inIdle=True;
                    }
                if (TScreen::checkForWindowSize())
                  {
                   setScreenMode(0xFFFF);
                   CLY_Redraw();
                  }
                idle();
                }
            else
                inIdle=False;
            }
        else
            inIdle=False;
        }

    if( statusLine != 0 )
        {
        if( (event.what & evKeyDown) != 0 ||
            ( (event.what & evMouseDown) != 0 &&
              firstThat( hasMouse, &event ) == statusLine
            )
          )
            statusLine->handleEvent( event );
        }
}

TPalette& TProgram::getPalette() const
{
    static TPalette color ( cpColor, sizeof( cpColor )-1 );
    static TPalette blackwhite( cpBlackWhite, sizeof( cpBlackWhite )-1 );
    static TPalette monochrome( cpMonochrome, sizeof( cpMonochrome )-1 );
    static TPalette *palettes[] =
        {
        &color,
        &blackwhite,
        &monochrome
        };
    return *(palettes[appPalette]);
}

void TProgram::handleEvent( TEvent& event )
{
    if( !doNotHandleAltNumber && event.what == evKeyDown )
        {
        char c = TGKey::GetAltChar( event.keyDown.keyCode, event.keyDown.charScan.charCode );
        if( c >= '1' && c <= '9' )
            {
               if (current->valid(cmReleasedFocus))
               {
                   if( message( deskTop,
                            evBroadcast,
                            cmSelectWindowNum,
                            (void *)(uipointer)(c - '0')
                           ) != 0 )
                   clearEvent( event );
               }
            }
        }

    TGroup::handleEvent( event );
    if( event.what == evCommand && event.message.command == cmQuit )
        {
        endModal( cmQuit );
        clearEvent( event );
        }
}

void TProgram::idle()
{
    if( statusLine != 0 )
        statusLine->update();

    if( commandSetChanged == True )
        {
        message( this, evBroadcast, cmCommandSetChanged, 0 );
        commandSetChanged = False;
        }
    // SET: Release the CPU unless the user doesn't want it.
    if( !doNotReleaseCPU )
        {
         CLY_ReleaseCPU(); // defined in ticks.cc
        }
}

TDeskTop *TProgram::initDeskTop( TRect r )
{
    if (menuBar)
      r.a.y += menuBar->size.y;
    else
      r.a.y++;
    if (statusLine)
      r.b.y -= statusLine->size.y;
    else
      r.b.y--;
    return new TDeskTop( r );
}

TMenuBar *TProgram::initMenuBar( TRect r )
{
    r.b.y = r.a.y + 1;
    return new TMenuBar( r, (TMenu *)0 );
}

void TProgram::initScreen()
{
    if( !TDisplay::dual_display && (TScreen::screenMode & 0x00FF) != TDisplay::smMono )
        {
        if( (TScreen::screenMode & TDisplay::smFont8x8) != 0 )
            shadowSize.x = 1;
        else
            shadowSize.x = 2;
        shadowSize.y = 1;
        showMarkers = False;
        if( (TScreen::screenMode & 0x00FF) == TDisplay::smBW80 )
            appPalette = apBlackWhite;
        else
            appPalette = apColor;
        }
    else
        {

        shadowSize.x = 0;
        shadowSize.y = 0;
        showMarkers = True;
        appPalette = apMonochrome;
        }
}

TStatusLine *TProgram::initStatusLine( TRect r )
{
    r.a.y = r.b.y - 1;
    return new TStatusLine( r,
        *new TStatusDef( 0, 0xFFFF ) +
            *new TStatusItem( __("~Alt-X~ Exit"), kbAltX, cmQuit ) +
            *new TStatusItem( 0, kbF10, cmMenu ) +
            *new TStatusItem( 0, kbAltF3, cmClose ) +
            *new TStatusItem( 0, kbF5, cmZoom ) +
            *new TStatusItem( 0, kbCtrlF5, cmResize )
            );
}

void TProgram::outOfMemory()
{
}

void TProgram::putEvent( TEvent & event )
{
    pending = event;
}

void TProgram::run()
{
    execute();
}

void TProgram::setScreenMode( ushort mode, char *command )
{
    TRect  r;

    TMouse::hide();
    if (!TDisplay::dual_display)
       {
       if (mode==0xFFFF && command)
          TScreen::setVideoModeExt( command );
       else
          TScreen::setVideoMode( mode );
       }
    initScreen();
    syncScreenBuffer();
    r = TRect( 0, 0, TScreen::screenWidth, TScreen::screenHeight );
    changeBounds( r );
    setState(sfExposed, False);
    redraw();
    setState(sfExposed, True);
    TMouse::show();
}

/**[txh]********************************************************************

  Description:
  Changes the screen mode to the closest resolution available. The @var{fW}
and @var{fH} is just a hint about the preferred font size and isn't
mandatory. If you really want to get a specific font size your application
should provide the font using a call back.
  
***************************************************************************/

// SET
void TProgram::setScreenMode( unsigned w, unsigned h, int fW, int fH )
{
    TRect  r;

    TMouse::hide();
    if (TScreen::setVideoModeRes( w, h, fW, fH ))
      {
       initScreen();
       syncScreenBuffer();
       r = TRect( 0, 0, TScreen::screenWidth, TScreen::screenHeight );
       changeBounds( r );
       setState(sfExposed, False);
       redraw();
       setState(sfExposed, True);
      }
    TMouse::show();
}

TView* TProgram::validView(TView* p)
{
    if( p == 0 )
        return 0;
    if( lowMemory() )
        {
        CLY_destroy( p );
        outOfMemory();
        return 0;
        }
    if( !p->valid( cmValid ) )
        {
        CLY_destroy( p );
        return 0;
        }
    return p;
}

void TProgram::syncScreenBuffer()
{
 buffer = TScreen::screenBuffer;
}

