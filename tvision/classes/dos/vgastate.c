/* Copyright 1998-1999 (c) by Salvador Eduardo Tropea
   This code is part of the port of Turbo Vision to gcc, please see the rest
 of the file for copying information.
 */
/*****************************************************************************

  ROUTINES to store/retrieve the VGA registers.

*****************************************************************************/
#include <tv/configtv.h>

typedef unsigned char uchar;
typedef unsigned short ushort;
#include "vgaregs.h"

#ifdef TVCompf_djgpp

#include <sys/movedata.h>

/**[txh]********************************************************************

  Description:
  This function stores ALL the VGA registers in an array.

***************************************************************************/

static
int VGASaveRegs(uchar *regs)
{
 int i;
 uchar MORval;

 /* I'm trying to do this routine as strong and generic as possible
    without loosing performance.
    So I ever put the VGA chip in the color mode (ports=0x3Dx) but I
    store the real state */
 MORval=ReadMOR();
 WriteMOR(MORval | 1);
 regs[MORbase]=MORval;

 for (i=0; i<CRTcant; i++)
     regs[CRTbase+i]=ReadCRT(i);

 /******* The Attribute Registers are worst that a pain in the ass I think
          the @#*$ people from the Giant Blue tried to make it hard to
          understand on purpose ********/
 for (i=0; i<ATTcant; i++)
     regs[ATTbase+i]=ReadATT(i);
 ATTEndReads();

 for (i=0; i<GRAcant; i++)
     regs[GRAbase+i]=ReadGRA(i);

 for (i=0; i<SEQcant; i++)
     regs[SEQbase+i]=ReadSEQ(i);

 WriteMOR(MORval);

 return VGARegsCant;
}

/**[txh]********************************************************************

  Description:
  Restores the VGA registers from an array.

***************************************************************************/

static
void VGALoadRegs(const uchar *regs)
{
 int i;
 uchar CRT11,MORval;
 
 MORval=ReadMOR();
 /* Ensure we are in the color mode or the ports could be moved to 0x3Bx */
 WriteMOR(MORval | 1);
 /* Wait a full retrace to avoid extra noise in screen */
 while (inportb(0x3DA) & 8);
 while (!(inportb(0x3DA) & 8));

 /* Screen Off to avoid funny things displayed */
 WriteSEQ(0x01,regs[SEQbase+1] | 0x20);

 /* Synchronous reset ON, we must do it or we could lose video RAM contents */
 WriteSEQ(0x00,1);
 for (i=2; i<SEQcant; i++)
     WriteSEQ(i,regs[SEQbase+i]);
 /* Synchronous reset restored */
 WriteSEQ(0x00,regs[SEQbase]);

 /* Deprotect CRT registers 0-7 */
 CRT11=regs[CRTbase+0x11];
 WriteCRT(0x11,CRT11 & 0x7F);
 /* write CRT registers */
 for (i=0; i<CRTcant; i++)
     if (i!=0x11)
        WriteCRT(i,regs[CRTbase+i]);
 /* Restore the protection state */
 WriteCRT(0x11,CRT11);
 
 for (i=0; i<GRAcant; i++)
     WriteGRA(i,regs[GRAbase+i]);

 /******* The Attribute Registers are worst that a pain in the ass I think
          the @#*$ people from the Giant Blue tried to make it hard to
          understand on purpose ********/
 /* Ensure we will write to the index */
 inportb(ATTdir);
 for (; i<ATTcant; i++)
     WriteATT(i,regs[ATTbase+i]);
 ATTEndReads();

 /* Restore MOR */
 WriteMOR(regs[MORbase]);

 /* Restore Screen On/Off status */
 WriteSEQ(0x01,regs[SEQbase+1]);
}

#define BIOSDataAreaSize    96
#define BIOSDataAreaStart 0x49

static
void SaveBIOSDTA(uchar *dta)
{
 dosmemget(0x400+BIOSDataAreaStart,BIOSDataAreaSize,dta);
}

static
void LoadBIOSDTA(uchar *dta)
{
 dosmemput(dta,BIOSDataAreaSize,0x400+BIOSDataAreaStart);
}

#define VGAPalSize 768

static
void VGASavePal(uchar *pal)
{
 RPF_GetPalRange(pal,0,256);
}

static
void VGALoadPal(uchar *pal)
{
 RPF_SetPalRange(pal,0,256);
}

/**[txh]********************************************************************

  Description:
  Returns the amount of memory neededed to save the state.

  Return:
  int memory needed.

***************************************************************************/

int VGAStateGetBufferSize(void)
{
 return VGARegsCant+VGAPalSize+BIOSDataAreaSize;
}

/**[txh]********************************************************************

  Description:
  Stores the current video mode information in the provided buffer. The
buffer must be large enough to hold all the information.

  Return:
  !=0 => success

***************************************************************************/

int VGAStateSave(void *buffer)
{
 uchar *s=(uchar *)buffer;
 if (!s) return 0;
 VGASaveRegs(s);
 VGASavePal(s+VGARegsCant);
 SaveBIOSDTA(s+VGARegsCant+VGAPalSize);
 VGALoadRegs(s);
 return 1;
}

/**[txh]********************************************************************

  Description:
  Restores the current video mode information from the provided buffer.

  Return:
  !=0 => success

***************************************************************************/

int VGAStateLoad(void *buffer)
{
 uchar *s=(uchar *)buffer;
 if (!s) return 0;
 LoadBIOSDTA(s+VGARegsCant+VGAPalSize);
 VGALoadPal(s+VGARegsCant);
 VGALoadRegs(s);
 return 1;
}

#endif // DJGPP
