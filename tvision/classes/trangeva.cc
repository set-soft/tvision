/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *

Modified by Robert H”hne to be used for RHIDE.
Heavily modified by Salvador E. Tropea (SET) to properly accept Hexadecimal
and octal numbers. Thanks to Andris Pavenis for suggestions.
 *
 *
 */
// SET: Moved the standard headers here because according to DJ
// they can inconditionally declare symbols like NULL
#define Uses_stdlib
#define Uses_string
#define Uses_limits
#define Uses_stdio

#define Uses_ipstream
#define Uses_opstream
#define Uses_TRangeValidator
#define Uses_MsgBox
#include <tv.h>

// SET: Added hexadecimal letters
#define  CT_VALID_CHARS "xX0123456789ABCDEFabcdef"
const char *ctValidChars="-+" CT_VALID_CHARS;
const char *ctValidCharsPos=ctValidChars+1;
const char *ctValidCharsNeg="-" CT_VALID_CHARS;

TRangeValidator::TRangeValidator() : TFilterValidator(ctValidChars)
{
  Min = LONG_MIN;
  Max = LONG_MAX;
}

TRangeValidator::TRangeValidator(long aMin,long aMax) :
  TFilterValidator(ctValidChars)
{
  Min = aMin;
  Max = aMax;
  if (Min >= 0) strcpy(ValidChars,ctValidCharsPos);
  if (Min < 0 && Max < 0) strcpy(ValidChars,ctValidCharsNeg);
}

void TRangeValidator::Error()
{
  if (Min >= 0)
    messageBox(mfError|mfOKButton,__("Value not in the range %lu to %lu"),
               (unsigned long)Min,(unsigned long)Max);
  else
    messageBox(mfError|mfOKButton,__("Value not in the range %li to %li"),
               Min,Max);
  // SET: Replaced %U and %D because MSVC lacks it
}

static long get_val(const char *buf)
{ // SET: Changed to use strtol
  char *end;
  return strtol(buf,&end,0);
}

static unsigned long get_uval(const char *buf)
{ // SET: Changed to use strtoul
  char *end;
  return strtoul(buf,&end,0);
}


Boolean TRangeValidator::IsValid(const char *S)
{
  long value;
  if (TFilterValidator::IsValid(S) == False) return False;
  if (Min >= 0)
    value = get_uval(S);
  else
    value = get_val(S);
  if (Min >= 0)
  {
    if ((unsigned long)value < (unsigned long)Min ||
        (unsigned long)value > (unsigned long)Max)
      return False;
  }
  else
  {
    if (value < Min || value > Max) return False;
  }
  return True;
}

ushort TRangeValidator::Transfer(char *S,void * Buffer,
  TVTransfer Flag)
{
  if (!(Options & voTransfer)) return 0;
  switch (Flag)
  {
    case vtGetData:
      if (Min >= 0)
        *((unsigned long *)Buffer) = get_uval(S);
      else
        *((long *)Buffer) = get_val(S);
      break;
    case vtSetData:
      if (Min >= 0)
        sprintf(S,"%lu",*((unsigned long *)Buffer));
      else
        sprintf(S,"%ld",*((long *)Buffer));
      break;
    default:
      break;
  }
  return sizeof(long);
}

#if !defined( NO_STREAM )
TRangeValidator::TRangeValidator(StreamableInit) :
  TFilterValidator(streamableInit)
{
}

TStreamable * TRangeValidator::build()
{
  return (TStreamable *) new TRangeValidator(streamableInit);
}

void TRangeValidator::write(opstream & os)
{
  TFilterValidator::write(os);
  os << Min << Max;
}

void * TRangeValidator::read(ipstream & is)
{
  TFilterValidator::read(is);
  is >> Min >> Max;
  return this;
}
#endif // NO_STREAM

