/* Copyright (C) 2001 by Salvador E. Tropea (SET),
   see copyrigh file for details */
/**[txh]********************************************************************

  Description:
  This file provides dummies for the libintl.a library so you can create
programs without international support even if your Turbo Vision library
was configured to have it.
  
***************************************************************************/

char *textdomain(char *domainname)
{
 return domainname;
}

char *bindtextdomain(char *domainname, char *dirname)
{
 return domainname ? domainname : dirname;
}

char *gettext(char *msgid)
{
 return msgid;
}

char *gettext__(char *msgid)
{
 return msgid;
}
