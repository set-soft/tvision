/* Copyright (C) 1995 DJ Delorie, see COPYING.DJ for details */
#include <cl/needs.h>
#ifdef NEEDS_STRLWR
#include <ctype.h>

char *strlwr(char *_s)
{
 char *rv = _s;
 while (*_s)
   {
     *_s = tolower((unsigned char)*_s);
     _s++;
   }
 return rv;
}
#endif
