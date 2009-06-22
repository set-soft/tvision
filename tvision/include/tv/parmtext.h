/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *
 *  Modified by Salvador E. Tropea (SET) for RHTVision port.
 *
 */

/* ---------------------------------------------------------------------- */
/*      class TParamText                                                  */
/*                                                                        */
/*      Palette layout                                                    */
/*        1 = Text                                                        */
/* ---------------------------------------------------------------------- */

#if defined( Uses_TParamText ) && !defined( __TParamText )
#define __TParamText

class TRect;

const int tparamTextMaxLen=256;
#define SAFE_CODE 1  // Don't corrupt memory if tparamTextMaxLen is small

class TParamText : public TStaticText
{
public:
 TParamText( const TRect& bounds );
 ~TParamText();

 virtual void getText( char *str, int maxLen );
 virtual void setText( const char *fmt, ... );
 virtual int  getTextLen();

protected:
 char *str;

#if !defined( NO_STREAM )
private:
  virtual const char *streamableName() const
        { return name; }
protected:
  TParamText( StreamableInit );
public:
  static const char * const name;
  static TStreamable *build();
#endif
};

#if !defined( NO_STREAM )
inline ipstream& operator >> ( ipstream& is, TParamText& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, TParamText*& cl )
    { return is >> (void *&)cl; }

inline opstream& operator << ( opstream& os, TParamText& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, TParamText* cl )
    { return os << (TStreamable *)cl; }
#endif // NO_STREAM

#endif  // Uses_TParamText

