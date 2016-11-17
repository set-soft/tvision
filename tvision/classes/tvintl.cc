/* Internationalization support routines.
   Copyright by Salvador E. Tropea (SET) (2003-2005)
   Covered by the GPL license. */

#define Uses_string
#define Uses_ctype
#define Uses_stdlib
#define Uses_stdarg
#define Uses_stdio
#define Uses_snprintf
#define Uses_limits
#ifdef MSS
 #define __MSS_USED__
 #undef MSS
#endif 
#include <compatlayer.h>
#ifdef __MSS_USED__
 #define MSS
 #undef __MSS_USED__
#endif 
#include <locale.h>
#include <tv/ttypes.h>
#define Uses_intl_fprintf
#include <tv/intl.h>
#include <tv.h>

#ifdef HAVE_INTL_SUPPORT

#define Uses_TVCodePage
class TVPartitionTree556;
#include <tv/codepage.h>

// Prototypes, we know they exists
#ifndef _LIBINTL_H
extern "C" {
char *textdomain(const char *domainname);
char *bindtextdomain(const char *domainname, const char *dirname);
char *gettext(const char *msgid);
char *dgettext(const char *domain, const char *msgid);
char *gettext__(const char *msgid);
char *__gettext(const char *msgid);
}
#endif

// Small mess to determine which function provides "gettext"
// Must use __DJGPP__ here
#ifndef __DJGPP__
# if !defined(__GLIBC__) || __GLIBC__<2
   // By default use gettext
#  define LibGetTextLow gettext
# else
#  if (__GLIBC__==2 && __GLIBC_MINOR__>0) || __GLIBC__>2
#   define LibGetTextLow gettext
#  else
    // exception: glibc 2.0 needs __gettext
#   define LibGetTextLow __gettext
#  endif
# endif
#else
# define LibGetTextLow gettext__
#endif

char TVIntl::translate=0;
char TVIntl::initialized=0;
char TVIntl::needsRecode=0;
int  TVIntl::forcedCatalogEncoding=-1;
int  TVIntl::catalogEncoding=885901;
const char *TVIntl::catalogEncodingName="8859_1";
uchar TVIntl::recodeTable[256];
void *TVIntl::previousCPCallBack=NULL;
char  TVIntl::packageName[20];

const char *TVIntl::defaultEncodingNames[]=
{
 "de",
 "es",
 "pl",
 "ru"
};
int TVIntl::defaultEncodings[]=
{
 885901, // de
 885901, // es
 885902, // pl
 100000  // ru
};
const char *TVIntl::defaultEncodingsNames[]=
{
 "8859_1", // de
 "8859_1", // es
 "8859_2", // pl
 "KOI8R"   // ru
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
    CLY_snprintf(packageName,libPackageNameLen,"tvision%s",TV_VERSION);
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
 if (map && previousCPCallBack)
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
 const char *ret=bindtextdomain(domainname,dirname);
 bind_textdomain_codeset(domainname,catalogEncodingName);
 return ret;
}

const char *TVIntl::getText(const char *msgid)
{
 if (!msgid || !msgid[0]) // gettext 0.14.4 feature: "" -> "Project-Id-Version...
    return msgid;
 const char *msgstr=LibGetTextLow(msgid);
 if (msgid==msgstr)
    msgstr=dgettext(packageName,msgstr);
 return msgstr;
}

void TVIntl::recodeStr(char *str, int len)
{
 int i;
 uchar *s=(uchar *)str;
 for (i=0; i<len; i++)
     s[i]=s[i]>127 ? recodeTable[s[i]] : s[i];
}

char *TVIntl::getTextNew(const char *msgid, Boolean onlyIntl)
{
 if (!msgid) return NULL;
 const char *source;
 if (translate)
    source=getText(msgid);
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

#if defined(TVOS_DOS) || (defined(TVOS_Win32) && !defined(TVCompf_Cygwin))
 #define NoHomeOrientedOS
#endif

int TVIntl::autoInit(const char *package, const char *localeDir)
{
 char localedir[PATH_MAX];
 setlocale(LC_ALL, "");
 if (!localeDir)
    localeDir=getenv("LOCALEDIR");

 if (!localeDir)
   {
    #ifdef NoHomeOrientedOS
    // if LOCALEDIR doesn't exists use %DJDIR%/share/locale
    localeDir=getenv("DJDIR");
    if (localeDir)
      {
       strcpy(localedir,localeDir);
       strcat(localedir,"/share/locale");
      }
    else
       return 0;
    #else
    strcpy(localedir,"/usr/share/locale");
    #endif
   }
 else
    strcpy(localedir,localeDir);

 bindTextDomain(package,localedir);
 textDomain(package);
 return 1;
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

