#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>



void main()
{
	INPUT_RECORD ir;
	HANDLE hInp;
	DWORD cEvents;
	
	hInp = GetStdHandle(STD_INPUT_HANDLE);
	
	while (1) {
		ReadConsoleInput(hInp, &ir, 1, &cEvents);
		if (ir.EventType == KEY_EVENT) {
			printf("KeyDown = %d, VK = %d, SC = %x Char = %d\n",
				ir.Event.KeyEvent.bKeyDown,
				ir.Event.KeyEvent.wVirtualKeyCode,
				ir.Event.KeyEvent.wVirtualScanCode,
				ir.Event.KeyEvent.uChar.AsciiChar);
		}
	}
}