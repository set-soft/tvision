/**[txh]********************************************************************

  Copyright (c) 2002 by Salvador E. Tropea (SET)
  Based on code contributed by Anatoli Soltan.

  Description:
  Win32 Display routines.
  The original implementation was done by Anatoli, I removed some code, added
some routines and adapted it to the new architecture.

  Notes:
  1) At least in W98SE the cursor can't be hided if we are windowed. I think
that's some bug in Windows. Something interesting: when I resize the window
and it can't go full screen anymore the colors get wrong (as observed in
NT 4.0) looks like Windows uses another drawing technic. In this particular
mode the cursor can be hided properly.
  2) The API reference I have says the cursor size can range from 1 to 100,
but in my system 99 is the maximum.
  
***************************************************************************/
#include <tv/configtv.h>

#define Uses_TScreen
#define Uses_TEvent
#define Uses_TFrame
#include <tv.h>

// I delay the check to generate as much dependencies as possible
#ifdef TVOS_Win32

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <tv/winnt/screen.h>

HANDLE   TDisplayWinNT::hIn =INVALID_HANDLE_VALUE;
HANDLE   TDisplayWinNT::hOut=INVALID_HANDLE_VALUE;
#ifdef USE_NEW_BUFFER
HANDLE   TDisplayWinNT::hStdOut=INVALID_HANDLE_VALUE;
#endif
unsigned TDisplayWinNT::currentCursorX,
         TDisplayWinNT::currentCursorY,
         TDisplayWinNT::curStart,
         TDisplayWinNT::curEnd;

void TDisplayWinNT::SetCursorPos(unsigned x, unsigned y)
{
 if (x!=currentCursorX || y!=currentCursorY)
   {
    SetCursorPosLow(x,y);
    currentCursorX=x;
    currentCursorY=y;
   }
}

// by SET
void TDisplayWinNT::SetCursorPosLow(unsigned x, unsigned y)
{
 COORD coord;
 coord.X=(SHORT)x;
 coord.Y=(SHORT)y;
 SetConsoleCursorPosition(hOut,coord);
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
    SetCursorShapeLow(start,end);
    curStart=start;
    curEnd=end;
   }
}

// by SET
void TDisplayWinNT::SetCursorShapeLow(unsigned start, unsigned end)
{
 CONSOLE_CURSOR_INFO inf;
 inf.bVisible=start>=end ? FALSE : TRUE;
 inf.dwSize=inf.bVisible ? end-start : 1;
 if (inf.dwSize>99) inf.dwSize=99;
 SetConsoleCursorInfo(hOut,&inf);
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

void TDisplayWinNT::Beep()
{
 MessageBeep(0xFFFFFFFF);
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
 beep=Beep;
}

#else

#include <tv/winnt/screen.h>

#endif // TVOS_Win32

