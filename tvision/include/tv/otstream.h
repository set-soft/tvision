/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *

Modified by Robert H”hne to be used for RHIDE.

 *
 *
 */

#if defined( Uses_otstream ) && !defined( __otstream )
#define __otstream

#include <iostream.h>

class otstream : public ostream
{

public:

    otstream( TTerminal * );

};

#endif

