/* Internationalization support routines header.
   Copyright by Salvador E. Tropea (SET) (2003)
   Covered by the GPL license. */

#ifdef FORCE_INTL_SUPPORT
 #define HAVE_INTL_SUPPORT 1
#else
 #include <tv/configtv.h>
#endif

#ifndef TVIntl_Included
#define TVIntl_Included

struct stTVIntl
{
 char *translation;
 int cp;
};

class TVIntl
{
public:
 TVIntl() {};
 // Set the domain or catalog of translations (usually the name of the program)
 static const char *textDomain(const char *domainname);
 // Indicate a directory to search for catalogs
 static const char *bindTextDomain(const char *domainname, const char *dirname);
 // Translate the message
 static const char *getText(const char *msgid);

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
 // Default encodings used for TV. Defined only for the available translations.
 static const char *defaultEncodingNames[];
 static int defaultEncodings[];
 static void *previousCPCallBack;
 static uchar recodeTable[256];

 static void initialize();
 static void codePageCB(ushort *map);
 static void recodeStr(char *str, int len);
};

#ifndef _
 #ifdef HAVE_INTL_SUPPORT
  #define _(msg) TVIntl::getText(msg)
 #else
  #define _(msg) (msg)
 #endif
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

#endif // TVIntl_Included

