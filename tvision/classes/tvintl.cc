/* Internationalization support routines.
   Copyright by Salvador E. Tropea (SET) (2003)
   Covered by the GPL license. */

#define Uses_string
#define Uses_ctype
#define Uses_stdlib
#define Uses_stdarg
#define Uses_stdio
#define Uses_snprintf
#include <compatlayer.h>
#include <tv/ttypes.h>
#define Uses_intl_fprintf
#include <tv/intl.h>

#ifdef HAVE_INTL_SUPPORT

#define Uses_TVCodePage
class TVPartitionTree556;
#include <tv/codepage.h>

// Prototypes, we know they exists
extern "C" {
char *textdomain(const char *domainname);
char *bindtextdomain(const char *domainname, const char *dirname);
char *gettext(const char *msgid);
char *gettext__(const char *msgid);
char *__gettext(const char *msgid);
}

// Small mess to determine which function provides "gettext"
// Must use __DJGPP__ here
#ifndef __DJGPP__
# if !defined(__GLIBC__) || __GLIBC__<2
   // By default use gettext
#  define LibGetText gettext
# else
#  if (__GLIBC__==2 && __GLIBC_MINOR__>0) || __GLIBC__>2
#   define LibGetText gettext
#  else
    // exception: glibc 2.0 needs __gettext
#   define LibGetText __gettext
#  endif
# endif
#else
# define LibGetText gettext__
#endif

char TVIntl::translate=0;
char TVIntl::initialized=0;
char TVIntl::needsRecode=0;
int  TVIntl::forcedCatalogEncoding=-1;
int  TVIntl::catalogEncoding=885901;
uchar TVIntl::recodeTable[256];
void *TVIntl::previousCPCallBack=NULL;

const char *TVIntl::defaultEncodingNames[]=
{
 "de",
 "es",
 "ru"
};
int TVIntl::defaultEncodings[]=
{
 885901,
 885901,
 100000
};
const int numEncs=3;


void TVIntl::initialize()
{
 if (initialized) return;
 initialized=1;
 const char *lang=getenv("LANG");
 if (lang)
   {
    translate=1;
    int i;
    char c0=tolower(lang[0]);
    char c1=tolower(lang[1]);
    for (i=0; i<numEncs; i++)
        if (defaultEncodingNames[i][0]==c0 && defaultEncodingNames[i][1]==c1)
          {
           catalogEncoding=defaultEncodings[i];
           break;
          }
    previousCPCallBack=(void *)TVCodePage::SetCallBack(codePageCB);
    codePageCB(NULL);
   }
}

void TVIntl::codePageCB(ushort *map)
{
 int curAppCP=TVCodePage::GetAppCodePage();
 if (translate)
   {
    int cpCatalog=forcedCatalogEncoding==-1 ? catalogEncoding : forcedCatalogEncoding;
    needsRecode=(cpCatalog!=curAppCP);
    if (needsRecode)
       TVCodePage::FillGenericRemap(cpCatalog,curAppCP,recodeTable);
   }
 if (map)
    ((TVCodePageCallBack)previousCPCallBack)(map);
}

const char *TVIntl::textDomain(const char *domainname)
{
 initialize();
 return (const char *)textdomain(domainname);
}

const char *TVIntl::bindTextDomain(const char *domainname, const char *dirname)
{
 initialize();
 return (const char *)bindtextdomain(domainname,dirname);
}

const char *TVIntl::getText(const char *msgid)
{
 return (const char *)LibGetText(msgid);
}

void TVIntl::recodeStr(char *str, int len)
{
 int i;
 for (i=0; i<len; i++)
     str[i]=recodeTable[(uchar)str[i]];
}

char *TVIntl::getTextNew(const char *msgid, Boolean onlyIntl)
{
 if (!msgid) return NULL;
 const char *source;
 if (translate)
    source=(const char *)LibGetText(msgid);
 else
    source=msgid;
 if (onlyIntl && source==msgid)
    return NULL;
 int len=strlen(source)+1;
 char *ret=new char[len];
 memcpy(ret,source,len);
 if (needsRecode)
   {
    //printf("Recodificando %s a ",ret);
    recodeStr(ret,len-1);
    //printf("%s\n",ret);
   }
 return ret;
}

const char *TVIntl::getText(const char *msgid, stTVIntl *&cache)
{
 if (!translate || !msgid) return msgid;
 int curCP=TVCodePage::GetAppCodePage();
 if (!cache)
    cache=new stTVIntl;
 else
   {
    if (cache->cp==-2)
       return msgid;
    if (curCP==cache->cp)
       return cache->translation ? cache->translation : msgid;
    DeleteArray(cache->translation);
   }
 cache->translation=getTextNew(msgid,True);
 cache->cp=curCP;
 return cache->translation ? cache->translation : msgid;
}

void TVIntl::freeSt(stTVIntl *&cache)
{
 if (!cache) return;
 DeleteArray(cache->translation);
 delete cache;
 cache=NULL;
}

stTVIntl *TVIntl::emptySt()
{
 stTVIntl *p=new stTVIntl;
 p->translation=NULL;
 p->cp=-1;
 return p;
}

stTVIntl *TVIntl::dontTranslateSt()
{
 stTVIntl *p=new stTVIntl;
 p->translation=NULL;
 p->cp=-2;
 return p;
}

int TVIntl::snprintf(char *dest, size_t sz, const char *fmt, ...)
{
 va_list argptr;
 char *intlFmt=getTextNew(fmt);
 va_start(argptr,fmt);
 int l=CLY_vsnprintf(dest,sz,intlFmt,argptr);
 va_end(argptr);
 DeleteArray(intlFmt);
 return l;
}

int TVIntl::fprintf(FILE *f, const char *fmt, ...)
{
 va_list argptr;
 char *intlFmt=getTextNew(fmt);
 va_start(argptr,fmt);
 int l=vfprintf(f,intlFmt,argptr);
 va_end(argptr);
 DeleteArray(intlFmt);
 return l;
}
#else
int TVIntl::snprintf(char *dest, size_t sz, const char *fmt, ...)
{
 va_list argptr;
 va_start(argptr,fmt);
 int l=CLY_vsnprintf(dest,sz,fmt,argptr);
 va_end(argptr);
 return l;
}

int TVIntl::fprintf(FILE *f, const char *fmt, ...)
{
 va_list argptr;
 va_start(argptr,fmt);
 int l=vfprintf(f,fmt,argptr);
 va_end(argptr);
 return l;
}
#endif /* HAVE_INTL_SUPPORT */

