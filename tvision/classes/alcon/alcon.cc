#define DEBUGMODE
// -*- mode:C++; tab-width: 3 -*-

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

// Forward declarations.
static void _AlCon_LoadCustomFont(const char *filename);

// Globals internal to AlCon.
static FONT *_current_font = 0;
static FONT _custom_font;
static FONT_GLYPH *_ascii_data[256];
static FONT_MONO_DATA _ascii_monofont =
{
 0x0, 0x100,                 /* begin, end characters */
 _ascii_data,                 /* the data set */
 0                           /* next */
};

static FONT _cursorFont;
static FONT_GLYPH *_cursorData[2];
static FONT_MONO_DATA _cursorMonoFont =
{
 0x0, 0x2,                   /* begin, end characters */
 _cursorData,                 /* the data set */
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

/*
   The variables _screen_width, _screen_height, _font_width and
   _font_height are read only. They are modified ONLY when the font
   is changed. The first two tell the number of text characters
   that fit into the physical screen. The last two tell the number
   of screen pixels a text cell has in Allegro. The font size is
   initialised by default to a 8x16 font so the AlCon_Init function
   can set a reasonable default screen size.
*/
static int _screen_width, _screen_height;
static int _font_width = 8, _font_height = 16;

static int colors[16];
static int fg,bg;
static char cursorEnabled=1, cursorInScreen=0;
static unsigned char *chars, *attrs;
static unsigned char curAttr;
static char cShapeFrom,cShapeTo;
static int al_text_mode;
static int al_mouse_buttons = 0;
static int al_mouse_wheel;



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
 AlCon_GotoXY(1, 1);
}

void AlCon_GotoXY(int x, int y)
{
 ASSERT(x > 0 && x <= _screen_width);
 ASSERT(y > 0 && y <= _screen_height);
 AlCon_ScareCursor();
 cursorX = x; cursorY = y;
 cursorPX = (x - 1) * _font_width;
 cursorPY = (y - 1) * _font_height;
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
 return _screen_width;
}

int AlCon_ScreenRows()
{
 return _screen_height;
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
 memset(chars,' ', _screen_width * _screen_height);
 memset(attrs,curAttr, _screen_width * _screen_height);
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
 if (cursorY == _screen_height)
   { // Scroll
    int ybot,xmax;
    unsigned offset;
    AlCon_DisableAsync();
    ybot = (_screen_height - 1) * _font_height;
    xmax = _screen_width * _font_width;
    blit(screen, screen, 0, _font_height, 0, 0, xmax, ybot);
    rectfill(screen,0,ybot,xmax, _screen_height * _font_height, colors[bg]);
    AlCon_EnableAsync();

    // Same for buffers
    memcpy(chars, chars + _screen_width, _screen_width * (_screen_height - 1));
    memcpy(attrs, attrs + _screen_width, _screen_width * (_screen_height - 1));
    offset = (_screen_height - 1) * _screen_width;
    memset(chars+offset,' ', _screen_width);
    memset(attrs+offset,curAttr, _screen_width);

    AlCon_GotoXY(1,cursorY);
   }
 else
    AlCon_GotoXY(1,cursorY+1);
}

void AlCon_PutStr(const char *s)
{
 ASSERT(s);
 ASSERT(_current_font);
 
 int l=strlen(s);
 AlCon_DisableAsync();
 while (cursorX + l > _screen_width)
   {
    char b[_screen_width + 1];
    int count = _screen_width - cursorX + 1;
    unsigned offset;
    strncpy(b,s,count);
    b[count]=0;
    textout_ex(screen,_current_font,b,cursorPX,cursorPY,colors[fg], colors[bg]);

    offset = (cursorX - 1) + (cursorY - 1) * _screen_width;
    memcpy(chars+offset,b,count);
    memset(attrs+offset,curAttr,count);

    AlCon_NewLine();
    s+=count;
    l-=count;
   }
 if (*s)
   {
    unsigned offset;
    textout_ex(screen,_current_font,s,cursorPX,cursorPY,colors[fg], colors[bg]);

    offset = (cursorX - 1) + (cursorY - 1) * _screen_width;
    memcpy(chars+offset,s,l);
    memset(attrs+offset,curAttr,l);

    cursorX+=l;
    cursorPX += l * _font_width;
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
 ASSERT(buffer);
 ASSERT(_current_font);
 int i,nBg,nFg;
 unsigned char *b=(unsigned char *)buffer;
 unsigned char aux[2];
 unsigned x,y,mx,my;
 int prevMode;
 aux[1]=0;

 x = (offset % _screen_width) * _font_width;
 y = (offset / _screen_width) * _font_height;
 mx = _screen_width * _font_width;
 my = _screen_height * _font_height;
 AlCon_DisableAsync();
 for (i=0; i<len; i++)
    {
     aux[0]=chars[offset]=b[charPos];
     attrs[offset]=b[attrPos];
     nBg=b[attrPos]>>4;
     nFg=b[attrPos] & 0xF;
     textout_ex(screen,_current_font,(char *)aux,x,y,colors[nFg], colors[nBg]);
     x += _font_width;
     if (x>mx)
       {
        x=0; y += _font_height;
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
 ASSERT(_current_font);
 int i,nBg,nFg;
 unsigned char *b=(unsigned char *)&value;
 unsigned char aux[2];
 unsigned x,y;
 int prevMode;
 aux[1]=0;

 offset/=2;
 x = (offset % _screen_width) * _font_width;
 y = (offset / _screen_width) * _font_height;
 AlCon_DisableAsync();

 aux[0]=chars[offset]=b[charPos];
 attrs[offset]=b[attrPos];
 nBg=b[attrPos]>>4;
 nFg=b[attrPos] & 0xF;
 textout_ex(screen,_current_font,(char *)aux,x,y,colors[nFg], colors[nBg]);

 AlCon_EnableAsync();
}

/*****************************************************************************

  Text cursor routines.
    
*****************************************************************************/

void AlCon_UnDrawCursor(int *aFgCol)
{
 ASSERT(_current_font);
 unsigned offset = (cursorX - 1) + (cursorY - 1) * _screen_width;
 int bg=attrs[offset]>>4;
 int fg=attrs[offset] & 0xF;
 char b[2];
 b[0]=chars[offset]; b[1]=0;
 textout_ex(screen,_current_font,b,cursorPX,cursorPY,colors[fg], colors[bg]);
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
       textout_ex(screen,&_cursorFont,"\x1",cursorPX,cursorPY,aFg,-1);
      }
    unscare_mouse();
   }
}

void AlCon_SetCursorShape(int from, int to)
{
 from&=0xF; to&=0xF;
 memset(_cursorData[1]->dat,0,_font_height);
 memset(_cursorData[1]->dat+from,0xFF,to-from+1);
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

/**[txh]********************************************************************

  Description: Initialization of the AlCon layer. Pass the number of text
  characters you want to have with a default 8x16 font.
  
  Return: Zero if everything went fine, non zero otherwise.
  
***************************************************************************/

int AlCon_Init(int w, int h)
{
   set_uformat(U_ASCII);
   //set_uformat(U_UTF8);
  
   allegro_init();
   install_keyboard();
   al_mouse_buttons = install_mouse();
   al_mouse_wheel = mouse_z;
   // Clamp allegro return of "no mouse" to TVision interpretation.
   if (al_mouse_buttons < 0)
      al_mouse_buttons = 0;
   // And add "fake wheel buttons" if the mouse has at least two.
   if (al_mouse_buttons > 1)
      al_mouse_buttons = 5;
   install_timer();
  
   /* If we are in a graphic mode and we know the depth use it */
   if (desktop_color_depth()) {
      set_color_depth(desktop_color_depth());
   } else {
      #ifdef BPP
      set_color_depth(BPP);
      #else
      set_color_depth(8);
      #endif
   }
   
   if (set_gfx_mode(GFX_AUTODETECT_WINDOWED, _font_width * w, _font_height * h,
         _font_width * w, _font_height * h)) {
      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
      allegro_message("Can't initialize graphics mode\n%s\n",allegro_error);
      return 2;
   }
  
   /* Load a binary font. Hack to get the font/screen properties set. */
   _AlCon_LoadCustomFont(0);
   //_AlCon_LoadCustomFont("rom-PC437.016");
  
   /* Create default cursor shape */
   _cursorData[0] =0;
   _cursorData[1] =(FONT_GLYPH *)malloc(sizeof(FONT_GLYPH) + _font_height);
   _cursorData[1]->w = _font_width;
   _cursorData[1]->h = _font_height;
   AlCon_SetCursorShape(14,15);
  
   /* Create the text mode palette */
   for (int i = 0; i < 16; i++)
       colors[i] = makecol(BIOSPalette[i].r,
         BIOSPalette[i].g, BIOSPalette[i].b);
  
   /* Allocate "screen" buffers */
   chars = (unsigned char *)malloc(_screen_width * _screen_height);
   attrs = (unsigned char *)malloc(_screen_width * _screen_height);
  
   install_int(AlCon_IntCursor, 500);
  
   AlCon_SetColors(7, 0);
   AlCon_ClrScr();
   show_mouse(screen);
   return 0;
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

// Returns 0 if there is no mouse.
int AlCon_GetMouseButtons(void)
{
 return al_mouse_buttons;
}

void AlCon_GetMousePos(int *x, int *y, int *buttons)
{
   if (al_mouse_buttons == 1)
      return ;
      
   poll_mouse();
   *x = mouse_x / _font_width;
   *y = mouse_y / _font_height;
   *buttons = mouse_b;
  
   // Emulate 4th and 5th buttons through wheelmouse control.
   if (mouse_z > al_mouse_wheel)
      *buttons |= 1 << 3;
   if (mouse_z < al_mouse_wheel)
      *buttons |= 1 << 4;
   al_mouse_wheel = mouse_z;
}

/**[txh]********************************************************************

  Description: Loads the specified binary format font into an internal
  structure like Allegro's font and then sets the static global font pointer
  to that. If the font could not be loaded, the previously loaded custom
  font will be used. If this was the first custom font call, Allegro's
  default font will be used if everything fails. If the filename is a NULL
  pointer, the default font will be used.
  
***************************************************************************/

static void _AlCon_LoadCustomFont(const char *filename)
{
   ASSERT(screen && "You have to call set_gfx_mode before.");
   static bool one_custom_font_loaded = false;

   FILE *file = 0;
   if (filename)
      file = fopen(filename, "rb");
      
   if (!file) {
      // We have to cover up if there was no previous font.
      if (!one_custom_font_loaded) {
         _current_font = font;
         // Allegro font is by default 8x8.
         _font_width = _font_height = 8;
      }
   } else {
      char font_buffer[4096];
      
      int read_bytes = fread(font_buffer, 1, 4096, file);
      fclose(file);
      if (read_bytes < 256) {// Absurd size limit
         allegro_message("Absurd custom font size (%d bytes)", read_bytes);
         exit(3);
      }
      one_custom_font_loaded = true;
      // Presume the font is 8 pixels wide and contains 256 characters.
      _font_width = 8;
      _font_height = read_bytes / 0x100;

      // Copy font from buffer to internal structure.
      for (int i = 0; i < 0x100; i++) {
         // Free previously allocated memory.
         if (_ascii_data[i])
            free(_ascii_data[i]);
            
         _ascii_data[i] = (FONT_GLYPH *)malloc(sizeof(FONT_GLYPH) + _font_height);
         _ascii_data[i]->w = _font_width;
         _ascii_data[i]->h = _font_height;

         memcpy(_ascii_data[i]->dat, &font_buffer[i * _font_height],
            _font_height);
      }
      _current_font = &_custom_font;
      _custom_font.vtable = font->vtable;
      _custom_font.height = _font_height;
      _custom_font.data = &_ascii_monofont;
   }

   // Fill global variables.
   _cursorFont.vtable = font->vtable;
   _cursorFont.height = _font_height;
   _cursorFont.data = &_cursorMonoFont;

   // Set the size of the font and screen.
   _screen_width = SCREEN_W / _font_width;
   _screen_height = SCREEN_H / _font_height;
}

/**[txh]********************************************************************

  Description: Fills in the width and height of a text cell.
  
***************************************************************************/

void AlCon_GetFontGeometry(unsigned int *w, unsigned int *h)
{
   ASSERT(w && h);
   *w = _font_width;
   *h = _font_height;
}
