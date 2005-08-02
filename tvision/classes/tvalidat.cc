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

TValidator::TValidator() : status(0), options(0)
{
}

Boolean TValidator::isValid(const char *)
{
  return True;
}

Boolean TValidator::isValidInput(char *,Boolean)
{
  return True;
}

ushort TValidator::transfer(char *,void *,TVTransfer)
{
  return 0;
}

Boolean TValidator::validate(const char *S)
{
  if (isValid(S))
     return True;
  error();
  return False;
}

// Not in TV 2.0
void TValidator::format(char *)
{
}

void TValidator::error()
{
}

#if !defined( NO_STREAM )
TValidator::TValidator(StreamableInit)
{
}

void TValidator::write(opstream & os)
{
  os << status << options;
}

void * TValidator::read(ipstream & is)
{
  is >> status >> options;
  // status=0 TV 2.0
  return this;
}
#endif // NO_STREAM

