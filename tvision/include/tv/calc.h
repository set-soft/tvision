/*-------------------------------------------------------*/
/*                                                       */
/*   Turbo Vision 1.0                                    */
/*   Copyright (c) 1991 by Borland International         */
/*                                                       */
/*   Calc.h: Header file for Calc.cpp                    */
/*-------------------------------------------------------*/
/* Ported by SET */

#if defined(Uses_TCalcDisplay) && !defined( TCalcDisplay__ )
#define TCalcDisplay__

// SET: The following should be enough to hold the output of the %f modifier
#define DISPLAYLEN  25      // Length (width) of calculator display

enum TCalcState { csFirst = 1, csValid, csError };

const int cmCalcButton  = 200,
          // SET: Defined as commands
          cmCalcClear   = cmCalcButton,
          cmCalcDelete  = cmCalcButton+1,
          cmCalcPercent = cmCalcButton+2,
          cmCalcPlusMin = cmCalcButton+3,
          cmCalc7       = cmCalcButton+4,
          cmCalc8       = cmCalcButton+5,
          cmCalc9       = cmCalcButton+6,
          cmCalcDiv     = cmCalcButton+7,
          cmCalc4       = cmCalcButton+8,
          cmCalc5       = cmCalcButton+9,
          cmCalc6       = cmCalcButton+10,
          cmCalcMul     = cmCalcButton+11,
          cmCalc1       = cmCalcButton+12,
          cmCalc2       = cmCalcButton+13,
          cmCalc3       = cmCalcButton+14,
          cmCalcMinus   = cmCalcButton+15,
          cmCalc0       = cmCalcButton+16,
          cmCalcDecimal = cmCalcButton+17,
          cmCalcEqual   = cmCalcButton+18,
          cmCalcPlus    = cmCalcButton+19;

class TRect;
class TPalette;
struct TEvent;

class CLY_EXPORT TCalcDisplay : public TView
{
public:

    TCalcDisplay(TRect& r);
    ~TCalcDisplay();
    virtual TPalette& getPalette() const;
    virtual void handleEvent(TEvent& event);
    virtual void draw();
    
private:

    TCalcState status;
    char *number;
    char sign;
    char operate;           // since 'operator' is a reserved word.
    double operand;

    void calcKey(unsigned char key, unsigned code);
    void checkFirst();
    void setDisplay(double r);
    void clear();
    void error();
    inline double getDisplay() { return( atof( number ) ); };

#if !defined( NO_STREAM )
    virtual const char *streamableName() const
        { return name; }

protected:

    virtual void write( opstream& );
    virtual void *read( ipstream& );

public:

    TCalcDisplay( StreamableInit ) : TView(streamableInit) { };
    static const char * const name;
    static TStreamable *build();
#endif  // NO_STREAM
};

#if !defined( NO_STREAM )
inline ipstream& operator >> ( ipstream& is, TCalcDisplay& cl )
    { return is >> (TStreamable&) cl; }
inline ipstream& operator >> ( ipstream& is, TCalcDisplay*& cl )
    { return is >> (void *&) cl; }

inline opstream& operator << ( opstream& os, TCalcDisplay& cl )
    { return os << (TStreamable&) cl; }
inline opstream& operator << ( opstream& os, TCalcDisplay* cl )
    { return os << (TStreamable *) cl; }
#endif  // NO_STREAM
#endif  // Uses_TCalcDisplay


#if defined(Uses_TCalculator) && !defined( TCalculator__ )
#define TCalculator__

class CLY_EXPORT TCalculator : public TDialog
{
public:

    TCalculator();
    static const char *keyChar[20];
    
#if !defined( NO_STREAM )
    TCalculator( StreamableInit ) :
        TWindowInit(&TCalculator::initFrame), TDialog(streamableInit) { };

private:

    virtual const char *streamableName() const
        { return name; }

//protected:

//    virtual void write( opstream& );
//    virtual void *read( ipstream& );

public:

    static const char * const name;
    static TStreamable *build();
#endif  // NO_STREAM
};

#if !defined( NO_STREAM )
inline ipstream& operator >> ( ipstream& is, TCalculator& cl )
    { return is >> (TStreamable&) cl; }
inline ipstream& operator >> ( ipstream& is, TCalculator*& cl )
    { return is >> (void *&) cl; }

inline opstream& operator << ( opstream& os, TCalculator& cl )
    { return os << (TStreamable&) cl; }
inline opstream& operator << ( opstream& os, TCalculator* cl )
    { return os << (TStreamable *) cl; }
#endif  // NO_STREAM
#endif  // Uses_TCalculator
