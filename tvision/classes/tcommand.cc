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

#define Uses_TCommandSet
#define Uses_string
#include <tv.h>

#include <stdlib.h>

#define MAX_COMMANDS (32 * 2048) /* 65536 commands */
#define COMMANDS_COUNT (MAX_COMMANDS / 32)
#define COMMANDS_SIZE (COMMANDS_COUNT * 4)

static
uint32 masks[32] =
{
    0x00000001,
    0x00000002,
    0x00000004,
    0x00000008,
    0x00000010,
    0x00000020,
    0x00000040,
    0x00000080,
    0x00000100,
    0x00000200,
    0x00000400,
    0x00000800,
    0x00001000,
    0x00002000,
    0x00004000,
    0x00008000,
    0x00010000,
    0x00020000,
    0x00040000,
    0x00080000,
    0x00100000,
    0x00200000,
    0x00400000,
    0x00800000,
    0x01000000,
    0x02000000,
    0x04000000,
    0x08000000,
    0x10000000,
    0x20000000,
    0x40000000,
    0x80000000
};

const uint32 fullMask=0xFFFFFFFF;

static inline
int loc( int cmd )
{
    return cmd / 32;
}

static inline
uint32 mask( int cmd )
{
    return masks[ cmd & 0x1F ];
}

static inline
uint32 rest( int cmd )
{
    return cmd & 0x1F;
}

TCommandSet::TCommandSet()
{
  cmds = (uint32 *)calloc(COMMANDS_COUNT,4);
}

void TCommandSet::enableAllCommands()
{
  memset(cmds,0xFF,COMMANDS_SIZE);
}

TCommandSet::~TCommandSet()
{
  free(cmds);
}

TCommandSet::TCommandSet( const TCommandSet& tc )
{
  cmds = (uint32 *)malloc(COMMANDS_SIZE);
  memcpy(cmds,tc.cmds,COMMANDS_SIZE);
}

TCommandSet& TCommandSet::operator = (const TCommandSet& tc)
{
  memcpy(cmds,tc.cmds,COMMANDS_SIZE);
  return *this;
}

Boolean TCommandSet::has( int cmd )
{
    return Boolean( (cmd >= MAX_COMMANDS ) ||
                    (cmds[ loc( cmd ) ] & mask( cmd )) != 0 );
}

void TCommandSet::disableCmd( int cmd )
{
  if (cmd < MAX_COMMANDS)
    cmds[ loc( cmd ) ] &= ~mask( cmd );
}

void TCommandSet::enableCmd( const TCommandSet& tc )
{
    for( int i = 0; i < COMMANDS_COUNT; i++ )
        cmds[i] |= tc.cmds[i];
}

void TCommandSet::disableCmd( const TCommandSet& tc )
{
    for( int i = 0; i < COMMANDS_COUNT; i++ )
        cmds[i] &= ~(tc.cmds[i]);
}

void TCommandSet::enableCmd( int cmd )
{
  if (cmd < MAX_COMMANDS)
    cmds[ loc( cmd ) ] |= mask( cmd );
}

/**[txh]********************************************************************

  Description:
  Enable a range of contiguous commands. As an example: my editor have more
than 128 commands, it means just 4 longs and not 128 calls! SET.@p
  Start is included but not end.

***************************************************************************/

void TCommandSet::enableCmd( int cmdStart, int cmdEnd )
{
  if (cmdEnd<MAX_COMMANDS && cmdEnd>cmdStart)
    {
     int pStart=loc(cmdStart);
     uint32 rStart=mask(cmdStart);
     int pEnd=loc(cmdEnd);
     uint32 rEnd=mask(cmdEnd);
     uint32 i;
     int j;

     // Both in the same word
     if (pStart==pEnd)
       {
        for (i=rStart; i!=rEnd; i<<=1)
            cmds[pStart]|=i;
        return;
       }
     // Not all of the pStart must be set
     if (rStart!=1)
       {
        for (i=rStart; i; i<<=1)
            cmds[pStart]|=i;
        pStart++;
       }
     // Set the words in the middle
     for (j=pStart; j<pEnd; j++)
         cmds[j]=fullMask;
     // The bits in the last position
     for (i=1; i<rEnd; i<<=1)
         cmds[pEnd]|=i;
    }
}

/**[txh]********************************************************************

  Description:
  Disable a range of contiguous commands. As an example: my editor have more
than 128 commands, it means just 4 longs and not 128 calls! SET.@p
  Start is included but not end.

***************************************************************************/

void TCommandSet::disableCmd( int cmdStart, int cmdEnd )
{
  if (cmdEnd<MAX_COMMANDS && cmdEnd>cmdStart)
    {
     int pStart=loc(cmdStart);
     uint32 rStart=mask(cmdStart);
     int pEnd=loc(cmdEnd);
     uint32 rEnd=mask(cmdEnd);
     uint32 i;
     int j;

     // Both in the same word
     if (pStart==pEnd)
       {
        for (i=rStart; i!=rEnd; i<<=1)
            cmds[pStart]&=~i;
        return;
       }
     // Not all of the pStart must be set
     if (rStart!=1)
       {
        for (i=rStart; i; i<<=1)
            cmds[pStart]&=~i;
        pStart++;
       }
     // Set the words in the middle
     for (j=pStart; j<pEnd; j++)
         cmds[j]=0;
     // The bits in the last position
     for (i=1; i<rEnd; i<<=1)
         cmds[pEnd]&=~i;
    }
}

TCommandSet& TCommandSet::operator &= ( const TCommandSet& tc )
{
    for( int i = 0; i < COMMANDS_COUNT; i++ )
        cmds[i] &= tc.cmds[i];
    return *this;
}

TCommandSet& TCommandSet::operator |= ( const TCommandSet& tc )
{
    for( int i = 0; i < COMMANDS_COUNT; i++ )
        cmds[i] |= tc.cmds[i];
    return *this;
}

TCommandSet operator & ( const TCommandSet& tc1, const TCommandSet& tc2 )
{
    TCommandSet temp( tc1 );
    temp &= tc2;
    return temp;
}

TCommandSet operator | ( const TCommandSet& tc1, const TCommandSet& tc2 )
{
    TCommandSet temp( tc1 );
    temp |= tc2;
    return temp;
}

Boolean TCommandSet::isEmpty()
{
    for( int i = 0; i < COMMANDS_COUNT; i++ )
        if( cmds[i] != 0 )
            return False;
    return True;
}

int operator == ( const TCommandSet& tc1, const TCommandSet& tc2 )
{
    for( int i = 0; i < COMMANDS_COUNT; i++ )
        if( tc1.cmds[i] != tc2.cmds[i] )
            return 0;
    return 1;
}

