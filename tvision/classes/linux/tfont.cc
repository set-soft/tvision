/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
/* Copyright (C) 1997 Salvador Eduardo Tropea */

#ifdef __linux__

#define Uses_TScreen
#include <tv.h>

int TFont::UseDefaultFontsNextTime=0;

int TFont::SelectFont(int , int, int, int, int )
{
 return 1;
}

#endif


