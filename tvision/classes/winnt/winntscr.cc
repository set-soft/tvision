/**[txh]********************************************************************

  Copyright (c) 2002-2005 by Salvador E. Tropea (SET)
  Based on code contributed by Anatoli Soltan.

  Description:
  WinNT Screen routines.
  The original implementation was done by Anatoli, I removed some code,
added some routines and adapted it to the new architecture.

  ToDo: Set UseScreenSaver when we are in full screen.

  Configuration variables:
  ScreenWidth
  ScreenHeight
  AppCP
  ScrCP
  InpCP

  Notes:
  1) I saw a problem in W98SE, it looks like a bug in Windows: If I
suspend to a shell and the resume doing window size changes at exit the
screen seems to be partially restored. But if you force windows to redraw
it (minimize/maximize for example) things gets right. This is problem only
affects the cursor when using USE_NEW_BUFFER.
  2) The USE_NEW_BUFFER mode is something I (SET) found in the Win32 API
docs that's much cleaver than saving/restoring the screen contents. When
defined I just create a new screen buffer and use it. To restore the screen
you just need to set the original STDOUT handle as the active. It makes the
code easier and exposes less Windows problems (bugs?). The only bizarre
thing I observe is "invisible cursor", but this is just Windows forgets to
update, as soon as the window needs a redraw the cursor gets visible again.
  3) Anatoli left commented:
 hIn=CreateFile("CONIN$", GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
 hOut=CreateFile("CONOUT$", GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
I think they aren't useful but I left it here in case I need this example.

***************************************************************************/
#include <tv/configtv.h>

#define Uses_stdlib
#define Uses_TScreen
#define Uses_TEvent
#define Uses_TGKey
#define Uses_TVCodePage
#define Uses_unistd
#include <tv.h>
#include <tv/win32/win32clip.h>

// I delay the check to generate as much dependencies as possible
#ifdef TVOS_Win32

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef YieldProcessor

#include <tv/winnt/screen.h>
#include <tv/winnt/mouse.h>
#include <tv/winnt/key.h>

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

#define TV_CONSOLE_MODE (ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT)

#ifndef USE_NEW_BUFFER
ushort  *TScreenWinNT::saveScreenBuf;
unsigned TScreenWinNT::saveScreenSize;
unsigned TScreenWinNT::saveScreenCursorStart,
         TScreenWinNT::saveScreenCursorEnd;
unsigned TScreenWinNT::saveScreenCursorX,
         TScreenWinNT::saveScreenCursorY;
#else
 #define SaveScreenReleaseMemory()
#endif
DWORD    TScreenWinNT::saveScreenConsoleMode;
unsigned TScreenWinNT::saveScreenWidth,
         TScreenWinNT::saveScreenHeight;

// Buffer used to arrange the data as needed by Win32 API
CHAR      *TScreenWinNT::outBuf;
WORD      *TScreenWinNT::outBufAttr;
CHAR_INFO *TScreenWinNT::outBufCI;
unsigned   TScreenWinNT::outBufCapacity;

void TScreenWinNT::ensureOutBufCapacity(unsigned count)
{
 count=(count+1) & 0xFFFFFFFE;
 if (outBufCapacity<count)
   {
    free(outBufCI);
    outBufCI=(CHAR_INFO*)malloc(count*sizeof(CHAR_INFO));
    outBuf=(CHAR*)outBufCI;
    outBufAttr=(WORD*)(outBuf+count);
    outBufCapacity=count;
   }
}

int TScreenWinNT::InitOnce()
{
 DWORD flags;
 // Check if we are running in a console
 if (!GetConsoleMode(GetStdHandle(STD_INPUT_HANDLE),&flags))
    return 0;
 // SET: On Win32 this value is symbolic, just a number that can't be a
 // malloced pointer, the screenBuffer isn't used to access the screen.
 screenBuffer=(ushort *)-1;

 hIn=GetStdHandle(STD_INPUT_HANDLE);
 #ifdef USE_NEW_BUFFER
 hStdOut=GetStdHandle(STD_OUTPUT_HANDLE);
 // Create a new buffer, it have their own content and cursor
 hOut=CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE,
                                0,NULL,CONSOLE_TEXTMODE_BUFFER,NULL);
 if (hStdOut==INVALID_HANDLE_VALUE || hOut==INVALID_HANDLE_VALUE)
    return 0; // Something went wrong
 // Make the new one the active
 if (!SetConsoleActiveScreenBuffer(hOut))
    return 0;
 //hCurrentOut=hOut;

 // If we are here this driver will be used
 initialized=1;
 if (dCB) dCB();

 // Console mode
 GetConsoleMode(hIn,&saveScreenConsoleMode);
 SetConsoleCtrlHandler(ConsoleEventHandler,TRUE);
 SetConsoleMode(hIn,TV_CONSOLE_MODE);
 #else
 hOut=GetStdHandle(STD_OUTPUT_HANDLE);
 #endif

 TDisplayWinNT::Init();

 setCharacter=SetCharacter;
 setCharacters=SetCharacters;
 getCharacter=GetCharacter;
 getCharacters=GetCharacters;
 TScreen::System_p=System;
 TScreen::Resume=Resume;
 TScreen::Suspend=Suspend;
 setCrtModeRes_p=SetCrtModeRes;

 TVWin32Clipboard::Init();
 TGKeyWinNT::Init();
 THWMouseWinNT::Init();

 // Cache these values
 GetCursorShapeLow(curStart,curEnd);
 GetCursorPosLow(currentCursorX,currentCursorY);

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


TScreenWinNT::TScreenWinNT()
{
 if (!InitOnce()) return;
 flags0=CodePageVar | CursorShapes | CanSetVideoSize;
 startupMode=getCrtMode();
 #ifdef USE_NEW_BUFFER
 // Just remmember the current window size
 saveScreenWidth =GetCols();
 saveScreenHeight=GetRows();
 #else
 Resume();
 #endif

 unsigned maxX=saveScreenWidth, maxY=saveScreenHeight;
 long aux;
 if (optSearch("ScreenWidth",aux))
    maxX=aux;
 if (optSearch("ScreenHeight",aux))
    maxY=aux;
 if (maxX!=saveScreenWidth || maxY!=saveScreenHeight)
   {
    cursorLines=getCursorType();
    setCrtModeRes(maxX,maxY);
   }

 screenMode=getCrtMode();
 setCrtData();

 suspended=0;
}

TScreenWinNT::~TScreenWinNT()
{
 suspend(); // High level suspend to avoid a double call
 SaveScreenReleaseMemory();
 free(outBufCI);
 outBufCI=0;
 outBuf=NULL;
 outBufAttr=0;
 outBufCapacity=0;
}

void TScreenWinNT::Resume()
{
 #ifdef USE_NEW_BUFFER
 GetConsoleMode(hIn,&saveScreenConsoleMode);
 // First switch to our handle
 SetConsoleActiveScreenBuffer(hOut);
 // Now we can save the current window size
 saveScreenWidth =GetCols();
 saveScreenHeight=GetRows();
 //hCurrentOut=hOut; Not useful
 // Restore our window size
 SetCrtModeRes(screenWidth,screenHeight);
 #else
 // Save:
 // Cursor shape
 GetCursorShapeLow(saveScreenCursorStart,saveScreenCursorEnd);
 // Cursor position
 GetCursorPosLow(saveScreenCursorX,saveScreenCursorY);
 // Console mode
 GetConsoleMode(hIn,&saveScreenConsoleMode);
 // Screen content
 SaveScreen();

 // Restore:
 // Window size
 SetCrtModeRes(screenWidth,screenHeight);
 // Cursor position
 SetCursorPosLow(currentCursorX,currentCursorY);
 // Cursor shape
 SetCursorShapeLow(curStart,curEnd);
 #endif
 // In case the size changed and we failed to restore it
 setCrtData();
 // Console mode
 SetConsoleCtrlHandler(ConsoleEventHandler,TRUE);
 SetConsoleMode(hIn,TV_CONSOLE_MODE);
}

void TScreenWinNT::Suspend()
{
 #ifdef USE_NEW_BUFFER
 // Restore window size (using our handle!)
 SetCrtModeRes(saveScreenWidth,saveScreenHeight);
 // Switch to the original handle
 SetConsoleActiveScreenBuffer(hStdOut);
 #else
 // Restore:
 // Window size
 SetCrtModeRes(saveScreenWidth,saveScreenHeight);
 // If size changed the content
 RestoreScreen();
 // Cursor shape
 SetCursorShape(saveScreenCursorStart,saveScreenCursorEnd);
 // Cursor position
 SetCursorPosLow(saveScreenCursorX,saveScreenCursorY);
 //FlushConsoleInputBuffer(hIn);
 #endif
 // Console mode
 SetConsoleCtrlHandler(ConsoleEventHandler,FALSE);
 SetConsoleMode(hIn,saveScreenConsoleMode);
}

ushort TScreenWinNT::GetCharacter(unsigned offset)
{
 COORD coord;
 coord.Y=(SHORT)((offset)/screenWidth);
 coord.X=(SHORT)((offset)%screenWidth);
 DWORD cRead;
 ushort ch;

 ReadConsoleOutputAttribute(hOut,&ch,1,coord,&cRead);
 ch<<=8;
 ReadConsoleOutputCharacter(hOut,(CHAR*)&ch,1,coord,&cRead);
 return ch;
}

void TScreenWinNT::GetCharacters(unsigned offset, ushort *buf, unsigned count)
{
 ensureOutBufCapacity(count);
 COORD coord;
 coord.Y=(SHORT)((offset)/screenWidth);
 coord.X=(SHORT)((offset)%screenWidth);
 DWORD cRead;

 ReadConsoleOutputAttribute(hOut,outBufAttr,count,coord,&cRead);
 ReadConsoleOutputCharacter(hOut,outBuf,count,coord,&cRead);
 for (count=0; count<cRead; count++, buf++)
     *buf=(ushort)(outBufAttr[count]<<8) | (ushort)(uchar)outBuf[count];
}

void TScreenWinNT::SetCharacter(unsigned offset, unsigned int value)
{
 ensureOutBufCapacity(1);
 COORD coord;
 coord.Y=(SHORT)((offset)/screenWidth);
 coord.X=(SHORT)((offset)%screenWidth);

 outBufCI[0].Char.AsciiChar=(CHAR)(value & 0xFF);;
 outBufCI[0].Attributes=(WORD)(value>>8);
   
 COORD dwBufferSize={1,1};
 COORD dwBufferCoord={0,0};
 SMALL_RECT rcWriteRegion={coord.X, coord.Y, coord.X, coord.Y};
 WriteConsoleOutput(hOut,outBufCI,dwBufferSize,dwBufferCoord,&rcWriteRegion);
   
 //FillConsoleOutputCharacter(hOut,(CHAR)(value & 0xFF),1,coord,&cWritten);
 //FillConsoleOutputAttribute(hOut,(WORD)(value >> 8),1,coord,&cWritten);
}

void TScreenWinNT::SetCharacters(unsigned offset, ushort *values, unsigned count)
{
 ensureOutBufCapacity(count);
 COORD coord;
 coord.Y=(SHORT)((offset)/screenWidth);
 coord.X=(SHORT)((offset)%screenWidth);
 unsigned i;

 for (i=0; i<count; i++, values++)
    {
     outBufCI[i].Char.AsciiChar=(CHAR)(*values & 0xFF);
     outBufCI[i].Attributes=(WORD)(*values>>8);
    }
  
 COORD dwBufferSize={count,1};
 COORD dwBufferCoord={0, 0};
 SMALL_RECT rcWriteRegion={coord.X, coord.Y, coord.X+count-1, coord.Y};
 WriteConsoleOutput(hOut,outBufCI,dwBufferSize,dwBufferCoord,&rcWriteRegion);
}

#ifndef USE_NEW_BUFFER
void TScreenWinNT::SaveScreen()
{
 unsigned rows=GetRows();
 unsigned cols=GetCols();
 
 saveScreenSize=rows*cols;
 free(saveScreenBuf);
 saveScreenBuf=(ushort *)malloc(saveScreenSize*sizeof(ushort));

 // Temporarily set these variables to let getCharacter work properly
 uchar screenWidthSave=screenWidth;
 uchar screenHeightSave=screenHeight;
 screenWidth=(uchar)cols;
 screenHeight=(uchar)rows;
 unsigned row, ofs;
 for (row=0, ofs=0; row<rows; row++, ofs+=cols)
     GetCharacters(ofs,saveScreenBuf+ofs,cols);
 screenWidth=screenWidthSave;
 screenHeight=screenHeightSave;
 saveScreenWidth=(uchar)cols;
 saveScreenHeight=(uchar)rows;

 /*FILE *f=fopen("screen.dat","wb");
 fwrite(saveScreenBuf,saveScreenSize,sizeof(ushort),f);
 fclose(f);*/
}

void TScreenWinNT::RestoreScreen()
{
 // Needed to make SetCharacters work OK
 unsigned actualW=screenWidth, actualH=screenHeight;
 screenWidth=saveScreenWidth;  screenHeight=saveScreenHeight;

 unsigned rows=saveScreenHeight, row, ofs;
 unsigned cols=saveScreenWidth;
 
 for (row=0, ofs=0; row<rows; row++, ofs+=cols)
     SetCharacters(ofs,saveScreenBuf+ofs,cols);

 screenWidth=actualW; screenHeight=actualH;
}

void TScreenWinNT::SaveScreenReleaseMemory()
{
 free(saveScreenBuf);
 saveScreenBuf=NULL;
 saveScreenSize=0;
}
#endif

void TScreenWinNT::YieldProcessor(int micros)
{
 DWORD msecs=micros<0 ? INFINITE : micros/1000;
 WaitForSingleObject(TScreenWinNT::hIn,msecs);
}

// May be I should move all the yield processor routines to TScreen
extern "C" void __tvWin32Yield(int micros)
{
 TScreenWinNT::YieldProcessor(micros);
}

BOOL WINAPI TScreenWinNT::ConsoleEventHandler(DWORD dwCtrlType)
{
 if (dwCtrlType==CTRL_C_EVENT || dwCtrlType==CTRL_BREAK_EVENT)
    return TRUE;
 return FALSE;
}

int TScreenWinNT::System(const char *command, pid_t *pidChild, int in, int out,
                         int err)
{
 // If the caller asks for redirection replace the requested handles
 if (in!=-1)
    dup2(in,STDIN_FILENO);
 if (out!=-1)
    dup2(out,STDOUT_FILENO);
 if (err!=-1)
    dup2(err,STDERR_FILENO);

 int rc=system(command);
 SetConsoleMode(hIn, TV_CONSOLE_MODE);
 // fork mechanism not implemented, indicate the child finished
 if (pidChild)
    *pidChild=0;
 return rc;
}

TScreen *TV_WinNTDriverCheck()
{
 TScreenWinNT *drv=new TScreenWinNT();
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

int TScreenWinNT::SetCrtModeRes(unsigned w, unsigned h, int fW, int fH)
{
 CONSOLE_SCREEN_BUFFER_INFO info;
 // Find current size
 if (!GetConsoleScreenBufferInfo(hCurrentOut,&info)) return 0;
 // Is the same used?
 if (info.dwSize.X==(int)w && info.dwSize.Y==(int)h)
   {
    DBPr3("Already using %d,%d size\n",w,h);
    return 0;
   }
 // Find the max. size, depends on the font and screen size.
 COORD max=GetLargestConsoleWindowSize(hCurrentOut);
 COORD newSize={w,h};
 if (newSize.X>max.X) newSize.X=max.X;
 if (newSize.Y>max.Y) newSize.Y=max.Y;
 // The buffer must be large enough to hold both modes (current and new)
 COORD newBufSize=newSize;
 if (info.dwMaximumWindowSize.X>newBufSize.X)
    newBufSize.X=info.dwMaximumWindowSize.X;
 if (info.dwMaximumWindowSize.Y>newBufSize.Y)
    newBufSize.Y=info.dwMaximumWindowSize.Y;
 // Enlarge the buffer size. It fails if not windowed.
 if (!SetConsoleScreenBufferSize(hCurrentOut,newBufSize)) return 0;
 // Resize the window.
 SMALL_RECT r={0,0,newSize.X-1,newSize.Y-1};
 if (!SetConsoleWindowInfo(hCurrentOut,TRUE,&r))
   {// Revert buffer size
    newSize.X=info.dwMaximumWindowSize.X;
    newSize.Y=info.dwMaximumWindowSize.Y;
    SetConsoleScreenBufferSize(hCurrentOut,newSize);
    return 0;
   }
 // Now we can shrink the buffer to the needed size
 SetConsoleScreenBufferSize(hCurrentOut,newSize);
 // This is something silly TV code spects: after a video mode change the
 // cursor should go to the "default" state.
 setCursorType(cursorLines);
 // Ok! we did it.
 return fW!=-1 || fH!=-1 || newSize.X!=(int)w || newSize.Y!=(int)h ? 2 : 1;
}

#else

#include <tv/winnt/screen.h>
#include <tv/winnt/mouse.h>
#include <tv/winnt/key.h>

#endif // TVOSf_WIN32
/*
Win32 API reference names 45 code pages. Only 20 of them are supported.

Code page identifiers:

*037	EBCDIC
 437	MS-DOS  United States          *
*500	EBCDIC "500V1"
*708	Arabic (ASMO 708)
*709	Arabic (ASMO 449+, BCON V4)
*710	Arabic (Transparent Arabic)
*720	Arabic (Transparent ASMO)
 737	Greek (formerly 437G)          *
 775	Baltic                         *
 850	MS-DOS  Multilingual (Latin I) *
 852	MS-DOS  Slavic (Latin II)      *
 855	IBM Cyrillic (primarily Russian*
 857	IBM Turkish                    *
 860	MS-DOS  Portuguese
 861	MS-DOS Icelandic
*862	Hebrew
 863	MS-DOS Canadian-French
*864	Arabic
 865	MS-DOS Nordic
 866	MS-DOS Russian                 *
 869	IBM Modern Greek               *
*874	Thai
*875	EBCDIC
*932	Japan
*936	Chinese (PRC, Singapore)
*949	Korean
*950	Chinese (Taiwan, Hong Kong)
*1026	EBCDIC
*1200	Unicode (BMP of ISO 10646)
 1250	Windows 3.1 Eastern European   *
 1251	Windows 3.1 Cyrillic           *
 1252	Windows 3.1 US (ANSI)          *
 1253	Windows 3.1 Greek              *
 1254	Windows 3.1 Turkish            *
*1255	Hebrew
*1256	Arabic
 1257	Baltic                         *
*1361	Korean (Johab)
*10000	Macintosh Roman
*10001	Macintosh Japanese
*10006	Macintosh Greek I
 10007	Macintosh Cyrillic
*10029	Macintosh Latin 2
*10079	Macintosh Icelandic
*10081	Macintosh Turkish
-----------------
My registry also says:

20866
28591
28592
28595
28597

As a Windows 9x is shipped working with only one code page I can't test what
a hell are these code pages. They seems to be some variant of 866 and some
kind of ISO cp, just a guess.
*/

