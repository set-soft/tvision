/**[txh]********************************************************************

  Copyright (c) 2002 by Salvador E. Tropea (SET)
  Based on code contributed by Anatoli Soltan.

  Description:
  Win32 Display routines.
  The original implementation was done by Anatoli, I removed some code, added
some routines and adapted it to the new architecture.
  
***************************************************************************/
#include <tv/configtv.h>

#ifdef TVOS_Win32

#define Uses_TScreen
#define Uses_TEvent
#define Uses_TFrame
#include <tv.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <tv/winnt/screen.h>

HANDLE   TDisplayWinNT::hIn =INVALID_HANDLE_VALUE;
HANDLE   TDisplayWinNT::hOut=INVALID_HANDLE_VALUE;
unsigned TDisplayWinNT::currentCursorX,
         TDisplayWinNT::currentCursorY,
         TDisplayWinNT::curStart,
         TDisplayWinNT::curEnd;

void TDisplayWinNT::SetCursorPos(unsigned x, unsigned y)
{
 if (x!=currentCursorX || y!=currentCursorY)
   {
    COORD coord;
    coord.X=(SHORT)x;
    coord.Y=(SHORT)y;
    SetConsoleCursorPosition(hOut,coord);
    currentCursorX=x;
    currentCursorY=y;
   }
}

void TDisplayWinNT::GetCursorPos(unsigned &x, unsigned &y)
{
 x=currentCursorX;
 y=currentCursorY;
}

// by SET
void TDisplayWinNT::GetCursorPosLow(unsigned &x, unsigned &y)
{
 CONSOLE_SCREEN_BUFFER_INFO screenInf;
 GetConsoleScreenBufferInfo(hOut,&screenInf);
 x=screenInf.dwCursorPosition.X;
 y=screenInf.dwCursorPosition.Y;
}

// by SET
void TDisplayWinNT::GetCursorShapeLow(unsigned &start, unsigned &end)
{
 CONSOLE_CURSOR_INFO cursorInf;
 GetConsoleCursorInfo(hOut,&cursorInf);
 if (cursorInf.bVisible)
   {// Visible
    // Win32API returns a "percent filled" value.
    start=100-cursorInf.dwSize;
    // Ever upto the end
    end  =100;
   }
 else
    // Invisible cursor
    start=end=0;
}

void TDisplayWinNT::GetCursorShape(unsigned &start, unsigned &end)
{
 start=curStart;
 end=curEnd;
}

// by SET
void TDisplayWinNT::SetCursorShape(unsigned start, unsigned end)
{
 if (start>=end && getShowCursorEver())
    return;

 if (start!=curStart || end!=curEnd)
   {
    CONSOLE_CURSOR_INFO inf;
    inf.bVisible=start>=end ? False : True;
    inf.dwSize=inf.bVisible ? end-start : 1;
    SetConsoleCursorInfo(hOut,&inf);
    curStart=start;
    curEnd=end;
   }
}

void TDisplayWinNT::ClearScreen(uchar screenWidth, uchar screenHeight)
{
 COORD coord={0,0};
 DWORD cWritten;

 FillConsoleOutputCharacter(hOut,' ',screenWidth*screenHeight,coord,&cWritten);
 FillConsoleOutputAttribute(hOut,FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
                            screenWidth*screenHeight,coord,&cWritten);
}

ushort TDisplayWinNT::GetRows()
{
 CONSOLE_SCREEN_BUFFER_INFO inf;
 GetConsoleScreenBufferInfo(hOut,&inf);
 return (ushort)inf.dwSize.Y;
}

ushort TDisplayWinNT::GetCols()
{
 CONSOLE_SCREEN_BUFFER_INFO inf;
 GetConsoleScreenBufferInfo(hOut,&inf);
 return (ushort)inf.dwSize.X;
}

const int mxTitleSize=256;

/**[txh]********************************************************************

  Description:
  Finds the main window title.

  Return:
  A pointer to a newly allocated string (new[]). Or 0 if fail. by SET.

***************************************************************************/

const char *TDisplayWinNT::GetWindowTitle(void)
{
 char buf[mxTitleSize];
 DWORD ret=GetConsoleTitle(buf,mxTitleSize);
 if (ret)
   {
    char *s=new char[ret+1];
    memcpy(s,buf,ret);
    s[ret]=0;
    return s;
   }
 return 0;
}

/**[txh]********************************************************************

  Description:
  Sets the main window title.

  Return:
  non-zero successful. by SET.

***************************************************************************/

int TDisplayWinNT::SetWindowTitle(const char *name)
{
 return SetConsoleTitle(name);
}

TDisplayWinNT::~TDisplayWinNT() {}

void TDisplayWinNT::Init()
{
 setCursorPos=SetCursorPos;
 getCursorPos=GetCursorPos;
 getCursorShape=GetCursorShape;
 setCursorShape=SetCursorShape;
 getRows=GetRows;
 getCols=GetCols;
 setWindowTitle=SetWindowTitle;
 getWindowTitle=GetWindowTitle;
 clearScreen=ClearScreen;
}

#endif // TVOS_Win32

