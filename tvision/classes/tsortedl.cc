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
// SET: Moved the standard headers here because according to DJ
// they can inconditionally declare symbols like NULL
#include <ctype.h>
#include <limits.h>
#include <string.h>

#define Uses_TKeys
#define Uses_TEvent
#define Uses_TSortedListBox
#define Uses_TSortedCollection
#include <tv.h>

#ifdef __TURBOC__
char &shiftKeys = *(char *)MK_FP( 0x40, 0x17 );
#else
#if 0
extern unsigned short getshiftstate(void);
#define shiftKeys (char)(getshiftstate() & 0xff)
#else
#define shiftKeys (char)(event.keyDown.shiftState & 0xFF)
#endif
#endif

TSortedListBox::TSortedListBox( const TRect& bounds,
                                ushort aNumCols,
                                TScrollBar *aScrollBar) :
    TListBox(bounds, aNumCols, aScrollBar),
    shiftState( 0 ),
    searchPos( (ushort)-1 )
{
    showCursor();
    setCursor(1, 0);
}

static Boolean equal( const char *s1, const char *s2, ushort count)
{
    return Boolean( strncasecmp( s1, s2, count ) == 0 );
}

void TSortedListBox::handleEvent(TEvent& event)
{
    char curString[256], newString[256];
    void* k;
    int value, oldPos, oldValue;

    oldValue = focused;
    TListBox::handleEvent( event );
    if( oldValue != focused )
        searchPos = USHRT_MAX;
    if (event.what == evBroadcast &&
        event.message.command == cmListItemSelected &&
        event.message.infoPtr == list())
    {
      /* When the item was selected, reset the search feature */
      searchPos = USHRT_MAX;
      clearEvent(event);
      return;
    }
    if( event.what == evKeyDown )
        {
        if( event.keyDown.charScan.charCode != 0 )
            {
            value = focused;
            if( value < range )
                getText( curString, value, 255 );
            else
                *curString = EOS;
            oldPos = searchPos;
            if( event.keyDown.keyCode == kbBack )
                {
                if( searchPos == USHRT_MAX )
                    return;
                // SET: 1) Avoid accessing curString[USHRT_MAX], 2) Do in the right order
                curString[searchPos--] = EOS;
                if( searchPos == USHRT_MAX )
                    shiftState = shiftKeys;
                }
            else if( (event.keyDown.charScan.charCode == '.') )
                {
                char *loc = strchr( curString, '.' );
                if( loc == 0 )
                    searchPos = USHRT_MAX;
                else
                    searchPos = ushort(loc - curString);
                }
            else
                {
                searchPos++;
                if( searchPos == 0 )
                    shiftState = shiftKeys;
                curString[searchPos] = event.keyDown.charScan.charCode;
                curString[searchPos+1] = EOS;
                }
            k = getKey(curString);
            list()->search( k, value );
            if( value < range )
                {
                getText( newString, value, 255 );
                if( equal( curString, newString, searchPos+1 ) )
                    {
                    if( value != oldValue )
                        {
                        focusItem(value);
                        setCursor( cursor.x+searchPos, cursor.y );
                        }
                    else
                        setCursor(cursor.x+(searchPos-oldPos), cursor.y );
                    }
                else
                    searchPos = oldPos;
                }
            else
                searchPos = oldPos;
            if( searchPos != oldPos ||
                ucisalpha( event.keyDown.charScan.charCode )
              )
                clearEvent(event);
            }
        }
}

void* TSortedListBox::getKey( const char *s )
{
    return (void *)s;
}

void TSortedListBox::newList( TSortedCollection *aList )
{
    TListBox::newList( aList );
    searchPos = (ushort)-1;
}

