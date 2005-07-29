/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *

Modified by Robert H”hne to be used for RHIDE.
Modified by Salvador E. Tropea to avoid macros collisions and make easier to use.

 */

#if !defined( __UTIL_H )
#define __UTIL_H

// SET: Used by names, they only include this file.
#ifdef Uses_n
 #include <compatlayer.h>
#endif

#undef __CM
#undef __HC
#undef __BASE
#define __BASE 128
#define __CM(x,y) const unsigned short cm##x = __BASE+y;
#define __HC(x,y) const unsigned short hc##x = __BASE+y;


#ifndef CLY_DONT_DEFINE_MIN_MAX

#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

inline int min( const int &a, const int &b )
{
    return (a>b) ? b : a;
}

inline int max( const int &a, const int &b )
{
    return (a<b) ? b : a;
}

// The following min/max works for any kind of data type
// Published by Vincent Van Den Berghe in the Windows/DOS developer's journal April 93
// But they don't replace the above functions for cases like:
//   max(int,unsigned int)
//
template <class T> inline T min( const T &a, const T &b )
{
    return (a>b) ? b : a;
}

template <class T> inline T max( const T &a, const T &b )
{
    return (a<b) ? b : a;
}

#endif

char CLY_EXPORT hotKey( const char *s );
unsigned short CLY_EXPORT ctrlToArrow( unsigned short );

unsigned short CLY_EXPORT historyCount( unsigned char id );
const char CLY_EXPORT *historyStr( unsigned char id, int index );
void CLY_EXPORT historyAdd( unsigned char id, const char * );

int cstrlen( const char * );

class TView;
void CLY_EXPORT *message( TView *receiver, unsigned short what, unsigned short command,
                          void *infoPtr );
Boolean CLY_EXPORT lowMemory();

char CLY_EXPORT *newStr( const char * );

/* SET: Added a conditional for it because n is too common. I remmember I had
   problems with it and Jerzy Witkowski <jwi@pl.ibm.com> complained about
   conflicts between a class member in your code and this macro */
#ifdef Uses_n
/* The following macro is a little hack to decrease the compile time
   for the TV lib when compiling all the n*.cc files */

#define n(CLASS)                          \
class CLY_EXPORT CLASS                    \
{                                         \
public:                                   \
  static const char * const name;         \
};                                        \
                                          \
const char * const CLASS::name = #CLASS;
#endif

#endif  // __UTIL_H
