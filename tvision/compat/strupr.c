/* Copyright (C) 1995 DJ Delorie, see COPYING.DJ for details */
#include <cl/needs.h>
#ifdef NEEDS_STRUPR
#include <ctype.h>

char *strupr(char *_s)
{
 char *rv = _s;
 while (*_s)
   {
     *_s = toupper((unsigned char)*_s);
     _s++;
   }
 return rv;
}
#endif
