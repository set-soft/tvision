/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *

Modified by Robert H”hne to be used for RHIDE.
Modified to compile with gcc v3.x by Salvador E. Tropea, with the help of
Andris Pavenis.

 *
 *
 */

#if defined( Uses_otstream ) && !defined( __otstream )
#define __otstream

class otstream : public CLY_std(ostream)
{
public:
 otstream( TTerminal * );
};

#endif

