/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *

Modified by Robert H”hne to be used for RHIDE.
Modified by Anatoli Soltan to be used under Win32 consoles.

 *
 *
 */
#include <tv/configtv.h>

#ifdef TVOS_Win32

// SET: Moved the standard headers here because according to DJ
// they can inconditionally declare symbols like NULL

#define Uses_TScreen
#define Uses_TEvent
#define Uses_TFrame
#include <tv.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

static ushort Equipment;
static uchar CrtInfo;
static uchar CrtRows;

ushort * TDisplay::equipment = &Equipment;
uchar  * TDisplay::crtInfo = &CrtInfo;
uchar  * TDisplay::crtRows = &CrtRows;
TFont  * TDisplay::font=0;

HANDLE __tvWin32ConInp = INVALID_HANDLE_VALUE;
HANDLE __tvWin32ConOut = INVALID_HANDLE_VALUE;

static int currentCursorX;
static int currentCursorY;
static ushort currentCursorType;

static void Initialize()
{
  //__tvWin32ConInp = CreateFile("CONIN$", GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
  //__tvWin32ConOut = CreateFile("CONOUT$", GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
  __tvWin32ConInp = GetStdHandle(STD_INPUT_HANDLE);
  __tvWin32ConOut = GetStdHandle(STD_OUTPUT_HANDLE);

  CONSOLE_CURSOR_INFO cursorInf;
  GetConsoleCursorInfo(__tvWin32ConOut, &cursorInf);
  currentCursorType = (ushort)(cursorInf.bVisible ? cursorInf.dwSize : 0x2000);
  
  CONSOLE_SCREEN_BUFFER_INFO screenInf;
  GetConsoleScreenBufferInfo(__tvWin32ConOut, &screenInf);
  currentCursorX = screenInf.dwCursorPosition.X;
  currentCursorY = screenInf.dwCursorPosition.Y;
}

void TDisplay::SetPage(uchar /*page*/)
{
}

void TDisplay::SetCursor(int x,int y)
{
  if (x != currentCursorX || y != currentCursorY)
  {
    COORD coord;
    coord.X = (SHORT)x;
    coord.Y = (SHORT)y;
    SetConsoleCursorPosition(__tvWin32ConOut, coord);
    currentCursorX = x;
    currentCursorY = y;
  }
}

void TDisplay::GetCursor(int &x,int &y)
{
  x = currentCursorX;
  y = currentCursorY;
}

ushort TDisplay::getCursorType()
{
  return currentCursorType;
}

void TDisplay::setCursorType( ushort ct )
{
  if (ct != currentCursorType)
  {
    CONSOLE_CURSOR_INFO inf;
    inf.bVisible = !((ct & 0xFF00) == 0x2000);
    inf.dwSize = inf.bVisible ? ct : 1;
    SetConsoleCursorInfo(__tvWin32ConOut, &inf);
    currentCursorType = ct;
  }
}

void TDisplay::clearScreen( uchar screenWidth, uchar screenHeight )
{
  COORD coord = {0, 0};
  DWORD cWritten;

  FillConsoleOutputCharacter(
    __tvWin32ConOut,
    ' ',
    screenWidth * screenHeight,
    coord,
    &cWritten);
  FillConsoleOutputAttribute(
    __tvWin32ConOut,
    FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
    screenWidth * screenHeight,
    coord,
    &cWritten);
}

void TDisplay::videoInt()
{
}

ushort TDisplay::getRows()
{
  CONSOLE_SCREEN_BUFFER_INFO inf;
  GetConsoleScreenBufferInfo(__tvWin32ConOut, &inf);
  return (ushort)inf.dwSize.Y;
}

ushort TDisplay::getCols()
{
  CONSOLE_SCREEN_BUFFER_INFO inf;
  GetConsoleScreenBufferInfo(__tvWin32ConOut, &inf);
  return (ushort)inf.dwSize.X;
}

ushort TDisplay::getCrtMode()
{
  if (__tvWin32ConInp == INVALID_HANDLE_VALUE)
    Initialize();
  return smCO80;
  //$todo: implement something
}

void TDisplay::setCrtMode( ushort /*mode*/ )
{
  //$todo: implement something
}


void TDisplay::setCrtMode( char * /*command*/ )
{
  //$todo: ???
}

void TDisplay::updateIntlChars()
{
 if(GetConsoleOutputCP() != 437 )
   TFrame::frameChars[30] = 'Í';
}

int TDisplay::SelectFont(int height, int noForce, int modeRecalculate, int width)
{
 if (!font)
    font=new TFont();
 return font->SelectFont(height,width,0,noForce,modeRecalculate);
}

void TDisplay::SetFontHandler(TFont *f)
{
 if (font) // The destructor is virtual
    delete font;
 font=f;
}

void TDisplay::RestoreDefaultFont(void)
{
 if (!font)
    font=new TFont();
 font->RestoreDefaultFont();
}

int TDisplay::CheckForWindowSize(void)
{
 //$todo: implement it
 return 0;
}

const int mxTitleSize=256;

/**[txh]********************************************************************

  Description:
  Finds the main window title.

  Return:
  A pointer to a newly allocated string (new[]). Or 0 if fail. by SET.

***************************************************************************/

char *TDisplay::GetWindowTitle(void)
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

int TDisplay::SetWindowTitle(const char *name)
{
 return SetConsoleTitle(name);
}

#endif // TVOS_Win32
// vi: set ts=8 sw=2 et : //
