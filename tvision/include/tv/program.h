/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *

Modified by Robert H”hne to be used for RHIDE.
Modified by Salvador E. Tropea (setScreenMode and other things)

 *
 *
 */

#if defined( Uses_TProgram ) && !defined( __TProgram )
#define __TProgram

#define cpColor \
    "\x71\x70\x78\x74\x20\x28\x24\x17\x1F\x1A\x31\x31\x1E\x71\x00" \
    "\x37\x3F\x3A\x13\x13\x3E\x21\x00\x70\x7F\x7A\x13\x13\x70\x7F\x00" \
    "\x70\x7F\x7A\x13\x13\x70\x70\x7F\x7E\x20\x2B\x2F\x78\x2E\x70\x30" \
    "\x3F\x3E\x1F\x2F\x1A\x20\x72\x31\x31\x30\x2F\x3E\x31\x13\x38\x00"

#define cpBlackWhite \
    "\x70\x70\x78\x7F\x07\x07\x0F\x07\x0F\x07\x70\x70\x07\x70\x00" \
    "\x07\x0F\x07\x70\x70\x07\x70\x00\x70\x7F\x7F\x70\x07\x70\x07\x00" \
    "\x70\x7F\x7F\x70\x07\x70\x70\x7F\x7F\x07\x0F\x0F\x78\x0F\x78\x07" \
    "\x0F\x0F\x0F\x70\x0F\x07\x70\x70\x70\x07\x70\x0F\x07\x07\x07\x00"

#define cpMonochrome \
    "\x70\x07\x07\x0F\x70\x70\x70\x07\x0F\x07\x70\x70\x07\x70\x00" \
    "\x07\x0F\x07\x70\x70\x07\x70\x00\x70\x70\x70\x07\x07\x70\x07\x00" \
    "\x70\x70\x70\x07\x07\x70\x70\x70\x0F\x07\x07\x0F\x70\x0F\x70\x07" \
    "\x0F\x0F\x07\x70\x07\x07\x70\x07\x07\x07\x70\x0F\x07\x07\x07\x00"


class  TStatusLine;
class  TMenuBar;
class  TDeskTop;
struct TEvent;
class  TView;
class  TScreen;
class  TVMainConfigFile;

class CLY_EXPORT TProgInit
{
public:
    TProgInit( TStatusLine *(*cStatusLine)( TRect ),
               TMenuBar *(*cMenuBar)( TRect ),
               TDeskTop *(*cDeskTop )( TRect )
             );
    virtual ~TProgInit();
    static TVMainConfigFile *config;

protected:
    TStatusLine *(*createStatusLine)( TRect );
    TMenuBar *(*createMenuBar)( TRect );
    TDeskTop *(*createDeskTop)( TRect );
    static TScreen *tsc;
};

/* ---------------------------------------------------------------------- */
/*      class TProgram                                                    */
/*                                                                        */
/*      Palette layout                                                    */
/*          1 = TBackground                                               */
/*       2- 7 = TMenuView and TStatusLine                                 */
/*       8-15 = TWindow(Blue)                                             */
/*      16-23 = TWindow(Cyan)                                             */
/*      24-31 = TWindow(Gray)                                             */
/*      32-63 = TDialog                                                   */
/* ---------------------------------------------------------------------- */

const int

//  TApplication palette entries

    apColor      = 0,
    apBlackWhite = 1,
    apMonochrome = 2;


class CLY_EXPORT TProgram : public TGroup, public virtual TProgInit
{

public:

    TProgram();
    virtual ~TProgram();

    virtual void getEvent(TEvent& event);
    virtual TPalette& getPalette() const;
    virtual void handleEvent(TEvent& event);
    virtual void idle();
    virtual void initScreen();
    virtual void outOfMemory();
    virtual void putEvent( TEvent& event );
    virtual void run();
    void setScreenMode( ushort mode, char *comm=0 );
    void setScreenMode( unsigned w, unsigned h, int fW=-1, int fH=-1 );
    TView *validView( TView *p );
    virtual void shutDown();

    virtual void suspend() {}
    virtual void resume() {}

    // This is a very nasty function only needed if you call
    // TScreen::resume bypassing TProgram::resume. In this case
    // the buffers could get unsychronized. This is done by
    // RHIDE.
    void syncScreenBuffer();
    //   { buffer = TScreen::screenBuffer; }

    static TStatusLine *initStatusLine( TRect );
    static TMenuBar *initMenuBar( TRect );
    static TDeskTop *initDeskTop( TRect );

    static TProgram * application;
    static TStatusLine * statusLine;
    static TMenuBar * menuBar;
    static TDeskTop * deskTop;
    static int appPalette;

    // Added by SET
    static clock_t lastIdleClock; // That's the value of clock in the moment
                                  // when idle is called. Used to accumulate
                                  // in the inIdleTime var
    static clock_t inIdleTime;  // That keeps track of the number of clocks
                                // elapsed since we entered in idle
    static Boolean inIdle;      // Indicates that we are in idle, a mouse or
                                // keyboard event resets it
    // I think that sometimes we can have a long elapsed time without events
    // so I provide a way to reset it
    static void resetIdleTime() { inIdle=False; };
    // SET: By default that's 0 and the idle member releases the CPU to the
    // OS. If for some reason you really need to avoid it and want to use
    // 100% of the CPU just assign 1 to this variable.
    static char doNotReleaseCPU;
    // SET: Disable Alt+number window selection. Useful if you have to do it
    // configurable like in SETEdit.
    static char doNotHandleAltNumber;

protected:

    static TEvent pending;

private:

//    static const char * exitText;

};

#endif

