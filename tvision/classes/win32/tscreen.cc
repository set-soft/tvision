/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *
 
Modified by Robert H”hne to be used for RHIDE.
Modified by Vadim Beloborodov to be used on WIN32 console 
 *
 *
 */

#ifdef _WIN32

#define Uses_string

#define Uses_TScreen
#define Uses_TEvent
#include <tv.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>


ushort TScreen::startupMode = 0xFFFF;
ushort TScreen::startupCursor = 0;
ushort TScreen::screenMode = 0xFFFF;
uchar TScreen::screenWidth = 0;
uchar TScreen::screenHeight = 0;
Boolean TScreen::hiResScreen = False;
Boolean TScreen::checkSnow = False;
ushort *TScreen::screenBuffer = NULL;
ushort TScreen::cursorLines = 0;
// SET: Indicates if the screen was initialized by the TScreen constructor
char TScreen::initialized = 0;
// SET: Starts as suspended to avoid TScreen::suspend() calls and other
// similar stuff before initializing
char TScreen::suspended = 1;

extern HANDLE STDOUT;
extern CONSOLE_SCREEN_BUFFER_INFO ConsoleInfo;
extern CONSOLE_CURSOR_INFO  ConsoleCursorInfo;


void SaveScreen();
void SaveScreenReleaseMemory();
void RestoreScreen();
void ScreenUpdate();

ushort user_mode=0;
int dual_display=0;

void InitConsole();
void DoneConsole();


void TScreen::GetCursor(int &x,int &y)
{
	TDisplay::GetCursor(x,y);
}

void TScreen::SetCursor(int x,int y)
{
	TDisplay::SetCursor(x,y);
}

TScreen::TScreen()
{
    InitConsole();
	user_mode = screenMode = startupMode = getCrtMode();
	suspended = 1;
	initialized = 1;
	resume();
	screenBuffer = new ushort[getRows()*getCols()];
	ZeroMemory(screenBuffer, getRows()*getCols()*sizeof(ushort));
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
	setCrtData();
	suspended = 0;
}

TScreen::~TScreen()
{
	suspend();
	SaveScreenReleaseMemory();
	DoneConsole();
	if (screenBuffer) delete screenBuffer;
}

void TScreen::suspend()
{
	if (suspended) return;
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
	cursorLines = 0x0607;
	setCursorType( 0x2000 );
}

void TScreen::clearScreen()
{
	TDisplay::clearScreen( screenWidth, screenHeight );
}

void TScreen::setVideoMode( ushort mode )
{
	if (screenBuffer) delete screenBuffer;
	setCrtMode( fixCrtMode( mode ) );
	setCrtData();
	screenBuffer = new ushort[screenWidth*screenHeight];
	ZeroMemory(screenBuffer, getRows()*getCols()*sizeof(ushort));
}

void TScreen::setVideoMode( char *mode )
{
	if (screenBuffer) delete screenBuffer;
	setCrtMode( mode );
	setCrtData();
	screenBuffer = new ushort[screenWidth*screenHeight];
	ZeroMemory(screenBuffer, getRows()*getCols()*sizeof(ushort));
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

ushort TScreen::getCharacter(unsigned offset)
{
	if (screenBuffer)
		return screenBuffer[offset];
	return 0;
}

void TScreen::getCharacter(unsigned offset,ushort *buf,unsigned count)
{
	if (screenBuffer)
		memcpy(buf, screenBuffer+offset, count*sizeof(ushort) );
	else
		memset(buf,0,count*sizeof(ushort));
}

void TScreen::setCharacter(unsigned offset,ushort value)
{
	setCharacter(offset, &value, 1 );
}

void TScreen::setCharacter(unsigned dst,ushort *src,unsigned len)
{
	ushort *old = ((ushort*)screenBuffer) + dst;
	ushort *old_right = old + len - 1;
	ushort *src_right = src + len - 1;

	while (len > 0 && *old == *src) {
		dst++;
		len--;
		old++;
		src++;
	}

	/* remove unchanged characters from right to left */

	while (len > 0 && *old_right == *src_right) {
		len--;
		old_right--;
		src_right--;
	}

	/* write only middle changed characters */

	if (len > 0) {
		CHAR_INFO ch[maxViewWidth];
		int i = 0;

		while (len-- > 0) {
			*old++ = *src;
			ch[i].Attributes = HIBYTE(*src);
			ch[i].Char.AsciiChar = LOBYTE(*src);
			i++;
			src++;
		}

		ushort x = dst % screenWidth, y = dst / screenWidth;

		SMALL_RECT _to = {x, y, x + i - 1, y};
		COORD _bsize = {i, 1};
		static COORD _from = {0, 0};
		WriteConsoleOutput(STDOUT, ch, _bsize, _from, &_to);
	}
}

void setIntenseState()
{
}

void setBlinkState()
{
}

int getBlinkState()
{
	return 1;
}


static CHAR_INFO* buffer = NULL;
static CONSOLE_SCREEN_BUFFER_INFO info;

void SaveScreen()
{
	GetConsoleScreenBufferInfo( STDOUT, &info );
	if ( buffer ) {
		delete buffer;
		buffer = NULL;
	}
	buffer = new CHAR_INFO[info.dwSize.Y * info.dwSize.X ];
	if( buffer ) {
		SMALL_RECT rect = { 0, 0, info.dwSize.X, info.dwSize.Y };
		COORD      start = { 0, 0 };
		ReadConsoleOutput( STDOUT,
		                   buffer,
		                   info.dwSize,
		                   start,
		                   &rect );
	}
}

void RestoreScreen()
{
	if( buffer ) {
		SMALL_RECT rect = { 0, 0, info.dwSize.X, info.dwSize.Y };
		COORD      start = { 0, 0 };
		WriteConsoleOutput( STDOUT,
		                    buffer,
		                    info.dwSize,
		                    start,
		                    &rect );
	}
}

void SaveScreenReleaseMemory(void)
{
	if (buffer) delete buffer;
	buffer = NULL;
}

#endif // _WIN32

