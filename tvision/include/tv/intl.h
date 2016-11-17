/* Internationalization support routines header.
   Copyright by Salvador E. Tropea (SET) (2003-2005)
   Covered by the GPL license. */
/* The dummies are in nointl.h to make this header less complex */

#ifndef TVIntl_Included
#define TVIntl_Included

#ifdef FORCE_INTL_SUPPORT
 #define HAVE_INTL_SUPPORT 1
#else
 #include <tv/configtv.h>
#endif

#ifndef HAVE_INTL_SUPPORT

#include <tv/nointl.h>

#else

#ifdef __cplusplus
struct stTVIntl
{
 char *translation;
 int cp;
};

const int libPackageNameLen=20;

class CLY_EXPORT TVIntl
{
public:
 TVIntl() {};
 // Set the domain or catalog of translations (usually the name of the program)
 static const char *textDomain(const char *domainname);
 // Indicate a directory to search for catalogs
 static const char *bindTextDomain(const char *domainname, const char *dirname);
 // Translate the message
 static const char *getText(const char *msgid);
 // What we think is a good way to initialize the domain
 static int         autoInit(const char *package, const char *localeDir=NULL);

 // Special (and tricky members)
 // Set the encoding for the catalog
 static void        setCatalogEncoding(int cp) { forcedCatalogEncoding=cp; };
 // Dis/Enable translations
 static void        enableTranslations() { initialize(); };
 static void        disableTranslations() { translate=0; };
 // Returns a newly allocated string and translated (if enabled)
 static char       *getTextNew(const char *msgid, Boolean onlyIntl=False);
 static const char *getText(const char *msgid, stTVIntl *&cache);
 static void        freeSt(stTVIntl *&cache);
 static stTVIntl   *emptySt();
 static stTVIntl   *dontTranslateSt();
 // Replacements that translates and recodes the format string
 static int         snprintf(char *dest, size_t sz, const char *fmt, ...);
 #ifdef Uses_intl_fprintf
 static int         fprintf(FILE *f, const char *fmt, ...);
 #endif

protected:
 // Enable translations
 static char translate;
 // Class already initialized
 static char initialized;
 // Indicates if the catalog must be recoded to match the application code page.
 static char needsRecode;
 // Used to change the implicit encoding
 static int  forcedCatalogEncoding;
 // Assumed catalog encoding. This is OS independent and is an arbitrary
 // criteria for TV. It makes easier to create portable programs.
 static int  catalogEncoding;
 static const char *catalogEncodingName;
 // Default encodings used for TV. Defined only for the available translations.
 static const char *defaultEncodingNames[];
 static int defaultEncodings[];
 static const char *defaultEncodingsNames[];
 static void *previousCPCallBack;
 static uchar recodeTable[256];
 static char  packageName[libPackageNameLen];

 static void initialize();
 static void codePageCB(ushort *map);
 static void recodeStr(char *str, int len);
};

#ifndef _
 #define _(msg) TVIntl::getText(msg)
#endif

#ifndef __
 #define __(msg) (msg)
#endif

#ifndef BINDTEXTDOMAIN
 #define BINDTEXTDOMAIN(a,b) TVIntl::bindTextDomain(a,b)
#endif

#ifndef TEXTDOMAIN
 #define TEXTDOMAIN(a) TVIntl::textDomain(a)
#endif

#else /* __cplusplus */

#ifndef _
 #ifdef HAVE_INTL_SUPPORT
  char *gettext(const char *msgid);
  #define _(msg) gettext(msg)
 #else
  #define _(msg) (msg)
 #endif
#endif

#ifndef __
 #define __(msg) (msg)
#endif

#ifndef BINDTEXTDOMAIN
 char *bindtextdomain(const char *domainname, const char *dirname);
 #define BINDTEXTDOMAIN(a,b) bindtextdomain(a,b)
#endif

#ifndef TEXTDOMAIN
 char *textdomain(const char *domainname);
 #define TEXTDOMAIN(a) textdomain(a)
#endif

#endif /* __cplusplus */

#endif /* HAVE_INTL_SUPPORT */

#endif /* TVIntl_Included */

