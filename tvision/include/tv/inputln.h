/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *

Modified by Robert H”hne to be used for RHIDE.
Heavily modified by Salvador E. Tropea to support multibyte encodings.
Now the original TInputLine is composed by:

TInputLineBase -> TInputLineBaseT<T,D> |-> TInputLine
                                       |-> TInputLineU16

The template class is used to avoid maintaining two versions of the same
code.
Added: TInput1Line class by SET (based on TVTools idea).

 *
 *
 */

/* ---------------------------------------------------------------------- */
/*      class TInputLine                                                  */
/*                                                                        */
/*      Palette layout                                                    */
/*        1 = Passive                                                     */
/*        2 = Active                                                      */
/*        3 = Selected                                                    */
/*        4 = Arrows                                                      */
/* ---------------------------------------------------------------------- */

#if defined( Uses_TInputLine ) && !defined( TInputLine_defined )
#define TInputLine_defined

class TRect;
struct TEvent;
class TValidator;

class TInputLineBase : public TView
{
public:
 TInputLineBase(const TRect& bounds, int aMaxLen);
 ~TInputLineBase();

 virtual uint32 dataSize();
 virtual void getData(void *rec);
 virtual TPalette& getPalette() const;
 virtual void handleEvent(TEvent& event);
 void selectAll(Boolean enable);
 virtual void setState(ushort aState, Boolean enable);
 void SetValidator(TValidator *);
 virtual Boolean valid(ushort);
 virtual Boolean insertChar(unsigned val); // Added by SET
 virtual Boolean insertChar(TEvent &event)=0;
 virtual void    assignPos(int index, unsigned val)=0;
 virtual Boolean pasteFromOSClipboard()=0;
 virtual void    copyToOSClipboard()=0;
 virtual void    setDataFromStr(void *str)=0;
 const void *getData() { return data; };

 int curPos;
 int firstPos;
 int selStart;
 int selEnd;

 static char rightArrow;
 static char leftArrow;
 static char orightArrow;
 static char oleftArrow;

protected:
 virtual void resizeData() {}
 TValidator * validator;
 void deleteSelect();
 void makeVisible(); // Added by SET
 Boolean canScroll( int delta );

 // Inline helpers to make the code cleaner
 int insertModeOn();
 int lineIsFull();
 int posIsEnd();

 // IMHO exposing these two is a very bad idea, I added a couple of members to
 // work with them: setDataFromStr & getData. All TV code uses these new
 // members. If we don't hide them then we must compute the string length all
 // the time. SET.
 char *data;
 int maxLen;

 int cellSize;
 int dataLen;

private:
 int mouseDelta( TEvent& event );
 int mousePos( TEvent& event );

#if !defined( NO_STREAM )
 //virtual const char *streamableName() const
 //    { return name; }

protected:

 TInputLineBase(StreamableInit);
 virtual void write(opstream&);
 virtual void *read(ipstream&);
 virtual void writeData(opstream&)=0;
 virtual void *readData(ipstream&)=0;

public:

 //static const char * const name;
 //static TStreamable *build();
#endif // NO_STREAM
};

#if !defined( NO_STREAM )
inline ipstream& operator >> ( ipstream& is, TInputLineBase& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, TInputLineBase*& cl )
    { return is >> (void *&)cl; }

inline opstream& operator << ( opstream& os, TInputLineBase& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, TInputLineBase* cl )
    { return os << (TStreamable *)cl; }
#endif // NO_STREAM

inline
int TInputLineBase::insertModeOn()
{
 return (state & sfCursorIns)==0;
}

inline
int TInputLineBase::lineIsFull()
{
 return dataLen>=maxLen;
}

inline
int TInputLineBase::posIsEnd()
{
 return curPos>=dataLen;
}

template <typename T, typename D>
class TInputLineBaseT: public TInputLineBase
{
public:
 TInputLineBaseT(const TRect& bounds, int aMaxLen);

 virtual void    setData(void *rec);
 virtual void    setDataFromStr(void *str);
 virtual void    assignPos(int index, unsigned val);
 virtual Boolean pasteFromOSClipboard();
 virtual void    copyToOSClipboard();
 virtual void    draw();

#if !defined( NO_STREAM )
protected:
 TInputLineBaseT(StreamableInit) : TInputLineBase(streamableInit) {}

 virtual void writeData(opstream&);
 virtual void *readData(ipstream&);
#endif // NO_STREAM
};

class TInputLine : public TInputLineBaseT<char,TDrawBuffer>
{
public:
 TInputLine(const TRect& bounds, int aMaxLen) :
   TInputLineBaseT<char,TDrawBuffer>(bounds,aMaxLen) {};

 virtual Boolean insertChar(TEvent &event);

#if !defined( NO_STREAM )
 virtual const char *streamableName() const
     { return name; }

protected:
 TInputLine::TInputLine(StreamableInit) :
   TInputLineBaseT<char,TDrawBuffer>(streamableInit) {}

public:
 static const char * const name;
 static TStreamable *build();
#endif // NO_STREAM
};

#if !defined( NO_STREAM )
inline ipstream& operator >> ( ipstream& is, TInputLine& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, TInputLine*& cl )
    { return is >> (void *&)cl; }

inline opstream& operator << ( opstream& os, TInputLine& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, TInputLine* cl )
    { return os << (TStreamable *)cl; }
#endif // NO_STREAM


class TInputLineU16 : public TInputLineBaseT<uint16,TDrawBufferU16>
{
public:
 TInputLineU16(const TRect& bounds, int aMaxLen) :
   TInputLineBaseT<uint16,TDrawBufferU16>(bounds,aMaxLen) {};

 virtual Boolean insertChar(TEvent &event);

#if !defined( NO_STREAM )
 virtual const char *streamableName() const
     { return name; }

protected:
 TInputLineU16(StreamableInit) :
   TInputLineBaseT<uint16,TDrawBufferU16>(streamableInit) {}
 
public:
 static const char * const name;
 static TStreamable *build();
#endif // NO_STREAM
};

#if !defined( NO_STREAM )
inline ipstream& operator >> ( ipstream& is, TInputLineU16& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, TInputLineU16*& cl )
    { return is >> (void *&)cl; }

inline opstream& operator << ( opstream& os, TInputLineU16& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, TInputLineU16* cl )
    { return os << (TStreamable *)cl; }
#endif // NO_STREAM

#endif  // Uses_TInputLine

#if defined( Uses_TInput1Line ) && !defined( TInput1Line_defined )
#define TInput1Line_defined
// This is based on TVTools idea, but I think is better to implement it
// in this way and not like a macro.
class TInput1Line : public TInputLine
{
public:
 TInput1Line(int x, int y, int max) :
   TInputLine(TRect(x,y,x+max+2,y+1), max)) {};
};

class TInput1LineU16 : public TInputLineU16
{
public:
 TInput1LineU16(int x, int y, int max) :
   TInputLineU16(TRect(x,y,x+max+2,y+1), max)) {};
}
#endif // Uses_TInput1Line

