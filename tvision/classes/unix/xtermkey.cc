/*****************************************************************************

  XTerm keyboard routines.
  Copyright (c) 2002-2010 by Salvador E. Tropea (SET)
  Covered by the GPL license.

  Module: XTerm Keyboard
  Comments:
  This driver is a relatively complex one because:@*
  1) Deals with console mode.@*
  2) Translates keyboard codes.@*
  3) Parses escape sequences.@*
  @p

  TODO:@*
  * Move the code that overlaps with Linux driver to a base class. Lets say
  TGKeyEscapeSequences.@*
  * When the list is big (>8?) us bsearch@*
  * Allow changing MIN and TIME termios values for slow connections.@*

*****************************************************************************/
#include <tv/configtv.h>

#define Uses_stdio
#define Uses_string
#define Uses_stdlib
#define Uses_unistd
#define Uses_ioctl
#define Uses_TEvent
#define Uses_TGKey
#define Uses_FullSingleKeySymbols
#include <tv.h>

// I delay the check to generate as much dependencies as possible
#if defined(TVOS_UNIX) && !defined(TVOSf_QNXRtP)

#include <termios.h>
#include <fcntl.h>

#include <tv/unix/xtkey.h>
#include <tv/linux/log.h>

int            TGKeyXTerm::hIn=-1;
FILE          *TGKeyXTerm::fIn=NULL;
int            TGKeyXTerm::oldInFlags;
int            TGKeyXTerm::newInFlags;
struct termios TGKeyXTerm::inTermiosOrig;
struct termios TGKeyXTerm::inTermiosNew;
const char    *TGKeyXTerm::error=NULL;
int            TGKeyXTerm::bufferKeys[MaxLenEscapeSequence];
int            TGKeyXTerm::keysInBuffer=0;
int            TGKeyXTerm::nextKey=-1;
int            TGKeyXTerm::lastKeyCode;
int            TGKeyXTerm::lastModifiers;
int            TGKeyXTerm::translatedModifiers;
char           TGKeyXTerm::ascii;
node          *TGKeyXTerm::Keys=NULL;
int            TGKeyXTerm::MouseButtons=0;

/* Linux IOCTL values found experimentally */
const int kblNormal=0,kblShift=1,kblAltR=2,kblCtrl=4,kblAltL=8;

/**[txh]********************************************************************

  Description:
  Does initialization tasks performed only once.
  
  Return:
  0 if success, !=0 if an error ocurred. In the last case the error member
points to a descriptive error.
  
***************************************************************************/

int TGKeyXTerm::InitOnce()
{
 LOG("TGKeyXTerm::InitOnce");
 hIn=fileno(stdin);

 if (!isatty(hIn))
   {
    error=_("that's an interactive application, don't redirect stdin");
    return 1;
   }

 char *ttyName=ttyname(hIn);
 if (!ttyName)
   {
    error=_("failed to get the name of the current terminal used for input");
    return 3;
   }
 fIn=fopen(ttyName,"r+b");
 if (!fIn)
   {
    error=_("failed to open the input terminal");
    return 4;
   }
 hIn=fileno(fIn);

 if (tcgetattr(hIn,&inTermiosOrig))
   {
    error=_("can't get input terminal attributes");
    return 2;
   }

 memcpy(&inTermiosNew,&inTermiosOrig,sizeof(inTermiosNew));
 // Ignore breaks
 inTermiosNew.c_iflag|= (IGNBRK | BRKINT);
 // Disable Xon/off
 inTermiosNew.c_iflag&= ~(IXOFF | IXON);
 // Character oriented, no echo, no signals
 inTermiosNew.c_lflag&= ~(ICANON | ECHO | ISIG);

 // The following are needed for Solaris. In 2.7 MIN is 4 and TIME 0
 // making things really annoying. In the future they could be driver
 // variables to make the use of bandwidth smaller. A value of 4 and 1 looks
 // usable. I verified that Solaris 9 (2.9) also uses 4/0.
 //
 // *BSD systems seems interpret 0/0 value in a different way and they need
 // 1/0 in order work properly (otherwise blocks forever). I verified that
 // 1/0 works for OpenBSD 3.4, FreeBSD 4.8 and NetBSD 1.6.1. All of them uses
 // 1/0 as default, Linux also does it.
 inTermiosNew.c_cc[VMIN]=1;
 inTermiosNew.c_cc[VTIME]=0; // No timeout, just don't block
 if (tcsetattr(hIn,TCSAFLUSH,&inTermiosNew))
   {
    error=_("can't set input terminal attributes");
    return 3;
   }
 // Don't block
 oldInFlags=fcntl(hIn,F_GETFL,0);
 newInFlags=oldInFlags | O_NONBLOCK;
 fcntl(hIn,F_SETFL,newInFlags);

 // We don't need to call Resume
 suspended=0;
 return 0;
}

/**[txh]********************************************************************

  Description:
  Restore the original console state.
  
***************************************************************************/

void TGKeyXTerm::Suspend()
{
 fcntl(hIn,F_SETFL,oldInFlags);
 tcsetattr(hIn,TCSAFLUSH,&inTermiosOrig);
 LOG("TGKeyXTerm::Suspend");
}

/**[txh]********************************************************************

  Description:
  Memorize current console state and setup the one needed for us.
  
***************************************************************************/

void TGKeyXTerm::Resume()
{// Read current state
 tcgetattr(hIn,&inTermiosOrig);
 oldInFlags=fcntl(hIn,F_GETFL,0);
 // Set our state
 tcsetattr(hIn,TCSAFLUSH,&inTermiosNew);
 fcntl(hIn,F_SETFL,newInFlags);
 LOG("TGKeyXTerm::Resume");
}

int TGKeyXTerm::KbHit()
{
 if (keysInBuffer || nextKey!=-1)
    return 1;     // We have a key waiting for processing
 nextKey=fgetc(fIn);
 return nextKey!=-1;
}

void TGKeyXTerm::Clear()
{
 // Discard our buffer
 keysInBuffer=0;
 // Discard a key waiting
 nextKey=-1;
 // Flush the input
 fflush(fIn);
}

/*****************************************************************************

  Here starts the keyboard parser and translator.
  It uses a static tree/hash to parse the escape sequences, may be this should
be configurable.

*****************************************************************************/

// -  9 = Tab tiene conflicto con kbI+Control lo cual es natural, por otro
// -lado Ctrl+Tab no lo reporta en forma natural
// -  a = Enter tiene conflicto con ^J, como ^J no lo reporta naturalmente sino
// -forzado por el keymap lo mejor es definirlo directamente.
unsigned char TGKeyXTerm::kbToName[128] =
{
 0,kbA,kbB,kbC,kbD,kbE,kbF,kbG,                                           // 00-07
 kbH,kbTab,kbEnter,kbK,kbL,kbM,kbN,kbO,                                   // 08-0F
 kbP,kbQ,kbR,kbS,kbT,kbU,kbV,kbW,                                         // 10-17
 kbX,kbY,kbZ,kbEsc,0,kbCloseBrace,kb6,kbMinus,                            // 18-1F
 kbSpace,kbAdmid,kbDobleQuote,kbNumeral,kbDolar,kbPercent,kbAmper,kbQuote,// 20-27
 kbOpenPar,kbClosePar,kbAsterisk,kbPlus,kbComma,kbMinus,kbStop,kbSlash,   // 28-2F
 kb0,kb1,kb2,kb3,kb4,kb5,kb6,kb7,                                         // 30-37
 kb8,kb9,kbDoubleDot,kbColon,kbLessThan,kbEqual,kbGreaterThan,kbQuestion, // 38-3F
 kbA_Roba,kbA,kbB,kbC,kbD,kbE,kbF,kbG,                                    // 40-47
 kbH,kbI,kbJ,kbK,kbL,kbM,kbN,kbO,                                         // 48-4F
 kbP,kbQ,kbR,kbS,kbT,kbU,kbV,kbW,                                         // 50-57
 kbX,kbY,kbZ,kbOpenBrace,kbBackSlash,kbCloseBrace,kbCaret,kbUnderLine,    // 58-5F
 kbGrave,kbA,kbB,kbC,kbD,kbE,kbF,kbG,                                     // 60-67
 kbH,kbI,kbJ,kbK,kbL,kbM,kbN,kbO,                                         // 68-6F
 kbP,kbQ,kbR,kbS,kbT,kbU,kbV,kbW,                                         // 70-77
 kbX,kbY,kbZ,kbOpenCurly,kbOr,kbCloseCurly,kbTilde,kbBackSpace            // 78-7F
};

unsigned char TGKeyXTerm::kbExtraFlags[128] =
{
 0,kblCtrl,kblCtrl,kblCtrl,kblCtrl,kblCtrl,kblCtrl,kblCtrl,               // 00-07
 kblCtrl,0,0,kblCtrl,kblCtrl,kblCtrl,kblCtrl,kblCtrl,                     // 08-0F
 kblCtrl,kblCtrl,kblCtrl,kblCtrl,kblCtrl,kblCtrl,kblCtrl,kblCtrl,         // 10-17
 kblCtrl,kblCtrl,kblCtrl,0,0,kblCtrl,kblCtrl,kblCtrl,                     // 18-1F
 0,kblShift,kblShift,kblShift,kblShift,kblShift,kblShift,0,               // 20-27
 kblShift,kblShift,kblShift,kblShift,0,0,0,0,                             // 28-2F
 0,0,0,0,0,0,0,0,                                                         // 30-37
 0,0,kblShift,0,kblShift,0,kblShift,kblShift,                             // 38-3F
 kblShift,kblShift,kblShift,kblShift,kblShift,kblShift,kblShift,kblShift, // 40-47
 kblShift,kblShift,kblShift,kblShift,kblShift,kblShift,kblShift,kblShift, // 48-4F
 kblShift,kblShift,kblShift,kblShift,kblShift,kblShift,kblShift,kblShift, // 50-57
 kblShift,kblShift,kblShift,0,0,0,kblShift,kblShift,                      // 58-5F
 0,0,0,0,0,0,0,0,                                                         // 60-67
 0,0,0,0,0,0,0,0,                                                         // 68-6F
 0,0,0,0,0,0,0,0,                                                         // 70-77
 0,0,0,kblShift,kblShift,kblShift,kblShift,0,                             // 78-7F
};

/************************** Escape sequences tree **************************/
struct node
{
 union
 {
  char value;
  char keys;
 };
 uchar code;
 uchar modifiers;
 node *next;
};

// This is how Xterm usually encodes the modifiers
static
uchar xtMods[7]=
{
 kblShift,                     // 2
 kblAltL,                      // 3
 kblShift | kblAltL,           // 4
 kblCtrl,                      // 5
 kblShift | kblCtrl,           // 6
 kblCtrl | kblAltL,            // 7
 kblShift | kblCtrl | kblAltL  // 8
};

const uchar fgEterm=1, fgOnlyEterm=2;
typedef struct
{
 uchar number;
 uchar code;
} stCsiKey;

typedef struct
{
 uchar number;
 uchar code;
 uchar mods;
} stEtKey;

// CSI Number ; Modifiers ~
const int cCsiKeys1=16;
static
stCsiKey csiKeys1[cCsiKeys1]=
{
 {  1, kbHome }, // Putty
 {  2, kbInsert },
 {  3, kbDelete },
 {  4, kbEnd },  // Putty
 {  5, kbPgUp }, // Prior
 {  6, kbPgDn }, // Next
 {  7, kbHome },
 {  8, kbEnd },
 { 15, kbF5 },
 { 17, kbF6 },
 { 18, kbF7 },
 { 19, kbF8 },
 { 20, kbF9 },
 { 21, kbF10 },
 { 23, kbF11 },
 { 24, kbF12 }
};

static
uchar csiFgKeys1[cCsiKeys1]=
{
 fgEterm,fgEterm,fgEterm,fgEterm,
 //fgOnlyEterm|fgEterm,fgOnlyEterm|fgEterm, // PgUp & PgDown
 fgEterm,fgEterm,
 0,0,0,0,0,0,0,0,0,0
};

// CSI Modifiers Letter
const int cCsiKeys2=9;
static
stCsiKey csiKeys2[cCsiKeys2]=
{
 { 'A', kbUp },
 { 'B', kbDown },
 { 'C', kbRight },
 { 'D', kbLeft },
 { 'E', kb5 }, // Key pad 5
 { 'F', kbEnd },
 { 'H', kbHome },
 { 'K', kbBackSpace }, // SET extension ;-)
 { 'T', kbTab }        // SET extension ;-)
};

// Modifiers O Letter
const int cCsiKeys3=4;
static
stCsiKey csiKeys3[cCsiKeys3]=
{
 { 'P', kbF1 },
 { 'Q', kbF2 },
 { 'R', kbF3 },
 { 'S', kbF4 }
};

// Eterm: O Letter and Application mode
const int cCsiKeys4=26;
static
stEtKey csiKeys4[cCsiKeys4]=
{
 { 'a', kbUp, kblCtrl },
 { 'b', kbDown, kblCtrl },
 { 'c', kbRight, kblCtrl },
 { 'd', kbLeft, kblCtrl },
 { 'A', kbUp, 0 },
 { 'B', kbDown, 0 },
 { 'C', kbRight, 0 },
 { 'D', kbLeft, 0 },
 { 'F', kbEnd, 0 },
 { 'H', kbHome, 0 },
 // Keypad
 { 'M', kbEnter, 0 },
 { 'j', kbAsterisk, 0 },
 { 'k', kbPlus, 0 },
 { 'm', kbMinus, 0 },
 { 'n', kbDelete, 0 },
 { 'o', kbSlash, 0 },
 { 'p', kbInsert, 0 },
 { 'q', kbEnd, 0 },
 { 'r', kbDown, 0 },
 { 's', kbPgDn, 0 },
 { 't', kbLeft, 0 },
 { 'u', kb5, 0 },
 { 'v', kbRight, 0 },
 { 'w', kbHome, 0 },
 { 'x', kbUp, 0 },
 { 'y', kbPgUp, 0 }
};

node *TGKeyXTerm::NewNode()
{
 node *ret=(node *)malloc(sizeof(node));
 ret->keys=0;
 return ret;
}

node *TGKeyXTerm::NewNode(node *p)
{
 node *ret=(node *)malloc(sizeof(node)*2);
 ret->keys=1;
 ret[1]=*p;
 return ret;
}

node *TGKeyXTerm::SearchInList(node *p, uchar k)
{
 if (p->keys==0) return NULL; // No keys
 int i,c=p->keys;
 for (i=1; i<=c; i++)
     if (p[i].value==k)
        return p+i;
 return NULL; // Not found
}

node *TGKeyXTerm::AddKey2List(node *p, uchar k, node **pa)
{
 int i=1,c=p->keys;
 while (i<=c && p->value<k) i++;
 p=(node *)realloc(p,sizeof(node)*(c+2));
 if (pa) *pa=p;
 if (i<=c)
   {// In the middle
    memmove(p+i+1,p+i,c-i+1);
   }
 p[i].value=k;
 p[i].next=NULL;
 p->keys++;
 return p+i;
}

void TGKeyXTerm::AddKey(const uchar *key, uchar code, uchar modifiers)
{
 if (!Keys)
    Keys=NewNode();
 node *p=Keys,*pk,**pa=&Keys;
 const uchar *s=key;

 while (*s)
   {
    pk=SearchInList(p,*s);
    if (!pk)
      {// Not in the list
       pk=AddKey2List(p,*s,pa);
       if (*(s+1))
         {// New ramification
          p=NewNode();
          pk->next=p;
          pa=&(pk->next);
         }
       else
         {// Just new key
          pk->next=NULL;
          pk->code=code;
          pk->modifiers=modifiers;
         }
      }
    else
      {// Already in the list
       if (*(s+1))
         {// It isn't the end, what is currently there?
          if (!pk->next)
            {// We have a key, so we must ramify without losing it
             pk->next=NewNode(pk);
            }
          pa=&(pk->next);
          p=pk->next;
         }
       else
         {
          if (pk->next)
             LOG("Error, this key is confusing");
          else
             LOG("Warning, this key was already defined [" << key << "]");
         }
      }
    s++;
   }
}

#ifdef DEBUGTREE
static
int maxKeys=0;

static
void Indent(int level)
{
 while (level--)
    fputc(' ',stderr);
}

static
void PrintTree(node *p, int level)
{
 int i,c;

 c=p->keys;
 Indent(level);
 fprintf(stderr,"Keys: %d\n",c);
 if (c>maxKeys)
    maxKeys=c;
 for (i=1; i<=c; i++)
    {
     Indent(level);
     fprintf(stderr,"Key %c ",p[i].value);
     if (p[i].next)
       {
        Indent(level);
        fprintf(stderr,"Ramification\n");
        PrintTree(p[i].next,level+1);
       }
     else
       {
        Indent(level);
        fprintf(stderr,"KeyCode %s\n",TGKey::NumberToKeyName(p[i].code));
       }
    }
 if (level==0)
    fprintf(stderr,"Max keys: %d\n",maxKeys);
}
#else
 #define PrintTree(a,b)
#endif

void TGKeyXTerm::PopulateTree()
{
 int i,j;
 char b[MaxLenEscapeSequence];
 
 for (i=0; i<cCsiKeys1; i++)
    {
     sprintf(b,"[%d~",csiKeys1[i].number);
     AddKey((uchar *)b,csiKeys1[i].code,0);
     if (!(csiFgKeys1[i] & fgOnlyEterm))
        for (j=0; j<7; j++)
           {
            sprintf(b,"[%d;%d~",csiKeys1[i].number,j+2);
            AddKey((uchar *)b,csiKeys1[i].code,xtMods[j]);
           }
     if (csiFgKeys1[i] & fgEterm)
       {
        sprintf(b,"[%d^",csiKeys1[i].number);
        AddKey((uchar *)b,csiKeys1[i].code,kblCtrl);
        sprintf(b,"[%d$",csiKeys1[i].number);
        AddKey((uchar *)b,csiKeys1[i].code,kblShift);
        sprintf(b,"[%d@",csiKeys1[i].number);
        AddKey((uchar *)b,csiKeys1[i].code,kblShift| kblCtrl);
       }
    }
 for (i=0; i<cCsiKeys2; i++)
    {
     sprintf(b,"[%c",csiKeys2[i].number);
     AddKey((uchar *)b,csiKeys2[i].code,0);
     for (j=0; j<7; j++)
        {
         sprintf(b,"[%d%c",j+2,csiKeys2[i].number);
         AddKey((uchar *)b,csiKeys2[i].code,xtMods[j]);
         // Newer versions:
         sprintf(b,"[1;%d%c",j+2,csiKeys2[i].number);
         AddKey((uchar *)b,csiKeys2[i].code,xtMods[j]);
        }
    }
 for (i=0; i<cCsiKeys3; i++)
    {
     sprintf(b,"O%c",csiKeys3[i].number);
     AddKey((uchar *)b,csiKeys3[i].code,0);
     for (j=0; j<7; j++)
        {
         sprintf(b,"O%d%c",j+2,csiKeys3[i].number);
         AddKey((uchar *)b,csiKeys3[i].code,xtMods[j]);
        }
    }
 for (i=0; i<cCsiKeys4; i++)
    {
     sprintf(b,"O%c",csiKeys4[i].number);
     AddKey((uchar *)b,csiKeys4[i].code,csiKeys4[i].mods);
    }

 // Eterm rarities:
 // Shift+arrows
 AddKey((uchar *)"[a",kbUp,kblShift);
 AddKey((uchar *)"[b",kbDown,kblShift);
 AddKey((uchar *)"[c",kbRight,kblShift);
 AddKey((uchar *)"[d",kbLeft,kblShift);
 // F1-F4
 AddKey((uchar *)"[11~",kbF1,0);
 AddKey((uchar *)"[12~",kbF2,0);
 AddKey((uchar *)"[13~",kbF3,0);
 AddKey((uchar *)"[14~",kbF4,0);
 // Ctrl+Fx
 AddKey((uchar *)"[11^",kbF1,kblCtrl);
 AddKey((uchar *)"[12^",kbF2,kblCtrl);
 AddKey((uchar *)"[13^",kbF3,kblCtrl);
 AddKey((uchar *)"[14^",kbF4,kblCtrl);
 AddKey((uchar *)"[15^",kbF5,kblCtrl);
 AddKey((uchar *)"[17^",kbF6,kblCtrl);
 AddKey((uchar *)"[18^",kbF7,kblCtrl);
 AddKey((uchar *)"[19^",kbF8,kblCtrl);
 AddKey((uchar *)"[20^",kbF9,kblCtrl);
 AddKey((uchar *)"[21^",kbF10,kblCtrl);
 // Shift+Fx (Shift F1 and F2 overlaps with F11 and F12)
 AddKey((uchar *)"[25~",kbF3,kblShift);
 AddKey((uchar *)"[26~",kbF4,kblShift);
 AddKey((uchar *)"[28~",kbF5,kblShift);
 AddKey((uchar *)"[29~",kbF6,kblShift);
 AddKey((uchar *)"[31~",kbF7,kblShift);
 AddKey((uchar *)"[32~",kbF8,kblShift);
 AddKey((uchar *)"[33~",kbF9,kblShift);
 AddKey((uchar *)"[34~",kbF10,kblShift);
 AddKey((uchar *)"[23$",kbF11,kblShift);
 AddKey((uchar *)"[24$",kbF12,kblShift);
 // Ctrl+Shift+Fx
 AddKey((uchar *)"[23^",kbF1,kblCtrl | kblShift);
 AddKey((uchar *)"[24^",kbF2,kblCtrl | kblShift);
 AddKey((uchar *)"[25^",kbF3,kblCtrl | kblShift);
 AddKey((uchar *)"[26^",kbF4,kblCtrl | kblShift);
 AddKey((uchar *)"[28^",kbF5,kblCtrl | kblShift);
 AddKey((uchar *)"[29^",kbF6,kblCtrl | kblShift);
 AddKey((uchar *)"[31^",kbF7,kblCtrl | kblShift);
 AddKey((uchar *)"[32^",kbF8,kblCtrl | kblShift);
 AddKey((uchar *)"[33^",kbF9,kblCtrl | kblShift);
 AddKey((uchar *)"[34^",kbF10,kblCtrl | kblShift);
 AddKey((uchar *)"[23@",kbF11,kblCtrl | kblShift);
 AddKey((uchar *)"[24@",kbF12,kblCtrl | kblShift);
 AddKey((uchar *)"]lTerminal", 0, 0); // Suppress the lTerminal escape sequence xterm sends

 // The mouse reporting mechanism:
 AddKey((uchar *)"[M",kbMouse,0);

 PrintTree(Keys,0);
}
/*********************** End of escape sequences tree ***********************/

/**[txh]********************************************************************

  Description:
  Parse a escape sequence.

  Returns: 1 if the sequence found, 0 if not and the keys are stored in the
buffer.
  
***************************************************************************/

int TGKeyXTerm::ProcessEscape()
{
 int nextVal, extraModifiers;

 nextVal=fgetc(fIn);
 if (nextVal==EOF)     // Just ESC
    return 0;
 // ESC + Escape sequence => Alt/Meta + Escape sequence
 extraModifiers=0;
 if (nextVal=='\e')
   {
    extraModifiers=kblAltL;
    nextVal=fgetc(fIn);
    if (nextVal==EOF)     // Just Alt+ESC
      {
       lastModifiers=kblAltL;
       return 0;
      }
   }

 node *p=Keys;
 int cant,i;
 keysInBuffer=0;
 while (nextVal!=EOF)
   {
    NextNode:
    bufferKeys[keysInBuffer++]=nextVal;
    cant=p->value;
    for (i=1; i<=cant; i++)
       {
        if (p[i].value==nextVal)
          {
           if (p[i].next)
             {
              p=p[i].next;
              nextVal=fgetc(fIn);
              goto NextNode;
             }
           lastKeyCode=p[i].code;
           lastModifiers=p[i].modifiers | extraModifiers;
           bufferKeys[keysInBuffer]=0;
           keysInBuffer=0;
           return 1;
          }
       }
    return 0;
   }
 return 0;
}

int TGKeyXTerm::GetKeyFromBuffer()
{
 int ret=bufferKeys[--keysInBuffer];
 if (keysInBuffer)
    memcpy(bufferKeys,bufferKeys+1,keysInBuffer);
 return ret;
}

/**[txh]********************************************************************

  Description:
  Gets a key from the buffer, waiting value or fIn and if needed calls
the escape sequence parser.
  
***************************************************************************/

int TGKeyXTerm::GetKeyParsed()
{
 lastModifiers=0;
 translatedModifiers=-1;
 // If we have keys in the buffer take from there.
 // They already failed the escape sequence test.
 if (keysInBuffer)
    return GetKeyFromBuffer();

 // Use the value we have on hold or get a new one
 int nextVal=nextKey;
 nextKey=-1;
 if (nextVal==-1)
    nextVal=fgetc(fIn);
 if (nextVal==-1)
    return -1;

 // Is that an escape sequence?
 if (nextVal=='\e')
   {
    if (ProcessEscape())
       return -2;
    if (!keysInBuffer)
       return '\e';
    lastKeyCode=GetKeyFromBuffer();
    lastModifiers=kblAltL;
    return -3;
   }

 return nextVal;
}

/**[txh]********************************************************************

  Description:
  Gets the next key, their modifiers and ASCII. Is a postprocessor for
GetKeyParsed.
  
***************************************************************************/

int TGKeyXTerm::GetRaw()
{
 int result=GetKeyParsed();

 if (result==-1)
    return 0;   // No key
 if (result==-2)
   {
    ascii=0;
    return 1;   // Key already processed
   }
 if (result==-3) // Forced modifier
    result=lastKeyCode;

 ascii=result;
 // Translate the key
 if (result>=128)
    lastKeyCode=kbUnkNown;
 else
   {
    lastModifiers|=kbExtraFlags[result];
    lastKeyCode=kbToName[result];
   }
 return 1;
}

/**[txh]********************************************************************

  Description:
  Gets a key from the input and converts it into the TV format.
  
***************************************************************************/

ushort TGKeyXTerm::GKey()
{
 if (GetRaw())
   { // Here I add the modifiers to the key code
    if (lastModifiers & kblShift)
       lastKeyCode|=kbShiftCode;
    if (lastModifiers & kblCtrl)
       lastKeyCode|=kbCtrlCode;
    switch (AltSet)
      {
       case 1: // Reverse thing
            if (lastModifiers & kblAltL)
               lastKeyCode|=kbAltRCode;
            break;
       default: // Compatibility
            if (lastModifiers & kblAltL)
               lastKeyCode|=kbAltLCode;
      }
    return lastKeyCode;
   }
 return kbUnkNown;
}
/*****************************************************************************
  End of parser and translator.
*****************************************************************************/

/**[txh]********************************************************************

  Description:
  Finds the value of the modifiers in TV format.
  
***************************************************************************/

unsigned TGKeyXTerm::GetShiftState()
{
 if (!lastModifiers) return 0;
 if (translatedModifiers==-1)
   {
    translatedModifiers=0;
    if (lastModifiers & kblShift)
       translatedModifiers|=kbLeftShiftDown | kbRightShiftDown;
    if (lastModifiers & kblCtrl)
       translatedModifiers|=kbLeftCtrlDown | kbRightCtrlDown | kbCtrlDown;
    if (lastModifiers & kblAltL)
       translatedModifiers|=kbLeftAltDown | kbAltDown;
   }
 return translatedModifiers;
}

const int MouseB1Down=0x20,MouseB2Down=0x21,MouseB3Down=0x22,MouseUp=0x23,
          MouseB4Down=0x60,MouseB5Down=0x61;

/**[txh]********************************************************************

  Description:
  Fills the TV event structure for a key.
  
***************************************************************************/

void TGKeyXTerm::FillTEvent(TEvent &e)
{
 GKey();
 if ((lastKeyCode & kbKeyMask)==kbMouse)
   { // Mouse events are traslated to keyboard sequences:
    int event=fgetc(fIn);
    int x=fgetc(fIn)-0x21; // They are 0x20+ and the corner is 1,1
    int y=fgetc(fIn)-0x21;
    // Filter the modifiers:
    event&= ~0x1C;
    // B4 and B5 behaves in a particular way
    MouseButtons&= ~(MouseB4Down | MouseB5Down);
    if (event>=0x60)
      {// B4 and B5, they seems to report a press and never a release
       if (event==MouseB4Down)
          MouseButtons|=mbButton4;
       else
         if (event==MouseB5Down)
            MouseButtons|=mbButton5;
      }
    else
      {
       if (event>=0x40) event-=0x20; // Translate motion values
       switch (event)
         {
          case MouseB1Down:
               MouseButtons|=mbLeftButton;
               break;
          case MouseB2Down:
               MouseButtons|=mbMiddleButton;
               break;
          case MouseB3Down:
               MouseButtons|=mbRightButton;
               break;
          case MouseUp: // fuzzy, which one?
               MouseButtons=0;
               break;
         }
      }
    THWMouse::forceEvent(x,y,MouseButtons);
    e.what=evMouseUp; // Acts like a "key"
    return;
   }

 e.keyDown.charScan.charCode=lastModifiers & kblAltL ? 0 : ascii;
 e.keyDown.charScan.scanCode=ascii;
 e.keyDown.raw_scanCode=ascii;
 e.keyDown.keyCode=lastKeyCode;
 e.keyDown.shiftState=lastModifiers;
 e.what=evKeyDown;
}

void TGKeyXTerm::Init()
{
 TGKey::Suspend      =TGKeyXTerm::Suspend;
 TGKey::Resume       =TGKeyXTerm::Resume;
 TGKey::kbhit        =KbHit;
 TGKey::clear        =Clear;
 TGKey::gkey         =GKey;
 TGKey::getShiftState=GetShiftState;
 TGKey::fillTEvent   =FillTEvent;
 if (Keys==NULL)
    PopulateTree();
}

#else // TVOS_UNIX && !TVOSf_QNXRtP

#include <tv/unix/xtkey.h>
#include <tv/linux/log.h>

#endif // else TVOS_UNIX && !TVOSf_QNXRtP
