/* Copyright 1998-2002 (c) by Salvador Eduardo Tropea
   This code is part of the port of Turbo Vision to gcc, please see the rest
 of the file for copying information.
 */

#if defined(TVCompf_djgpp) && !defined(VGAREGS_HEADER_INCLUDED)
#define VGAREGS_HEADER_INCLUDED

#include <pc.h>

/*  CRT (Cathode Rays Tube) Controller Registers
    They control the sync signals.
    0x00 to 0x18 */
// SVGALib and XFree86 uses <0x18 so they miss the line compare register,
// don't know why.
#define CRTbase  0
#define CRTcant  25
/*  ATT Attribute Controller Registers
    They control some attributes like 16 colors palette, overscan color, etc.
    0x00 to 0x14 */
#define ATTbase  (CRTbase+CRTcant)
#define ATTcant  21
/*  GRA Graphics Controller Registers
    They control the read/write mode to the video memory.
    0x00 to 0x08 */
#define GRAbase  (ATTbase+ATTcant)
#define GRAcant  9
/*  SEQ Sequence Registers
    They control how the memory is scanned.
    0x00 to 0x04 */
#define SEQbase  (GRAbase+GRAcant)
#define SEQcant  5
/*  MOR Miscellaneous Output Register
    1 register */
#define MORbase  (SEQbase+SEQcant)
#define MORcant  1

#define VGARegsCant (MORbase+MORcant)

/* (I don't take care about mono by now) */
/* VGA Atttibute controller */
#define ATTindex                 0x3C0
#define ATTdataW                 0x3C0
#define ATTdataR                 0x3C1
#define ATTdir                   0x3DA
/* Miscellaneous Output Register */
#define MORdataW                 0x3C2
#define MORdataR                 0x3CC
/* VGA Sequencer Registers */
#define Sequencer                0x3C4
#define SequencerIndex           0x3C4
#define SequencerData            0x3C5
/* VGA Palette Registers */
#define ReadDataAddress          0x3C7
#define WriteDataAddress         0x3C8
#define PaletteDataRegister      0x3C9
/* VGA Graphics Controller Registers */
#define GraphicsController       0x3CE
#define GraphicsControllerIndex  0x3CE
#define GraphicsControllerData   0x3CF
/* VGA CRT Controller Register */
#define CRTController            0x3D4
#define CRTControllerIndex       0x3D4
#define CRTControllerData        0x3D5
/* VGA Input Status Register 1 */
#define InputStatusRegister1     0x3DA
/* Trident's DAC/Clock */
#define EDACindex                0x83C8
#define EDACdata                 0x83C6

#if __GNUC__<3
 // GCC 3.0 doesn't support it for local variables.
 // But takes the right way.
 #define RegEAX  asm("%eax")
#else
 #define RegEAX
#endif

#ifndef NO_INLINE
//#define SAFE_IO

#ifdef SAFE_IO
/* C approach, safier than my assembler ;-) */

extern inline
uchar ReadCRT(uchar index)
{
 outportb(CRTControllerIndex,index);
 return inportb(CRTControllerData);
}

extern inline
uchar ReadGRA(uchar index)
{
 outportb(GraphicsControllerIndex,index);
 return inportb(GraphicsControllerData);
}

extern inline
uchar ReadSEQ(uchar index)
{
 outportb(SequencerIndex,index);
 return inportb(SequencerData);
}

extern inline
void WriteCRT(uchar index, uchar value)
{
 outportb(CRTControllerIndex,index);
 outportb(CRTControllerData,value);
}


extern inline
void WriteGRA(uchar index, uchar value)
{
 outportb(GraphicsControllerIndex,index);
 outportb(GraphicsControllerData,value);
}

extern inline
void WriteSEQ(uchar index, uchar value)
{
 outportb(SequencerIndex,index);
 outportb(SequencerData,value);
}

extern inline
void WaitVRT()
{
 while (inportb(InputStatusRegister1) & 8);
 while (!(inportb(InputStatusRegister1) & 8));
}

#else
/*
  Assembler stuff: It is normally more compact and in some cases faster. As
  the functions are inline I think size is important.
  They save 524 bytes in the driver (1.89%).
*/

extern inline
uchar ReadCRT(uchar index)
{
 int dummy;
 uchar a RegEAX;
 a=index;
 asm volatile (
"     outb %%al,%%dx    \n"
"     incl %%edx        \n"
"     inb  %%dx,%%al    \n"
      : "=a" (a), "=d" (dummy) : "a" (a), "d" (CRTController));
 return a;
}


extern inline
uchar ReadGRA(uchar index)
{
 int dummy;
 uchar a RegEAX;
 a=index;
 asm volatile (
"     outb %%al,%%dx    \n"
"     incl %%edx        \n"
"     inb  %%dx,%%al    \n"
      : "=a" (a), "=d" (dummy) : "a" (a), "d" (GraphicsController));
 return a;
}

extern inline
uchar ReadSEQ(uchar index)
{
 int dummy;
 uchar a RegEAX;
 a=index;
 asm volatile (
"     outb %%al,%%dx    \n"
"     incl %%edx        \n"
"     inb  %%dx,%%al    \n"
      : "=a" (a), "=d" (dummy)  : "a" (a), "d" (Sequencer));
 return a;
}

extern inline
void WriteCRT(uchar index, uchar value)
{
 int dummy;
 asm volatile (
"     movb %1,%%ah      \n"
"     outw %%ax,%%dx    \n"
     : "=a" (dummy) : "qi" (value), "a" (index), "d" (CRTController));
}

extern inline
void WriteGRA(uchar index, uchar value)
{
 int dummy;
 asm volatile (
"     movb %1,%%ah      \n"
"     outw %%ax,%%dx    \n"
     : "=a" (dummy) : "qi" (value), "a" (index), "d" (GraphicsController));
}

extern inline
void WriteSEQ(uchar index, uchar value)
{
 int dummy;
 asm volatile (
"     movb %1,%%ah      \n"
"     outw %%ax,%%dx    \n"
      : "=a" (dummy) : "qi" (value), "a" (index), "d" (Sequencer));
}


extern inline
void WaitVRT()
{
 asm volatile(
" 1:                       \n"
"     inb   %%dx,%%al      \n"
"     testb $8,%%al        \n"
"     jne 1b               \n"
"     .align 2,0x90        \n"
" 2:                       \n"
"     inb %%dx,%%al        \n"
"     testb $8,%%al        \n"
"     je 2b                \n"
      : : "d" (InputStatusRegister1) : "%eax" );
}

#endif

extern inline
uchar ReadATT(int index)
{
 /* Ensure we will write to the index */
 inportb(ATTdir);
 /* Set the index and disable the screen or we will read nothing */
 outportb(ATTindex,index);
 return inportb(ATTdataR);
}

extern inline
void ATTEndReads(void)
{
 /* Ensure we will write to the index */
 inportb(ATTdir);
 /* Enable the screen */
 outportb(ATTindex,0x20);
}

extern inline
void WriteATT(int index, int val)
{
 outportb(ATTindex,index);
 outportb(ATTdataW,val);
}

extern inline
uchar ReadMOR(void)
{
 return inportb(MORdataR);
}

extern inline
void WriteMOR(int val)
{
 outportb(MORdataW,val);
}

extern inline
uchar ReadEDAC(int index)
{
 outportb(EDACindex,index);
 return inportb(EDACdata);
}

extern inline
void WriteEDAC(int index, int val)
{
 outportb(EDACindex,index);
 outportb(EDACdata,val);
}

extern inline
void RPF_SetPalRange(unsigned char *_pal_ptr, int color, int cant)
{
 int dummy1,dummy2,dummy3,dummy4;
 asm volatile (
"     outb %%al,%%dx    \n"
"     incl %%edx        \n"
"     cli               \n"
"     rep               \n"
"     outsb             \n"
"     sti               \n"
: "=a" (dummy1), "=d" (dummy2), "=S" (dummy3), "=c" (dummy4)
: "c" (cant*3), "S" (_pal_ptr), "a" (color), "d" (0x3C8)
);
}

extern inline
void RPF_GetPalRange(unsigned char *_pal_ptr, int color, int cant)
{
 int dummy1,dummy2,dummy3,dummy4;
 asm volatile (
"     outb %%al,%%dx    \n"
"     addl $2,%%edx     \n"
"     cli               \n"
"     rep               \n"
"     insb              \n"
"     sti               \n"
: "=a" (dummy1), "=d" (dummy2), "=D" (dummy3), "=c" (dummy4)
: "c" (cant*3), "D" (_pal_ptr), "a" (color), "d" (0x3C7)
);
}
#endif // NO_INLINE

#endif // defined(TVCompf_djgpp) && !defined(VGAREGS_HEADER_INCLUDED)

