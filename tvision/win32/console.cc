/* 
  Win32 console events handlers
  by Vadim Beloborodov
*/
#define Uses_TEvent
#define Uses_TKeys
#include <tv.h>

#include <string.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

int WindowSizeChanged=0;
int ExitEventThread=0;
ushort LastControlKeyState=0;
CRITICAL_SECTION lockWindowSizeChanged;
HANDLE STDOUT=NULL;
HANDLE STDIN=NULL;
HANDLE EventThreadHandle=NULL;
CONSOLE_SCREEN_BUFFER_INFO ConsoleInfo;
CONSOLE_CURSOR_INFO  ConsoleCursorInfo;

char testchars[]=
"`1234567890-="
"~!@#$%^&*()_+"
"qwertyuiop[]"
"QWERTYUIOP{}"
"asdfghjkl;'\\"
"ASDFGHJKL:\"|"
"zxcvbnm,./"
"ZXCVBNM<>?";

ushort transShiftState( DWORD state );
int transKeyEvent( KeyDownEvent& dst, KEY_EVENT_RECORD& src );

ushort getshiftstate()
{
	return LastControlKeyState;
};

//********************** KEYBOARD QUEUE FUNCTIONS ****************************
#define eventKeyboardQSize 16
static KeyDownEvent *evKeyboardIn;
static KeyDownEvent *evKeyboardOut;
static KeyDownEvent evKeyboardQueue[eventKeyboardQSize];
unsigned evKeyboardLength;
CRITICAL_SECTION lockKeyboard;

void putConsoleKeyboardEvent(KeyDownEvent& key)
{
	EnterCriticalSection( &lockKeyboard );
	if (evKeyboardLength < eventKeyboardQSize) {
		evKeyboardLength++;
		*evKeyboardIn = key;
		if (++evKeyboardIn >= &evKeyboardQueue[eventKeyboardQSize]) evKeyboardIn = &evKeyboardQueue[0];
	}
	LeaveCriticalSection( &lockKeyboard );
}

int getConsoleKeyboardEvent(KeyDownEvent& key)
{
	EnterCriticalSection( &lockKeyboard );
	int haskeyevent = evKeyboardLength>0;
	if (haskeyevent) {
		evKeyboardLength--;
		key = *evKeyboardOut;
		if (++evKeyboardOut >= &evKeyboardQueue[eventKeyboardQSize])
			evKeyboardOut = &evKeyboardQueue[0];
	}
	LeaveCriticalSection( &lockKeyboard );
	return haskeyevent;
}

void HandleKeyEvent()
{
	INPUT_RECORD ir;
	DWORD dwRead;
	PeekConsoleInput(STDIN,&ir,1,&dwRead);
	if ( (dwRead==1) && (ir.EventType==KEY_EVENT) ) {
		if( ir.Event.KeyEvent.bKeyDown ) {
			//support for non US keyboard layout on Windows95
			if ( ((ir.Event.KeyEvent.dwControlKeyState & (RIGHT_ALT_PRESSED|LEFT_ALT_PRESSED|RIGHT_CTRL_PRESSED|LEFT_CTRL_PRESSED)) == 0) && 
				(ir.Event.KeyEvent.uChar.AsciiChar) &&
				strchr(testchars,ir.Event.KeyEvent.uChar.AsciiChar) )  {
				uchar chr;
				ReadConsole(STDIN,&chr,1,&dwRead,NULL);
				ir.Event.KeyEvent.uChar.AsciiChar = chr;
				dwRead = 0;
			}
			else {
				ReadConsoleInput(STDIN,&ir,1,&dwRead);
				dwRead = 0;
			}
			//translate event
			KeyDownEvent key;
			if( transKeyEvent( key, ir.Event.KeyEvent ) ) {
				putConsoleKeyboardEvent( key );
			}
		}
		LastControlKeyState = transShiftState(ir.Event.KeyEvent.dwControlKeyState);
	}
	if (dwRead==1) {
		ReadConsoleInput(STDIN,&ir,1,&dwRead);
	}
}
//********************** MOUSE QUEUE FUNCTIONS ******************************
#define eventMouseQSize 300
static MouseEventType *evMouseIn;
static MouseEventType *evLastMouseIn=NULL;
static MouseEventType *evMouseOut;
static MouseEventType evMouseQueue[eventMouseQSize];
static unsigned evMouseLength;
CRITICAL_SECTION lockMouse;

void putConsoleMouseEvent(MouseEventType& mouse)
{
	EnterCriticalSection( &lockMouse );
	if (evMouseLength < eventMouseQSize) {
		//compress mouse events
		if (evLastMouseIn && evMouseLength && (evLastMouseIn->buttons==mouse.buttons) )
			*evLastMouseIn = mouse;
		else {
			evMouseLength++;
			*evMouseIn = mouse;
			evLastMouseIn = evMouseIn;
			if (++evMouseIn >= &evMouseQueue[eventMouseQSize]) evMouseIn = &evMouseQueue[0];
		}
	}
	LeaveCriticalSection( &lockMouse );
}

int getConsoleMouseEvent(MouseEventType& mouse)
{
	EnterCriticalSection( &lockMouse );
	int hasmouseevent = evMouseLength>0;
	if (hasmouseevent) {
		evMouseLength--;
		mouse = *evMouseOut;
		if (++evMouseOut >= &evMouseQueue[eventMouseQSize])
			evMouseOut = &evMouseQueue[0];
	}
	LeaveCriticalSection( &lockMouse );
	return hasmouseevent;
}

void HandleMouseEvent()
{
	INPUT_RECORD ir;
	DWORD dwRead;
	ReadConsoleInput(STDIN,&ir,1,&dwRead);
	if ( (dwRead==1) && (ir.EventType==MOUSE_EVENT) ) {
		MouseEventType mouse;
		mouse.where.x = ir.Event.MouseEvent.dwMousePosition.X;
		mouse.where.y = ir.Event.MouseEvent.dwMousePosition.Y;
		mouse.buttons = 0;
		mouse.doubleClick = False;
		if (ir.Event.MouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED)
			mouse.buttons |= mbLeftButton;
		if (ir.Event.MouseEvent.dwButtonState & RIGHTMOST_BUTTON_PRESSED)
			mouse.buttons |= mbRightButton;
		putConsoleMouseEvent(mouse);
		LastControlKeyState = transShiftState(ir.Event.KeyEvent.dwControlKeyState);
	}
};


DWORD WINAPI HandleEvents(void* p)
{
	INPUT_RECORD ir;
	DWORD dwRead;
	while (!ExitEventThread) {
		WaitForSingleObject(STDIN,INFINITE);
		if (!ExitEventThread) {
			if (PeekConsoleInput(STDIN,&ir,1,&dwRead) && dwRead >0 ) {
				switch (ir.EventType) {
				case MOUSE_EVENT:
					HandleMouseEvent();
					break;
				case KEY_EVENT:
					HandleKeyEvent();
					break;
				case WINDOW_BUFFER_SIZE_EVENT:
					EnterCriticalSection( &lockWindowSizeChanged );
					WindowSizeChanged=1;
					LeaveCriticalSection( &lockWindowSizeChanged );
				default:
					ReadConsoleInput(STDIN,&ir,1,&dwRead);
				}
			}
		} else {
			ReadConsoleInput(STDIN,&ir,1,&dwRead);
		}
	}
	return 0;
}

void InitConsole()
{
	STDOUT = GetStdHandle(STD_OUTPUT_HANDLE);
	STDIN  = GetStdHandle(STD_INPUT_HANDLE);

	//enable mouse input
	DWORD mode;
	GetConsoleMode( STDIN, &mode );
	mode |=	ENABLE_MOUSE_INPUT|ENABLE_WINDOW_INPUT;
	mode &= ~(ENABLE_LINE_INPUT|ENABLE_ECHO_INPUT);
	SetConsoleMode(STDIN,mode);

	GetConsoleScreenBufferInfo(STDOUT, &ConsoleInfo);

	InitializeCriticalSection(&lockWindowSizeChanged);
	InitializeCriticalSection(&lockMouse);
	InitializeCriticalSection(&lockKeyboard);

	evMouseLength = 0;
	evMouseIn = evMouseOut = &evMouseQueue[0];

	evKeyboardLength = 0;
	evKeyboardIn = evKeyboardOut = &evKeyboardQueue[0];

	DWORD  EventThreadID;
	EventThreadHandle=CreateThread(NULL,0,HandleEvents,NULL,0,&EventThreadID);
}

void DoneConsole()
{
	INPUT_RECORD ir;
	DWORD written;

	ZeroMemory(&ir,sizeof(ir));
	ExitEventThread = 1;
	ir.EventType=KEY_EVENT;
	WriteConsoleInput(STDIN,&ir,1,&written);
	WaitForSingleObject(EventThreadHandle,INFINITE);
	CloseHandle(EventThreadHandle);

	DeleteCriticalSection(&lockWindowSizeChanged);
	DeleteCriticalSection(&lockMouse);
	DeleteCriticalSection(&lockKeyboard);
}
