/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
/* Modified by Vadim Beloborodov to be used on WIN32 console */
#ifdef _WIN32

#define Uses_TDisplay
#define Uses_TScreen
#include <tv.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

extern HANDLE STDOUT;
extern CONSOLE_SCREEN_BUFFER_INFO ConsoleInfo;
extern CONSOLE_CURSOR_INFO  ConsoleCursorInfo;

extern CRITICAL_SECTION lockWindowSizeChanged;
extern int WindowSizeChanged;

static ushort Equipment;
static uchar CrtInfo;
static uchar CrtRows;

TFont *TDisplay::font=0;

ushort * TDisplay::equipment = &Equipment;
uchar * TDisplay::crtInfo = &CrtInfo;
uchar * TDisplay::crtRows = &CrtRows;
uchar TDisplay::Page = 0;

void TDisplay::SetPage(uchar)
{
}

void TDisplay::SetCursor(int x,int y)
{
	COORD pos;
	pos.X=x;
	pos.Y=y;
	SetConsoleCursorPosition(STDOUT,pos);
}

void TDisplay::GetCursor(int &x,int &y)
{
	GetConsoleScreenBufferInfo(STDOUT, &ConsoleInfo);
	x = ConsoleInfo.dwCursorPosition.X;
	y = ConsoleInfo.dwCursorPosition.Y;
}

ushort TDisplay::getCursorType()
{
	GetConsoleCursorInfo(STDOUT,&ConsoleCursorInfo);
	if( ConsoleCursorInfo.bVisible ) {
		if (ConsoleCursorInfo.dwSize<=10)
			return 0x0607;
		return 0xFFFF;
	}
	return 0x2000;
}

void TDisplay::setCursorType( ushort ct)
{
	GetConsoleCursorInfo(STDOUT,&ConsoleCursorInfo);
	if ( ct == 0x2000) {
		ConsoleCursorInfo.bVisible=False;
	} else {
		ConsoleCursorInfo.bVisible=True;
		if ( ct == 0x0607 )
			ConsoleCursorInfo.dwSize=10;
		else
			ConsoleCursorInfo.dwSize=99;
	}
	SetConsoleCursorInfo(STDOUT,&ConsoleCursorInfo);
}

void TDisplay::clearScreen( uchar w, uchar h)
{
	COORD coord = { 0, 0 };
	DWORD read;
	FillConsoleOutputAttribute( STDOUT, 0x07, w*h, coord, &read );
	FillConsoleOutputCharacterA( STDOUT, ' ', w*h, coord, &read );
}

void TDisplay::videoInt()
{
}

ushort TDisplay::getRows()
{
#if 0
	ushort Rows=ConsoleInfo.srWindow.Bottom-ConsoleInfo.srWindow.Top+1;
	if ( Rows>ConsoleInfo.dwMaximumWindowSize.Y )
		Rows = ConsoleInfo.dwMaximumWindowSize.Y;
	return Rows;
#else
	return ConsoleInfo.dwSize.Y;
#endif
}

ushort TDisplay::getCols()
{
#if 0
	ushort Cols =ConsoleInfo.srWindow.Right-ConsoleInfo.srWindow.Left+1;
	if ( Cols>ConsoleInfo.dwMaximumWindowSize.X )
		Cols = ConsoleInfo.dwMaximumWindowSize.X;
	return Cols;
#else
	return ConsoleInfo.dwSize.X;
#endif
}

ushort TDisplay::getCrtMode()
{
	GetConsoleScreenBufferInfo(STDOUT, &ConsoleInfo);
	return smCO80;
}

void TDisplay::setCrtMode( ushort )
{
	GetConsoleScreenBufferInfo(STDOUT, &ConsoleInfo);
	EnterCriticalSection( &lockWindowSizeChanged );
	LeaveCriticalSection( &lockWindowSizeChanged );
}

void TDisplay::setCrtMode( char * )
{
	GetConsoleScreenBufferInfo(STDOUT, &ConsoleInfo);
	EnterCriticalSection( &lockWindowSizeChanged );
	LeaveCriticalSection( &lockWindowSizeChanged );
}

void TDisplay::updateIntlChars()
{
}

void TDisplay::SetFontHandler(TFont *)
{
}

int TDisplay::CheckForWindowSize(void)
{
	int SizeChanged=WindowSizeChanged;
	EnterCriticalSection( &lockWindowSizeChanged );
	WindowSizeChanged=0;
	LeaveCriticalSection( &lockWindowSizeChanged );
	if (SizeChanged) {
		GetConsoleScreenBufferInfo(STDOUT, &ConsoleInfo);
	}
	return SizeChanged;
}
#endif // _WIN32
