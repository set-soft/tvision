/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *

Modified by Robert H”hne to be used for RHIDE.

 *
 *
 */

#if !defined( __UTIL_H )
#define __UTIL_H
#undef __CM
#undef __HC
#undef __BASE
#define __BASE 128
#define __CM(x,y) const unsigned short cm##x = __BASE+y;
#define __HC(x,y) const unsigned short hc##x = __BASE+y;

inline int min( int a, int b )
{
    return (a>b) ? b : a;
}

inline int max( int a, int b )
{
    return (a<b) ? b : a;
}

void fexpand( char * );

char hotKey( const char *s );
unsigned short ctrlToArrow( unsigned short );

unsigned short historyCount( unsigned char id );
const char *historyStr( unsigned char id, int index );
void historyAdd( unsigned char id, const char * );

int cstrlen( const char * );

class TView;
void *message( TView *receiver, unsigned short what, unsigned short command,
               void *infoPtr );
Boolean lowMemory();

char *newStr( const char * );

Boolean driveValid( char drive );

Boolean isDir( const char *str );

Boolean pathValid( const char *path );

Boolean validFileName( const char *fileName );

void getCurDir( char *dir );

#ifndef __DJGPP__ /* some usefull functions from DJGPP */
void _fixpath(const char *in,char *out);
int __file_exists(const char *fname);
#endif

Boolean isWild( const char *f );

/* Returns True, if the path is not absolute (checks only for / or ([a-zA-Z]:/)) */
Boolean relativePath(const char *); // tfiledia.cc

/* extracts from path the directory part and filename part.
   if 'dir' and/or 'file' == NULL, it is not filled.
   The directory will have a trailing slash */
void expandPath(const char *path, char *dir, char *file);

/* SET: Added a conditional for it because n is too common. I remmember I had
   problems with it and Jerzy Witkowski <jwi@pl.ibm.com> complained about
   conflicts between a class member in your code and this macro */
#ifdef Uses_n
/* The following macro is a little hack to decrease the compile time
   for the TV lib when compiling all the n*.cc files */

#define n(CLASS)                          \
class CLASS                               \
{                                         \
public:                                   \
  static const char * const name;         \
};                                        \
                                          \
const char * const CLASS::name = #CLASS;
#endif

/* return the number of ticks (on MSDOS 1 tick is 1/18 sec),
   this is used to compute the double click */
ushort TICKS(void);

/* The following macros are defined to avoid passing negative values to
   the ctype functions in the common case: isxxxx(char).
   They are suitable for x86 DOS and Linux, perhaps not for others */
#define ucisalnum(a)  isalnum((unsigned char)a)
#define ucisalpha(a)  isalpha((unsigned char)a)
#define ucisascii(a)  isascii((unsigned char)a)
#define uciscntrl(a)  iscntrl((unsigned char)a)
#define ucisdigit(a)  isdigit((unsigned char)a)
#define ucisgraph(a)  isgraph((unsigned char)a)
#define ucislower(a)  islower((unsigned char)a)
#define ucisprint(a)  isprint((unsigned char)a)
#define ucispunct(a)  ispunct((unsigned char)a)
#define ucisspace(a)  isspace((unsigned char)a)
#define ucisupper(a)  isupper((unsigned char)a)
#define ucisxdigit(a) isxdigit((unsigned char)a)
#define uctoascii(a)  toascii((unsigned char)a)
#define uctolower(a)  tolower((unsigned char)a)
#define uctoupper(a)  toupper((unsigned char)a)

#endif  // __UTIL_H
