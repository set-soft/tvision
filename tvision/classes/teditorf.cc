/* Modified by Robert Hoehne and Salvador Eduardo Tropea for the gcc port */
/*----------------------------------------------------------*/
/*                                                          */
/*   Turbo Vision 1.0                                       */
/*   Copyright (c) 1991 by Borland International            */
/*                                                          */
/*----------------------------------------------------------*/
/*****************************************************************************

  These are the routines that formats the lines before writing to the buffer.
They are a hack, the original TV had it made in assembler (edits.asm) and
Robert did a quick hack. We still using some of this code so isn't too bad
;-)

  JASC -> Jul 2000, endian stuff

*****************************************************************************/

#define Uses_TEditor
#include <tv.h>

#if defined(TV_BIG_ENDIAN)
 #define endianCol( letra, color ) ((((unsigned)letra)<<8) | ((unsigned)color))
#else
 #define endianCol( letra, color ) ((((unsigned)color)<<8) | ((unsigned)letra))
#endif

#define CALL if (call10(this,(ushort *)DrawBuf,color,count,offset,LinePtr,bufptr,Width) == False) return

Boolean call10(const TEditor *edit, ushort *drawBuf, ushort color, int cx,
               int &offset, unsigned &lineptr, int &bufptr, int Width)
{
  uchar c;
  int count = cx - lineptr;
  if (count<=0) return True;
  do {
    c = edit->buffer[lineptr++];
    if (c == 0x0a || c == 0x0d || c == 0x09)
    {
      if (c == 0x09)
      {
        do 
        {
         drawBuf[bufptr++] = endianCol(' ', color);
         offset++;
#if 1
        } while (offset & 7);
#else
        } while ((offset % TEditor::tabSize) != 0);
#endif
      }
      else
      {
        count = Width-offset;
        if (count<=0)
           return True;
        while (count--)
           drawBuf[bufptr++] = endianCol(' ', color);
        return False;
      }
    }
    else
    {
     drawBuf[bufptr++] = endianCol(c, color);
     offset++;
    }
    if (offset >= Width)
    {
      return False;
    }
    count--;
  } while (count);
  return True;
}

#define normalColor (Colors & 0xff)
#define selectColor (Colors >> 8)

void TEditor::formatLine( void *DrawBuf, uint32 LinePtr, int Width,
                          ushort Colors )
{
  int count,offset,bufptr;
  ushort color;
  bufptr = 0;
  offset = 0;
  if (selStart > LinePtr)
  {
    color = normalColor;
    count = selStart;
    CALL;
  }
  color = selectColor;
  count = curPtr;
  CALL;
  LinePtr += gapLen;
  count = selEnd+gapLen;
  CALL;
  color = normalColor;
  count = bufSize;
  CALL;
  count = Width-offset;
  //if (count<=offset) return;
  if (count<1) return;
  while (count--) ((ushort *)(DrawBuf))[bufptr++] = endianCol(' ', color);
}

