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
#define Uses_TRangeValidator
#define Uses_MsgBox
#include <tv.h>

TRangeValidator::TRangeValidator() : TFilterValidator("0123456789-+")
{
  Min = LONG_MIN;
  Max = LONG_MAX;
}

TRangeValidator::TRangeValidator(long aMin,long aMax) :
  TFilterValidator("xX0123456789+-")
{
  Min = aMin;
  Max = aMax;
  if (Min >= 0) strcpy(ValidChars,"xX0123456789+");
  if (Min < 0 && Max < 0) strcpy(ValidChars,"xX0123456789-");
}

void TRangeValidator::Error()
{
  if (Min >= 0)
    messageBox(mfError|mfOKButton,_("Value not in the range %lu to %lu"),
               (unsigned long)Min,(unsigned long)Max);
  else
    messageBox(mfError|mfOKButton,_("Value not in the range %li to %li"),
               Min,Max);
  // SET: Replaced %U and %D because MSVC lacks it
}

static long get_val(const char *buf)
{
  long val;
  if (buf[0] == '0')
  {
    if (buf[1] == 'x' || buf[1] == 'X')
      sscanf(buf+2, "%lx", &val);
    else
      sscanf(buf, "%lo", &val);
  }
  else
    sscanf(buf, "%ld", &val);
  return val;
}

static unsigned long get_uval(const char *buf)
{
  unsigned long val;
  if (buf[0] == '0')
  {
    if (buf[1] == 'x' || buf[1] == 'X')
      sscanf(buf+2, "%lux", &val);
    else
      sscanf(buf, "%luo", &val);
  }
  else
    sscanf(buf, "%lud", &val);
  return val;
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

