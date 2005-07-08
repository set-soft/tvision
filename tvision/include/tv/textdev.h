/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *

Modified by Robert H”hne to be used for RHIDE.
Modified to compile with gcc v3.x by Salvador E. Tropea, with the help of
Andris Pavenis.

 *
 *
 */

#if defined( Uses_TTextDevice ) && !defined( __TTextDevice )
#define __TTextDevice

class TRect;
class TScrollBar;

class CLY_EXPORT TTextDevice : public CLY_streambuf, public TScroller
/* I have changed the order of the baseclasses, because there is a bug in
   the iostream library. The functions in this library call the functions
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

