#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdarg.h>
#include <termios.h>
#include <string.h>
#include <stdlib.h>

#define T1 1

int hOut, hIn;
FILE *fIn;
int oldInFlags, newInFlags;
termios outTermiosOrig;
termios outTermiosNew;
termios inTermiosOrig;
termios inTermiosNew;

int Init()
{
 hOut=fileno(stdout);
 // SET: Original code tried to open a tty at any cost, eg: if stdout was
 // redirected it tried to open stdin's tty for writing. I don't see the
 // point of such an effort and in fact crashes the library, so here I just
 // explain to the user how to run the application in the right way.
 if (!isatty(hOut))
   {
    printf("that's an interactive application, don't redirect stdout. "
           "If you want to collect information redirect stderr like this: "
           "program 2> file\n");
    return 1;
   }

 if (tcgetattr(hOut,&outTermiosOrig))
   {
    printf("can't get output terminal information\n");
    return 2;
   }

 // Save cursor position, attributes and charset
 fputs("\E7",stdout);
 memcpy(&outTermiosNew,&outTermiosOrig,sizeof(outTermiosNew));
 outTermiosNew.c_oflag|=OPOST;
 if (tcsetattr(hOut,TCSAFLUSH,&outTermiosNew))
   {
    printf("can't configure terminal mode\n");
    return 3;
   }

 // Impossible to know the actual palette, just assume a default
 //memcpy(OriginalPalette,PC_BIOSPalette,sizeof(OriginalPalette));
 //memcpy(ActualPalette,PC_BIOSPalette,sizeof(ActualPalette));

 hIn=fileno(stdin);

 if (!isatty(hIn))
   {
    printf("that's an interactive application, don't redirect stdin\n");
    return 1;
   }

 char *ttyName=ttyname(hIn);
 if (!ttyName)
   {
    printf("failed to get the name of the current terminal used for input\n");
    return 3;
   }
 fIn=fopen(ttyName,"r+b");
 if (!fIn)
   {
    printf("failed to open the input terminal\n");
    return 4;
   }
 hIn=fileno(fIn);

 if (tcgetattr(hIn,&inTermiosOrig))
   {
    printf("can't get input terminal attributes\n");
    return 2;
   }

 memcpy(&inTermiosNew,&inTermiosOrig,sizeof(inTermiosNew));
 // Ignore breaks
 inTermiosNew.c_iflag|= (IGNBRK | BRKINT);
 // Disable Xon/off
 inTermiosNew.c_iflag&= ~(IXOFF | IXON);
 // Character oriented, no echo, no signals
 inTermiosNew.c_lflag&= ~(ICANON | ECHO /*| ISIG*/);
 // The following are needed for Solaris. In 2.7 MIN is around 4 and TIME 0
 // making things really annoying. In the future they could be driver
 // variables to make the use of bandwidth smaller. A value of 4 and 1 looks
 // usable.
 if (T1)
   {
    printf("c_cc[VMIN]=%d\nc_cc[VTIME]=%d\n",inTermiosNew.c_cc[VMIN],inTermiosNew.c_cc[VTIME]);
    inTermiosNew.c_cc[VMIN]=1;
    inTermiosNew.c_cc[VTIME]=0;
   }
 if (tcsetattr(hIn,TCSAFLUSH,&inTermiosNew))
   {
    printf("can't set input terminal attributes\n");
    return 3;
   }
 // Don't block
 oldInFlags=fcntl(hIn,F_GETFL,0);
 newInFlags=oldInFlags | O_NONBLOCK;
 fcntl(hIn,F_SETFL,newInFlags);
 
 return 0;
}

int DeInit()
{
 // Restore cursor position, attributes and charset
 fputs("\E8",stdout);
 // Ensure all is processed before leaving
 fflush(stdout);
 // Restore console mode, I think drain is better here, but the previous flush
 // is needed anyways.
 tcsetattr(hOut,TCSADRAIN,&outTermiosOrig);
 
 fcntl(hIn,F_SETFL,oldInFlags);
 tcsetattr(hIn,TCSAFLUSH,&inTermiosOrig);

 return 0;
}

void SetCursorPos(unsigned x, unsigned y)
{
 fprintf(stdout,"\E[%d;%dH",y+1,x+1);
}

const int PAL_HIGH=1, PAL_LOW=0;
int palette=PAL_HIGH;
int oldCol=-1,
    oldBack=-1,
    oldFore=-1;
int repeat;

void mapColor(int col)
{
 static char map[]={0, 4, 2, 6, 1, 5, 3, 7, 0, 4, 2, 6, 1, 5, 3, 7};
 int back,fore;

 if (col==oldCol)
    return;
 oldCol=col;
 back=(col >> 4) & 15;
 fore=col & 15;

 if (palette==PAL_LOW)
   {// Just 8 colors, but use bold and blink getting 16+16
    if (fore!=oldFore && back!=oldBack)
       fprintf(stdout,"\E[%d;%d;%d;%dm",
               fore>7 ? 1 : 22,30+map[fore],
               back>7 ? 5 : 25,40+map[back]);
    else
     {
      if (fore!=oldFore)
         fprintf(stdout,"\E[%d;%dm",fore>7 ? 1 : 22,30+map[fore]);
      else
         fprintf(stdout,"\E[%d;%dm",back>7 ? 5 : 25,40+map[back]);
     }
   }
 else
   { // 16 fore and 16 background colors, yuuuhuuu! ;-)
    if (fore!=oldFore && back!=oldBack)
       fprintf(stdout,"\E[%d;%dm",map[fore]+(fore>7 ?  90 : 30),
                                  map[back]+(back>7 ? 100 : 40));
    else
      {
       if (fore!=oldFore)
          fprintf(stdout,"\E[%dm",map[fore]+(fore>7 ?  90 : 30));
       else
          fprintf(stdout,"\E[%dm",map[back]+(back>7 ? 100 : 40));
      }
   }

 oldFore = fore;
 oldBack = back;
}


int Test()
{
 if (0)
   {
    int c, d=0;
    do
      {
       c=fgetc(fIn);
       if (c==-1)
          d++;
       else
          printf("%d\n",c);
      }
    while (c!='Q');
    printf("Returned -1 %d times\n",d);
   }
 else
   {
    /*int i,j;
    for (j=0; j<20; j++)
    for (i=0; i<20; i++)
        fprintf(stdout,"\E[%d;%dH\E[5;%d;%d;%dm123456789*123456789*123456789*12345789*123456789*123456789*12345789*\n",
                i,0,30+i,40+i+1);*/

    unsigned i,j,k;
    for (k=0; k<repeat; k++)
       {
        SetCursorPos(0,0);
        for (j=0; j<13; j++)
           {
            for (i=0; i<16; i++)
               {
                mapColor(i+(15-i)*16);
                fprintf(stdout," Hello! ");
               }
           }
       }
   }
 return 0;
}

int main(int argc, char *argv[])
{
 if (argc!=2)
   {
    printf("Usage: %s number\n",argv[0]);
    return 2;
   }
 repeat=atoi(argv[1]);
 if (!repeat || repeat>100)
   {
    printf("The number must be between 1 and 100\n");
    return 3;
   }
 if (Init())
   return 1;
 Test();
 DeInit();
 return 0;
}

