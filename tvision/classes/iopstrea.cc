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

#define Uses_pstream
#define Uses_iopstream
#include <tv.h>

iopstream::iopstream( streambuf * sb )
{
    pstream::init( sb );
}

iopstream::~iopstream()
{
}

iopstream::iopstream()
{
}

