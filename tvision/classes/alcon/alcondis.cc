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


void TDisplayAlcon::Init()
{
	TDisplay::setCursorPos=SetCursorPos;
    TDisplay::getCursorPos=GetCursorPos;
    TDisplay::getCursorShape=GetCursorShape;
    TDisplay::setCursorShape=SetCursorShape;
//    TDisplay::clearScreen=clearScreen;
    TDisplay::getRows=GetRows;
    TDisplay::getCols=GetCols;
//    TDisplay::setCrtMode=SetCrtMode;
//    TDisplay::getCrtMode=GetCrtMode;
//    TDisplay::setCrtModeExt=SetCrtModeExt;
//    TDisplay::setCrtModeRes_p=SetCrtModeRes;
//    TDisplay::getWindowTitle=GetWindowTitle;
//    TDisplay::setWindowTitle=SetWindowTitle;
//    TDisplay::setDisPaletteColors=SetDisPaletteColors;
//    TDisplay::getDisPaletteColors=GetDisPaletteColors;
//    TDisplay::beep=Beep;

}

TDisplayAlcon::~TDisplayAlcon()
{
	// Debug message, to know when the display is destroyed...
	allegro_message("Bye bye TDisplayAlcon!\n");
}

void TDisplayAlcon::GetCursorPos(unsigned &x, unsigned &y)
{
	x = AlCon_WhereX();
	y = AlCon_WhereY();
}

void TDisplayAlcon::SetCursorPos(unsigned  x, unsigned  y)
{
	AlCon_GotoXY(x, y);
}

void TDisplayAlcon::SetCursorShape(unsigned start, unsigned end)
{
	AlCon_SetCursorShape(start, end);
}

void TDisplayAlcon::GetCursorShape(unsigned &start, unsigned &end)
{
	int _start, _end;
	AlCon_GetCursorShape(&_start, &_end);
	start = _start;
	end = _end;
}

ushort TDisplayAlcon::GetRows()
{
	return AlCon_ScreenRows();
}

ushort TDisplayAlcon::GetCols()
{
	return AlCon_ScreenCols();
}
