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
    printf("TV_AlconDriverCheck\n");
    TScreenAlcon *drv = new TScreenAlcon();
    if (!TScreen::initialized) {
        delete drv;
        return 0;
    }
    return drv;
}


TScreenAlcon::TScreenAlcon()
{
    printf("TScreenAlcon(): start\n");
    // Create screen.
    AlCon_Init(80, 25);
    
    // Set background to black and foreground to light gray.
    textattr(LIGHTGRAY | (BLACK<<4));
    
    // Jump to top left (1, 1).
    gotoxy(1, 1);

	cputs("TScreenAlcon was just initialised\n");

    TDisplayAlcon::Init();

    TScreen::clearScreen=clearScreen;
    
    TScreen::getCharacters=getCharacters;
    TScreen::getCharacter=getCharacter;
    TScreen::setCharacter=setCharacter;
    TScreen::setCharacters=setCharacters;
    //static int    SetDisPaletteColors(int from, int number, TScreenColor *colors);
    TScreen::getFontGeometry=GetFontGeometry;
//    TScreen::setFont_p=SetFont;
//    TScreen::restoreFonts=RestoreFonts;
	initialized=1;
    printf("TScreenAlcon(): end\n");
}


TScreenAlcon::~TScreenAlcon()
{
    printf("~TScreenAlcon\n");
    // Deinitialize all.
    AlCon_Exit();
}


void TScreenAlcon::clearScreen()
{
    printf("TScreenAlcon::clearScreen()\n");
	TDisplay::clearScreen(AlCon_ScreenCols(), AlCon_ScreenCols());
}


ushort TScreenAlcon::getCharacter(unsigned offset)
{
    printf("TScreenAlcon::getCharacter %d\n", offset);
    ushort buffer;
	AlCon_GetScrChars(offset, &buffer, 1);
    return buffer;
}


void TScreenAlcon::getCharacters(unsigned offset, ushort *buffer, unsigned count)
{
    printf("TScreenAlcon::getCharacters offset %d, count %d\n", offset, count);
	AlCon_GetScrChars(offset, buffer, count);
}


void TScreenAlcon::setCharacter(unsigned offset, uint32 value)
{
    printf("TScreenAlcon::setCharacter offset %d value %d\n", offset, value);
	AlCon_PutChar(offset, value);
}


void TScreenAlcon::setCharacters(unsigned offset, ushort *values, unsigned count)
{
    printf("TScreenAlcon::setCharacters offset %d, count %d\n", offset, count);
	AlCon_PutBuf(offset, values, count);
}


int TScreenAlcon::GetFontGeometry(unsigned &w, unsigned &h)
{
    printf("TScreenAlcon::GetFontGeometry\n");
	w = 8;
    h = 16;
	return 1;
}


