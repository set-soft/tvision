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

#if defined( Uses_TTerminal ) && !defined( __TTerminal )
#define __TTerminal

class TRect;
class TScrollBar;

class CLY_EXPORT TTerminal: public TTextDevice
{

public:

    TTerminal( const TRect& bounds,
	       TScrollBar *aHScrollBar,
	       TScrollBar *aVScrollBar,
	       uint32 aBufSize
	     );
    virtual ~TTerminal();

    virtual int do_sputn( const char *s, int count );

    void bufInc(uint32& val) { if (++val>=bufSize) val=0; }
    Boolean canInsert( uint32 amount );
    short calcWidth();
    virtual void draw();
    uint32 nextLine( uint32 pos );
    uint32 prevLines( uint32 pos, uint32 lines );
    Boolean queEmpty();

protected:

    uint32 bufSize;
    char *buffer;
    uint32 queFront, queBack;
    void bufDec(uint32& val) { if (val==0) val=bufSize - 1; else val--; }
};

#endif  // Uses_TTerminal

