#include <tv/configtv.h>

#ifdef HAVE_SSC

#define Uses_SSC_Streams
#define Uses_stdio
#define Uses_unistd
#define Uses_fcntl
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
    printf("Error en open\n");
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

#ifdef TEST
int main(int argc, char *argv[])
{
 CLY_filebuf pp;
 pp.open("test.txt",CLY_IOSIn | CLY_IOSOut | CLY_IOSBin,0644);
 pp.sputn("Hola\n",5);
 pp.pubseekoff(0,CLY_IOSBeg);
 char buf[6];
 pp.sgetn(buf,5);
 buf[5]=0;
 printf("%s\n",buf);
 return 0;
}
#endif

#endif

