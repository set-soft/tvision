/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *

Modified by Robert H”hne to be used for RHIDE.
Modified to compile with gcc v3.x by Salvador E. Tropea, with the help of
Andris Pavenis and Christoph Bauer.

 *
 *
 */

#define Uses_pstream
#define Uses_fpbase
#define Uses_PubStreamBuf
#include <tv.h>

fpbase::fpbase()
{
 buf=new filebuf();
 pstream::init(buf);
}

fpbase::fpbase(int f)
{
 buf=CLY_NewFBFromFD(f);
 pstream::init(buf);
}

fpbase::fpbase( const char *name, CLY_OpenModeT omode, int prot )
{
 buf=new filebuf();
 open(name,omode,prot);
 pstream::init(buf);
}

fpbase::fpbase( int f, char *b, int len )
{
 buf=CLY_NewFBFromFD(f);
 buf->CLY_PubSetBuf(b,len);
 pstream::init(buf);
}

void fpbase::setbuf(char* b, int len)
{
 if (buf->CLY_PubSetBuf(b,len))
    clear(ios::goodbit);
 else
    setstate(ios::failbit);
}

#ifdef CLY_HaveFBAttach
void fpbase::attach(int f)
{
 if (buf->is_open())
    setstate(ios::failbit);
 else if(buf->attach(f))
    clear(ios::goodbit);
 else
    clear(ios::badbit);
}
#endif

fpbase::~fpbase()
{
 delete buf;
}

void fpbase::open( const char *b, CLY_OpenModeT m, int prot )
{
 if (buf->is_open())
    clear(ios::failbit);        // fail - already open
 else if (buf->CLY_FBOpen(b,m,prot))
    clear(ios::goodbit);        // successful open
 else
    clear(ios::badbit);     // open failed
}

void fpbase::close()
{
 if (buf->close())
    clear(ios::goodbit);
 else
    setstate(ios::failbit);
}

filebuf *fpbase::rdbuf()
{
 return buf;
}

