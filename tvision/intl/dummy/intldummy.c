/* Copyright (C) 2001-2003 by Salvador E. Tropea (SET),
   see copyrigh file for details */
/**[txh]********************************************************************

  Description:
  This file provides dummies for the libintl.a library so you can create
programs without international support even if your Turbo Vision library
was configured to have it.
  
***************************************************************************/

char *textdomain(const char *domainname)
{
 return (char *)domainname;
}

char *bindtextdomain(const char *domainname, const char *dirname)
{
 return (char *)(domainname ? domainname : dirname);
}

char *gettext(const char *msgid)
{
 return (char *)msgid;
}

char *dgettext(const char *domain, const char *msgid)
{
 return (char *)msgid;
}

char *gettext__(const char *msgid)
{
 return (char *)msgid;
}
