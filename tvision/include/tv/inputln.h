/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *

Modified by Robert H”hne to be used for RHIDE.
Added: TInput1Line class by SET (based on TVTools idea).
Added: modeOptions stuff by SET.

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

const unsigned ilValidatorBlocks=1;  // Don't pass the focus if the validator indicates
                                     // the data isn't valid. by SET.

class CLY_EXPORT TInputLine : public TView
{
public:

    TInputLine( const TRect& bounds, int aMaxLen, TValidator *aValid=NULL );
    ~TInputLine();

    virtual unsigned dataSize();
    virtual void draw();
    virtual void getData( void *rec );
    virtual TPalette& getPalette() const;
    virtual void handleEvent( TEvent& event );
    void selectAll( Boolean enable );
    virtual void setData( void *rec );
    virtual void setState( ushort aState, Boolean enable );
    void setValidator(TValidator *);
    virtual Boolean valid(ushort);
    virtual Boolean insertChar(char value); // Added by SET
    // This is for compatibility with the Unicode branch
    const void *getData() { return data; }; // Added by SET
    void setDataFromStr(void *value) // Added by SET
      { setData(value); };

    char* data;
    int maxLen;
    int curPos, oldCurPos;
    int firstPos, oldFirstPos;
    int selStart, oldSelStart;
    int selEnd, oldSelEnd;

    // Functions to fine tune the behavior. by SET.
    unsigned setModeOptions(unsigned newOps)
      { unsigned old=modeOptions; modeOptions=newOps; return old; }
    unsigned getModeOptions()
      { return modeOptions; }
    static unsigned setDefaultModeOptions(unsigned newOps)
      { unsigned old=defaultModeOptions; defaultModeOptions=newOps; return old; }
    static unsigned getDefaultModeOptions()
      { return defaultModeOptions; }


    static char rightArrow;
    static char leftArrow;
    static char orightArrow;
    static char oleftArrow;

protected:
    virtual void resizeData() {}
    TValidator * validator;
    char* oldData;
    void deleteSelect();
    void makeVisible(); // Added by SET
    Boolean canScroll( int delta );
    void saveState();
    void restoreState();
    Boolean checkValid(Boolean);

    // To fine tune the behavior. SET.
    static unsigned defaultModeOptions;
    unsigned modeOptions;

private:
    int mouseDelta( TEvent& event );
    int mousePos( TEvent& event );

#if !defined( NO_STREAM )
    virtual const char *streamableName() const
        { return name; }

protected:

    TInputLine( StreamableInit );
    virtual void write( opstream& );
    virtual void *read( ipstream& );

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

#endif  // Uses_TInputLine

#if defined( Uses_TInput1Line ) && !defined( TInput1Line_defined )
#define TInput1Line_defined
// This is based on TVTools idea, but I think is better to implement it
// in this way and not like a macro.
class CLY_EXPORT TInput1Line : public TInputLine
{
public:
 TInput1Line(int x, int y, int max, TValidator *aValid=NULL) :
   TInputLine(TRect(x,y,x+max+2,y+1), max, aValid) {};
};
#endif // Uses_TInput1Line

