/*
  Copyright (c) 2001 by Salvador E. Tropea (SET) <set@ieee.org>
  This code is covered by the GPL license. A copy of the license should
be provided in the same package.
*/
#include <stdio.h>
#include <string.h>
#include <tv/alcon/alcon.h>
#include <allegro/internal/aintern.h>
#define Uses_TScreen
#define Uses_TEvent
#define Uses_TGKey
#include <tv.h>
#include <tv/gkey.h>

FONT sfont;
FONT_GLYPH *ascii_data[256];
FONT_MONO_DATA ascii_monofont =
{
 0x0, 0x100,                 /* begin, end characters */
 ascii_data,                 /* the data set */
 0                           /* next */
};

FONT cursorFont;
FONT_GLYPH *cursorData[2];
FONT_MONO_DATA cursorMonoFont =
{
 0x0, 0x2,                   /* begin, end characters */
 cursorData,                 /* the data set */
 0                           /* next */
};

typedef struct
{
 unsigned char r,g,b;
} PalCol;

static
PalCol BIOSPalette[16]={
{ 0x00, 0x00, 0x00 },
{ 0x00, 0x00, 0xA8 },
{ 0x00, 0xA8, 0x00 },
{ 0x00, 0xA8, 0xA8 },
{ 0xA8, 0x00, 0x00 },
{ 0xA8, 0x00, 0xA8 },
{ 0xA8, 0x54, 0x00 },
{ 0xA8, 0xA8, 0xA8 },
{ 0x54, 0x54, 0x54 },
{ 0x54, 0x54, 0xFC },
{ 0x54, 0xFC, 0x54 },
{ 0x54, 0xFC, 0xFC },
{ 0xFC, 0x54, 0x54 },
{ 0xFC, 0x54, 0xFC },
{ 0xFC, 0xFC, 0x54 },
{ 0xFC, 0xFC, 0xFC }};

static int cursorX, cursorY;
static int cursorPX, cursorPY;
static int maxX, maxY;
static int colors[16];
static int fg,bg;
static char cursorEnabled=1, cursorInScreen=0;
static unsigned char *chars, *attrs;
static unsigned char curAttr;
static char cShapeFrom,cShapeTo;
static int al_text_mode;

/*****************************************************************************

  Functions to hide/unhide the mouse and text cursors.
  I use the scare_mouse() stuff and also implemented the same concept for the
text cursor.
    
*****************************************************************************/


void AlCon_UnDrawCursor(int *aFgCol);

#define maxCurMode 32
char CursorMode[maxCurMode];
int CursorModeIndex=0;

static
void PushCursorMode()
{
 if (CursorModeIndex<maxCurMode)
    CursorMode[CursorModeIndex++]=cursorEnabled;
}

static
void PullCursorMode()
{
 if (CursorModeIndex)
    cursorEnabled=CursorMode[--CursorModeIndex];
}

void AlCon_ScareCursor()
{
 PushCursorMode();
 if (!cursorEnabled)
    return;
 cursorEnabled=0;
 if (cursorInScreen)
   {
    int color;
    scare_mouse(); /* Could be necesary. i.e. gotoxy */
    AlCon_UnDrawCursor(&color);
    unscare_mouse();
    cursorInScreen=0;
   }
}

void AlCon_UnScareCursor()
{
 PullCursorMode();
}

void AlCon_DisableAsync()
{
 scare_mouse();
 AlCon_ScareCursor();
}

void AlCon_EnableAsync()
{
 AlCon_UnScareCursor();
 unscare_mouse();
}

/*****************************************************************************

  Cursor positioning routines.
  They take care of the text cursor.
    
*****************************************************************************/


void AlCon_CursorHome()
{
 AlCon_ScareCursor();
 cursorX=cursorY=cursorPX=cursorPY=1;
 AlCon_UnScareCursor();
}

void AlCon_GotoXY(int x, int y)
{
 AlCon_ScareCursor();
 cursorX=x; cursorY=y;
 cursorPX=(x-1)*8; cursorPY=(y-1)*16;
 AlCon_UnScareCursor();
}

int AlCon_WhereX()
{
 return cursorX;
}

int AlCon_WhereY()
{
 return cursorY;
}

int AlCon_ScreenCols()
{
 return maxX;
}

int AlCon_ScreenRows()
{
 return maxY;
}


/*****************************************************************************

  Color selection routines
    
*****************************************************************************/

void AlCon_SetColors(int aFg, int aBg)
{
 fg=aFg; bg=aBg;
 curAttr=(bg<<4) | (fg & 0xF);
}

void AlCon_TextAttr(int attr)
{
 unsigned char aBg=attr>>4;
 unsigned char aFg=attr & 0xF;
 curAttr=attr;
 fg=aFg; bg=aBg;
}

void AlCon_TextBackground(int aBg)
{
 AlCon_SetColors(fg,aBg);
}

void AlCon_TextColor(int aFg)
{
 AlCon_SetColors(aFg,bg);
}

/*****************************************************************************

  Clear routines
    
*****************************************************************************/

void AlCon_Clear()
{
 AlCon_DisableAsync();
 clear_to_color(screen,bg);
 AlCon_EnableAsync();
 memset(chars,' ',maxX*maxY);
 memset(attrs,curAttr,maxX*maxY);
}

void AlCon_ClrScr()
{
 AlCon_Clear();
 AlCon_CursorHome();
}

/*****************************************************************************

 Text output routines, including newline and tab management
    
*****************************************************************************/

void AlCon_NewLine()
{
 if (cursorY==maxY)
   { // Scroll
    int ybot,xmax;
    unsigned offset;
    AlCon_DisableAsync();
    ybot=(maxY-1)*16;
    xmax=maxX*8;
    blit(screen,screen,0,16,0,0,xmax,ybot);
    rectfill(screen,0,ybot,xmax,maxY*16,colors[bg]);
    AlCon_EnableAsync();

    // Same for buffers
    memcpy(chars,chars+maxX,maxX*(maxY-1));
    memcpy(attrs,attrs+maxX,maxX*(maxY-1));
    offset=(maxY-1)*maxX;
    memset(chars+offset,' ',maxX);
    memset(attrs+offset,curAttr,maxX);

    AlCon_GotoXY(1,cursorY);
   }
 else
    AlCon_GotoXY(1,cursorY+1);
}

void AlCon_PutStr(const char *s)
{
 int l=strlen(s);
 AlCon_DisableAsync();
 while (cursorX+l>maxX)
   {
    char b[maxX+1];
    int count=maxX-cursorX+1;
    unsigned offset;
    strncpy(b,s,count);
    b[count]=0;
    textout_ex(screen,&sfont,b,cursorPX,cursorPY,colors[fg], colors[bg]);

    offset=(cursorX-1)+(cursorY-1)*maxX;
    memcpy(chars+offset,b,count);
    memset(attrs+offset,curAttr,count);

    AlCon_NewLine();
    s+=count;
    l-=count;
   }
 if (*s)
   {
    unsigned offset;
    textout_ex(screen,&sfont,s,cursorPX,cursorPY,colors[fg], colors[bg]);

    offset=(cursorX-1)+(cursorY-1)*maxX;
    memcpy(chars+offset,s,l);
    memset(attrs+offset,curAttr,l);

    cursorX+=l;
    cursorPX+=l*8;
   }
 AlCon_EnableAsync();
}

void AlCon_Puts(const char *s)
{
 AlCon_PutStr(s);
 AlCon_NewLine();
}

void AlCon_Tab()
{
 int count=8-(cursorX & 7);
 char b[9];
 memset(b,' ',count);
 b[count]=0;
 AlCon_PutStr(b);
}

void AlCon_CPrintf(AL_CONST char *format, ...)
{
 char buf[4096];
 int ret;
 char *s,*next,aux;

 va_list ap;
 va_start(ap, format);
 ret=vsprintf(buf, format, ap);
 if (ret>4090)
   {
    set_gfx_mode(GFX_TEXT,0,0,0,0);
    allegro_message("Buffer overflow in AlCon_CPrintf\n");
    exit(100);
   }
 va_end(ap);

 s=buf;
 while ((next=strpbrk(s,"\n\t"))!=0 && *next)
   {
    aux=*next; *next=0;
    AlCon_PutStr(s);
    *next=aux;
    if (aux=='\n')
       AlCon_NewLine();
    else if (aux=='\t')
       AlCon_Tab();
    s=next+1;
   }
 if (*s)
    AlCon_PutStr(s);
}

void AlCon_PutBuf(unsigned offset, uint16 *buffer, int len)
{
 int i,nBg,nFg;
 unsigned char *b=(unsigned char *)buffer;
 unsigned char aux[2];
 unsigned x,y,mx,my;
 int prevMode;
 aux[1]=0;

 offset/=2;
 x=(offset%maxX)*8;
 y=(offset/maxX)*16;
 mx=maxX*8; my=maxY*16;
 AlCon_DisableAsync();
 for (i=0; i<len; i++)
    {
     aux[0]=chars[offset]=b[charPos];
     attrs[offset]=b[attrPos];
     nBg=b[attrPos]>>4;
     nFg=b[attrPos] & 0xF;
     textout_ex(screen,&sfont,(char *)aux,x,y,colors[nFg], colors[nBg]);
     x+=8;
     if (x>mx)
       {
        x=0; y+=16;
        if (y>my)
           break;
       }
     b+=2;
     offset++;
    }
 AlCon_EnableAsync();
}

void AlCon_GetScrChars(unsigned offset, uint16 *buffer, unsigned count)
{
 unsigned char *b=(unsigned char *)buffer;
 int i;

 offset/=2;
 while (count--)
   {
    b[charPos]=chars[offset];
    b[attrPos]=attrs[offset];
    b+=2;
    offset++;
   }
}

void AlCon_PutChar(unsigned offset, uint16 value)
{
 int i,nBg,nFg;
 unsigned char *b=(unsigned char *)&value;
 unsigned char aux[2];
 unsigned x,y;
 int prevMode;
 aux[1]=0;

 offset/=2;
 x=(offset%maxX)*8;
 y=(offset/maxX)*16;
 AlCon_DisableAsync();

 aux[0]=chars[offset]=b[charPos];
 attrs[offset]=b[attrPos];
 nBg=b[attrPos]>>4;
 nFg=b[attrPos] & 0xF;
 textout_ex(screen,&sfont,(char *)aux,x,y,colors[nFg], colors[nBg]);

 AlCon_EnableAsync();
}

/*****************************************************************************

  Text cursor routines.
    
*****************************************************************************/

void AlCon_UnDrawCursor(int *aFgCol)
{
 unsigned offset=(cursorX-1)+(cursorY-1)*maxX;
 int bg=attrs[offset]>>4;
 int fg=attrs[offset] & 0xF;
 char b[2];
 b[0]=chars[offset]; b[1]=0;
 textout_ex(screen,&sfont,b,cursorPX,cursorPY,colors[fg], colors[bg]);
 *aFgCol=colors[fg];
}

void AlCon_IntCursor()
{
 if (cursorEnabled)
   {
    int aFg,prevMode;
    scare_mouse();
    cursorInScreen=!cursorInScreen;
    if (cursorInScreen)
      {
       textout_ex(screen,&cursorFont,"\x1",cursorPX,cursorPY,aFg,-1);
      }
    unscare_mouse();
   }
}

void AlCon_SetCursorShape(int from, int to)
{
 from&=0xF; to&=0xF;
 memset(cursorData[1]->dat,0,16);
 memset(cursorData[1]->dat+from,0xFF,to-from+1);
 cShapeFrom=from;
 cShapeTo=to;
}

void AlCon_GetCursorShape(int *from, int *to)
{
 *from=cShapeFrom;
 *to=cShapeTo;
}

void AlCon_HideCursor()
{
 cursorEnabled=0;
}

void AlCon_ShowCursor()
{
 cursorEnabled=1;
}

int AlCon_IsVisCursor()
{
 return cursorEnabled;
}

/*****************************************************************************

  Initialization routines
    
*****************************************************************************/

void AlCon_Init(int w, int h)
{
 char font_buffer[4096];
 bool font_buffer_loaded = false;
 FILE *f;
 int i;

 set_uformat(U_ASCII);
 //set_uformat(U_UTF8);

 allegro_init();
 install_keyboard();
 install_mouse();
 install_timer();

 /* Load a binary font */
 f=fopen("rom-PC437.016","rb");
 if (!f)
   {
    allegro_message("Unable to load font\n");
    exit(1);
   }
 else
   {
    fread(font_buffer,4096,1,f);
    fclose(f);
    font_buffer_loaded = true;
   }

 /* If we are in a graphic mode and we know the depth use it */
 if (desktop_color_depth())
    set_color_depth(desktop_color_depth());
 else
#ifdef BPP
    set_color_depth(BPP);
#else
    set_color_depth(8);
#endif
 if (set_gfx_mode(GFX_AUTODETECT_WINDOWED,8*w,16*h,0,0))
   {
    set_gfx_mode(GFX_TEXT,0,0,0,0);
    allegro_message("Can't initialize graphics mode\n%s\n",allegro_error);
    exit(2);
   }

 w = SCREEN_W/8; h = SCREEN_H/16;
 maxX=w; maxY=h;

 /* Create an Allegro mono font from it */
 cursorFont.vtable=sfont.vtable=font->vtable;
 cursorFont.height=sfont.height=16;
 sfont.data=&ascii_monofont;
 cursorFont.data=&cursorMonoFont;

 for (i=0x0; i<0x100; i++)
    {
     ascii_data[i]=(FONT_GLYPH *)malloc(sizeof(FONT_GLYPH)+16);
     ascii_data[i]->w=8;
     ascii_data[i]->h=16;
     memcpy(ascii_data[i]->dat,&font_buffer[i*16],16);
    }
 /* Create default cursor shape */
 cursorData[0]=0;
 cursorData[1]=(FONT_GLYPH *)malloc(sizeof(FONT_GLYPH)+16);
 cursorData[1]->w=8;
 cursorData[1]->h=16;
 AlCon_SetCursorShape(14,15);

 /* Create the text mode palette */
 for (i=0; i<16; i++)
     colors[i]=makecol(BIOSPalette[i].r,BIOSPalette[i].g,BIOSPalette[i].b);

 /* Allocate "screen" buffers */
 chars=(unsigned char *)malloc(w*h);
 attrs=(unsigned char *)malloc(w*h);

 install_int(AlCon_IntCursor,500);

 AlCon_SetColors(7,0);
 AlCon_ClrScr();
 show_mouse(screen);
}

void AlCon_Exit()
{
 remove_int(AlCon_IntCursor);
 set_gfx_mode(GFX_TEXT,0,0,0,0);
}

/*****************************************************************************

  Keyboard input routines
    
*****************************************************************************/

int AlCon_GetCh()
{
 return readkey();
}

int AlCon_KbHit()
{
 return keypressed();
}

char *AlCon_KeyNames[]=
{
"Unknown",
"A","B","C","D","E","F","G","H","I","J","K",
"L","M","N","O","P","Q","R","S","T","U","V",
"W","X","Y","Z",
"OpenBrace","BackSlash","CloseBrace","Pause","Esc",
"0","1","2","3","4","5","6","7","8","9",
"BackSpace","Tab","Enter","Colon","Quote","Grave",
"Comma","Stop","Slash","Asterisk","Space","Minus",
"Plus","PrnScr","Equal","F1","F2","F3","F4","F5",
"F6","F7","F8","F9","F10","F11","F12","Home",
"Up","PgUp","Left","Right","End","Down","PgDn",
"Insert","Delete","Caret","Admid","DobleQuote",
"Numeral","Dolar","Percent","Amper","OpenPar",
"ClosePar","DoubleDot","LessThan","GreaterThan",
"Question","A_Roba","Or","UnderLine","OpenCurly",
"CloseCurly","Tilde","Macro","WinLeft","WinRight","WinSel",
"Mouse"
};

#define NumKeyNames (sizeof(AlCon_KeyNames)/sizeof(char *))

static
uchar keyCode[KEY_MAX]=
{
 kbUnkNown,kbA,kbB,kbC,kbD,kbE,kbF,kbG,kbH,kbI,kbJ,kbK,kbL,kbM,kbN,kbO,kbP,
 kbQ,kbR,kbS,kbT,kbU,kbV,kbW,kbX,kbY,kbZ,kb0,kb1,kb2,kb3,kb4,kb5,kb6,kb7,
 kb8,kb9,kb0,kb1,kb2,kb3,kb4,kb5,kb6,kb7,kb8,kb9,kbF1,kbF2,kbF3,kbF4,kbF5,
 kbF6,kbF7,kbF8,kbF9,kbF10,kbF11,kbF12,kbEsc,kbGrave,kbMinus,kbEqual,
 kbBackSpace,kbTab,kbOpenBrace,kbCloseBrace,kbEnter,kbColon,kbQuote,
 kbBackSlash,kbBackSlash,kbComma,kbStop,kbSlash,kbSpace,kbInsert,kbDelete,
 kbHome,kbEnd,kbPgUp,kbPgDn,kbLeft,kbRight,kbUp,kbDown,kbSlash,kbAsterisk,
 kbMinus,kbPlus,kbDelete,kbEnter,kbPrnScr,kbPause,kbUnkNown,kbUnkNown,
 kbUnkNown,kbUnkNown,kbUnkNown,kbA_Roba,kbCaret,kbColon,kbUnkNown,kbUnkNown,
 kbUnkNown,kbUnkNown,kbUnkNown,kbUnkNown,kbUnkNown,kbUnkNown,kbWinLeft,
 kbWinRight,kbWinSel,kbUnkNown,kbUnkNown//,kbUnkNown
};

static
uchar KeyCodeByASCII[96]=
{
 kbSpace,kbAdmid,kbDobleQuote,kbNumeral,kbDolar,kbPercent,kbAmper,kbQuote,
 kbOpenPar,kbClosePar,kbAsterisk,kbPlus,kbComma,kbMinus,kbStop,kbSlash,
 kb0,kb1,kb2,kb3,kb4,kb5,kb6,kb7,
 kb8,kb9,kbDoubleDot,kbColon,kbLessThan,kbEqual,kbGreaterThan,kbQuestion,
 kbA_Roba,kbA,kbB,kbC,kbD,kbE,kbF,kbG,
 kbH,kbI,kbJ,kbK,kbL,kbM,kbN,kbO,
 kbP,kbQ,kbR,kbS,kbT,kbU,kbV,kbW,
 kbX,kbY,kbZ,kbOpenBrace,kbBackSlash,kbCloseBrace,kbCaret,kbUnderLine,
 kbGrave,kbA,kbB,kbC,kbD,kbE,kbF,kbG,
 kbH,kbI,kbJ,kbK,kbL,kbM,kbN,kbO,
 kbP,kbQ,kbR,kbS,kbT,kbU,kbV,kbW,
 kbX,kbY,kbZ,kbOpenCurly,kbOr,kbCloseCurly,kbTilde,kbBackSpace
};

int AlCon_GetKey(unsigned *aSymbol, uchar *aScan, unsigned *aFlags)
{
 int key=readkey();
 int scan=key>>8;
 int ascii=key & 0xFF;
 int name,flags;

 //AlCon_CPrintf("key: 0x%04X scan: 0x%02X ascii: 0x%02X ",key,scan,ascii);
 if (ascii>=32 && ascii<128)
    name=KeyCodeByASCII[ascii-32];
 else
    name=keyCode[scan];

 flags=0;
 if (key_shifts & KB_SHIFT_FLAG)
    flags|=kbShiftCode;
 if (key_shifts & KB_CTRL_FLAG)
    flags|=kbCtrlCode;
 if (key_shifts & KB_ALT_FLAG)
    flags|=kbAltLCode;

 *aSymbol=ascii;
 *aFlags=flags;
 *aScan=scan;

 return name|flags;
}

void AlCon_ClearKeyBuf()
{
 clear_keybuf();
}

/*****************************************************************************

  Mouse input routines
    
*****************************************************************************/

void AlCon_GetMousePos(int *x, int *y, int *buttons)
{
 poll_mouse();
 *x=mouse_x/8;
 *y=mouse_y/16;
 *buttons=mouse_b;
}
