/**[txh]********************************************************************

  Copyright (c) 2003 by Salvador E. Tropea
  This code is part of our Turbo Vision port and is covered by the GPL
license.
  
***************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#ifndef SEPARATED_HANDLE
 #define SEPARATED_HANDLE 0
#endif

struct termios inTermiosOrig;
struct termios inTermiosNew;
struct termios outTermiosOrig;
struct termios outTermiosNew;
int            oldInFlags;
int            newInFlags;
FILE          *fIn;
FILE          *fOut;
int            hIn;
int            hOut;
const char    *error;
int            tioclinuxOK;

int InitIn()
{
 fputs("TGKeyLinux::InitOnce\n",stderr);
 hIn=fileno(stdin);
 
 if (!isatty(hIn))
   {
    error="that's an interactive application, don't redirect stdin";
    return 1;
   }
 //  We can't use stdin for all the operations, instead we must open it again
 // using another file descriptor.
 //  Here is why: In order to get some keys, I saw it for ESC pressed alone,
 // we must set the O_NONBLOCK attribute.
 //  We can do it, but the stdout file handle seems to be just a copy of the
 // stdin file handle (dupped?). When you use duplicated file handles they
 // share the "File Status Flags". It means that setting O_NONBLOCK will
 // set O_NONBLOCK for output too. So what? well when we do that the one that
 // doesn't block is Linux kernel, so if we send too much information to
 // stdout, no matters if we use fflush, sometimes Linux could lose data.
 //  This effect seems to happend at exit, may be because owr process dies
 // before Linux have a chance to process the remaining data and it closes
 // the file handle. The fact is that using O_NONBLOCK sometimes we fail to
 // restore the cursor position.
 //  Now a question remains: do I have to restore the mode?
 char *ttyName=ttyname(hIn);
 if (!ttyName)
   {
    error="failed to get the name of the current terminal used for input";
    return 3;
   }
 fIn=fopen(ttyName,"r+b");
 if (!fIn)
   {
    error="failed to open the input terminal";
    return 4;
   }
 hIn=fileno(fIn);

 if (tcgetattr(hIn,&inTermiosOrig))
   {
    error="can't get input terminal attributes";
    return 2;
   }

 memcpy(&inTermiosNew,&inTermiosOrig,sizeof(inTermiosNew));
 // Ignore breaks
 inTermiosNew.c_iflag|= (IGNBRK | BRKINT);
 // Disable Xon/off
 inTermiosNew.c_iflag&= ~(IXOFF | IXON);
 // Character oriented, no echo, no signals
 inTermiosNew.c_lflag&= ~(ICANON | ECHO | ISIG);
 if (tcsetattr(hIn,TCSAFLUSH,&inTermiosNew))
   {
    error="can't set input terminal attributes";
    return 3;
   }
 // Don't block, needed to get some keys, even when the input is in character
 // oriented mode. I saw it for ESC alone.
 oldInFlags=fcntl(hIn,F_GETFL,0);
 newInFlags=oldInFlags | O_NONBLOCK;
 fcntl(hIn,F_SETFL,newInFlags);

 return 0;
}

int InitOut()
{
 fputs("TScreenLinux::InitOnce\n",stderr);
 hOut=fileno(stdout);
 // SET: Original code tried to open a tty at any cost, eg: if stdout was
 // redirected it tried to open stdin's tty for writing. I don't see the
 // point of such an effort and in fact crashes the library, so here I just
 // explain to the user how to run the application in the right way.
 if (!isatty(hOut))
   {
    error="that's an interactive application, don't redirect stdout. "
           "If you want to collect information redirect stderr like this: "
           "program 2> file";
    return 1;
   }
 if (SEPARATED_HANDLE)
   {
    fputs("Using separated handle\n",stderr);
    /* Testing a reopen */
    char *ttyName=ttyname(hOut);
    if (!ttyName)
      {
       error="failed to get the name of the current terminal used for output";
       return 3;
      }
    fOut=fopen(ttyName,"w+b");
    if (!fOut)
      {
       error="failed to open the output terminal";
       return 4;
      }
    int fgs=fcntl(hOut,F_GETFL,0);
    fcntl(fileno(fOut),F_SETFL,fgs);
   
    if (tcgetattr(hOut,&outTermiosOrig))
      {
       error="can't get output terminal information";
       return 2;
      }
    hOut=fileno(fOut);
    setvbuf(fOut,NULL,_IONBF,0);
   }
 else
   {
    fOut=stdout;
   
    if (tcgetattr(hOut,&outTermiosOrig))
      {
       error="can't get output terminal information";
       return 2;
      }
   }

 // Save cursor position, attributes and charset
 fputs("\E7",fOut);
 memcpy(&outTermiosNew,&outTermiosOrig,sizeof(outTermiosNew));
 outTermiosNew.c_oflag|=OPOST;
 if (tcsetattr(hOut,TCSAFLUSH,&outTermiosNew))
   {
    error="can't configure terminal mode";
    return 3;
   }

 // Test the TIOCLINUX
 int arg=6;
 tioclinuxOK=(ioctl(hOut,TIOCLINUX,&arg)!=-1);
 if (tioclinuxOK)
    fputs("Linux Console IOCTL working\n",stderr);

 return 0;
}


int Init()
{
 // Identify the terminal, if that's linux or console go on.
 char *terminal=getenv("TERM");
 if (!terminal || !(!strcmp(terminal,"console") || !strncmp(terminal,"linux",5)))
   {
    fputs("Not a Linux console\n",stderr);
    return 1;
   }

 // Initialize terminal
 if (InitOut())
   {
    fprintf(stderr,"Error! %s",error);
    fprintf(stderr,"\r\n");
    return 2;
   }
 // Initialize keyboard
 if (InitIn())
   {
    tcsetattr(hOut,TCSAFLUSH,&outTermiosOrig);
    fprintf(stderr,"Error! %s",error);
    fprintf(stderr,"\r\n");
    return 3;
   }
 return 0;
}

void DeInitIn()
{
 tcsetattr(hIn,TCSAFLUSH,&inTermiosOrig);
}

void DeInitOut()
{
 // Is that a Linux bug? Sometime \E8 works, others not.
 fputs("\E8",fOut);
 // Ensure the last command is executed
 fflush(fOut);
 // Restore console mode
 tcsetattr(hOut,TCSAFLUSH,&outTermiosOrig);
}

void DeInit()
{
 DeInitOut();
 DeInitIn();
}

int oldCol=-1, oldFore=-1, oldBack=-1;

void mapColor(int col)
{
 static char map[] = {0, 4, 2, 6, 1, 5, 3, 7};
 int back,fore;

 if (col==oldCol)
    return;
 oldCol=col;
 back=(col >> 4) & 7;
 fore=col & 15;

 #define SB set_a_background ? set_a_background : set_background
 #define SF set_a_foreground ? set_a_foreground : set_foreground

 if (fore!=oldFore && back!=oldBack)
    fprintf(fOut,"\E[%d;%d;%dm",fore>7 ? 1 : 22,30+map[fore & 7],40+map[back]);
 else
   {
    if (fore!=oldFore)
       fprintf(fOut,"\E[%d;%dm",fore>7 ? 1 : 22,30+map[fore & 7]);
    else
       fprintf(fOut,"\E[%dm",40+map[back]);
   }

 oldFore = fore;
 oldBack = back;

 #undef SB
 #undef SF
}

void SetCursorPos(unsigned x, unsigned y)
{
 fprintf(fOut,"\E[%d;%dH",y+1,x+1);
}

int main(int argc, char *argv[])
{
 if (argc!=2)
   {
    printf("Usage: %s number\n",argv[0]);
    return 2;
   }
 int repeat=atoi(argv[1]);
 if (!repeat || repeat>100)
   {
    printf("The number must be between 1 and 100\n");
    return 3;
   }
 if (Init())
    return 1;
 // Exit UTF-8 mode
 fputs("\e%@",fOut);
 // G1 in kernel mode and selected
 fputs("\e)K\xE",fOut);
 unsigned i,j,k;
 for (k=0; k<repeat; k++)
    {
     SetCursorPos(0,0);
     for (j=0; j<13; j++)
        {
         for (i=0; i<16; i++)
            {
             mapColor(i+(15-i)*16);
             fprintf(fOut,"%c Hello! ",133);
            }
        }
    }
 // G1 in normal mode and unselected
 fputs("\e)0\xF",fOut);
 DeInit();
 return 0;
}

