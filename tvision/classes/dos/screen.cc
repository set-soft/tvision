/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
/* Copyright 1998-2002 Salvador E. Tropea */
#include <tv/configtv.h>

#define Uses_stdlib
#define Uses_TScreen
#include <tv.h>

// I delay the check to generate as much dependencies as possible
#ifdef TVCompf_djgpp
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


