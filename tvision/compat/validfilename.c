/*
 Copyright (C) 1996-1998 Robert H”hne
 Copyright (C) 2000      Vadim Beloborodov
 Copyright (C) 2000      Anatoli Soltan
 Copyright (C) 2000      Salvador E. Tropea
 Covered by the GPL license.
*/

#define Uses_string
#include <compatlayer.h>

#ifdef TVCompf_djgpp
#include <fcntl.h>
static const char * const IllegalChars1 = ";,=+<>|\"[]/";
static const char * const IllegalChars2 = "<>|/\"";

inline static
const char *GetInvalidChars(const char *fileName)
{
 // SET: Added a check for LFNs, in LFN drives ;,[]=+ are valid
 if (_use_lfn(fileName))
    return IllegalChars2;
 return IllegalChars1;
}
#else
static const char * const IllegalChars = "<>|/\"";

static
const char *GetInvalidChars(const char *unused)
{
 if (unused) unused=0;
 return IllegalChars;
}
#endif

int CLY_ValidFileName(const char *fileName)
{
  static const char * illegalChars;
  const char *slash=strrchr(fileName,DIRSEPARATOR);
  
  illegalChars=GetInvalidChars(fileName);
  if (!slash)
  {
    if (strpbrk(fileName,illegalChars) != 0)
      return 0;
    return 1;
  }
  if (strpbrk(slash+1,illegalChars) != 0)
    return 0;
  return CLY_PathValid(fileName);
}
