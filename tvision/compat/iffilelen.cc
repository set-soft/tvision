/*
 Copyright (c) 2001 by Salvador E. Tropea
 Covered by the GPL license.
 Note: I included CLY_IfStreamGetLine here. I know it could be confusing but
 adding a new file isn't good.
*/
#define Uses_filelength
#define Uses_fstream
#include <compatlayer.h>
#include <cl/needs.h>

UsingNamespaceStd

#if CLY_ISOCpp98
long CLY_ifsFileLength(ifstream &f)
{
 std::streampos pos=f.tellg();
 long ret=f.seekg(0,std::ios::end).tellg();
 f.seekg(pos,std::ios::beg);
 return ret;
}
#else
long CLY_ifsFileLength(ifstream &f)
{
 return filelength(f.rdbuf()->fd());
}
#endif

#ifdef NEEDS_IFSTREAMGETLINE
int CLY_IfStreamGetLine(ifstream &is, char *buffer, unsigned len)
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
