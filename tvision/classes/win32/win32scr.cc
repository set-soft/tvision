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
 ToDo: Set UseScreenSaver when we are in full screen.

 Configuration variables:
 ScreenWidth
 ScreenHeight
 AppCP
 ScrCP
 InpCP

 Notes:
1) I changed all the code related to the save/restore state using a new
screen buffer. I comment more in the "WinNT" driver. One really strange
thing is that this driver behaves better with the cursor. Don't know why.
I also added the resize window stuff.
  
***************************************************************************/

#include <tv/configtv.h>

#define Uses_string
#define Uses_stdlib
#define Uses_unistd
#define Uses_TScreen
#define Uses_TEvent
#define Uses_TGKey
#define Uses_TVCodePage
#include <tv.h>
#include <tv/win32/win32clip.h>

// I delay the check to generate as much dependencies as possible
#ifdef TVOS_Win32

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <tv/win32/screen.h>
#include <tv/win32/mouse.h>
#include <tv/win32/key.h>

//#define DEBUG
#ifdef DEBUG
 #define DBPr1(a)     fputs(a,stderr)
 #define DBPr2(a,b)   fprintf(stderr,a,b)
 #define DBPr3(a,b,c) fprintf(stderr,a,b,c)
#else
 #define DBPr1(a)
 #define DBPr2(a,b)
 #define DBPr3(a,b,c)
#endif

CONSOLE_SCREEN_BUFFER_INFO TScreenWin32::info;
int                        TScreenWin32::ExitEventThread=0;
HANDLE                     TScreenWin32::EventThreadHandle=NULL;
DWORD                      TScreenWin32::oldConsoleMode,
                           TScreenWin32::newConsoleMode;
unsigned                   TScreenWin32::xCurStart,
                           TScreenWin32::yCurStart;
unsigned                   TScreenWin32::saveScreenWidth,
                           TScreenWin32::saveScreenHeight;

TScreenWin32::TScreenWin32()
{
 if (!InitConsole()) return;
 flags0=CodePageVar | CursorShapes | CanSetVideoSize;
 startupMode=getCrtMode();
 startupCursor=getCursorType();
 saveScreenWidth =GetCols();
 saveScreenHeight=GetRows();

 unsigned maxX=saveScreenWidth, maxY=saveScreenHeight;
 long aux;
 if (optSearch("ScreenWidth",aux))
    maxX=aux;
 if (optSearch("ScreenHeight",aux))
    maxY=aux;
 if (maxX!=saveScreenWidth || maxY!=saveScreenHeight)
   {
    setCrtModeRes(maxX,maxY);
    // Update cached values
    GetConsoleScreenBufferInfo(hOut,&ConsoleInfo);
   }

 cursorLines=getCursorType();
 screenWidth =GetCols();
 screenHeight=GetRows();

 screenBuffer=new ushort[screenHeight*screenWidth];
 ZeroMemory(screenBuffer,screenHeight*screenWidth*sizeof(ushort));

 GetCursorPos(xCurStart,yCurStart);
 suspended=0;
 setCrtData();
}

int TScreenWin32::InitConsole()
{
 DWORD flags;
 // Check if we are running in a console
 if (!GetConsoleMode(GetStdHandle(STD_INPUT_HANDLE),&flags))
    return 0;
 // Get handles to access Standard Input and Output
 hIn    =GetStdHandle(STD_INPUT_HANDLE);
 hStdOut=GetStdHandle(STD_OUTPUT_HANDLE);
 // Create a new buffer, it have their own content and cursor
 hOut=CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE,
                                0,NULL,CONSOLE_TEXTMODE_BUFFER,NULL);
 if (hStdOut==INVALID_HANDLE_VALUE || hOut==INVALID_HANDLE_VALUE)
    return 0; // Something went wrong
 // Make the new one the active
 if (!SetConsoleActiveScreenBuffer(hOut))
    return 0;

 // If we are here this driver will be used
 initialized=1;
 if (dCB) dCB();

 // Enable mouse input
 GetConsoleMode(hIn,&oldConsoleMode);
 newConsoleMode=oldConsoleMode | ENABLE_MOUSE_INPUT|ENABLE_WINDOW_INPUT;
 newConsoleMode&=~(ENABLE_LINE_INPUT|ENABLE_ECHO_INPUT|ENABLE_PROCESSED_INPUT);
 SetConsoleMode(hIn,newConsoleMode);
 SetConsoleCtrlHandler(ConsoleEventHandler,TRUE);

 GetConsoleScreenBufferInfo(hOut,&ConsoleInfo);

 InitializeCriticalSection(&lockWindowSizeChanged);

 TDisplayWin32::Init();

 TScreen::clearScreen=clearScreen;
 TScreen::setCharacter=setCharacter;
 TScreen::setCharacters=setCharacters;
 TScreen::System_p=System;
 TScreen::Resume=Resume;
 TScreen::Suspend=Suspend;
 TScreen::setCrtModeRes_p=SetCrtModeRes;
 TScreen::setVideoModeRes_p=SetVideoModeRes;
 TScreen::setVideoMode=SetVideoMode;
 TScreen::setVideoModeExt=SetVideoModeExt;

 TVWin32Clipboard::Init();
 TGKeyWin32::Init();
 THWMouseWin32::Init();

 DWORD EventThreadID;
 EventThreadHandle=CreateThread(NULL,0,HandleEvents,NULL,0,&EventThreadID);

 UINT outCP=GetConsoleOutputCP();
 UINT  inCP=GetConsoleCP();
 // Look for user settings
 optSearch("AppCP",forcedAppCP);
 optSearch("ScrCP",forcedScrCP);
 optSearch("InpCP",forcedInpCP);
 // User settings have more priority than detected settings
 codePage=new TVCodePage(forcedAppCP!=-1 ? forcedAppCP : outCP,
                         forcedScrCP!=-1 ? forcedScrCP : outCP,
                         forcedInpCP!=-1 ? forcedInpCP : inCP);
 SetDefaultCodePages(outCP,outCP,inCP);

 return 1;
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

             // Vadim wrote it, I never could make Windows resize.
             // Not even providing a larger screen buffer.
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
 DBPr1("TScreenWin32::Resume\n");
 // First switch to our handle
 SetConsoleActiveScreenBuffer(hOut);
 // Now we can save the current window size
 GetConsoleScreenBufferInfo(hOut,&ConsoleInfo);
 saveScreenWidth =ConsoleInfo.dwSize.X;
 saveScreenHeight=ConsoleInfo.dwSize.Y;
 // Restore our window size
 SetCrtModeRes(screenWidth,screenHeight);
 GetConsoleScreenBufferInfo(hOut,&ConsoleInfo);
 setCrtData();
 // Invalidate the cache to force a redraw
 ZeroMemory(screenBuffer,screenHeight*screenWidth*sizeof(ushort));

 GetConsoleMode(hIn,&oldConsoleMode);
 SetConsoleMode(hIn,newConsoleMode);
 SetConsoleCtrlHandler(ConsoleEventHandler,TRUE);
}

void TScreenWin32::Suspend()
{
 // Restore window size (using our handle!)
 SetCrtModeRes(saveScreenWidth,saveScreenHeight);
 // Switch to the original handle
 SetConsoleActiveScreenBuffer(hStdOut);
 SetConsoleMode(hIn,oldConsoleMode);
 SetConsoleCtrlHandler(ConsoleEventHandler,FALSE);
}

TScreenWin32::~TScreenWin32()
{
 Suspend();
 suspended=1;
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

void TScreenWin32::setCharacter(unsigned offset, unsigned int value)
{
 setCharacters(offset,reinterpret_cast<ushort*>(&value),1);
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

#ifndef TVCompf_Cygwin
int TScreenWin32::System(const char *command, pid_t *pidChild, int in,
                         int out, int err)
{
  // fork mechanism not implemented, indicate the child finished
  if (pidChild)
     *pidChild=0;
  // If the caller asks for redirection replace the requested handles
  if (in!=-1)
     dup2(in,STDIN_FILENO);
  if (out!=-1)
     dup2(out,STDOUT_FILENO);
  if (err!=-1)
     dup2(err,STDERR_FILENO);
  return system(command);
}
#else
// fork mechanism is implemented in Cygwin, so linux code should work -- OH

// SET: Call to an external program, optionally forking
int TScreenWin32::System(const char *command, pid_t *pidChild, int in,
                         int out, int err)
{
 if (!pidChild)
   {
    // If the caller asks for redirection replace the requested handles
    if (in!=-1)
       dup2(in,STDIN_FILENO);
    if (out!=-1)
       dup2(out,STDOUT_FILENO);
    if (err!=-1)
       dup2(err,STDERR_FILENO);
    return system(command);
   }

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

    // If the caller asks for redirection replace the requested handles
    if (in!=-1)
       dup2(in,STDIN_FILENO);
    if (out!=-1)
       dup2(out,STDOUT_FILENO);
    if (err!=-1)
       dup2(err,STDERR_FILENO);

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

/**[txh]********************************************************************

  Description:
  Change the window size to the desired value. The font size is ignored
because it can't be controlled by the application, the Win32 API reference
I have says: "A screen buffer can be any size, limited only by available
memory. The dimensions of a screen buffer's window cannot exceed the
corresponding dimensions of either the screen buffer or the maximum window
that can fit on the screen based on the current font size (controlled
exclusively by the user).".@*
  It only works if we are windowed and this will prevent from going full
screen unless Windows knows an equivalent text mode.
  
  Return: 0 no change, 1 full change, 2 approx. change. by SET
  
***************************************************************************/

int TScreenWin32::SetCrtModeRes(unsigned w, unsigned h, int fW, int fH)
{
 CONSOLE_SCREEN_BUFFER_INFO info;
 DBPr3("TScreenWin32::SetCrtModeRes(%d,%d)\n",w,h);
 // Find current size
 if (!GetConsoleScreenBufferInfo(hOut,&info))
   {
    DBPr1("GetConsoleScreenBufferInfo failed\n");
    return 0;
   }
 // Is the same used?
 if (info.dwSize.X==(int)w && info.dwSize.Y==(int)h)
   {
    DBPr3("Already using %d,%d size\n",w,h);
    return 0;
   }
 // Find the max. size, depends on the font and screen size.
 COORD max=GetLargestConsoleWindowSize(hOut);
 COORD newSize={w,h};
 if (newSize.X>max.X) newSize.X=max.X;
 if (newSize.Y>max.Y) newSize.Y=max.Y;
 // The buffer must be large enough to hold both modes (current and new)
 COORD newBufSize=newSize;
 if (info.dwMaximumWindowSize.X>newBufSize.X)
    newBufSize.X=info.dwMaximumWindowSize.X;
 if (info.dwMaximumWindowSize.Y>newBufSize.Y)
    newBufSize.Y=info.dwMaximumWindowSize.Y;
 DBPr3("Enlarging buffer from %d,%d to",info.dwMaximumWindowSize.X,
       info.dwMaximumWindowSize.Y);
 DBPr3(" %d,%d\n",newBufSize.X,newBufSize.Y);
 // Enlarge the buffer size. It fails if not windowed.
 if (!SetConsoleScreenBufferSize(hOut,newBufSize))
   {
    DBPr1("SetConsoleScreenBufferSize failed!\n");
    return 0;
   }
 // Resize the window.
 SMALL_RECT r={0,0,newSize.X-1,newSize.Y-1};
 DBPr3("Resizing window to %d,%d\n",newSize.X,newSize.Y);
 if (!SetConsoleWindowInfo(hOut,TRUE,&r))
   {// Revert buffer size
    DBPr1("SetConsoleWindowInfo failed!\n");
    newSize.X=info.dwMaximumWindowSize.X;
    newSize.Y=info.dwMaximumWindowSize.Y;
    SetConsoleScreenBufferSize(hOut,newSize);
    return 0;
   }
 // Now we can shrink the buffer to the needed size
 SetConsoleScreenBufferSize(hOut,newSize);
 // Ok! we did it.
 return fW!=-1 || fH!=-1 || newSize.X!=(int)w || newSize.Y!=(int)h ? 2 : 1;
}

void TScreenWin32::CheckSizeBuffer(int oldWidth, int oldHeight)
{
 // allocating a zeroed screenBuffer, because this function
 // is called in most cases (in RHIDE) after a SIGWINCH
 if (screenWidth!=oldWidth || screenHeight!=oldHeight || !screenBuffer)
   {
    // Realloc screen buffer only if actually needed (it doesn't exist
    // or screen size is changed)
    if (screenBuffer)
       DeleteArray(screenBuffer);
    screenBuffer=new ushort[screenWidth*screenHeight];
   }
 memset(screenBuffer,0,screenWidth*screenHeight*sizeof(ushort));
}

void TScreenWin32::SetVideoMode(ushort mode)
{
 int oldWidth=screenWidth;
 int oldHeight=screenHeight;

 defaultSetVideoMode(mode);
 CheckSizeBuffer(oldWidth,oldHeight);
}

void TScreenWin32::SetVideoModeExt(char *mode)
{
 int oldWidth=screenWidth;
 int oldHeight=screenHeight;

 defaultSetVideoModeExt(mode);
 CheckSizeBuffer(oldWidth,oldHeight);
}

int TScreenWin32::SetVideoModeRes(unsigned w, unsigned h, int fW, int fH)
{// Set the screen mode
 int ret=setCrtModeRes(w,h,fW,fH);
 if (ret)
   {// Memorize new values:
    // Cache the values for TDisplay
    GetConsoleScreenBufferInfo(hOut,&ConsoleInfo);
    screenWidth =ConsoleInfo.dwSize.X;
    screenHeight=ConsoleInfo.dwSize.Y;
    DeleteArray(screenBuffer);
    screenBuffer=new ushort[screenHeight*screenWidth];
    ZeroMemory(screenBuffer,screenHeight*screenWidth*sizeof(ushort));
    // This is something silly TV code spects: after a video mode change the
    // cursor should go to the "default" state.
    setCursorType(cursorLines);
    // Cache the data about it and initialize related stuff
    setCrtData();
   }
 return ret;
}

// Anatoli's idea
BOOL WINAPI TScreenWin32::ConsoleEventHandler(DWORD dwCtrlType)
{
 if (dwCtrlType==CTRL_C_EVENT || dwCtrlType==CTRL_BREAK_EVENT)
    return TRUE;
 return FALSE;
}

#else

#include <tv/win32/screen.h>
#include <tv/win32/mouse.h>
#include <tv/win32/key.h>

#endif // TVOS_Win32

