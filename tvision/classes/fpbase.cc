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

UsingNamespaceStd

fpbase::fpbase()
{
 buf=new CLY_int_filebuf();
 pstream::init(buf);
}

fpbase::fpbase(int f)
{
 CLY_NewFBFromFD(buf,f);
 pstream::init(buf);
}

fpbase::fpbase( const char *name, CLY_OpenModeT omode, int prot )
{
 buf=new CLY_int_filebuf();
 open(name,omode,prot);
 pstream::init(buf);
}

fpbase::fpbase( int f, char *b, int len )
{
 CLY_NewFBFromFD(buf,f);
 buf->CLY_PubSetBuf(b,len);
 pstream::init(buf);
}

void fpbase::setbuf(char* b, int len)
{
 if (buf->CLY_PubSetBuf(b,len))
    clear(CLY_IOSGoodBit);
 else
    setstate(CLY_IOSFailBit);
}

#ifdef CLY_HaveFBAttach
void fpbase::attach(int f)
{
 if (buf->is_open())
    setstate(CLY_IOSFailBit);
 else if(buf->attach(f))
    clear(CLY_IOSGoodBit);
 else
    clear(CLY_IOSBadBit);
}
#endif

fpbase::~fpbase()
{
 delete buf;
}

void fpbase::open( const char *b, CLY_OpenModeT m, int prot )
{
 if (buf->is_open())
    clear(CLY_IOSFailBit);        // fail - already open
 else if (buf->CLY_FBOpen(b,m,prot))
    clear(CLY_IOSGoodBit);        // successful open
 else
    clear(CLY_IOSBadBit);     // open failed
}

void fpbase::close()
{
 if (buf->close())
    clear(CLY_IOSGoodBit);
 else
    setstate(CLY_IOSFailBit);
}

CLY_filebuf *fpbase::rdbuf()
{
 return buf;
}

#ifdef CLY_DefineSpecialFileBuf
// gcc 3.1 specific
CLY_int_filebuf *CLY_int_filebuf::open(FILE *f, ios_base::openmode mode)
{
 CLY_int_filebuf *ret=NULL;
 if (!this->is_open())
   {
    _M_file.sys_open(f,mode);
    if (this->is_open())
      {
       _M_allocate_internal_buffer();
       _M_mode=mode;
       
       // For time being, set both (in/out) sets  of pointers.
       _M_set_indeterminate();
       if ((mode & ios_base::ate) &&
           this->seekoff(0,ios_base::end,mode)<0)
          this->close();
       ret=this;
      }
   }
 return ret;
}

CLY_int_filebuf *CLY_int_filebuf::open(int h, ios_base::openmode mode)
{
 CLY_int_filebuf *ret=NULL;
 if (!this->is_open())
   {
    _M_file.sys_open(h,mode,false);
    if (this->is_open())
      {
       _M_allocate_internal_buffer();
       _M_mode=mode;
       
       // For time being, set both (in/out) sets  of pointers.
       _M_set_indeterminate();
       if ((mode & ios_base::ate) &&
           this->seekoff(0,ios_base::end,mode)<0)
          this->close();
       ret=this;
      }
   }
 return ret;
}
#endif // CLY_DefineSpecialFileBuf

