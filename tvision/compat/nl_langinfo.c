/*
 Copyright (C) 2001-2005      Salvador E. Tropea
 Covered by the GPL license.
*/

#include <cl/needs.h>

#ifdef NEEDS_NL_LANGINFO

#define Uses_CLY_nl_langinfo
#define Uses_string
#define Uses_stdlib
#define Uses_stdio  // OW needs it
#include <compatlayer.h>
#include <locale.h>

char *CLY_nl_langinfo(nl_item item)
{
 static char *OldLocale=0;
 static struct lconv *locales;
 char *ret="";
 char *NewLocale;

 /* Is that really needed? */
 /* Get the locales and retain it */
 if (!OldLocale)
   {
    OldLocale=strdup(setlocale(LC_ALL,NULL));
    locales=localeconv();
   }
 else
   {
    NewLocale=setlocale(LC_ALL,NULL);
    if (strcmp(NewLocale,OldLocale))
      {
       free(OldLocale);
       OldLocale=strdup(NewLocale);
       locales=localeconv();
      }
   }

 switch (item)
   {
    #define C(a,b) case a: ret=locales->b; break;
    C(CURRENCY_SYMBOL,currency_symbol)
    C(DECIMAL_POINT,decimal_point)
    C(GROUPING,grouping)
    C(INT_CURR_SYMBOL,int_curr_symbol)
    C(MON_DECIMAL_POINT,mon_decimal_point)
    C(MON_GROUPING,mon_grouping)
    C(NEGATIVE_SIGN,negative_sign)
    C(POSITIVE_SIGN,positive_sign)
    C(THOUSANDS_SEP,thousands_sep)
    #undef C
    #define C(a,b) case a: ret=&locales->b; break;
    C(FRAC_DIGITS    ,frac_digits)
    C(INT_FRAC_DIGITS,int_frac_digits)
    C(N_CS_PRECEDES  ,n_cs_precedes)
    C(N_SEP_BY_SPACE ,n_sep_by_space)
    C(P_SIGN_POSN    ,p_sign_posn)
    C(N_SIGN_POSN    ,n_sign_posn)
    C(P_CS_PRECEDES  ,p_cs_precedes)
    C(P_SEP_BY_SPACE ,p_sep_by_space)
    #undef C
   }

 return ret;
}

#endif // NEEDS_NL_LANGINFO

