#include <tv/configtv.h>

#define Uses_TScreen
#define Uses_TEvent
#define Uses_TGKey
#include <tv.h>

#include <tv/alcon/screen.h>
#include <tv/alcon/mouse.h>
#include <tv/alcon/key.h>
#define Uses_AlCon_conio
#include <tv/alcon/alcon.h>

TScreen *TV_AlconDriverCheck()
{
    TScreenAlcon *drv = new TScreenAlcon();
    if (!TScreen::initialized) {
        delete drv;
        return 0;
    }
    return drv;
}

TScreenAlcon::TScreenAlcon()
{
    // Create an 80x25 screen => 640x400.
    AlCon_Init(80,25);
    
    // Set background to black and foreground to light gray.
    textattr(LIGHTGRAY | (BLACK<<4));
    
    // Jump to top left (1, 1).
    gotoxy(1, 1);
}


TScreenAlcon::~TScreenAlcon()
{
    // Deinitialize all.
    AlCon_Exit();
}
