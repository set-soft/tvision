#ifndef Included_SSC_Streams
#define Included_SSC_Streams 1

#define SSC_IOSSeekDir SSC_ios::seekdir
#define SSC_StreamOffT SSC_ios::seekoff
#define SSC_OpenModeT  SSC_ios::openmode

class SSC_ios
{
public:
 typedef int  seekdir;
 typedef long seekoff;
 typedef unsigned openmode;
 typedef unsigned iostate;

 static const seekdir beg=seekdir(0);
 static const seekdir cur=seekdir(SEEK_CUR);
 static const seekdir end=seekdir(SEEK_END);

 static const openmode app=1;
 static const openmode ate=2;
 static const openmode binary=4;
 static const openmode bin=4;
 static const openmode in=8;
 static const openmode out=16;
 static const openmode trunc=32;

 static const iostate badbit =  iostate(4);
 static const iostate eofbit =  iostate(2);
 static const iostate failbit = iostate(1);
 static const iostate goodbit = iostate(0);
};

class SSC_streambuf
{
public:
 virtual SSC_StreamOffT seekoff(SSC_StreamOffT pos, SSC_IOSSeekDir dir, SSC_OpenModeT mode=0);
         SSC_StreamOffT pubseekoff(SSC_StreamOffT pos, SSC_IOSSeekDir dir, SSC_OpenModeT mode=0)
  { return seekoff(pos,dir,mode); };
 virtual void   sync();
         void   pubsync() { sync(); };
 virtual void   sputc(char c);
 virtual void   sputn(char *data, size_t cant);
 virtual int    sbumpc();
 virtual size_t sgetn(char *data, size_t cant);
/*protected:
 SSC_streambuf(int =0) {};*/
 virtual ~SSC_streambuf () {}
};

class SSC_filebuf : public SSC_streambuf
{
public:
 SSC_filebuf();
 SSC_filebuf(int fh, SSC_OpenModeT mode);
 virtual ~SSC_filebuf() {}

 SSC_filebuf *open(const char *name, SSC_OpenModeT mode, int prot);
 int  is_open() { return opened; };
 SSC_filebuf *close(); // 0 on error
 SSC_filebuf *attach(int fh);
 virtual SSC_streambuf *setbuf(char *buffer, size_t len);
 virtual SSC_StreamOffT seekoff(SSC_StreamOffT pos, SSC_IOSSeekDir dir,
                                SSC_OpenModeT mode=0);
 virtual void   sync();
 virtual void   sputc(char c);
 virtual void   sputn(char *data, size_t cant);
 virtual int    sbumpc();
 virtual size_t sgetn(char *data, size_t cant);
 int fd() { return fileno(fde); };

protected:
 FILE *fde;
 int opened;
 SSC_OpenModeT mode;
};

/* Just for RHIDE, very elemental.
   Important!!! it doesn't check for int overflow!!! */
class SSC_strstreambuf : public SSC_streambuf
{
public:
 SSC_strstreambuf();
 SSC_strstreambuf(void *buf, int len);
 virtual ~SSC_strstreambuf();
 void *pbase() { return buffer; }
 int   pcount() { return length; }
 virtual SSC_StreamOffT seekoff(SSC_StreamOffT pos, SSC_IOSSeekDir dir,
                                SSC_OpenModeT mode=0);
 virtual void   sputc(char c);
 virtual void   sputn(char *data, size_t cant);
 virtual int    sbumpc();
 virtual size_t sgetn(char *data, size_t cant);

protected:
 void *buffer;
 int   length, offset, tlen;
 void  MakeRoomFor(size_t bytes);
};
#endif // Included_SSC_Streams

