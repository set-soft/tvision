/**[txh]********************************************************************

  Copyright (c) 2002 by Salvador E. Tropea (SET)
  Based on code contributed by Anatoli Soltan.

  Description:
  Win32 Clipboard routines.
  
***************************************************************************/

#include <tv/configtv.h>

#define Uses_string
#define Uses_TVOSClipboard
#include <tv.h>
#include <tv/win32/win32clip.h>

// I delay the check to generate as much dependencies as possible
#ifdef TVOS_Win32

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// Strings for the errors
const char *TVWin32Clipboard::win32NameError[]=
{
 NULL,
 NULL
};

void TVWin32Clipboard::Init()
{
 TVOSClipboard::copy=copy;
 TVOSClipboard::paste=paste;
 TVOSClipboard::destroy=destroy;
 TVOSClipboard::available=1; // We have 1 clipboard
 TVOSClipboard::name="Windows";
 // We get the error from windows, so we just change the pointer
 TVOSClipboard::errors=1;
 TVOSClipboard::nameErrors=win32NameError;
}

void TVWin32Clipboard::getErrorString()
{
 if (win32NameError[1])
    LocalFree((LPVOID)win32NameError[1]);

 LPVOID lpMsgBuf;
 
 FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
               NULL,GetLastError(),
               MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
               (LPTSTR)&lpMsgBuf,0,NULL);
 win32NameError[1]=(const char *)lpMsgBuf;
 TVOSClipboard::error=1;
}

void TVWin32Clipboard::destroy()
{
 if (win32NameError[1])
    LocalFree((LPVOID)win32NameError[1]);
}

char *TVWin32Clipboard::paste(int id, unsigned &len)
{
 if (id!=0) return NULL;

 char* p=NULL;
 HGLOBAL hMem;

 if (OpenClipboard(NULL))
   {
    hMem=GetClipboardData(CF_TEXT);
    if (hMem)
      {
       char *d=(char*)GlobalLock(hMem);
       if (d)
         {
          len=GlobalSize(hMem)-1;
          p=new char[len+1];
          memcpy(p,d,len);
          p[len]=0;
          // The returned length doesn't have to be the exact length of the string.
          // Reported by Warren H. Holt
          len=strlen(p);
          GlobalUnlock(hMem);
         }
       else
          getErrorString();
      }
    else
       getErrorString();
    CloseClipboard();
   }
 else
    getErrorString();

 return p;
}

int TVWin32Clipboard::copy(int id, const char *buffer, unsigned len)
{
 if (id!=0) return 0;
 if (!len || !buffer) return 1;

 HGLOBAL hMem=GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE,len+1);
 if (!hMem)
   {
    getErrorString();
    return 0;
   }

 char *d=(char *)GlobalLock(hMem);
 if (!d)
   {
    getErrorString();
    GlobalFree(hMem);
    return 0;
   }
 memcpy(d,buffer,len);
 d[len]=0;
 GlobalUnlock(hMem);

 if (OpenClipboard(NULL))
   {
    EmptyClipboard();
    if (SetClipboardData(CF_TEXT,hMem))
      {
       CloseClipboard();
       // Windows now owns the memory, we doesn't have to release it.
       return 1;
      }
    getErrorString();
    CloseClipboard();
   }
 else
    getErrorString();
 GlobalFree(hMem);

 return 0;
}

#endif // TVOS_Win32

