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

#if defined( Uses_TTextDevice ) && !defined( __TTextDevice )
#define __TTextDevice

#include <iostream.h>

class TRect;
class TScrollBar;

class TTextDevice : public streambuf, public TScroller
/* I have changed the order of the basclasses, because there is a bug in
   the iostream libraray. The functions in this library call the functions
   if of a streambuf object with a this pointer to the streambuf object,
   and not, when the object has multiple base classes, with the this pointer
   to the real object */
{

public:

    TTextDevice( const TRect& bounds,
                 TScrollBar *aHScrollBar,
                 TScrollBar *aVScrollBar
               );

    virtual int do_sputn( const char *s, int count ) = 0;
    virtual int overflow( int = EOF );

};

#endif  // Uses_TTextDevice

