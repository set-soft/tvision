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

#define Uses_TPWrittenObjects
#include <tv.h>

TPWObj::TPWObj( const void *adr, P_id_type id ) : address( adr ), ident( id )
{
}

