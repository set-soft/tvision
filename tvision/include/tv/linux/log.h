/**[txh]********************************************************************

  Debug logger helper Copyright 1996-1998 by Robert Hoehne
  Description:
  Used to debug Linux driver.
  I (SET) moved it here to avoid repeating it in each module.
  The idea is from Robert, I usually use another approach.
  
***************************************************************************/

#if defined(TVOSf_Linux) && !defined(LOGLINUXDEBUG_INCLUDED)
#define LOGLINUXDEBUG_INCLUDED

//#define DEBUG
#ifdef DEBUG
 extern char *program_invocation_short_name;
 #define LOG(s) do {CLY_std(cerr) << program_invocation_short_name << ": " << s << CLY_std(endl); fflush(stderr);} while(0)
#else
 #define LOG(s) do {;} while(0)
#endif

#else
 #define LOG(s) do {;} while(0)
#endif // LOGLINUXDEBUG_INCLUDED

