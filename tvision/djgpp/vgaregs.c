/* Copyright 2000 (c) by Salvador Eduardo Tropea
   This code is part of the port of Turbo Vision to gcc, please see the rest
 of the file for copying information.
 */
/*
  This module provides the functions declared inline in vgaregs.h.
  They are needed only if you compile without optimizations.
*/
#include <pc.h>
#define NO_INLINE
#include "vgaregs.h"

typedef unsigned char uchar;
typedef unsigned short ushort;

#ifdef SAFE_IO
/* C approach, safier than my assembler ;-) */

uchar ReadCRT(uchar index)
{
 outportb(CRTControllerIndex,index);
 return inportb(CRTControllerData);
}

uchar ReadGRA(uchar index)
{
 outportb(GraphicsControllerIndex,index);
 return inportb(GraphicsControllerData);
}

uchar ReadSEQ(uchar index)
{
 outportb(SequencerIndex,index);
 return inportb(SequencerData);
}

void WriteCRT(uchar index, uchar value)
{
 outportb(CRTControllerIndex,index);
 outportb(CRTControllerData,value);
}


void WriteGRA(uchar index, uchar value)
{
 outportb(GraphicsControllerIndex,index);
 outportb(GraphicsControllerData,value);
}

void WriteSEQ(uchar index, uchar value)
{
 outportb(SequencerIndex,index);
 outportb(SequencerData,value);
}

void WaitVRT()
{
 while (inportb(InputStatusRegister1) & 8);
 while (!(inportb(InputStatusRegister1) & 8));
}

#else
uchar ReadCRT(uchar index)
{
 int dummy;
 uchar a asm("%eax");
 a=index;
 asm volatile ("
     outb %%al,%%dx
     incl %%edx
     inb  %%dx,%%al
     " : "=a" (a), "=d" (dummy) : "a" (a), "d" (CRTController));
 return a;
}


uchar ReadGRA(uchar index)
{
 int dummy;
 uchar a asm("%eax");
 a=index;
 asm volatile ("
     outb %%al,%%dx
     incl %%edx
     inb  %%dx,%%al
     " : "=a" (a), "=d" (dummy) : "a" (a), "d" (GraphicsController));
 return a;
}

uchar ReadSEQ(uchar index)
{
 int dummy;
 uchar a asm("%eax");
 a=index;
 asm volatile ("
     outb %%al,%%dx
     incl %%edx
     inb  %%dx,%%al
     " : "=a" (a), "=d" (dummy)  : "a" (a), "d" (Sequencer));
 return a;
}

void WriteCRT(uchar index, uchar value)
{
 int dummy;
 asm volatile ("
     movb %1,%%ah
     outw %%ax,%%dx
     " : "=a" (dummy) : "qi" (value), "a" (index), "d" (CRTController));
}

void WriteGRA(uchar index, uchar value)
{
 int dummy;
 asm volatile ("
     movb %1,%%ah
     outw %%ax,%%dx
     " : "=a" (dummy) : "qi" (value), "a" (index), "d" (GraphicsController));
}

void WriteSEQ(uchar index, uchar value)
{
 int dummy;
 asm volatile ("
     movb %1,%%ah
     outw %%ax,%%dx
     " : "=a" (dummy) : "qi" (value), "a" (index), "d" (Sequencer));
}

void WaitVRT()
{
 asm volatile("
 1:
     inb   %%dx,%%al
     testb $8,%%al
     jne 1b
     .align 2,0x90
 2:
     inb %%dx,%%al
     testb $8,%%al
     je 2b
     " : : "d" (InputStatusRegister1) : "%eax" );
}

#endif

uchar ReadATT(int index)
{
 /* Ensure we will write to the index */
 inportb(ATTdir);
 /* Set the index and disable the screen or we will read nothing */
 outportb(ATTindex,index);
 return inportb(ATTdataR);
}

void ATTEndReads(void)
{
 /* Ensure we will write to the index */
 inportb(ATTdir);
 /* Enable the screen */
 outportb(ATTindex,0x20);
}

void WriteATT(int index, int val)
{
 outportb(ATTindex,index);
 outportb(ATTdataW,val);
}

uchar ReadMOR(void)
{
 return inportb(MORdataR);
}

void WriteMOR(int val)
{
 outportb(MORdataW,val);
}

uchar ReadEDAC(int index)
{
 outportb(EDACindex,index);
 return inportb(EDACdata);
}

void WriteEDAC(int index, int val)
{
 outportb(EDACindex,index);
 outportb(EDACdata,val);
}

void RPF_SetPalRange(unsigned char *_pal_ptr, int color, int cant)
{
 int dummy1,dummy2,dummy3,dummy4;
__asm__ __volatile__("
     outb %%al,%%dx
     incl %%edx
     cli
     rep
     outsb
     sti"
: "=a" (dummy1), "=d" (dummy2), "=S" (dummy3), "=c" (dummy4)
: "c" (cant*3), "S" (_pal_ptr), "a" (color), "d" (0x3C8)
);
}

void RPF_GetPalRange(unsigned char *_pal_ptr, int color, int cant)
{
 int dummy1,dummy2,dummy3,dummy4;
__asm__ __volatile__("
     outb %%al,%%dx
     addl $2,%%edx
     cli
     rep
     insb
     sti"
: "=a" (dummy1), "=d" (dummy2), "=D" (dummy3), "=c" (dummy4)
: "c" (cant*3), "D" (_pal_ptr), "a" (color), "d" (0x3C7)
);
}
