//  Copyright (c) 1992 by James H. Price, All rights reserved
//
//  DYNTEXT.CPP
//
//    Member functions for DynamicText class
//

#define Uses_string
#define Uses_TView
#define Uses_TStreamableClass
#define Uses_TPalette
#include <tv.h>

#include "dyntext.h"

#define cpDynamicText "\x06"

DynamicText::DynamicText( const TRect& r, const char *aText,
  Boolean rj ) : TView( r ), text( new char[size.x+1] ),
  rightJustify( rj )
{
  strncpy( text, aText, size.x );
  text[size.x] = EOS;
}

DynamicText::~DynamicText()
{
  delete text;
}

void DynamicText::draw()
{
  TDrawBuffer b;
  uchar color = getColor(1);
  int offset = ( rightJustify ) ? size.x-strlen(text) : 0;

  b.moveChar( 0, ' ', color, size.x );
  b.moveStr( offset, text, color );
  writeBuf( 0, 0, size.x, 1, b );
}

TPalette& DynamicText::getPalette() const
{
  static TPalette palette( cpDynamicText, sizeof( cpDynamicText )-1 );
  return palette;
}

void DynamicText::setText( const char *s )
{
  setData( (void *)s );
  drawView();
}

void DynamicText::setData( void *rec )
{
  memmove( text, rec, size.x+1 );
  text[size.x] = EOS;
}

void DynamicText::getData( void *rec )
{
  strcpy( (char *)rec, text );
}

unsigned DynamicText::dataSize()
{
  return( size.x+1 );
}

void DynamicText::write( opstream& os )
{
  TView::write( os );
  os.writeString( text );
  os << (int)rightJustify;
}

void *DynamicText::read( ipstream& is )
{
  int rj;
  TView::read( is );
  text = is.readString();
  is >> rj;
  rightJustify = Boolean( rj );
  return this;
}

const char * const DynamicText::name = "DynamicText";
TStreamableClass RDynamicText( DynamicText::name,
  DynamicText::build,  __DELTA(DynamicText) );
