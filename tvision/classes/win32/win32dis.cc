/**[txh]********************************************************************

  Copyright (c) 2002 by Salvador E. Tropea (SET)
  Based on code contributed by Vadim Beloborodov.

  Description:
  Win32 Display routines.
  The original implementation was done by Vadim, I removed some code, added
some routines and adapted it to the new architecture.
  One important detail is that Vadim implemented it with threads. The
thread that monitors the window size is different, that's why a lock is
needed.
  
***************************************************************************/

#include <tv/configtv.h>

#define Uses_stdio
#define Uses_TDisplay
#define Uses_TScreen
#include <tv.h>

// I delay the check to generate as much dependencies as possible
#ifdef TVOS_Win32

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <tv/win32/screen.h>

const int mxTitleSize=256;

HANDLE                     TDisplayWin32::hOut=INVALID_HANDLE_VALUE;
HANDLE                     TDisplayWin32::hIn =INVALID_HANDLE_VALUE;
HANDLE                     TDisplayWin32::hStdOut=INVALID_HANDLE_VALUE;
CONSOLE_SCREEN_BUFFER_INFO TDisplayWin32::ConsoleInfo;
CONSOLE_CURSOR_INFO        TDisplayWin32::ConsoleCursorInfo;
CRITICAL_SECTION           TDisplayWin32::lockWindowSizeChanged;
int                        TDisplayWin32::WindowSizeChanged=0;

void TDisplayWin32::SetCursorPos(unsigned x, unsigned y)
{
 COORD pos;
 pos.X=x;
 pos.Y=y;
 SetConsoleCursorPosition(hOut,pos);
}

void TDisplayWin32::GetCursorPos(unsigned &x, unsigned &y)
{
 GetConsoleScreenBufferInfo(hOut,&ConsoleInfo);
 x=ConsoleInfo.dwCursorPosition.X;
 y=ConsoleInfo.dwCursorPosition.Y;
}

// by SET
void TDisplayWin32::GetCursorShape(unsigned &start, unsigned &end)
{
 GetConsoleCursorInfo(hOut,&ConsoleCursorInfo);
 if (ConsoleCursorInfo.bVisible)
   {// Visible
    // Win32API returns a "percent filled" value.
    start=100-ConsoleCursorInfo.dwSize;
    // Ever upto the end
    end  =100;
    return;
   }
 // Invisible cursor
 start=end=0;
}

// by SET
void TDisplayWin32::SetCursorShape(unsigned start, unsigned end)
{
 if (start>=end && getShowCursorEver())
    return;

 GetConsoleCursorInfo(hOut,&ConsoleCursorInfo);
 if (start>=end)
    ConsoleCursorInfo.bVisible=FALSE;
 else
   {
    ConsoleCursorInfo.bVisible=TRUE;
    ConsoleCursorInfo.dwSize=end-start;
    if (ConsoleCursorInfo.dwSize>=100)
       ConsoleCursorInfo.dwSize=99;
   }
 SetConsoleCursorInfo(hOut,&ConsoleCursorInfo);
}

ushort TDisplayWin32::GetRows()
{
#if 0
 ushort Rows=ConsoleInfo.srWindow.Bottom-ConsoleInfo.srWindow.Top+1;
 if (Rows>ConsoleInfo.dwMaximumWindowSize.Y)
    Rows=ConsoleInfo.dwMaximumWindowSize.Y;
 return Rows;
#else
 return ConsoleInfo.dwSize.Y;
#endif
}

ushort TDisplayWin32::GetCols()
{
#if 0
 ushort Cols=ConsoleInfo.srWindow.Right-ConsoleInfo.srWindow.Left+1;
 if (Cols>ConsoleInfo.dwMaximumWindowSize.X)
    Cols=ConsoleInfo.dwMaximumWindowSize.X;
 return Cols;
#else
 return ConsoleInfo.dwSize.X;
#endif
}

int TDisplayWin32::CheckForWindowSize(void)
{
 int SizeChanged=WindowSizeChanged;
 EnterCriticalSection(&lockWindowSizeChanged);
 WindowSizeChanged=0;
 LeaveCriticalSection(&lockWindowSizeChanged);
 if (SizeChanged)
    GetConsoleScreenBufferInfo(hOut,&ConsoleInfo);

 return SizeChanged;
}

void TDisplayWin32::SetCrtMode(ushort)
{
 SetCursorShape(0x58,0x64);
}

void TDisplayWin32::SetCrtModeExt(char *)
{
 SetCursorShape(0x58,0x64);
}

/**[txh]********************************************************************

  Description:
  Finds the main window title.

  Return:
  A pointer to a newly allocated string (new[]). Or 0 if fail. by SET.

***************************************************************************/

const char *TDisplayWin32::GetWindowTitle(void)
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

int TDisplayWin32::SetWindowTitle(const char *name)
{
 return SetConsoleTitle(name);
}

void TDisplayWin32::Beep()
{
 MessageBeep(0xFFFFFFFF);
}

TDisplayWin32::~TDisplayWin32() {}

void TDisplayWin32::Init()
{
 setCursorPos=SetCursorPos;
 getCursorPos=GetCursorPos;
 getCursorShape=GetCursorShape;
 setCursorShape=SetCursorShape;
 getRows=GetRows;
 getCols=GetCols;
 checkForWindowSize=CheckForWindowSize;
 setWindowTitle=SetWindowTitle;
 getWindowTitle=GetWindowTitle;
 setCrtMode=SetCrtMode;
 setCrtModeExt=SetCrtModeExt;
 beep=Beep;
}

/* Not implemented
void TDisplay::setCrtMode(ushort )
{
	GetConsoleScreenBufferInfo(hOut, &ConsoleInfo);
	EnterCriticalSection( &lockWindowSizeChanged );
	LeaveCriticalSection( &lockWindowSizeChanged );
}

void TDisplay::setCrtMode(char * )
{
	GetConsoleScreenBufferInfo(hOut, &ConsoleInfo);
	EnterCriticalSection( &lockWindowSizeChanged );
	LeaveCriticalSection( &lockWindowSizeChanged );
}

ushort TDisplay::getCrtMode()
{
	GetConsoleScreenBufferInfo(hOut, &ConsoleInfo);
	return smCO80;
}
*/

#else // TVOS_Win32

#include <tv/win32/screen.h>

#endif // else TVOS_Win32

