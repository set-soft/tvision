/*------------------------------------------------------------*/
/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *
 * Modified by Salvador E. Tropea. Covered by the GPL license.
 * * Change NO_STREAM usage.
 * * Fixed missing explicit base class initialization.
 * * Changed 0 to NULL.
 * * Added headers request.
 */

#define Uses_string
#define Uses_TStringLookupValidator
#define Uses_MsgBox
#include <tv.h>

// TLookupValidator

Boolean TLookupValidator::isValid(const char* s)
{
  return lookup(s);
}

Boolean TLookupValidator::lookup(const char* s)
{
  return True;
}

#if !defined(NO_STREAM)
TStreamable * TLookupValidator::build()
{
  return (TStreamable *) new TLookupValidator(streamableInit);
}

TLookupValidator::TLookupValidator( StreamableInit s ) : TValidator(s)
{
}
#endif // NO_STREAM

// TStringLookupValidator

TStringLookupValidator::TStringLookupValidator(TStringCollection* aStrings) :
  TLookupValidator()
{
  strings = aStrings;
}

TStringLookupValidator::~TStringLookupValidator()
{
  newStringList(NULL);
}

const char * TStringLookupValidator::errorMsg = __("Input is not in list of valid strings");

void TStringLookupValidator::error()
{
  messageBox(errorMsg, mfError | mfOKButton);
}

static Boolean stringMatch(void* a1, void* a2)
{
    return Boolean(strcmp((const char*)a1, (const char*)a2) == 0);
}


Boolean TStringLookupValidator::lookup(const char* s)
{
    return Boolean(strings->firstThat(stringMatch,(void*)s) != 0);
}

// Attention!!! the collection is deleted, NOT destroyed.
void TStringLookupValidator::newStringList(TStringCollection* aStrings)
{
  if (strings)
      delete strings;

  strings = aStrings;
}

#if !defined(NO_STREAM)
void TStringLookupValidator::write( opstream& os )
{
  TLookupValidator::write( os );
  os << strings;
}

void* TStringLookupValidator::read( ipstream& is )
{
  TLookupValidator::read(is);
  is >> strings;

  return this;
}

TStreamable * TStringLookupValidator::build()
{
  return (TStreamable *) new TStringLookupValidator(streamableInit);
}

TStringLookupValidator::TStringLookupValidator( StreamableInit s ) :
        TLookupValidator(s)
{
}
#endif // NO_STREAM



