/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* Copyright (C) 1999-2000 Salvador Eduardo Tropea */
#define Uses_TDisplay
#define Uses_TScreen
#define Uses_string
#include <tv.h>

static ushort Equipment;
static uchar CrtInfo;
static uchar CrtRows;

#include <stdlib.h>
#include <unistd.h>
#include <term.h>
#include <sys/ioctl.h>

// SET: By now that isn't used but is requested by the inline destructor so
// I added it here (even when I could made the destructor different for
// Linux version).
TFont *TDisplay::font=0;

ushort * TDisplay::equipment = &Equipment;
uchar * TDisplay::crtInfo = &CrtInfo;
uchar * TDisplay::crtRows = &CrtRows;
uchar TDisplay::Page = 0;

void TDisplay::SetPage(uchar)
{
}

inline void safeput(char *&p, char *cap)
{
	if (cap != NULL) while (*cap != '\0') *p++ = *cap++;
}

int cur_x = 0, cur_y = 0;

void TDisplay::SetCursor(int x,int y)
{
  if (canWriteVCS)
  {
    unsigned char where[2] = {x, y};

    lseek(vcsWfd, 2, SEEK_SET);
    write(vcsWfd, where, sizeof(where));
    // SET: cache the value to avoid the need to read it
    cur_x=x; cur_y=y;
  }
  else			/* standard out */
  {
    char out[1024], *p = out;

    safeput(p, tparm(cursor_address,y, x));
    write(tty_fd, out, p - out);
    cur_x = x;
    cur_y = y;
  }
}

void TDisplay::GetCursor(int &x,int &y)
{
  if (canWriteVCS) /* use vcs */
    {
     // SET: We need special priviledges to read /dev/vcsaN, but not for
     // writing so avoid the need of reads.
     if (canReadVCS)
       {
        unsigned char where[2];
    
        lseek(vcsRfd, 2, SEEK_SET);
        read(vcsRfd, where, sizeof(where));
        x = where[0];
        y = where[1];
       }
     else
       {
        x=cur_x; y=cur_y;
       }
    }
  else
  {
    char s[40];
    
    // write/read are better here, because other functions might be buffered
    write(tty_fd,"\e[6n",4); // Request cursor position from terminal
    read(tty_fd,s,sizeof(s)); // Should never overflow...
  
    y = atoi(s+2)-1;
    x = atoi(strchr(s,';')+1)-1;
  }
}

ushort TDisplay::getCursorType()
{
  return 0;
}

void TDisplay::setCursorType( ushort ct)
{
  char out[1024], *p = out;
  if (ct == 0x2000) // hide
  {
    safeput(p, tparm(cursor_invisible));
    write(tty_fd, out, p - out);
  }
  else
  {
    safeput(p, tparm(cursor_normal));
    write(tty_fd, out, p - out);
  }
}

void TDisplay::clearScreen( uchar , uchar )
{
}

void TDisplay::videoInt()
{
}

ushort TDisplay::getRows()
{
  winsize win;
  ioctl(tty_fd,TIOCGWINSZ,&win);
  return win.ws_row;
}

ushort TDisplay::getCols()
{
  winsize win;
  ioctl(tty_fd,TIOCGWINSZ,&win);
  return win.ws_col;
}

ushort TDisplay::getCrtMode()
{
  return smCO80;
}

void TDisplay::setCrtMode( ushort )
{
}

// I'm not sure about it check it Robert
void TDisplay::setCrtMode( char *mode )
{
 system(mode);
}

void TDisplay::updateIntlChars()
{
}

void TDisplay::SetFontHandler(TFont *)
{
}

int TDisplay::CheckForWindowSize(void)
{
 int ret=windowSizeChanged!=0;
 windowSizeChanged=0;
 return ret;
}

char *TDisplay::GetWindowTitle(void)
{
 return 0;
}

int TDisplay::SetWindowTitle(const char *)
{
 return 0;
}

