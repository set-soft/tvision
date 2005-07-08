/*
 Copyright (c) 2001 by Salvador E. Tropea
 Covered by the GPL license.
 Note: I included CLY_IfStreamGetLine here. I know it could be confusing but
 adding a new file isn't good.
*/
#include <tv/configtv.h>
#ifndef HAVE_SSC

#define Uses_filelength
#define Uses_fstream
#include <compatlayer.h>

#include <cl/needs.h>

UsingNamespaceStd

#if CLY_ISOCpp98
long CLY_ifsFileLength(ifstream &f)
{
 std::streampos pos=f.tellg();
 long ret=f.seekg(0,CLY_IOSEnd).tellg();
 f.seekg(pos,CLY_IOSBeg);
 return ret;
}
#else
CLY_EXPORT long CLY_ifsFileLength(ifstream &f)
{
 return filelength(f.rdbuf()->fd());
}
#endif

#ifdef NEEDS_IFSTREAMGETLINE
CLY_EXPORT int CLY_IfStreamGetLine(ifstream &is, char *buffer, unsigned len)
{
 if (is.get(buffer,len))
   {
    char c;
    is.get(c); // grab trailing newline
    return 1;
   }
 return 0;
}
#endif // NEEDS_IFSTREAMGETLINE

#endif
