#include <tv/configtv.h>

#ifdef HAVE_SSC

#define Uses_SSC_Streams
#define Uses_stdio
#define Uses_stdlib
#define Uses_unistd
#define Uses_fcntl
#define Uses_string
#include <compatlayer.h>

// Ultra dummies:
SSC_StreamOffT SSC_streambuf::seekoff(SSC_StreamOffT , SSC_IOSSeekDir,
                                      SSC_OpenModeT)
{
 return SSC_StreamOffT(0);
}

void SSC_streambuf::sync() {}

void SSC_streambuf::sputc(char ) {}

void SSC_streambuf::sputn(char *, size_t ) {}

int SSC_streambuf::sbumpc() { return EOF; }

size_t SSC_streambuf::sgetn(char *, size_t )
{
 return size_t(0);
}

SSC_filebuf::SSC_filebuf()
{
 opened=0;
 fde=NULL;
 mode=0;
}

SSC_filebuf::SSC_filebuf(int fh, SSC_OpenModeT aMode)
{
 mode=aMode;
 opened=1;
 attach(fh);
}

SSC_filebuf *SSC_filebuf::open(const char *name, SSC_OpenModeT aMode, int prot)
{
 //printf("Open %s\n",name);
 mode=aMode;
 int flags=0;
 // Note: glibc docs lies, O_RDWR!=(O_RDONLY|O_WRONLY)
 if (mode & SSC_ios::in)
   {
    if (mode & SSC_ios::out)
       flags|=O_RDWR | O_CREAT;
    else
       flags|=O_RDONLY;
   }
 else
   {
    if (mode & SSC_ios::out)
       flags|=O_WRONLY | O_CREAT;
   }
 if (mode & SSC_ios::trunc) flags|=O_TRUNC;
 if (mode & SSC_ios::app)   flags|=O_APPEND;
 int fh=::open(name,flags,prot);
 if (fh==-1)
   {
    //printf("Error en open\n");
    return NULL;
   }
 if (mode & SSC_ios::ate)
    lseek(fh,0,SEEK_END);
 attach(fh);
 return this;
}

SSC_filebuf *SSC_filebuf::attach(int fh)
{
 char sMode[4];
 int index=0;

 if (!mode) mode=SSC_ios::in|SSC_ios::out|SSC_ios::binary;

 if (mode & SSC_ios::in)
    sMode[index++]='r';
 else if (mode & SSC_ios::out)
    sMode[index++]='w';

 sMode[index++]=mode & SSC_ios::binary ? 'b' : 't';

 if ((mode & SSC_ios::in) && (mode & SSC_ios::out))
    sMode[index++]='+';

 sMode[index]=0;
 fde=fdopen(fh,sMode);
 if (fde)
    opened=1;
 else
    perror("fdopen");
 //printf("Attach %d %s %d\n",fh,sMode,opened);

 return this;
}

SSC_filebuf *SSC_filebuf::close()
{
 if (!opened)
    return 0;
 opened=0;
 if (fclose(fde)==0)
    return this;
 return 0;
}

SSC_streambuf *SSC_filebuf::setbuf(char *buffer, size_t len)
{
 if (!opened) return NULL;
 if (setvbuf(fde,buffer,_IOFBF,len)==0)
    return (SSC_streambuf *)1; // :-))
 return NULL;
}

SSC_StreamOffT SSC_filebuf::seekoff(SSC_StreamOffT pos, SSC_IOSSeekDir dir,
                                    SSC_OpenModeT)
{
 if (!opened) return SSC_StreamOffT(0);
 fseek(fde,pos,dir);
 return (SSC_StreamOffT)ftell(fde);
}

void SSC_filebuf::sync()
{
 if (opened)
    fflush(fde);
}

void SSC_filebuf::sputc(char c)
{
 if (opened)
    fputc(c,fde);
}

void SSC_filebuf::sputn(char *data, size_t cant)
{
 if (opened)
    fwrite(data,cant,1,fde);
}

int SSC_filebuf::sbumpc()
{
 if (opened)
    return fgetc(fde);
 return EOF;
}

size_t SSC_filebuf::sgetn(char *data, size_t cant)
{
 if (opened)
    return fread(data,1,cant,fde);
 return size_t(0);
}

SSC_strstreambuf::SSC_strstreambuf()
{
 buffer=NULL;
 tlen=length=offset=0;
}

SSC_strstreambuf::SSC_strstreambuf(void *buf, int len)
{
 buffer=buf;
 tlen=length=len;
 offset=0;
}

SSC_StreamOffT SSC_strstreambuf::seekoff(SSC_StreamOffT pos, SSC_IOSSeekDir dir,
                                    SSC_OpenModeT)
{
 switch (dir)
   {
    case SSC_ios::beg:
         if (pos>=0 && pos<=length)
            offset=pos;
         break;
    case SSC_ios::cur:
         if (pos>0)
           {
            if (length-offset>=pos)
               offset+=pos;
           }
         else
           {
            if (offset>=pos)
               offset+=pos;
           }
         break;
    case SSC_ios::end:
         if (pos<=0 && pos<=length)
            offset=length+pos;
         break;
   }
 return offset;
}

void SSC_strstreambuf::sputc(char c)
{
 MakeRoomFor(1);
 char *s=(char *)buffer;
 s[offset++]=c;
}

void SSC_strstreambuf::sputn(char *data, size_t cant)
{
 MakeRoomFor(cant);
 memcpy(((char *)buffer)+offset,data,cant);
 offset+=cant;
}

int SSC_strstreambuf::sbumpc()
{
 if (offset<length)
   {
    char *s=(char *)buffer;
    return s[offset++];
   }
 return EOF;
}

size_t SSC_strstreambuf::sgetn(char *data, size_t cant)
{
 if (!buffer || !length)
    return 0;
 size_t avail=length-offset;
 if (cant>avail)
    cant=avail;
 memcpy(data,((char *)buffer)+offset,cant);
 offset+=cant;
 return cant;
}

SSC_strstreambuf::~SSC_strstreambuf()
{
 free(buffer);
}

void SSC_strstreambuf::MakeRoomFor(size_t bytes)
{
 size_t avail=length-offset;
 if (avail>=bytes)
    return;
 size_t tavail=tlen-offset;
 if (tavail>=bytes)
   {
    length=offset+bytes;
    return;
   }
 tlen=bytes+offset+4096;
 buffer=realloc(buffer,tlen);
 if (!buffer)
    abort();
 length=offset+bytes;
}

#ifdef TEST
// g++ -DTEST -o /tmp/test -I../include ssc_streams.cc 

int main(int argc, char *argv[])
{
 CLY_filebuf pp;
 pp.open("test.txt",CLY_IOSIn | CLY_IOSOut | CLY_IOSBin,0644);
 pp.sputn("Hola\n",5);
 pp.pubseekoff(0,CLY_IOSBeg);
 char buf[6+5];
 pp.sgetn(buf,5);
 buf[5]=0;
 printf("%s\n",buf);

 printf("----- strbase test\n");
 CLY_strstreambuf pp2;
 pp2.sputn("Hola\n",5);
 pp2.sputn("Chau\n",5);
 pp2.pubseekoff(0,CLY_IOSBeg);
 pp2.sgetn(buf,5+5);
 buf[5+5]=0;
 printf("%s",buf);

 return 0;
}
#endif

#endif

