/**[txh]********************************************************************

  Description:
  getline.c -- Replacement for GNU C library function getline.
  
***************************************************************************/
#include <cl/needs.h>

#ifdef NEEDS_GETLINE
/* getline.c -- Replacement for GNU C library function getline

Copyright (C) 1993 Free Software Foundation, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the
License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.  */

/* Written by Jan Brittenson, bson@gnu.ai.mit.edu.  */
/* Small modifications by Salvador E. Tropea <set@ieee.org> */

#define NDEBUG

/* Already included */
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
/*#include "getline.h"*/
/** Header **/
#define GETLINE_NO_LIMIT -1

#if defined(TVCompf_MinGW)
 #if defined(_SSIZE_T_) && !defined(_NO_OLDNAMES)
  #define MinGW_SSIZE_T 0
 #else
  #define MinGW_SSIZE_T 1
 #endif
#endif

#if defined(TVComp_BCPP) || MinGW_SSIZE_T || defined(TVComp_MSC) || defined(TVComp_Watcom)
typedef long ssize_t;
#endif

ssize_t CLY_getline(char **_lineptr, size_t *_n, FILE *_stream);
ssize_t CLY_getline_safe(char **_lineptr, size_t *_n, FILE *_stream, int limit);
ssize_t CLY_getstr(char **_lineptr, size_t *_n, FILE *_stream,
                   char _terminator, int _offset, int limit);
/** End of Header **/

/* Always add at least this many bytes when extending the buffer.  */
#define MIN_CHUNK 64

/* Read up to (and including) a TERMINATOR from STREAM into *LINEPTR
   + OFFSET (and null-terminate it).  If LIMIT is non-negative, then
   read no more than LIMIT chars.

   *LINEPTR is a pointer returned from malloc (or NULL), pointing to
   *N characters of space.  It is realloc'd as necessary.  

   Return the number of characters read (not including the null
   terminator), or -1 on error or EOF.  On a -1 return, the caller
   should check feof(), if not then errno has been set to indicate the
   error.  */

ssize_t CLY_getstr(char **lineptr, size_t *n, FILE *stream, char terminator,
                   int offset, int limit)
{
 int nchars_avail;             /* Allocated but unused chars in *LINEPTR.  */
 char *read_pos;               /* Where we're reading into *LINEPTR. */
 ssize_t ret;

 if (!lineptr || !n || !stream)
   {
    errno=EINVAL;
    return -1;
   }

 if (!*lineptr)
   {
    *n=MIN_CHUNK;
    *lineptr=(char *)malloc(*n);
    if (!*lineptr)
      {
       errno=ENOMEM;
       return -1;
      }
   }

 nchars_avail=*n-offset;
 read_pos=*lineptr+offset;

 for (;;)
   {
    int save_errno;
    register int c;

    if (limit==0)
       break;
    else
      {
       c=getc(stream);
       /* If limit is negative, then we shouldn't pay attention to
          it, so decrement only if positive. */
       if (limit>0)
          limit--;
      }
    save_errno=errno;

    /* We always want at least one char left in the buffer, since we
       always (unless we get an error while reading the first char)
       NUL-terminate the line buffer.  */

    assert((*lineptr+*n)==(read_pos+nchars_avail));
    if (nchars_avail<2)
      {
       if (*n>MIN_CHUNK)
          *n*=2;
       else
          *n+=MIN_CHUNK;

       nchars_avail=*n+*lineptr-read_pos;
       *lineptr=(char *)realloc(*lineptr,*n);
       if (!*lineptr)
         {
          errno=ENOMEM;
          return -1;
         }
       read_pos=*n-nchars_avail+*lineptr;
       assert((*lineptr+*n)==(read_pos+nchars_avail));
      }

    if (ferror(stream))
      {
       /* Might like to return partial line, but there is no
          place for us to store errno.  And we don't want to just
          lose errno.  */
       errno = save_errno;
       return -1;
      }

    if (c==EOF)
      {
       /* Return partial line, if any.  */
       if (read_pos==*lineptr)
          return -1;
       else
          break;
      }

    /* SET: Why in the hell a low level function should do it? */
    #if 0 /*_WIN32*/
    /* Text mode translation, CR/LF pairs into LF */
    if (terminator=='\n' && c=='\r')
       continue;
    #endif

    *read_pos++=c;
    nchars_avail--;

    if (c==terminator)
       /* Return the line. */
       break;
   }

 /* Done - NUL terminate and return the number of chars read.  */
 *read_pos='\0';

 ret=read_pos-(*lineptr+offset);
 return ret;
}

ssize_t CLY_getline(char **lineptr, size_t *n, FILE *stream)
{
 return CLY_getstr(lineptr,n,stream,'\n',0,GETLINE_NO_LIMIT);
}

ssize_t CLY_getline_safe(char **lineptr, size_t *n, FILE *stream, int limit)
{
 return CLY_getstr(lineptr,n,stream,'\n',0,limit);
}
#endif // NEEDS_GETLINE

