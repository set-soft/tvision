/*
 Copyright (c) 2001 by Salvador E. Tropea
 Covered by the GPL license.
*/
#define Uses_filelength
#include <compatlayer.h>
#include <fstream.h>

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
