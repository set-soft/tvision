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
 // This needs iostream and looks like when we compile with gcc 3.x it ins't defined
 // at this point.
 #ifndef Included_iostream
   #include IOSTREAM_HEADER
 #endif
 extern char *program_invocation_short_name;
 #define LOG(s) do {cerr << program_invocation_short_name << ": " << s << endl; fflush(stderr);} while(0)
#else
 #define LOG(s) do {;} while(0)
#endif

#else
 #define LOG(s) do {;} while(0)
#endif // LOGLINUXDEBUG_INCLUDED

