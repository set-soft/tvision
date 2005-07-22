/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *

Modified by Robert H”hne to be used for RHIDE.
Modified by Salvador E. Tropea (to be compatible with TV 2.0)

 *
 *
 */
#define Uses_string
#define Uses_limits
#define Uses_stdio
#define Uses_ipstream
#define Uses_opstream
#define Uses_TFilterValidator
#define Uses_MsgBox
#include <tv.h>

TFilterValidator::TFilterValidator() :
  TValidator(),
  validChars(NULL)
{
}

TFilterValidator::TFilterValidator(const char * chars) :
  TValidator()
{
  validChars = newStr(chars);
}

TFilterValidator::~TFilterValidator()
{
  delete[] validChars;
}

Boolean TFilterValidator::isValid(const char *S)
{
  int i=0;
  while (S[i])
  {
    if (!strchr(validChars,S[i])) return False;
    i++;
  }
  return True;
}

Boolean TFilterValidator::isValidInput(char *S,Boolean)
{
  return TFilterValidator::isValid(S);
}

void TFilterValidator::error()
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
  os.writeString(validChars);
}

void * TFilterValidator::read(ipstream & is)
{
  TValidator::read(is);
  validChars = is.readString();
  return this;
}
#endif // NO_STREAM

