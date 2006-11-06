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
Copyright (c) 2003-2006
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
#define Uses_TKeys_Extended
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
#define Uses_TGroup
#include <tv.h>

unsigned TInputLineBase::defaultModeOptions=0;

// Helpers to find the length of the data, overloaded for each data size
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

TInputLineBase::TInputLineBase(const TRect& bounds, int aMaxLen, TValidator *aValid) :
    TView(bounds),
    curPos(0),
    firstPos(0),
    selStart(0),
    selEnd(0),
    validator(aValid),
    maxLen(aMaxLen-1),
    dataLen(0)
{
 state|=sfCursorVis;
 options|=ofSelectable | ofFirstClick;
 modeOptions=defaultModeOptions;
}

template <typename T, typename D>
TInputLineBaseT<T,D>::TInputLineBaseT(const TRect& bounds, int aMaxLen, TValidator *aValid) :
  TInputLineBase(bounds,aMaxLen,aValid)
{
 data=(char *)new T[aMaxLen];
 *((T *)data)=EOS;
 oldData=(char *)new T[aMaxLen];
 cellSize=sizeof(T);
 hideContent=False;
}

void TInputLineBase::setValidator(TValidator *aValidator)
{
 CLY_destroy(validator);
 validator=aValidator;
}

TInputLineBase::~TInputLineBase()
{
 delete[] data;
 delete[] oldData;
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
unsigned TInputLineBase::dataSize()
{
 unsigned dSize=0;

 if (validator)
    dSize=validator->transfer(data,NULL,vtDataSize);
 if (dSize==0)
    dSize=maxLen+1;
 return dSize*cellSize;
}

// TODO: avoid hardcoded arrows
template <typename T, typename D>
void TInputLineBaseT<T,D>::draw()
{
 int l, r;
 D b;
 
 uchar color=(state & sfFocused) ? getColor(2) : getColor(1);
 
 b.moveChar(0,' ',color,size.x);
 if (hideContent)
   {
    int rest=dataLen-firstPos;
    if (rest>0)
       b.moveChar(1,'*',color,min(size.x-2,rest));
   }
 else
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
 if (!validator || !validator->transfer(data,rec,vtGetData))
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
    CLY_memcpy(data+selStart*cellSize,data+selEnd*cellSize,(dataLen-selEnd+1)*cellSize);
    dataLen-=selEnd-selStart;
    curPos=selStart;
   }
}

template <typename T, typename D>
void TInputLineBaseT<T,D>::assignPos(int index, unsigned val)
{
 ((T *)data)[index]=val;
}

void TInputLineBase::saveState()
{
 if (validator)
   {// Save data to unroll
    oldDataLen=dataLen;
    oldCurPos=curPos;
    oldFirstPos=firstPos;
    oldSelStart=selStart;
    oldSelEnd=selEnd;
    memcpy(oldData,data,dataLen*cellSize);
   }
}

void TInputLineBase::restoreState()
{
 if (validator)
   {// Unroll the changes
    dataLen=oldDataLen;
    memcpy(data,oldData,dataLen*cellSize);
    assignPos(dataLen,0);
    curPos=oldCurPos;
    firstPos=oldFirstPos;
    selStart=oldSelStart;
    selEnd=oldSelEnd;
   }
}

template <typename T, typename D>
unsigned TInputLineBaseT<T,D>::recomputeDataLen()
{
 return StrLen((T *)data);
}

Boolean TInputLineBase::checkValid(Boolean noAutoFill)
{
 if (validator)
   {// IMPORTANT!!! The validator can write more than maxLen chars.
    if (!validator->isValidInput(data,noAutoFill))
      {
       restoreState();
       return False;
      }
    else
      {
       int newLen=recomputeDataLen();
       if (curPos>=dataLen && newLen>dataLen)
          curPos=newLen;
       dataLen=newLen;
       return True;
      }
   }
 return True;
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
Boolean TInputLineBase::insertChar(unsigned value)
{
 saveState();
 // Operate
 if (insertModeOn())
    deleteSelect();
 if (( insertModeOn() && lineIsFull()) ||
     (!insertModeOn() && posIsEnd()))
    resizeData();

 if (insertModeOn())
   {
    if (!lineIsFull())
      {
       memmove(data+(curPos+1)*cellSize,data+curPos*cellSize,
               ((dataLen-curPos)+1)*cellSize);
       dataLen++;
       if (firstPos>curPos)
          firstPos=curPos;
       assignPos(curPos++,value);
      }
   }
 else if (dataLen==curPos)
   {
    assignPos(curPos+1,0);
    data[curPos+1]=0;
   }
 else
   {
    if (firstPos>curPos)
       firstPos=curPos;
    if (curPos==dataLen)
       dataLen++;
    assignPos(curPos++,value);
   }

 return checkValid(False);
}

Boolean TInputLine::insertCharEv(TEvent &event)
{
 if (event.keyDown.charScan.charCode>=' ')
   {
    if (!insertChar(event.keyDown.charScan.charCode))
       clearEvent(event);
    return True;
   }
 return False;
}

Boolean TInputLineU16::insertCharEv(TEvent &event)
{
 TGKey::fillCharCode(event);
 //printf("insertChar de Unicode: U+%04X\n",event.keyDown.charCode);
 if (event.keyDown.charCode>=' ' && event.keyDown.charCode<0xFF00)
   {
    if (!insertChar(event.keyDown.charCode))
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
        insertChar(p[i]);
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

#define adjustSelectBlock() \
          if( curPos < anchor )  \
              {                  \
              selStart = curPos; \
              selEnd = anchor;   \
              }                  \
          else                   \
              {                  \
              selStart = anchor; \
              selEnd = curPos;   \
              }

void TInputLineBase::handleEvent(TEvent& event)
{
 ushort key;
 Boolean extendBlock;
 TView::handleEvent(event);

 int delta, anchor=0;
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
                  adjustSelectBlock()
                  drawView();
                 }
               while (mouseEvent(event,evMouseMove | evMouseAuto));
               if (TVOSClipboard::isAvailable()>1)
                  copyToOSClipboard();
              }
            clearEvent(event);
            break;

       case evKeyDown:
            key=ctrlToArrow(event.keyDown.keyCode);
            extendBlock=False;
            if (key & kbShiftCode)
              {
               ushort keyS=key & (~kbShiftCode);
               if (keyS==kbHome || keyS==kbLeft || keyS==kbRight || keyS==kbEnd)
                 {
                  if (curPos==selEnd)
                     anchor=selStart;
                  else
                     anchor=selEnd;
                  key=keyS;
                  extendBlock=True;
                 }
              }
                        
            switch (key)
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
               case kbBackSpace:
                    if (curPos>0)
                      {
                       saveState();
                       selStart=curPos-1;
                       selEnd  =curPos;
                       deleteSelect();
                       if (firstPos>0)
                          firstPos--;
                       checkValid(True);
                      }
                    break;
               case kbDelete:
                    saveState();
                    if (selStart==selEnd)
                       if (!posIsEnd())
                         {
                          selStart=curPos;
                          selEnd  =curPos+1;
                         }
                    deleteSelect();
                    checkValid(True);
                    break;
               case kbInsert:
                    setState(sfCursorIns,Boolean(!(state & sfCursorIns)));
                    break;
               case kbCtY:
                    assignPos(0,EOS);
                    curPos=0;
                    dataLen=0;
                    break;
               // Let them pass even if these contains a strange ASCII (SET)
               case kbEnter:
               case kbTab:
                    return;
               default:
                    if (!insertCharEv(event))
                       return;
              }
            if (extendBlock)
              {
               adjustSelectBlock()
              }
            else
              {
               selStart=0;
               selEnd=0;
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
 if (!validator || !validator->transfer(data,rec,vtSetData))
   {
    #if 1
    dataLen=StrLen((T *)rec);
    unsigned ds=dataSize()-sizeof(T);
    unsigned dataLenBytes=dataLen*sizeof(T);
    if (dataLenBytes>ds)
       dataLenBytes=ds;
    memcpy(data,rec,dataLenBytes);
    memset(data+dataLenBytes,EOS,ds-dataLenBytes+1);
    #else // Old code, can read out of bounds.
    unsigned ds=dataSize()-sizeof(T);
    memcpy(data,rec,ds);
    *((T *)(data+ds))=EOS;
    dataLen=StrLen((T *)data);
    #endif
   }
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
 if (validator &&                           // We have a validator
     (modeOptions & ilValidatorBlocks)  &&  // We want to block if invalid
     owner && (owner->state & sfActive) &&  // The owner is visible
     aState==sfFocused && enable==False)    // We are losing the focus
   {
    TValidator *v=validator;
    validator=NULL;             // Avoid nested tests
    Boolean ret=v->validate(data); // Check if we have valid data
    validator=v;
    if (!ret)                   // If not refuse the focus change
       return;
   }
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
 oldData=(char *)new T[maxLen+1];
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

Boolean TInputLineBase::valid(ushort cmd)
{
 if (validator)
   {
    if (cmd==cmValid)
       return Boolean(validator->status==vsOk);
    else
       if (cmd!=cmCancel)
          if (!validator->validate(data))
            {
             owner->current = 0;
             select();
             return False;
            }
   }
 return True;
}

// Moved after all the declarations. gcc 4.0.3 seems to have a bug that
// produces link errors when this instantiation is done before all the
// members are defined. Pointed out by Donald R. Ziesig.
template class TInputLineBaseT<char,TDrawBuffer>;
template class TInputLineBaseT<uint16,TDrawBufferU16>;

