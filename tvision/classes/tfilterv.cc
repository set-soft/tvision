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
#define Uses_string
#include <limits.h>
#include <stdio.h>

#define Uses_ipstream
#define Uses_opstream
#define Uses_TFilterValidator
#define Uses_MsgBox
#include <tv.h>

TFilterValidator::TFilterValidator() :
  TValidator(),
  ValidChars(NULL)
{
}

TFilterValidator::TFilterValidator(const char * chars) :
  TValidator()
{
  ValidChars = new char[strlen(chars)+1];
  strcpy(ValidChars,chars);
}

TFilterValidator::~TFilterValidator()
{
  delete ValidChars;
}

Boolean TFilterValidator::IsValid(const char *S)
{
  int i=0;
  while (S[i])
  {
    if (!strchr(ValidChars,S[i])) return False;
    i++;
  }
  return True;
}

Boolean TFilterValidator::IsValidInput(char *S,Boolean)
{
  return TFilterValidator::IsValid(S);
}

void TFilterValidator::Error()
{
  messageBox(__("Invalid character in input"),mfError | mfOKButton);
}

#if !defined( NO_STREAM )
TFilterValidator::TFilterValidator(StreamableInit) :
  TValidator(streamableInit)
{
}

TStreamable * TFilterValidator::build()
{
  return (TStreamable *) new TFilterValidator(streamableInit);
}

void TFilterValidator::write(opstream & os)
{
  TValidator::write(os);
  os.writeString(ValidChars);
}

void * TFilterValidator::read(ipstream & is)
{
  TValidator::read(is);
  ValidChars = is.readString();
  return this;
}
#endif // NO_STREAM

