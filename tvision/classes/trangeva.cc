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

TRangeValidator::TRangeValidator() :
  TFilterValidator(ctValidChars)
{
  min = LONG_MIN;
  max = LONG_MAX;
}

TRangeValidator::TRangeValidator(long aMin,long aMax) :
  TFilterValidator(ctValidChars)
{
  min = aMin;
  max = aMax;
  if (min >= 0) strcpy(validChars,ctValidCharsPos);
  if (min < 0 && max < 0) strcpy(validChars,ctValidCharsNeg);
}

void TRangeValidator::error()
{
  if (min >= 0)
    messageBox(mfError|mfOKButton,__("Value not in the range %lu to %lu"),
               (unsigned long)min,(unsigned long)max);
  else
    messageBox(mfError|mfOKButton,__("Value not in the range %li to %li"),
               min,max);
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


Boolean TRangeValidator::isValid(const char *S)
{
  long value;
  if (TFilterValidator::isValid(S) == False) return False;
  if (min >= 0)
    value = get_uval(S);
  else
    value = get_val(S);
  if (min >= 0)
  {
    if ((unsigned long)value < (unsigned long)min ||
        (unsigned long)value > (unsigned long)max)
      return False;
  }
  else
  {
    if (value < min || value > max) return False;
  }
  return True;
}

ushort TRangeValidator::transfer(char *S,void * Buffer,
  TVTransfer Flag)
{
  if (!(options & voTransfer)) return 0;
  switch (Flag)
  {
    case vtGetData:
      if (min >= 0)
        *((unsigned long *)Buffer) = get_uval(S);
      else
        *((long *)Buffer) = get_val(S);
      break;
    case vtSetData:
      if (min >= 0)
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
  os << min << max;
}

void * TRangeValidator::read(ipstream & is)
{
  TFilterValidator::read(is);
  is >> min >> max;
  return this;
}
#endif // NO_STREAM

