/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#ifdef __DJGPP__
// SET: Moved the Linux code to tscreen.cc
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

void SaveScreen()
{
  if (screen_saving == MODE_SET)
    set_SaveVideo(TDisplay::getCols(),TDisplay::getRows());
  else
  if (screen_saving == MODE_RH)
    rh_SaveVideo(TDisplay::getCols(),TDisplay::getRows());
}

// SET: Added to release the memory allocated here
void SaveScreenReleaseMemory(void)
{
  if (screen_saving == MODE_RH)
     rh_SaveVideoReleaseMemory();
}

void RestoreScreen()
{
  if (screen_saving == MODE_SET)
    set_RestoreVideo();
  else
  if (screen_saving == MODE_RH)
    rh_RestoreVideo();
}
#endif


