/* Copyright 2000 (c) by Salvador Eduardo Tropea
   This code is part of the port of Turbo Vision to gcc, please see the rest
 of the file for copying information.
 */
/*
  This module provides the functions declared inline in vgaregs.h.
  They are needed only if you compile without optimizations.
*/
#include <tv/configtv.h>

#define NO_INLINE
typedef unsigned char uchar;
typedef unsigned short ushort;
#include "vgaregs.h"

#ifdef TVCompf_djgpp

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
 uchar a RegEAX;
 a=index;
 asm volatile (
"     outb %%al,%%dx          \n"
"     incl %%edx              \n"
"     inb  %%dx,%%al          \n"
      : "=a" (a), "=d" (dummy) : "a" (a), "d" (CRTController));
 return a;
}


uchar ReadGRA(uchar index)
{
 int dummy;
 uchar a RegEAX;
 a=index;
 asm volatile (
"     outb %%al,%%dx          \n"
"     incl %%edx              \n"
"     inb  %%dx,%%al          \n"
      : "=a" (a), "=d" (dummy) : "a" (a), "d" (GraphicsController));
 return a;
}

uchar ReadSEQ(uchar index)
{
 int dummy;
 uchar a RegEAX;
 a=index;
 asm volatile (
"     outb %%al,%%dx          \n"
"     incl %%edx              \n"
"     inb  %%dx,%%al          \n"
     : "=a" (a), "=d" (dummy)  : "a" (a), "d" (Sequencer));
 return a;
}

void WriteCRT(uchar index, uchar value)
{
 int dummy;
 asm volatile (
"     movb %1,%%ah            \n"
"     outw %%ax,%%dx          \n"
     : "=a" (dummy) : "qi" (value), "a" (index), "d" (CRTController));
}

void WriteGRA(uchar index, uchar value)
{
 int dummy;
 asm volatile (
"     movb %1,%%ah          \n"
"     outw %%ax,%%dx        \n"
     : "=a" (dummy) : "qi" (value), "a" (index), "d" (GraphicsController));
}

void WriteSEQ(uchar index, uchar value)
{
 int dummy;
 asm volatile (
"     movb %1,%%ah          \n"
"     outw %%ax,%%dx        \n"
     : "=a" (dummy) : "qi" (value), "a" (index), "d" (Sequencer));
}

void WaitVRT()
{
 asm volatile(
" 1:                         \n"
"     inb   %%dx,%%al        \n"
"     testb $8,%%al          \n"
"     jne 1b                 \n"
"     .align 2,0x90          \n"
" 2:                         \n"
"     inb %%dx,%%al          \n"
"     testb $8,%%al          \n"
"     je 2b                  \n"
      : : "d" (InputStatusRegister1) : "%eax" );
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
 asm volatile(
"     outb %%al,%%dx          \n"
"     incl %%edx              \n"
"     cli                     \n"
"     rep                     \n"
"     outsb                   \n"
"     sti                     \n"
: "=a" (dummy1), "=d" (dummy2), "=S" (dummy3), "=c" (dummy4)
: "c" (cant*3), "S" (_pal_ptr), "a" (color), "d" (0x3C8)
);
}

void RPF_GetPalRange(unsigned char *_pal_ptr, int color, int cant)
{
 int dummy1,dummy2,dummy3,dummy4;
 asm volatile (
"     outb %%al,%%dx          \n"
"     addl $2,%%edx           \n"
"     cli                     \n"
"     rep                     \n"
"     insb                    \n"
"     sti                     \n"
: "=a" (dummy1), "=d" (dummy2), "=D" (dummy3), "=c" (dummy4)
: "c" (cant*3), "D" (_pal_ptr), "a" (color), "d" (0x3C7)
);
}
#endif // DJGPP

