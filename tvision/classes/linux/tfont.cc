/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* Copyright (C) 1997-2000 Salvador Eduardo Tropea */
#define Uses_TScreen
#include <tv.h>

int TFont::UseDefaultFontsNextTime=0;

// SET: That's a dummy because I don't have an easy way to change fonts in
// Linux
int TFont::SelectFont(int , int, int, int, int )
{
 return 1;
}


