/**[txh]********************************************************************

  Copyright (c) 2002 by Salvador E. Tropea (SET)
  Based on code contributed by Anatoli Soltan.

  Description:
  WinNT Screen routines.
  The original implementation was done by Anatoli, I removed some code,
added some routines and adapted it to the new architecture.

  ToDo: The save/restore mechanism have many flaws, I fixed some, but not
  all.
  ToDo: Set UseScreenSaver when we are in full screen.
  
***************************************************************************/
#include <tv/configtv.h>

#ifdef TVOS_Win32

#define Uses_stdlib
#define Uses_TScreen
#define Uses_TEvent
#define Uses_TGKey
#include <tv.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <tv/winnt/screen.h>
#include <tv/winnt/mouse.h>
#include <tv/winnt/key.h>

#define TV_CONSOLE_MODE (ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT)

ushort  *TScreenWinNT::saveScreenBuf;
unsigned TScreenWinNT::saveScreenSize;
unsigned TScreenWinNT::saveScreenCursorStart,
         TScreenWinNT::saveScreenCursorEnd;
unsigned TScreenWinNT::saveScreenWidth,
         TScreenWinNT::saveScreenHeight;
unsigned TScreenWinNT::saveScreenCursorX,
         TScreenWinNT::saveScreenCursorY;
DWORD    TScreenWinNT::saveScreenConsoleMode;

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

void TScreenWinNT::InitOnce()
{
 // SET: On Win32 this value is symbolic, just a number that can't be a
 // malloced pointer, the screenBuffer isn't used to access the screen.
 screenBuffer=(ushort *)-1;

 //hIn=CreateFile("CONIN$", GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
 //hOut=CreateFile("CONOUT$", GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
 hIn=GetStdHandle(STD_INPUT_HANDLE);
 hOut=GetStdHandle(STD_OUTPUT_HANDLE);

 TDisplayWinNT::Init();

 setCharacter=SetCharacter;
 setCharacters=SetCharacters;
 getCharacter=GetCharacter;
 getCharacters=GetCharacters;
 TScreen::System=System;
 TScreen::Resume=Resume;
 TScreen::Suspend=Suspend;

 TGKeyWinNT::Init();
 THWMouseWinNT::Init();

 // Cache these values
 GetCursorShapeLow(curStart,curEnd);
 GetCursorPosLow(currentCursorX,currentCursorY);
}


TScreenWinNT::TScreenWinNT()
{
 InitOnce();
 flags0=CodePageVar | CursorShapes;
 screenMode=startupMode=getCrtMode();
 Resume();

 initialized=1;
}

TScreenWinNT::~TScreenWinNT()
{
 Suspend();
 SaveScreenReleaseMemory();
 free(outBufCI);
 outBufCI=0;
 outBuf=NULL;
 outBufAttr=0;
 outBufCapacity=0;
}

void TScreenWinNT::Resume()
{
 SaveScreen();
 // Set our values
 SetCursorPos(currentCursorX,currentCursorY);
 SetCursorShape(curStart,curEnd);
 // In case resolution changed?
 setCrtData();
  
 SetConsoleCtrlHandler(ConsoleEventHandler,TRUE);
 SetConsoleMode(hIn,TV_CONSOLE_MODE);
}

void TScreenWinNT::Suspend()
{
 SetConsoleCtrlHandler(ConsoleEventHandler,FALSE);
 SetConsoleMode(hIn,saveScreenConsoleMode);
 RestoreScreen();
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

void TScreenWinNT::SetCharacter(unsigned offset, ushort value)
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
 
 GetCursorShapeLow(saveScreenCursorStart,saveScreenCursorEnd);
 saveScreenWidth=(uchar)cols;
 saveScreenHeight=(uchar)rows;
 GetCursorPosLow(saveScreenCursorX,saveScreenCursorY);
 GetConsoleMode(hIn,&saveScreenConsoleMode);
}

void TScreenWinNT::RestoreScreen()
{
 unsigned rows=saveScreenHeight, row, ofs;
 unsigned cols=saveScreenWidth;
 
 for (row=0, ofs=0; row<rows; row++, ofs+=cols)
     SetCharacters(ofs,saveScreenBuf+ofs,cols);

 unsigned x=currentCursorX, y=currentCursorY;
 SetCursorPos(saveScreenCursorX,saveScreenCursorY);
 currentCursorX=x; y=currentCursorY=y;

 unsigned s=curStart, e=curEnd;
 SetCursorShape(saveScreenCursorStart,saveScreenCursorEnd);
 curStart=s; curEnd=e;
}

void TScreenWinNT::SaveScreenReleaseMemory()
{
 free(saveScreenBuf);
 saveScreenBuf=NULL;
 saveScreenSize=0;
}

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

int TScreenWinNT::System(const char *command, pid_t *pidChild)
{
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
#endif // TVOSf_WIN32


