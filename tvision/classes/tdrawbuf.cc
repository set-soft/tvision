/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *
 *
 *
 */

#define Uses_TDrawBuffer
#include <tv.h>

/**[txh]********************************************************************

  Description:
  Copies count bytes from source to the data buffer starting at indent. The
function uses the provided attribute, but if the attribute is 0 the attribute
in the buffer is unmodified.@p
  Added a check to avoid writings passing the end of the buffer.@p

***************************************************************************/

void TDrawBuffer::moveBuf(unsigned indent, const void *source,
                          unsigned attr, unsigned count)

{
  if (!count || (indent >= (unsigned)maxViewWidth)) return;
  if (count+indent > (unsigned)maxViewWidth)
    count = maxViewWidth - indent;

  ushort *dest = &data[indent];
  const uchar  *s = (const uchar *)source;

  if (attr)
  {
    while (count-- && *s)
    {
      ((uchar *)dest)[0] = *s++;
      ((uchar *)dest++)[1] = (uchar)attr;
    }
  }
  else
  {
    while (count-- && *s)
    {
      *(uchar *)dest++ = *s++;
    }
  }
}

/**[txh]********************************************************************

  Description:
  Fills count bytes in the buffer starting at the indent position. If the
attribute is 0 the original is left unchanged. If the character is 0 only
the attribute is used.@p
  Added a check to avoid writings passing the end of the buffer.@p

***************************************************************************/

void TDrawBuffer::moveChar(unsigned indent, char c, unsigned attr,
                           unsigned count )
{
  if (!count || (indent >= (unsigned)maxViewWidth)) return;
  if (count+indent > (unsigned)maxViewWidth)
    count = maxViewWidth - indent;

  ushort *dest=&data[indent];

  if (attr)
  {
    if (c)
    {
      while (count--)
      {
        ((uchar*)dest)[0] = c;
        ((uchar*)dest++)[1] = attr;
      }
    }
    else
    {
      dest=(ushort *)((uchar *)dest+1);
      while (count--)
      {
        *(uchar *)dest++ = attr;
      }
    }
  }
  else
  {
    while (count--)
    {
      *(uchar *)dest++ = c;
    }
  }
}

/**[txh]********************************************************************

  Description:
  That's the same as moveStr but the attrs parameter holds two attributes
the lower 8 bits are the normal value and the upper 8 bits define the
attribute to be used for text enclosed by ASCII 126. @x{::moveStr}.@p
  The routine was modified to avoid writes passing the end of the buffer.
Additionally was re-writed in assembler (I guess the Borland's original code
was assembler, but I didn't take a look to it) because the check slow downs
the routine so I wanted to avoid a lose in performance. SET.

***************************************************************************/

void TDrawBuffer::moveCStr( unsigned indent, const char *str, unsigned attrs )
{
#if !defined(TVCPU_x86) || !defined(TVComp_GCC)
//$todo: implement it in asm for Win32
  uchar bh = attrs >> 8, ah = attrs & 0xff;
  uchar al;
  ushort *dest = data+indent;
  ushort *end = data+maxViewWidth;
  while (*str && dest<end)
  {
    al = *str++;
    if (al == '~')
    {
      al = ah;
      ah = bh;
      bh = al;
    }
    else
    {
      ((uchar*)dest)[0] = al;
      ((uchar*)dest++)[1] = ah;
    }
  }
#else
  asm("                                           \n\
   // EDI = &data[indent]                         \n\
   // ESI = &data[maxViewWidth]                   \n\
   // EDX = str                                   \n\
   // EAX = attrs                                 \n\
                                                  \n\
// SET: It isn't supposed to be needed if I put   \n\
// the register in the clobered list, but ...     \n\
// gcc 2.7.x have a bug when handling it and      \n\
// fails to save/restore bug when handling it and \n\
// fails to save/restore it.                      \n\
//   It didn't show until I compiled with -fPIC   \n\
// to create a dynamic ELF library. In this case  \n\
// EBX is used as a base for the dynamic jmp      \n\
// table and the code misserably crash.           \n\
// I found it thanks to the disassemble window.   \n\
     pushl %%ebx                                  \n\
                                                  \n\
     // showed                                    \n\
     // save the attributes in EBX                \n\
        movl %%eax,%%ebx                          \n\
     // take the initial attribute in AH          \n\
        movb %%al,%%ah                            \n\
     // first character                           \n\
        movb (%%edx),%%al                         \n\
     // EOS ?                                     \n\
        cmpb $0,%%al                              \n\
        je 2f                                     \n\
        .align 2,0x90                             \n\
                                                  \n\
3:                                                \n\
     // indent >= maxViewWidth ?                  \n\
        cmpl %%esi,%%edi                          \n\
        jae 2f                                    \n\
                                                  \n\
        incl %%edx                                \n\
     // character == '~' ?                        \n\
        cmpb $126,%%al                            \n\
        jne 0f                                    \n\
     // exchange the attributes                   \n\
        movb %%ah,%%al                            \n\
        movb %%bh,%%ah                            \n\
        movb %%al,%%bh                            \n\
        jmp 1f                                    \n\
        .align 2,0x90                             \n\
                                                  \n\
0:                                                \n\
        movw %%ax,(%%edi)                         \n\
        addl $2,%%edi                             \n\
                                                  \n\
1:                                                \n\
        movb (%%edx),%%al                         \n\
        cmpb $0,%%al                              \n\
        jne 3b                                    \n\
2:                                                \n\
        popl %%ebx                                \n\
   " : : "D"(&data[indent]), "S"(&data[maxViewWidth]), "d"(str), "a"(attrs)
     //: "%ebx" I save EBX because 2.7.x forgets it
  );
#endif
}

/**[txh]********************************************************************

  Description:
  Writes a string in the buffer with the provided attribute. The routine
copies until the EOS is found or the buffer is filled.@p
  Modified to avoid writes passing the end of the buffer. Optimized for
32 bits. Translated to asm just for fun, I think is a little bit faster.
SET.

***************************************************************************/

void TDrawBuffer::moveStr( unsigned indent, const char *str, unsigned attr )
{
#if !defined(TVCPU_x86) || !defined(TVComp_GCC)
//$todo: implement it in asm
  ushort *dest = data+indent;
  ushort *end = data+maxViewWidth;
  while (*str && dest<end)
  {
    ((uchar*)dest)[0] = *(str++);
    ((uchar*)dest++)[1] = attr;
  }
#else
  asm("                                           \n\
   // EDI = &data[indent]                         \n\
   // ESI = &data[maxViewWidth]                   \n\
   // EDX = str                                   \n\
   // EAX = attr                                  \n\
     // Move the attribute to the high byte       \n\
        movb %%al,%%ah                            \n\
                                                  \n\
     // First byte in the string                  \n\
        movb (%%edx),%%al                         \n\
        cmpb $0,%%al                              \n\
        je 2f                                     \n\
        .align 2,0x90                             \n\
                                                  \n\
3:                                                \n\
     // Bounds check                              \n\
        cmpl %%esi,%%edi                          \n\
        jae 2f                                    \n\
                                                  \n\
     // Next byte                                 \n\
        incl %%edx                                \n\
                                                  \n\
     // Store it                                  \n\
        movw %%ax,(%%edi)                         \n\
        addl $2,%%edi                             \n\
                                                  \n\
     // Check EOS                                 \n\
        movb (%%edx),%%al                         \n\
        cmpb $0,%%al                              \n\
        jne 3b                                    \n\
2:                                                \n\
   " : : "D"(&data[indent]), "S"(&data[maxViewWidth]), "d"(str), "a"(attr)
  );
#endif
}


