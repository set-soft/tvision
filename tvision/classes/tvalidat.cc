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
#define Uses_TValidator
#include <tv.h>

TValidator::TValidator() : Status(0), Options(0), Owner(NULL)
{
}

Boolean TValidator::IsValid(const char *)
{
  return True;
}

Boolean TValidator::IsValidInput(char *,Boolean)
{
  return True;
}

ushort TValidator::Transfer(char *,void *,TVTransfer)
{
  return 0;
}

Boolean TValidator::Valid(const char *S)
{
  if (IsValid(S) == True) return True;
  Error();
  return False;
}

void TValidator::Format(char *)
{
}

#if !defined( NO_STREAM )
TValidator::TValidator(StreamableInit)
{
}

void TValidator::write(opstream & os)
{
  os << Status << Options;
}

void * TValidator::read(ipstream & is)
{
  is >> Status >> Options;
  return this;
}
#endif // NO_STREAM

