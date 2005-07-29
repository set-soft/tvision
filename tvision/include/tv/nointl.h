/* Internationalization support dummies routines header.
   Copyright by Salvador E. Tropea (SET) (2003-2005)
   Covered by the GPL license. */
/* Included from intl.h never directly */

#ifdef __cplusplus
struct stTVIntl
{
 char *translation;
 int cp;
};

extern CLY_EXPORT char *newStr( const char * );

class CLY_EXPORT TVIntl
{
public:
 TVIntl() {}
 static const char *textDomain(const char *) { return NULL; }
 static const char *bindTextDomain(const char *, const char *) { return NULL; }
 static int         autoInit(const char *package, const char *localeDir=NULL) { return 1; }
 static const char *getText(const char *msgid) { return msgid; }
 static void        setCatalogEncoding(int ) {}
 static void        enableTranslations() {}
 static void        disableTranslations() {}
 static char       *getTextNew(const char *msgid, Boolean onlyIntl=False)
                    { return onlyIntl ? NULL : newStr(msgid); }
 static const char *getText(const char *msgid, stTVIntl *&)
                    { return msgid; }
 static void        freeSt(stTVIntl *&) {}
 static stTVIntl   *emptySt() { return NULL; }
 static stTVIntl   *dontTranslateSt() { return NULL; }
 static int         snprintf(char *dest, size_t sz, const char *fmt, ...);
 #ifdef Uses_intl_fprintf
 static int         fprintf(FILE *f, const char *fmt, ...);
 #endif
};
#endif /* __cplusplus */

#ifndef _
 #define _(msg) (msg)
#endif

#ifndef __
 #define __(msg) (msg)
#endif

#ifndef BINDTEXTDOMAIN
 #define BINDTEXTDOMAIN(a,b)
#endif

#ifndef TEXTDOMAIN
 #define TEXTDOMAIN(a)
#endif
