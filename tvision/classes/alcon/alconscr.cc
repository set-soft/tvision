// -*- mode:C++; tab-width: 4 -*-
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
    // Create screen.
    AlCon_Init(80, 25);
    
    // Set background to black and foreground to light gray.
    textattr(LIGHTGRAY | (BLACK<<4));
    
    // Jump to top left (1, 1).
    gotoxy(1, 1);

	cputs("Urgh!");

    TDisplayAlcon::Init();

    TScreen::clearScreen=clearScreen;
    
    TScreen::getCharacter=getCharacter;
    TScreen::getCharacters=getCharacters;
    TScreen::setCharacter=setCharacter;
    TScreen::setCharacters=setCharacters;
    TScreen::getFontGeometry=GetFontGeometry;
//    TScreen::setFont_p=SetFont;
//    TScreen::restoreFonts=RestoreFonts;
	initialized=1;
}


TScreenAlcon::~TScreenAlcon()
{
    // Deinitialize all.
    AlCon_Exit();
}


void TScreenAlcon::clearScreen()
{
	TDisplay::clearScreen(AlCon_ScreenCols(), AlCon_ScreenCols());
}


ushort TScreenAlcon::getCharacter(unsigned offset)
{
    ushort buffer;
	AlCon_GetScrChars(offset, &buffer, 1);
    return buffer;
}


void TScreenAlcon::getCharacters(unsigned offset, ushort *buffer, unsigned count)
{
	AlCon_GetScrChars(offset, buffer, count);
}


void TScreenAlcon::setCharacter(unsigned offset, uint32 value)
{
	AlCon_PutChar(offset, value);
}


void TScreenAlcon::setCharacters(unsigned offset, ushort *values, unsigned count)
{
	AlCon_PutBuf(offset, values, count);
}


int TScreenAlcon::GetFontGeometry(unsigned &w, unsigned &h)
{
	w = 8;
    h = 16;
	return 1;
}


