//  Copyright (c) 1992 by James H. Price, All rights reserved
//
//  DYNTEXT.H
//
//    Dynmaic text class
//

#if !defined( __DYNTEXT_H )
#define __DYNTEXT_H

#define Uses_TView
#include <tv.h>

class TStreamable;
class TPalette;

class DynamicText : public TView {

public:

  DynamicText( const TRect& r, const char *aText, Boolean rj=True );
  ~DynamicText();

  void draw();
  TPalette& getPalette() const;
  unsigned dataSize();
  void getData( void *rec );
  void setData( void *rec );
  void setText( const char *s );

protected:

  char *text;
  Boolean rightJustify;

private:
  virtual const char *streamableName() const { return name; }
protected:
  DynamicText( StreamableInit ) : TView( streamableInit ) {}
  virtual void write( opstream& );
  virtual void *read( ipstream& );
public:
  static const char * const name;
  static TStreamable *build()
    { return new DynamicText( streamableInit ); }
};

inline ipstream& operator >> ( ipstream& is, DynamicText& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, DynamicText*& cl )
    { return is >> (void *&)cl; }
inline opstream& operator << ( opstream& os, DynamicText& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, DynamicText* cl )
    { return os << (TStreamable *)cl; }


#endif

