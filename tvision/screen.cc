/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#define Uses_TScreen
#include <tv.h>
#include <stdlib.h>

ushort user_mode;

#define MODE_SET 1 // Use the routines from SET
#define MODE_RH  0 // Use the routines from RH

int screen_saving = MODE_RH;

void rh_SaveVideo(int,int);
void rh_SaveVideoReleaseMemory(void);
void rh_RestoreVideo();
void set_SaveVideo(int,int);
void set_RestoreVideo();

#ifndef __DJGPP__
static ushort *user_buffer=0;
static int user_buffer_size;
static int user_cursor_x,user_cursor_y;
extern int vcs_fd;
#endif

void SaveScreen()
{
#ifdef __DJGPP__
  if (screen_saving == MODE_SET)
    set_SaveVideo(TDisplay::getCols(),TDisplay::getRows());
  else
  if (screen_saving == MODE_RH)
    rh_SaveVideo(TDisplay::getCols(),TDisplay::getRows());
#else
  if (vcs_fd >= 0)
  {
    user_buffer_size = TScreen::getCols()*TScreen::getRows();
    user_buffer = (ushort *)realloc(user_buffer,user_buffer_size*sizeof(ushort));
    TScreen::getCharacter(0,user_buffer,user_buffer_size);
    TScreen::GetCursor(user_cursor_x,user_cursor_y);
  }
#endif
}

// SET: Added to release the memory allocated here
void SaveScreenReleaseMemory(void)
{
#ifdef __DJGPP__
  if (screen_saving == MODE_RH)
     rh_SaveVideoReleaseMemory();
#else
  free(user_buffer);
#endif
}

void RestoreScreen()
{
#ifdef __DJGPP__
  if (screen_saving == MODE_SET)
    set_RestoreVideo();
  else
  if (screen_saving == MODE_RH)
    rh_RestoreVideo();
#else
  if (vcs_fd >= 0)
  {
    TScreen::setCharacter(0,user_buffer,user_buffer_size);
    TScreen::SetCursor(user_cursor_x,user_cursor_y);
  }
#endif
}


