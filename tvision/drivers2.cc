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

#define Uses_TKeys
#include <tv.h>

/*------------------------------------------------------------------------*/
/*                                                                        */
/*  ctrlToArrow                                                           */
/*                                                                        */
/*  argument:                                                             */
/*                                                                        */
/*      keyCode - scan code to be mapped to keypad arrow code             */
/*                                                                        */
/*  returns:                                                              */
/*                                                                        */
/*      scan code for arrow key corresponding to Wordstar key,            */
/*      or original key code if no correspondence exists                  */
/*                                                                        */
/*------------------------------------------------------------------------*/
ushort ctrlToArrow(ushort keyCode)
{

static const ushort ctrlCodes[] =
    {
    kbCtrlS, kbCtrlD, kbCtrlE, kbCtrlX, kbCtrlA,
    kbCtrlF, kbCtrlG, kbCtrlV, kbCtrlR, kbCtrlC, kbCtrlH
    };

static const ushort arrowCodes[] =
    {
    kbLeft, kbRight, kbUp, kbDown, kbHome,
    kbEnd,  kbDel,   kbIns,kbPgUp, kbPgDn, kbBack
    };

    /* The keycode contains now also the shift flags, which the
       caller don't want to see */
    ushort _keyCode = keyCode & 0x7F;

    for( unsigned i = 0; i<(sizeof(ctrlCodes)/sizeof(ushort)) ; i++ )
        if( _keyCode==ctrlCodes[i] )
            return arrowCodes[i];
    /* If it was not found, return the original code */
    return keyCode;
}

/*------------------------------------------------------------------------*/
/*                                                                        */
/*  cstrlen                                                               */
/*                                                                        */
/*  argument:                                                             */
/*                                                                        */
/*      s       - pointer to 0-terminated string                          */
/*                                                                        */
/*  returns                                                               */
/*                                                                        */
/*      length of string, ignoring '~' characters.                        */
/*                                                                        */
/*  Comments:                                                             */
/*                                                                        */
/*      Used in determining the displayed length of command strings,      */
/*      which use '~' to toggle between display attributes                */
/*                                                                        */
/*------------------------------------------------------------------------*/

int cstrlen( const char *s )
{
    int len = 0;
    while( *s != EOS )
        {
        if( *s++ != '~' )
            len++;
        }
    return len;
}

