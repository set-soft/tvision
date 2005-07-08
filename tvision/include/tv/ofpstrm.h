/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *

Modified by Robert H”hne to be used for RHIDE.
Modified to compile with gcc v3.x by Salvador E. Tropea, with the help of
Andris Pavenis and Christoph Bauer.

 *
 *
 */

/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   class ofpstream                                                       */
/*                                                                         */
/*   Base class for writing streamable objects to file streams             */
/*                                                                         */
/* ------------------------------------------------------------------------*/

#if defined( Uses_ofpstream ) && !defined( __ofpstream )
#define __ofpstream

class CLY_EXPORT ofpstream : public fpbase, public opstream
{
public:
 ofpstream();
 ofpstream(const char *,
           CLY_OpenModeT = CLY_IOSOut,
           int = CLY_FBOpenProtDef);
 ofpstream(int);
 ofpstream(int, char *, int);
 ~ofpstream();

 CLY_filebuf *rdbuf();
 void open(const char *,
           CLY_OpenModeT = CLY_IOSOut,
           int = CLY_FBOpenProtDef);
};

#endif  // Uses_ofpstream

