/**[txh]********************************************************************

  Copyright (c) 2002 by Salvador E. Tropea (SET)
  Based on code contributed by Vadim Beloborodov.

  Description:
  Win32 Screen routines.
  The original implementation was done by Vadim, I removed some code, added
some routines and adapted it to the new architecture.
  One important detail is that Vadim implemented it with threads. The
thread that monitors the window size is different, that's why a lock is
needed.
  
***************************************************************************/

#include <tv/configtv.h>

#ifdef TVOS_Win32

#define Uses_string
#define Uses_stdlib

#ifdef TVCompf_Cygwin
 #define Uses_unistd
#endif

#define Uses_TScreen
#define Uses_TEvent
#define Uses_TGKey
#include <tv.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <tv/win32/screen.h>
#include <tv/win32/mouse.h>
#include <tv/win32/key.h>

CHAR_INFO                 *TScreenWin32::buffer=NULL;
CONSOLE_SCREEN_BUFFER_INFO TScreenWin32::info;
int                        TScreenWin32::ExitEventThread=0;
HANDLE                     TScreenWin32::EventThreadHandle=NULL;
DWORD                      TScreenWin32::oldConsoleMode,
                           TScreenWin32::newConsoleMode;
unsigned                   TScreenWin32::xCurStart,
                           TScreenWin32::yCurStart;

TScreenWin32::TScreenWin32()
{
 InitConsole();
 flags0=CodePageVar;
 screenMode=startupMode=getCrtMode();
 cursorLines=startupCursor=getCursorType();
 screenWidth =GetCols();
 screenHeight=GetRows();
 screenBuffer=new ushort[screenHeight*screenWidth];
 ZeroMemory(screenBuffer,screenHeight*screenWidth*sizeof(ushort));

 SaveScreen();
 GetCursorPos(xCurStart,yCurStart);
 initialized=1;
 setCrtData();
}

void TScreenWin32::InitConsole()
{
 // Get handles to access Standard Input and Output
 hOut=GetStdHandle(STD_OUTPUT_HANDLE);
 hIn =GetStdHandle(STD_INPUT_HANDLE);

 // Enable mouse input
 GetConsoleMode(hIn,&oldConsoleMode);
 newConsoleMode=oldConsoleMode | ENABLE_MOUSE_INPUT|ENABLE_WINDOW_INPUT;
 newConsoleMode&=~(ENABLE_LINE_INPUT|ENABLE_ECHO_INPUT|ENABLE_PROCESSED_INPUT);
 SetConsoleMode(hIn,newConsoleMode);

 GetConsoleScreenBufferInfo(hOut,&ConsoleInfo);

 InitializeCriticalSection(&lockWindowSizeChanged);

 TDisplayWin32::Init();

 TScreen::clearScreen=clearScreen;
 TScreen::setCharacter=setCharacter;
 TScreen::setCharacters=setCharacters;
 TScreen::System=System;
 TScreen::Resume=Resume;
 TScreen::Suspend=Suspend;

 TGKeyWin32::Init();
 THWMouseWin32::Init();

 DWORD EventThreadID;
 EventThreadHandle=CreateThread(NULL,0,HandleEvents,NULL,0,&EventThreadID);
}

void TScreenWin32::DoneConsole()
{
 INPUT_RECORD ir;
 DWORD written;

 // Stop the events thread
 ZeroMemory(&ir,sizeof(ir));
 ExitEventThread=1;
 ir.EventType=KEY_EVENT;
 WriteConsoleInput(hIn,&ir,1,&written);
 WaitForSingleObject(EventThreadHandle,INFINITE);
 CloseHandle(EventThreadHandle);
 
 DeleteCriticalSection(&lockWindowSizeChanged);
 THWMouseWin32::DeInit();
 TGKeyWin32::DeInit();
}

DWORD WINAPI TScreenWin32::HandleEvents(void *)
{
 INPUT_RECORD ir;
 DWORD dwRead;

 while (!ExitEventThread)
   {
    WaitForSingleObject(hIn,INFINITE);
    if (!ExitEventThread)
      {
       if (PeekConsoleInput(hIn,&ir,1,&dwRead) && dwRead>0)
         {
          switch (ir.EventType)
            {
             case MOUSE_EVENT:
                  THWMouseWin32::HandleMouseEvent();
                  break;

             case KEY_EVENT:
                  TGKeyWin32::HandleKeyEvent();
                  break;

             case WINDOW_BUFFER_SIZE_EVENT:
                  EnterCriticalSection(&lockWindowSizeChanged);
                  WindowSizeChanged=1;
                  LeaveCriticalSection(&lockWindowSizeChanged);

             default:
                  ReadConsoleInput(hIn,&ir,1,&dwRead);
            }
         }
      }
    else
       ReadConsoleInput(hIn,&ir,1,&dwRead);
   }
 return 0;
}


void TScreenWin32::Resume()
{
 GetCursorPos(xCurStart,yCurStart);
 SaveScreen();
 setCrtData();
 GetConsoleMode(hIn,&oldConsoleMode);
 SetConsoleMode(hIn,newConsoleMode);
}

void TScreenWin32::Suspend()
{
 RestoreScreen();
 SetCursorPos(xCurStart,yCurStart);
 SetConsoleMode(hIn,oldConsoleMode);
}

TScreenWin32::~TScreenWin32()
{
 Suspend();
 suspended=1;
 SaveScreenReleaseMemory();
 setCursorType(startupCursor);
 DoneConsole();
 if (screenBuffer)
   {
    delete screenBuffer;
    screenBuffer=0;
   }
}

void TScreenWin32::clearScreen()
{
 COORD coord ={0,0};
 DWORD read;
 unsigned size=GetRows()*GetCols();
 FillConsoleOutputAttribute(hOut,0x07,size,coord,&read);
 FillConsoleOutputCharacterA(hOut,' ',size,coord,&read);
}

void TScreenWin32::setCharacter(unsigned offset, ushort value)
{
 setCharacters(offset,&value,1);
}

void TScreenWin32::setCharacters(unsigned dst, ushort *src, unsigned len)
{
 ushort *old=((ushort*)screenBuffer)+dst;
 ushort *old_right=old+len-1;
 ushort *src_right=src+len-1;
 
 while (len>0 && *old==*src)
   {
    dst++;
    len--;
    old++;
    src++;
   }
 
 /* remove unchanged characters from right to left */
 while (len>0 && *old_right==*src_right)
   {
    len--;
    old_right--;
    src_right--;
   }
 
 /* write only middle changed characters */
 if (len>0)
   {
    CHAR_INFO ch[maxViewWidth];
    int i=0;
 
    while (len-->0)
      {
       *old++=*src;
       ch[i].Attributes=HIBYTE(*src);
       ch[i].Char.AsciiChar=LOBYTE(*src);
       i++;
       src++;
      }
 
   ushort x=dst%screenWidth, y=dst/screenWidth;
 
   SMALL_RECT to={x,y,x+i-1,y};
   COORD bsize={i,1};
   static COORD from={0,0};
   WriteConsoleOutput(hOut,ch,bsize,from,&to);
  }
}

void TScreenWin32::SaveScreen()
{
 GetConsoleScreenBufferInfo(hOut,&info);
 if (buffer)
   {
    delete buffer;
    buffer=NULL;
   }
 buffer=new CHAR_INFO[info.dwSize.Y*info.dwSize.X];
 if (buffer)
   {// SET: The rectangle is 0 to size-1
    SMALL_RECT rect={ 0,0, info.dwSize.X-1,info.dwSize.Y-1 };
    COORD      start={0,0};
    ReadConsoleOutput(hOut,buffer,info.dwSize,start,&rect);
  }
}

void TScreenWin32::RestoreScreen()
{
 if (buffer)
   {
    SMALL_RECT rect={ 0,0, info.dwSize.X-1,info.dwSize.Y-1 };
    COORD      start={0,0};
    WriteConsoleOutput(hOut,buffer,info.dwSize,start,&rect);
   }
}

void TScreenWin32::SaveScreenReleaseMemory(void)
{
 if (buffer)
    delete buffer;
 buffer = NULL;
}

#ifndef TVCompf_Cygwin
int TScreenWin32::System(const char *command, pid_t *pidChild)
{
  // fork mechanism not implemented, indicate the child finished
  if (pidChild)
     *pidChild=0;
  return system(command);
}
#else
// fork mechanism is implemented in Cygwin, so linux code should work -- OH!
// SET: Call to an external program, optionally forking
int TScreenWin32::System(const char *command, pid_t *pidChild)
{
 if (!pidChild)
    return system(command);

 pid_t cpid=fork();
 if (cpid==0)
   {// Ok, we are the child
    //   I'm not sure about it, but is the best I have right now.
    //   Doing it we can kill this child and all the subprocesses
    // it creates by killing the group. It also have an interesting
    // effect that I must evaluate: By doing it this process lose
    // the controlling terminal and won't be able to read/write
    // to the parents console. I think that's good.
    if (setsid()==-1)
       _exit(127);
    char *argv[4];

    argv[0]=getenv("SHELL");
    if (!argv[0])
       argv[0]="/bin/sh";
    argv[1]="-c";
    argv[2]=(char *)command;
    argv[3]=0;
    execvp(argv[0],argv);
    // We get here only if exec failed
    _exit(127);
   }
 if (cpid==-1)
   {// Fork failed do it manually
    *pidChild=0;
    return system(command);
   }
 *pidChild=cpid;
 return 0;
}
#endif

TScreen *TV_Win32DriverCheck()
{
 TScreenWin32 *drv=new TScreenWin32();
 if (!TScreen::initialized)
   {
    delete drv;
    return 0;
   }
 return drv;
}

/*
void TScreenWin32::setVideoMode( ushort mode )
{
   if (screenBuffer) delete screenBuffer;
   setCrtMode( fixCrtMode( mode ) );
   setCrtData();
   screenBuffer = new ushort[screenWidth*screenHeight];
   ZeroMemory(screenBuffer, getRows()*getCols()*sizeof(ushort));
}

void TScreenWin32::setVideoMode( char *mode )
{
   if (screenBuffer) delete screenBuffer;
   setCrtMode( mode );
   setCrtData();
   screenBuffer = new ushort[screenWidth*screenHeight];
   ZeroMemory(screenBuffer, getRows()*getCols()*sizeof(ushort));
}
*/
#endif // TVOS_Win32

