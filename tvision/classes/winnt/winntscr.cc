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

#ifdef _WIN32

#define Uses_TScreen
#define Uses_TEvent
#include <tv.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define TV_CONSOLE_MODE (ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT)

int dual_display = 0;
static ushort user_mode = 0;

extern HANDLE __tvWin32ConOut;
extern HANDLE __tvWin32ConInp;

ushort TScreen::startupMode = 0xFFFF;
ushort TScreen::startupCursor = 0;
ushort TScreen::screenMode = 0xFFFF;
uchar TScreen::screenWidth = 0;
uchar TScreen::screenHeight = 0;
uchar TDisplay::Page = 0;
Boolean TScreen::hiResScreen = False;
Boolean TScreen::checkSnow = True;
// SET: On Win32 this value is symbolic, just a number that can't be a
// malloced pointer, the screenBuffer isn't used to access the screen.
ushort *TScreen::screenBuffer = (ushort *)-1;
ushort TScreen::cursorLines = 0;
// SET: Indicates if the screen was initialized by the TScreen constructor
char TScreen::initialized = 0;
// SET: Starts as suspended to avoid TScreen::suspend() calls and other
// similar stuff before initializing
char TScreen::suspended = 1;

static ushort*  saveScreenBuf;
static unsigned saveScreenSize;
static ushort   saveScreenCursorType;
static ushort   saveScreenWidth, saveScreenHeight;
static int      saveScreenCursorX, saveScreenCursorY;
static DWORD    saveScreenConsoleMode;
static void SaveScreen();
static void SaveScreenReleaseMemory();
static void RestoreScreen();
static void ScreenUpdate();
static BOOL WINAPI ConsoleEventHandler(DWORD dwCtrlType);

static int was_blink = 0;
void setBlinkState();
void setIntenseState();
int getBlinkState();

static CHAR* outBuf;
static WORD* outBufAttr;
static CHAR_INFO* outBufCI;
static unsigned outBufCapacity;

static void ensureOutBufCapacity(unsigned count)
{
  count = (count + 1) & 0xFFFFFFFE;
  if (outBufCapacity < count)
    {
      free(outBufCI);
      outBufCI = (CHAR_INFO*)malloc(count * sizeof(CHAR_INFO));
      outBuf = (CHAR*)outBufCI;
      outBufAttr = (WORD*)(outBuf + count);
      outBufCapacity = count;
    }
}

TScreen::TScreen()
{
  user_mode = screenMode = startupMode = getCrtMode();
  suspended = 1;
  initialized = 1;
  resume();
}

TScreen::~TScreen()
{
  suspend();
  SaveScreenReleaseMemory();
//  setCrtMode(startupMode);
//  textmode(startupMode);
//  if (startupMode == user_mode)
//    ScreenUpdate();
  free(outBufCI);
  outBufCI = 0;
  outBuf = NULL;
  outBufAttr = 0;
  outBufCapacity = 0;
}

void TScreen::resume()
{
  if (!initialized || !suspended)
     return;
  SaveScreen();
  if (screenMode == 0xffff)
    screenMode = getCrtMode();
  if (screenMode != user_mode)
    setCrtMode( screenMode );
  if (was_blink)
    setBlinkState();
  else
    setIntenseState();
  setCrtData();
  suspended = 0;
  
  SetConsoleCtrlHandler(ConsoleEventHandler, TRUE);
  SetConsoleMode(__tvWin32ConInp, TV_CONSOLE_MODE);
}

void TScreen::suspend()
{
  if (suspended)
    return;
  SetConsoleCtrlHandler(ConsoleEventHandler, FALSE);
  SetConsoleMode(__tvWin32ConInp, saveScreenConsoleMode);
  was_blink = getBlinkState();
  RestoreScreen();
  suspended = 1;
}

ushort TScreen::fixCrtMode( ushort mode )
{
  return mode;
}

void TScreen::setCrtData()
{
  screenMode = getCrtMode();
  screenWidth = getCols();
  screenHeight = getRows();
  hiResScreen = Boolean(screenHeight > 25);
  cursorLines = 15;
  setCursorType( 0x2000 );
}

void TScreen::clearScreen()
{
  TDisplay::clearScreen(screenWidth, screenHeight);
}

void TScreen::setVideoMode( ushort mode )
{
  setCrtMode( fixCrtMode( mode ) );
  setCrtData();
}

void TScreen::setVideoMode( char *mode )
{
  setCrtMode( mode );
  setCrtData();
}

void TScreen::setCursorType(ushort ct)
{
  TDisplay::setCursorType(ct);
}

ushort TScreen::getCursorType()
{
  return TDisplay::getCursorType();
}

ushort TScreen::getRows()
{
  return TDisplay::getRows();
}

ushort TScreen::getCols()
{
  return TDisplay::getCols();
}

void TScreen::GetCursor(int &x,int &y)
{
  TDisplay::GetCursor(x,y);
}

void TScreen::SetCursor(int x,int y)
{
  TDisplay::SetCursor(x,y);
}


ushort TScreen::getCharacter(unsigned offset)
{
  COORD coord;
  coord.Y = (SHORT)((offset) / screenWidth);
  coord.X = (SHORT)((offset) % screenWidth);
  DWORD cRead;
  ushort ch;

  ReadConsoleOutputAttribute(
    __tvWin32ConOut,
    &ch,
    1,
    coord,
    &cRead);
  ch <<= 8;

  ReadConsoleOutputCharacter(
    __tvWin32ConOut,
    (CHAR*)&ch,
    1,
    coord,
    &cRead);

  return ch;
}

void TScreen::getCharacter(unsigned offset,ushort *buf,unsigned count)
{
  ensureOutBufCapacity(count);
  COORD coord;
  coord.Y = (SHORT)((offset) / screenWidth);
  coord.X = (SHORT)((offset) % screenWidth);
  DWORD cRead;

  ReadConsoleOutputAttribute(
    __tvWin32ConOut,
    outBufAttr,
    count,
    coord,
    &cRead);

  ReadConsoleOutputCharacter(
    __tvWin32ConOut,
    outBuf,
    count,
    coord,
    &cRead);

  for (count = 0; count < cRead; count++, buf++)
     *buf = (ushort)(outBufAttr[count] << 8) | (ushort)(uchar)outBuf[count];
}

void TScreen::setCharacter(unsigned offset,ushort value)
{
  ensureOutBufCapacity(1);
  COORD coord;
  coord.Y = (SHORT)((offset) / screenWidth);
  coord.X = (SHORT)((offset) % screenWidth);

  outBufCI[0].Char.AsciiChar = (CHAR)(value & 0xFF);;
  outBufCI[0].Attributes = (WORD)(value >> 8);
    
  COORD dwBufferSize = {1, 1};
  COORD dwBufferCoord = {0, 0};
  SMALL_RECT rcWriteRegion = {coord.X, coord.Y, coord.X, coord.Y};
  WriteConsoleOutput(
    __tvWin32ConOut,
    outBufCI,
    dwBufferSize, dwBufferCoord,
    &rcWriteRegion);
    
  /*
  FillConsoleOutputCharacter(
    __tvWin32ConOut,
    (CHAR)(value & 0xFF),
    1,
    coord,
    &cWritten);
  FillConsoleOutputAttribute(
    __tvWin32ConOut,
    (WORD)(value >> 8),
    1,
    coord,
    &cWritten);
  */
}

void TScreen::setCharacter(unsigned offset,ushort *values,unsigned count)
{
  ensureOutBufCapacity(count);
  COORD coord;
  coord.Y = (SHORT)((offset) / screenWidth);
  coord.X = (SHORT)((offset) % screenWidth);

  for (unsigned i = 0; i < count; i++, values++)
    {
      outBufCI[i].Char.AsciiChar = (CHAR)(*values & 0xFF);
      outBufCI[i].Attributes = (WORD)(*values >> 8);
    }
   
  COORD dwBufferSize = {count, 1};
  COORD dwBufferCoord = {0, 0};
  SMALL_RECT rcWriteRegion = {coord.X, coord.Y, coord.X + count - 1, coord.Y};
  WriteConsoleOutput(
    __tvWin32ConOut,
    outBufCI,
    dwBufferSize, dwBufferCoord,
    &rcWriteRegion);
}

static void SaveScreen()
{
  unsigned rows = TScreen::getRows();
  unsigned cols = TScreen::getCols();
  
  saveScreenSize = rows * cols;
  free(saveScreenBuf);
  saveScreenBuf = (ushort*)malloc(saveScreenSize * sizeof(ushort));

  uchar screenWidthSave = TScreen::screenWidth;
  uchar screenHeightSave = TScreen::screenHeight;

  // temporarily set these variables to let getCharacter work properly
  TScreen::screenWidth = (uchar)cols;
  TScreen::screenHeight = (uchar)rows;
  for (unsigned row = 0, ofs = 0; row < rows; row++, ofs += cols)
  {
    TScreen::getCharacter(ofs, saveScreenBuf + ofs, cols);
  }
  TScreen::screenWidth = screenWidthSave;
  TScreen::screenHeight = screenHeightSave;
  
  saveScreenCursorType = TScreen::getCursorType();
  saveScreenWidth = (uchar)cols;
  saveScreenHeight = (uchar)rows;
  TScreen::GetCursor(saveScreenCursorX, saveScreenCursorY);

  GetConsoleMode(__tvWin32ConInp, &saveScreenConsoleMode);
}

static void RestoreScreen()
{
  unsigned rows = saveScreenHeight;
  unsigned cols = saveScreenWidth;
  
  for (unsigned row = 0, ofs = 0; row < rows; row++, ofs += cols)
  {
    TScreen::setCharacter(ofs, saveScreenBuf + ofs, cols);
  }

  TScreen::setCursorType(saveScreenCursorType);
  TScreen::SetCursor(saveScreenCursorX, saveScreenCursorY);
}

static void SaveScreenReleaseMemory()
{
  free(saveScreenBuf);
  saveScreenBuf = NULL;
  saveScreenSize = 0;
}


void setBlinkState()
{
}
void setIntenseState()
{
}
int getBlinkState()
{
 return 0;
}

extern "C" void __tvWin32Yield(int micros)
{
  DWORD msecs = micros < 0 ? INFINITE : micros / 1000;
  WaitForSingleObject(__tvWin32ConInp, msecs);
}

static BOOL WINAPI ConsoleEventHandler(DWORD dwCtrlType)
{
  if (dwCtrlType == CTRL_C_EVENT || dwCtrlType == CTRL_BREAK_EVENT)
    return TRUE;
  return FALSE;
}

int TV_System(const char *command)
{
  int rc = system(command);
  SetConsoleMode(__tvWin32ConInp, TV_CONSOLE_MODE);
  return rc;
}

#endif // _WIN32
// vi: set ts=8 sw=2 et : //

