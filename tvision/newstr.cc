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

#define Uses_string
#include <tv.h>

char *newStr( const char *s )
{
    if( s == NULL )
        return NULL;
    char *temp = new char[ strlen(s)+1 ];
    strcpy( temp, s );
    return temp;
}
