/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#ifdef __linux__

#define Uses_TDisplay
#define Uses_TScreen
#include <tv.h>

static ushort Equipment;
static uchar CrtInfo;
static uchar CrtRows;

#include <stdlib.h>
#include <string.h>
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
extern int vcs_fd;
extern int tty_fd;

void TDisplay::SetCursor(int x,int y)
{
  if (vcs_fd >= 0)	/* use vcs */
  {
    unsigned char where[2] = {x, y};

    lseek(vcs_fd, 2, SEEK_SET);
    write(vcs_fd, where, sizeof(where));
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
  if (vcs_fd >= 0)	/* use vcs */
  {
    unsigned char where[2];

    lseek(vcs_fd, 2, SEEK_SET);
    read(vcs_fd, where, sizeof(where));
    x = where[0];
    y = where[1];
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

#endif // __linux__
