/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *

Modified by Robert H”hne to be used for RHIDE.
Modified by Salvador E. Tropea (added insertChar, middle button paste and
other stuff).
This class was reworked to support Unicode 16 by Salvador E. Tropea
Copyright (c) 2003
I basically created a new class TInputLineBase that's pure virtual and
contains code that can be used for any cell size.
Then I moved the cell size dependent code to TInputLine and created another
new classs called TInputLineU16 suitable for Unicode 16.
A lot of internal details changed.

 *
 *
 */
// SET: Moved the standard headers here because according to DJ
// they can inconditionally declare symbols like NULL
// Added insertChar member.
//#define Uses_stdio // debug

#define Uses_ctype
#define Uses_string
#define Uses_TKeys
#define Uses_TInputLine
#define Uses_TDrawBuffer
#define Uses_TEvent
#define Uses_opstream
#define Uses_ipstream
#define Uses_TStreamableClass
#define Uses_TValidator
#define Uses_TPalette
#define Uses_TVOSClipboard
#define Uses_TGKey
#include <tv.h>

// TODO: Not used here should be moved away
char hotKey( const char *s )
{
    const char *p;

    if( (p = strchr( s, '~' )) != 0 )
        return uctoupper(p[1]);
    else
        return 0;
}

// TODO: Also this
inline
unsigned StrLen(const char *s)
{
 return strlen(s);
}

inline
unsigned StrLen(const uint16 *s)
{
 unsigned l;
 for (l=0; s[l]; l++);
 return l;
}

#define cpInputLine "\x13\x13\x14\x15"

TInputLineBase::TInputLineBase(const TRect& bounds, int aMaxLen) :
    TView(bounds),
    curPos(0),
    firstPos(0),
    selStart(0),
    selEnd(0),
    validator(NULL),
    maxLen(aMaxLen-1),
    dataLen(0)
{
 state|=sfCursorVis;
 options|=ofSelectable | ofFirstClick;
}

template class TInputLineBaseT<char,TDrawBuffer>;
template class TInputLineBaseT<uint16,TDrawBufferU16>;

template <typename T, typename D>
TInputLineBaseT<T,D>::TInputLineBaseT(const TRect& bounds, int aMaxLen) :
  TInputLineBase(bounds,aMaxLen)
{
 data=(char *)new T[aMaxLen];
 *((T *)data)=EOS;
 cellSize=sizeof(T);
}

void TInputLineBase::SetValidator(TValidator * aValidator)
{
 CLY_destroy(validator);
 validator=aValidator;
 if (validator)
    validator->SetOwner(this);
}

TInputLineBase::~TInputLineBase()
{
 delete[] data;
 CLY_destroy(validator);
}

Boolean TInputLineBase::canScroll( int delta )
{
 if (delta<0)
    return Boolean(firstPos>0);
 else
    if (delta>0)
       return Boolean(dataLen-firstPos+2>size.x);
    else
       return False;
}

// Optimized for size, it could be 2 specialized members avoiding a generic
// multiply.
uint32 TInputLineBase::dataSize()
{
 return (maxLen+1)*cellSize;
}

// TODO: avoid hardcoded arrows
template <typename T, typename D>
void TInputLineBaseT<T,D>::draw()
{
 int l, r;
 D b;
 
 uchar color=(state & sfFocused) ? getColor(2) : getColor(1);
 
 b.moveChar(0,' ',color,size.x);
 b.moveStr(1,((T *)data)+firstPos,color,size.x-2);
 
 if (canScroll(1))
    b.moveChar(size.x-1,sizeof(T)==1 ? rightArrow : 0x25b6,(uchar)getColor(4),1);
 if (canScroll(-1))
    b.moveChar(0,sizeof(T)==1 ? leftArrow: 0x25c0,(uchar)getColor(4),1);
 if (state & sfSelected)
   {
    l=selStart-firstPos;
    r=selEnd-firstPos;
    l=max(0,l);
    r=min(size.x-2,r);
    if (l<r)
       b.moveChar(l+1,0,(uchar)getColor(3),r-l);
   }
 writeLine(0,0,size.x,size.y,b);
 setCursor(curPos-firstPos+1,0);
}

void TInputLineBase::getData(void *rec)
{
 memcpy(rec,data,dataSize());
}

TPalette& TInputLineBase::getPalette() const
{
 static TPalette palette(cpInputLine,sizeof(cpInputLine)-1);
 return palette;
}

int TInputLineBase::mouseDelta( TEvent& event )
{
 TPoint mouse=makeLocal( event.mouse.where );
 
 if (mouse.x<=0)
    return -1;
 else
    if (mouse.x>=size.x-1)
       return 1;
    else
       return 0;
}

int TInputLineBase::mousePos( TEvent& event )
{
 TPoint mouse=makeLocal(event.mouse.where);
 mouse.x=max(mouse.x,1);
 int pos=mouse.x+firstPos-1;
 pos=max(pos,0);
 pos=min(pos,dataLen);
 return pos;
}

// Also optimize for size
void  TInputLineBase::deleteSelect()
{
 if (selStart<selEnd)
   {
    memcpy(data+selStart*cellSize,data+selEnd*cellSize,(dataLen-selEnd+1)*cellSize);
    dataLen-=selEnd-selStart;
    curPos=selStart;
   }
}

template <typename T, typename D>
void TInputLineBaseT<T,D>::assignPos(int index, unsigned val)
{
 ((T *)data)[index]=val;
}

/**[txh]********************************************************************

  Description:
  Inserts a character at the cursor position. If the text is currently
selected it's removed. If a validator is defined it's called. This basic
input line have a fixed size and will refuse to insert the character if
there is not enough space, but the virtual resizeData() is called giving
a chance to create variable size input lines.@*
  I (SET) moved it to allow insertions from sources other than the keyboard
emulating it's behavior.
  
  Return: False if the validator canceled the character, otherwise True.
  
***************************************************************************/
// TODO: The validator can't be 8 bits for an Unicode class.
//       I think IsValidInput should support one char.
Boolean TInputLineBase::insertChar(unsigned value)
{
 if (validator)
   {
    char tmp[2];
    tmp[0]=value;
    tmp[1]=0;
    if (validator->IsValidInput(tmp,False)==False)
       return False;
   }
 if (insertModeOn())
    deleteSelect();
 if (( insertModeOn() && lineIsFull()) ||
     (!insertModeOn() && posIsEnd()))
    resizeData();
 if (insertModeOn())
   {
    if (dataLen<maxLen)
      {
       memmove(data+(curPos+1)*cellSize,data+curPos*cellSize,
               ((dataLen-curPos)+1)*cellSize);
       dataLen++;
      }
   }
 else if (dataLen==curPos)
   {
    assignPos(curPos+1,0);
    data[curPos+1]=0;
   }
 if (( insertModeOn() && !lineIsFull()) ||
     (!insertModeOn() && !posIsEnd()))
   {
    if (firstPos>curPos)
       firstPos=curPos;
    if (curPos==dataLen)
       dataLen++;
    assignPos(curPos++,value);
   }
 return True;
}

Boolean TInputLine::insertChar(TEvent &event)
{
 if (event.keyDown.charScan.charCode>=' ')
   {
    if (!TInputLineBase::insertChar(event.keyDown.charScan.charCode))
       clearEvent(event);
    return True;
   }
 return False;
}

Boolean TInputLineU16::insertChar(TEvent &event)
{
 TGKey::fillCharCode(event);
 //printf("insertChar de Unicode: U+%04X\n",event.keyDown.charCode);
 if (event.keyDown.charCode>=' ' && event.keyDown.charCode<0xFF00)
   {
    if (!TInputLineBase::insertChar(event.keyDown.charCode))
       clearEvent(event);
    return True;
   }
 return False;
}

/**[txh]********************************************************************

  Description:
  Used internally to ensure the cursor is at a visible position, unselect
the text and force a draw.
  
***************************************************************************/

void TInputLineBase::makeVisible()
{
 selStart=0;
 selEnd  =0;
 if (firstPos>curPos)
    firstPos=curPos;
 int i=curPos-size.x+2;
 if (firstPos<i)
    firstPos=i;
 drawView();
}

// TODO: Get the clipboard in unicode format
template <typename T, typename D>
Boolean TInputLineBaseT<T,D>::pasteFromOSClipboard()
{
 if (sizeof(T)!=1) return False;
 unsigned size,i;
 T *p=(T *)TVOSClipboard::paste(1,size);
 if (p)
   {
    for (i=0; i<size; i++)
       {
        TInputLineBase::insertChar(p[i]);
        selStart=selEnd=0; // Reset the selection or we will delete the last insertion
       }
    DeleteArray(p);
    makeVisible();
    return True;
   }
 return False;
}

template <typename T, typename D>
void TInputLineBaseT<T,D>::copyToOSClipboard()
{
 if (sizeof(T)==1)
    TVOSClipboard::copy(1,data+selStart,selEnd-selStart);
 // else if 2 ....
 // TODO: Put to the clipboard in unicode format
}

void TInputLineBase::handleEvent(TEvent& event)
{
 TView::handleEvent(event);

 int delta, anchor;
 if ((state & sfSelected)!=0)
    switch (event.what)
      {
       case evMouseDown:
            if (event.mouse.buttons==mbMiddleButton &&
                TVOSClipboard::isAvailable()>1)
              {
               pasteFromOSClipboard();
              }
            else if (canScroll(delta=mouseDelta(event)))
              {
               do
                 {
                  if (canScroll(delta))
                    {
                     firstPos+=delta;
                     drawView();
                    }
                  }
               while (mouseEvent(event,evMouseAuto));
              }
            else if (event.mouse.doubleClick)
              {
               selectAll(True);
              }
            else
              {
               anchor=mousePos(event);
               do
                 {
                  if (event.what==evMouseAuto &&
                      canScroll(delta=mouseDelta(event)))
                     firstPos+=delta;
                  curPos=mousePos(event);
                  if (curPos<anchor)
                    {
                     selStart=curPos;
                     selEnd=anchor;
                    }
                  else
                    {
                     selStart=anchor;
                     selEnd=curPos;
                    }
                  drawView();
                 }
               while (mouseEvent(event,evMouseMove | evMouseAuto));
               if (TVOSClipboard::isAvailable()>1)
                  copyToOSClipboard();
              }
            clearEvent(event);
            break;

       case evKeyDown:
            switch (ctrlToArrow(event.keyDown.keyCode))
              {
               case kbLeft:
                    if (curPos>0)
                       curPos--;
                    break;
               case kbRight:
                    if (!posIsEnd())
                       curPos++;
                    break;
               case kbHome:
                    curPos=0;
                    break;
               case kbEnd:
                    curPos=dataLen;
                    break;
               case kbBack:
                    if (curPos>0)
                      {
                       selStart=curPos-1;
                       selEnd  =curPos;
                       deleteSelect();
                       if (firstPos>0)
                          firstPos--;
                      }
                    break;
               case kbDel:
                    if (selStart==selEnd)
                       if (!posIsEnd())
                         {
                          selStart=curPos;
                          selEnd  =curPos+1;
                         }
                    deleteSelect();
                    break;
               case kbIns:
                    setState(sfCursorIns,Boolean(!(state & sfCursorIns)));
                    break;
               case kbCtrlY:
                    assignPos(0,EOS);
                    curPos=0;
                    break;
               // Let them pass even if these contains a strange ASCII (SET)
               case kbEnter:
               case kbTab:
                    return;
               default:
                    if (!insertChar(event))
                       return;
              }
            makeVisible();
            clearEvent(event);
            break;
      }
}

void TInputLineBase::selectAll( Boolean enable )
{
 selStart=0;
 if (enable)
    curPos=selEnd=dataLen;
 else
    curPos=selEnd=0;
 firstPos=max(0,curPos-size.x+2);
 if (TVOSClipboard::isAvailable()>1)
    copyToOSClipboard();
 drawView();
}

template <typename T, typename D>
void TInputLineBaseT<T,D>::setData(void *rec)
{
 uint32 ds=dataSize()-sizeof(T);
 memcpy(data,rec,ds);
 *((T *)(data+ds))=EOS;
 dataLen=StrLen((T *)data);
 selectAll(True);
}

template <typename T, typename D>
void TInputLineBaseT<T,D>::setDataFromStr(void *str)
{
 unsigned ds=dataSize()/sizeof(T)-1, i;
 T *s=(T *)str;
 T *d=(T *)data;
 for (i=0; i<ds && s[i]; i++)
     d[i]=s[i];
 d[i]=EOS;
 dataLen=i;
}

void TInputLineBase::setState(ushort aState, Boolean enable)
{
 TView::setState(aState,enable);
 if (aState==sfSelected ||
     (aState==sfActive && (state & sfSelected)))
    selectAll(enable);
}

#if !defined( NO_STREAM )
void TInputLineBase::write(opstream& os)
{
 TView::write(os);
 os << maxLen << curPos << firstPos
    << selStart << selEnd;
 writeData(os);
 os << validator;
}

template <typename T, typename D>
void TInputLineBaseT<T,D>::writeData(opstream& os)
{
 os.writeString((T *)data);
}

void *TInputLineBase::read(ipstream& is)
{
 TView::read(is);
 is >> maxLen >> curPos >> firstPos
    >> selStart >> selEnd;
 readData(is);
 state|=sfCursorVis;
 is >> validator;
 return this;
}

template <typename T, typename D>
void *TInputLineBaseT<T,D>::readData(ipstream& is)
{
 cellSize=sizeof(T);
 data=(char *)new T[maxLen+1];
 is.readString((T *)data,maxLen+1);
 return data;
}

TStreamable *TInputLine::build()
{
 return new TInputLine(streamableInit);
}

TStreamable *TInputLineU16::build()
{
 return new TInputLineU16(streamableInit);
}

TInputLineBase::TInputLineBase(StreamableInit) :
  TView(streamableInit),
  validator(NULL)
{
}

#endif // NO_STREAM

Boolean TInputLineBase::valid(ushort )
{
 Boolean ret=True;
 if (validator)
   {
    ret=validator->Valid(data);
    if (ret==True)
      {
       validator->Format(data);
       drawView();
      }
   }
 return ret;
}

