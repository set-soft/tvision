#define DEBUGMODE
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

#define PRINTF(FORMAT, ...)  printf("%s " FORMAT "\n", __PRETTY_FUNCTION__, ## __VA_ARGS__)


void TDisplayAlcon::Init()
{
    setCursorPos=SetCursorPos;
    getCursorPos=GetCursorPos;
    getCursorShape=GetCursorShape;
    setCursorShape=SetCursorShape;
    // DOS version has this, where? TDisplay::clearScreen=ClearScreen;
    getRows=GetRows;
    getCols=GetCols;
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
}

void TDisplayAlcon::GetCursorPos(unsigned &x, unsigned &y)
{
    // AlCon returns vars in the range of 1 <= var <= inf.
    x = AlCon_WhereX() - 1;
    y = AlCon_WhereY() - 1;
}

void TDisplayAlcon::SetCursorPos(unsigned  x, unsigned  y)
{
    // AlCon expects a range of 1 <= var <= inf.
    AlCon_GotoXY(x + 1, y + 1);
}

/**[txh]********************************************************************

  Description: Called with percentage values from 0 to 100 inclusive.
  If start is bigger or equal end (usually both 0), this means hide the
  cursor.

***************************************************************************/

void TDisplayAlcon::SetCursorShape(unsigned start, unsigned end)
{
    if (start >= end && getShowCursorEver())
        return;
        
    if (start >= end) {
        AlCon_HideCursor();
    } else {
        AlCon_ShowCursor();
        unsigned int h;
        AlCon_GetFontGeometry(0, &h);
        AlCon_SetCursorShape(int(start * (h - 0.00001f) / 100),
            int(end * (h - 0.00001f) / 100));
    }
}

/**[txh]********************************************************************

  Description: Fills into start and end the beginning and end of the cursor
  shape as percentage values from 0 to 1.
  
***************************************************************************/

void TDisplayAlcon::GetCursorShape(unsigned &start, unsigned &end)
{
    int _start, _end;
    unsigned int h;
    AlCon_GetCursorShape(&_start, &_end);
    AlCon_GetFontGeometry(0, &h);
    start = 100 * _start / h;
    end = 100 * _end / h;
}

ushort TDisplayAlcon::GetRows()
{
    return AlCon_ScreenRows();
}

ushort TDisplayAlcon::GetCols()
{
    return AlCon_ScreenCols();
}
