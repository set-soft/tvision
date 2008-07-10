/**[txh]********************************************************************

  Copyright 1996-2003 by Salvador Eduardo Tropea (SET)
  This file is covered by the GPL license.

  Module: Code Page
  Include: TVCodePage
  Comments:
  This module provides code page remapping needed to adapt the special
character to all the supported terminals.@p
  It was originally designed for SETEdit and moved to Turbo Vision in 2002.@p
  The internal encoding system maps the first 256 to CP 437.@p
  Important: The Turbo Vision never had such a thing and this is completly
new code that affects a lot of TV components.@p
  The mechanism used is similar to what Linux kernel uses, why? simple:@*
1) That's the most complex mechanism I found in all the targets so far.@*
2) This is quite crazy but at the same time flexible.@*
  What's similar?@*
  We have two maps. The first map is what Linux calls SFM (Screen Font Map).
It defines which internal codes are rendered by each font character (which
unicodes in Linux kernel). This is basically a map that describes the font.
This is the only map I had in the editor. The curScrCP variable holds the
ID of this map.@p
  The second map is what the Linux kernel calls ACM (Application Charset
Map). This map describes how is the application data encoded, as an example:
how is encoded the text. It doesn't have to map one to one with the font,
and that's the complex stuff. For this reason this map is used to do an
"on-the-fly" remap before sending the characters to the screen. If this map
is identical to the SFM (maps 1 to 1 => what Linux calls trivial mapping)
we don't do the remap. The OnTheFlyRemapNeeded() and OnTheFlyRemap(uchar val)
are used for the remap. The curAppCP holds the ID of this map.@p
  Is important to note that we don't do 8-bits -> ACM -> Unicode -> SFM ->
8-bits Screen like the Linux kernel does. We recode the application using
the ACM and if ACM!=SFM we use a simple table that makes: 8-bits -> 8-bits
Screen and if it isn't needed we just send the code to the screen.@p
  I added a new code page to this: the input code page. In Linux systems
the input is supposed to be encoded in the ACM code page. In Windows systems
they can be different and they are asked with different calls. It can be
useful if you want to use an encoding different than the currently used by
the OS. You can change the screen encoding and load an appropriate font for
it, then you change the application code page and you can properly see
documents encoded in the target code page. But in order to edit them you
need to translate the codes that come from keyboard to the target code page.
That's a good use for the input code page.
  
***************************************************************************/

#define Uses_stdio

#define Uses_string
#define Uses_stdlib //bsearch
#define Uses_TStringCollection
#define Uses_TVCodePage

// Classes we remap chars
#define Uses_TScreen
#define Uses_TRadioButtons
#define Uses_TMenuBox
#define Uses_TFrame
#define Uses_TIndicator
#define Uses_THistory
#define Uses_TColorSelector
#define Uses_TMonoSelector
#define Uses_TColorDialog
#define Uses_TInputLine
#define Uses_TStatusLine
#define Uses_TCheckBoxes
#define Uses_TScrollBar
#define Uses_TButton
#define Uses_TDirListBox
#define Uses_TFileEditor
#define Uses_TFileInfoPane
#define Uses_TDeskTop
#define Uses_TListViewer
#define Uses_TGKey

// Needed for extra remaps
#define Uses_TProgram
#define Uses_TBackground
#define Uses_TEvent

#include <tv.h>

// List of supported code pages
TVCodePageCol *TVCodePage::CodePages=NULL;
// The full description of desired code page
ushort         TVCodePage::CPTable[257];
// toUpper/Lower and isAlpha tables
// I initialize it with some defaults in case they are needed before setting a
// code page, in this case only the ASCII range is really usable. The tables
// are for PC437.
uchar          TVCodePage::AlphaTable[256]=
{
 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
 0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x00,0x00,0x00,0x00,0x00,0x00,
 0x00,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,
 0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x00,0x00,0x00,0x00,0x00,
 0x00,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,
 0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x00,0x00,0x00,0x00,0x00,
 0x05,0x03,0x03,0x01,0x03,0x01,0x03,0x03,0x01,0x01,0x01,0x01,0x01,0x01,0x05,0x05,
 0x05,0x03,0x05,0x01,0x03,0x01,0x01,0x01,0x01,0x05,0x05,0x00,0x00,0x00,0x00,0x00,
 0x01,0x01,0x01,0x01,0x03,0x05,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
 0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};
uchar          TVCodePage::toUpperTable[256]=
{
 0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,
 0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,
 0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F,
 0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,0x3E,0x3F,
 0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4A,0x4B,0x4C,0x4D,0x4E,0x4F,
 0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5A,0x5B,0x5C,0x5D,0x5E,0x5F,
 0x60,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4A,0x4B,0x4C,0x4D,0x4E,0x4F,
 0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5A,0x7B,0x7C,0x7D,0x7E,0x7F,
 0x80,0x9A,0x90,0x83,0x8E,0x85,0x8F,0x80,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,
 0x90,0x92,0x92,0x93,0x99,0x95,0x96,0x97,0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F,
 0xA0,0xA1,0xA2,0xA3,0xA5,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,
 0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF,
 0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,
 0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF,
 0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEE,0xEF,
 0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF
};
uchar          TVCodePage::toLowerTable[256]=
{
 0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,
 0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,
 0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F,
 0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,0x3E,0x3F,
 0x40,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6A,0x6B,0x6C,0x6D,0x6E,0x6F,
 0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7A,0x5B,0x5C,0x5D,0x5E,0x5F,
 0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6A,0x6B,0x6C,0x6D,0x6E,0x6F,
 0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7A,0x7B,0x7C,0x7D,0x7E,0x7F,
 0x87,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x84,0x86,
 0x82,0x91,0x91,0x93,0x94,0x95,0x96,0x97,0x98,0x94,0x81,0x9B,0x9C,0x9D,0x9E,0x9F,
 0xA0,0xA1,0xA2,0xA3,0xA4,0xA4,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,
 0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF,
 0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,
 0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF,
 0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEE,0xEF,
 0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF
};
// Some Linux codepages aren't what they claim to be just to put the frames
// in a better place.
uchar          TVCodePage::OnTheFlyMap[256];
char           TVCodePage::NeedsOnTheFlyRemap=0;
// This is a translation for the codes from keyboard.
uchar          TVCodePage::OnTheFlyInpMap[256];
char           TVCodePage::NeedsOnTheFlyInpRemap=0;
// Current code pages
//  The source code is encoded in CP 437
int            TVCodePage::curAppCP=437;
//  We assume the screen is also CP 437, if it isn't true the driver will inform it.
int            TVCodePage::curScrCP=437;
// What we get from the keyboard
int            TVCodePage::curInpCP=437;
// Default values suggested by the current driver
int            TVCodePage::defAppCP=437;
int            TVCodePage::defScrCP=437;
int            TVCodePage::defInpCP=437;
// Helpers to convert internal buffers
uint16         TVCodePage::appToUnicode[256];
TVPartitionTree556
              *TVCodePage::unicodeToApp=NULL;
uint16         TVCodePage::inpToUnicode[256];
TVPartitionTree556
              *TVCodePage::unicodeToInp=NULL;
// User provided function to call each time we change the code page.
// This is called before sending a broadcast.
void         (*TVCodePage::UserHook)(ushort *map)=NULL;
// Table used to find something that can represent an absent code.
// This is for CP 437 use
uchar          TVCodePage::Similar[]=
{
  ' ', '@', '@', '*', '*', '*', '*', '*', //   0-  7
 0x04, 'o',0xDB, 'M', 'F', 'd', 'd', '*', //   8- 15
 0x1A,0x1B, 'I',0xBA, 'P', 'S',0xDC,0x12, //  16- 23
  '^', 'V', '>', '<',0xC0, '-',0x18,0x19, //  24- 31
  ' ', '!', '"', '#', '$', '%', '&','\'', //  32- 39
  '(', ')', '*', '+', ',', '-', '.', '/', //  40- 47
  '0', '1', '2', '3', '4', '5', '6', '7', //  48- 55
  '8', '9', ':', ';', '<', '=', '>', '?', //  56- 63
  '@', 'A', 'B', 'C', 'D', 'E', 'F', 'G', //  64- 71
  'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', //  72- 79
  'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', //  80- 87
  'X', 'Y', 'Z', '[','\\', ']', '^', '_', //  88- 95
  '`', 'a', 'b', 'c', 'd', 'e', 'f', 'g', //  96-103
  'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', // 104-111
  'p', 'q', 'r', 's', 't', 'u', 'v', 'w', // 112-119
  'x', 'y', 'z', '{', '|', '}', '~', '^', // 120-127
  'C', 'u', 'e', 'a', 'a', 'a', 'a', 'c', // 128-135
  'e', 'e', 'e', 'i', 'i', 'i', 'A', 'A', // 136-143
  'E', 'a', 'A', 'o', 'o', 'o', 'u', 'u', // 144-151
  'y', 'O', 'U', 'c', 'L', 'Y', 'P', 'f', // 152-159
  'a', 'i', 'o', 'u', 'n', 'N', 'a', 'o', // 160-167
  '?',0xDA,0xBF, '*', '*', '!', '<', '>', // 168-175
  ' ', ' ', ' ', '|',0xB3,0xB3,0xBA,0xBF, // 176-183
 0xBF,0xBA,0xB3,0xBF,0xD9,0xD9,0xD9, '*', // 184-191
  '*',0xC4,0xC4,0xB3, '-', '+',0xB3,0xBA, // 192-199
 0xC0,0xDA,0xCD,0xCD,0xBA,0xC4,0xBA,0xCD, // 200-207
 0xC4,0xCD,0xC4,0xC0,0xC0,0xDA,0xDA,0xC5, // 208-215
 0xC5, '*', '*', ' ', ' ', ' ', ' ', ' ', // 216-223
  'a', 'B', 'G', 'p', 'S', 's', 'm', 't', // 224-231
  'F', 'f', 'O', 'd', 'i', 'o', 'e', 'U', // 232-239
  '=', '+', '>', '<',0xDA,0xD9, '/', '~', // 240-247
  'o', '.', '.', 'V', 'n', '2', '*', ' ', // 248-255
};
// This table is used to reduce any of our codes into a similar one when absent.
// It covers all the known symbols not covered by Similar and if the value doesn't
// exist in the current code page you must enter in the table again until you get
// an ASCII value.
ushort         TVCodePage::Similar2[]=
{
 'c','l','n','r','s','y','z', // 0x100-0x106 small acute
 'A','C','I','L','N','O','R','S','U','Y','Z', // 0x107-0x111 capital acute
 'U', // 0x112 double acute
 'A','E','I','O','U', // 0x113-0x117 capital grave
 'E','I', // 0x118-0x119 capital diaeresis
 'U','u', // 0x11A-0x11B ring above
 'a','o','u','A','O', // 0x11C-0x120 tilde/double acute
 'A','E','I','O','U', // 0x121-0x125 capital circumflex
 'c','n','r','s','z','C','D','E','N','R','S','T','Z', // 0x126-0x132 caron
 's','t','S','T', // 0x133-0x136 cedilla
 'a','e','A','E', // 0x137-0x13A ogonek
 'z','Z', // 0x13B-013C dot above
 'l','o','L','O','d', // 0x13D-0x141 stroke
 0x141, // 0x142 latin small letter eth
 'D', // 0x143 stroke
 'd','l','t','L', // 0x144-0x147 caron
 'a','e','A', // 0x14-0x14A breve
 '$', // 0x14B currency sign (o four feets), I guess that's a good fall back
      // specially thinking all people understand $ as money
 'P','p', // 0x14C-0x14D thorn (Icelandic) sorry I know that's arbitrary and
          // silly. Anyone have a better choice?
 '*', // 14E multiplication sign
 'R', // 14F registered sign (R)
 'c', // 150 copyright sign (c)
 'i', // 151  latin small letter dotless i
 '|', // 152  broken bar (|)
 '_', // 153  macron (_ but high)
 '\'',// 154  acute accent
 '-', // 155  soft hyphen (- long)
 '_', // 156  double low line (_ double)
 '3', // 157  vulgar fraction three quarters
 ',', // 158  cedilla
 '"', // 159  diaeresis (di‚resis, umlaut)
 '1', // 15a  superscript one
 '3', // 15b  superscript three
 '"', // 15c  double acute accent
 ';', // 15d  ogonek (cedilla inv.)
 '<', // 15e  caron (mandarin chinese third tone)
 '(', // 15f  breve
 '.', // 160  dot above (mandarin chinese light tone)
 'o','O', // 0x161-0x162 double acute
 0x145,0x147,0x146,'e',0x144, // 0x163-0x167 caron, fall back to the other representation
 // 0x168
 'A','B','G','D','Z','I','J','L','P','U','F','C','C','S','S','"','Y','"',
 'E','U','A', // 0x168-0x17C Cyrillic capitals group 1
 'b','v','g','d','z','z','i','j','k','l','m','n','p','t','u','f','c','c',
 's','s','\'','y','\'','e','u','a', // 0x17D-0x196 Cyrillic smalls group 1
 'I','i', // 0x197-0x198 ukrainian ie
 'Z', // 0x199 capital letter ze
 ' ', // 0x19A free slot
 'N', // 0x19B number
 // ISO-5 cyrillics:
 'D', // 19C capital letter dje (Serbocroatian) (¢)-5
 'd', // 19D small letter dje (Serbocroatian) (ò)-5
 'G', // 19E capital letter gje (£)-5
 'g', // 19F small letter gje (ó)-5
 'Y', // 1A0 capital letter yi (Ukrainian) (¦)-5
 'L', // 1A1 capital letter lje (©)-5
 'l', // 1A2 small letter lje (ù)-5
 'N', // 1A3 capital letter nje (ª)-5
 'n', // 1A4 small letter nje (ú)-5
 'T', // 1A5 capital letter tshe (Serbocroatian) («)-5
 't', // 1A6 small letter tshe (Serbocroatian) (û)-5
 'K', // 1A7 capital letter kje (¬)-5
 'k', // 1A8 small letter kje (ü)-5
 'V', // 1A9 capital letter short U (Byelorussian) (®)-5
 'v', // 1AA small letter short U (Byelorussian) (þ)-5
 'D', // 1AB capital letter dzhe (¯)-5
 'd', // 1AC small letter dzhe (ÿ)-5
 // Windows CP 1251 (russian)
 '"', // 1AD double low-9 quotation mark („)-cp1251
 '.', // 1AE horizontal ellipsis         (…)-cp1251
 '|', // 1AF dagger                      (†)-cp1251
 '|', // 1B0 double dagger               (‡)-cp1251
 '%', // 1B1 per mille sign              (‰)-cp1251
 '"', // 1B2 left double quotation mark  (“)-cp1251
 '"', // 1B3 right double quotation mark (”)-cp1251
 'T', // 1B4 trade mark sign             (™)-cp1251
 0x16A, // 1B5 cyrillic capital letter ghe with upturn, default to GHE (¥)-cp1251
 0x17F, // 1B6 cyrillic small letter ghe with upturn, default to ghe   (´)-cp1251
 '?',   // 1B7 That's a ? inside a circle, it means the character is unknown
 'O', // 1B8 latin capital ligature OE
 'o', // 1B9 latin small ligature oe
 // Idiot ISO-1 font found in Linux, only to break standards and annoy people
 't', // 1BA  symbol for horizontal tabulation
 'f', // 1BB  symbol for form feed
 'c', // 1BC  symbol for carriage return
 'l', // 1BD  symbol for line feed
 'n', // 1BE  symbol for newline
 'v', // 1BF  symbol for vertical tabulation
 '#', // 1C0  not equal to
 'v', // 1C1  downwards arrow with corner leftwards
 'Y', // 1C2  latin capital letter y with diaeresis
 ' ', // 1C3  free
 0xB3,// 1C4  box drawings light up ()-1 linux
 0xC4,// 1C5  box drawings light right (‚)-1 linux
 0xB3,// 1C6  box drawings light down („)-1 linux
 0xC4,// 1C7  box drawings light left (ˆ)-1 linux
 ' ', // 1C8  free
 0xBA,// 1C9  box drawings heavy up (‘)-1 linux
 0xCD,// 1CA  box drawings heavy right (’)-1 linux
 0xBA,// 1CB  box drawings heavy down (”)-1 linux
 0xCD,// 1CC  box drawings heavy left (˜)-1 linux
 0x1A8,//1CD  latin small letter kra
 'N','n', // Latin letter eng
 // Circumflex 1D0-1DD
 'C','c','G','g','H','h','J','j','S','s','W','w','Y','y',
 // Dot above 1DE-1EF
 'B','b','C','c','D','d','F','f','g','I','M','m','P','p','S','s','T','t',
 // Macron 1F0-1F7
 'E','e','I','i','O','o','U','u',
 // Breve 1F8-1FD
 'G','g','I','i','U','u',
 // Cedilla 1FE-205
 'G','g','K','k','N','n','R','r',
 // Tilde 206-209
 'I','i','U','u',
 // Ogonek 20A-20F
 'I','i','U','u','W','w',
 'Y',// 210   0x1EF2 LATIN CAPITAL LETTER Y WITH GRAVE
 'y',// 211   0x1EF3 LATIN SMALL LETTER Y WITH GRAVE
 'W',// 212   0x1E82 LATIN CAPITAL LETTER W WITH ACUTE
 'w',// 213   0x1E83 LATIN SMALL LETTER W WITH ACUTE
 'W',// 214   0x1E84 LATIN CAPITAL LETTER W WITH DIAERESIS
 'w',// 215   0x1E85 LATIN SMALL LETTER W WITH DIAERESIS
 'H',// 216   0x0126 LATIN CAPITAL LETTER H WITH STROKE
 'h',// 217   0x0127 LATIN SMALL LETTER H WITH STROKE
 'T',// 218   0x0166 LATIN CAPITAL LETTER T WITH STROKE
 't',// 219   0x0167 LATIN SMALL LETTER T WITH STROKE
 'O',// 21A   0x01A0 LATIN CAPITAL LETTER O WITH HORN
 'o',// 21B   0x01A1 LATIN SMALL LETTER O WITH HORN
 'U',// 21C   0x01AF LATIN CAPITAL LETTER U WITH HORN
 'u',// 21D   0x01B0 LATIN SMALL LETTER U WITH HORN
 'E',// 21E   0x0116 LATIN CAPITAL LETTER E WITH DOT ABOVE
 'e',// 21F   0x0117 LATIN SMALL LETTER E WITH DOT ABOVE
 'L',// 220   0x013B LATIN CAPITAL LETTER L WITH CEDILLA
 'l',// 221   0x013C LATIN SMALL LETTER L WITH CEDILLA
 // Greek smalls 222-22F
 'g','z','h','u','i','l','n','j','r','s','y','x','c','v',
 // Greek capitals 230-233
 'D','J','P','C',
 // Greek accented
 0x159,// 234   0x0385 GREEK DIALYTIKA TONOS => diaeresis
 0xE0, // 235   0x03AC GREEK SMALL LETTER ALPHA WITH TONOS => alpha
 0xEE, // 236   0x03AD GREEK SMALL LETTER EPSILON WITH TONOS => epsilon
 0x224,// 237   0x03AE GREEK SMALL LETTER ETA WITH TONOS => eta
 0x226,// 238   0x03AF GREEK SMALL LETTER IOTA WITH TONOS => iota
 'o',  // 239   0x03CC GREEK SMALL LETTER OMICRON WITH TONOS => omicron
 0x22C,// 23A   0x03CD GREEK SMALL LETTER UPSILON WITH TONOS => upsilon
 0x22F,// 23B   0x03CE GREEK SMALL LETTER OMEGA WITH TONOS => omega
 'A',  // 23C   0x0386 GREEK CAPITAL LETTER ALPHA WITH TONOS
 'E',  // 23D   0x0388 GREEK CAPITAL LETTER EPSILON WITH TONOS
 'H',  // 23E   0x0389 GREEK CAPITAL LETTER ETA WITH TONOS
 'I',  // 23F   0x038A GREEK CAPITAL LETTER IOTA WITH TONOS
 'O',  // 240   0x038C GREEK CAPITAL LETTER OMICRON WITH TONOS
 'Y',  // 241   0x038E GREEK CAPITAL LETTER UPSILON WITH TONOS
 0xEA, // 242   0x038F GREEK CAPITAL LETTER OMEGA WITH TONOS => omega
 0x226,// 243   0x03CA GREEK SMALL LETTER IOTA WITH DIALYTIKA => iota
 0x22C,// 244   0x03CB GREEK SMALL LETTER UPSILON WITH DIALYTIKA => upsilon
 0x238,// 245   0x0390 GREEK SMALL LETTER IOTA WITH DIALYTIKA AND TONOS => iota + tonos
 0x23A,// 246   0x03B0 GREEK SMALL LETTER UPSILON WITH DIALYTIKA AND TONOS => upsilon + tonos
 'L',  // 247   <U039B> GREEK CAPITAL LETTER LAMDA
 'a','A',  // 248/9   <U0101/0> LATIN LETTER A WITH MACRON
 'G', //  24A   <U0120> LATIN CAPITAL LETTER G WITH DOT ABOVE
 'E', //  24B   <U20AC> Euro Sign
 // Cyrillics that are identical to latin letters
 'B','E','K','M','H','O','P','C','T','X','a','e','o','p','c','x',
 137,280,'s','i',139,'j',281,'J','S',
 // Greeks that are identical to latin letters
 0xE1,0x185,0xE6,'o','A','B','E','Z','H','I','K','M','N','O','P','T','Y','X',
 0x119,0x1C2,
};


// We use an internal code. This is currently a 16 bits code.
// The ammount of defined symbols: [0 ... maxSymbolDefined-1]
const unsigned maxSymbolDefined=632+1;

// Helper to sort Unicode tables or search in sorted Unicode tables
static
int compare(const void *v1, const void *v2)
{
 stIntCodePairs *p1=(stIntCodePairs *)v1;
 stIntCodePairs *p2=(stIntCodePairs *)v2;
 return (p1->unicode>p2->unicode)-(p1->unicode<p2->unicode);
}

// This is what usually call: TNoCaseNoOwnerStringCollection, but isn't
// available in TV, yet
class TVCodePageCol : public TStringCollection
{
public:
 TVCodePageCol(ccIndex aLimit, ccIndex aDelta) :
      TStringCollection(aLimit,aDelta) {};
 int compare(void *s1,void *s2) { return strcasecmp((char *)s1,(char *)s2); };
 void freeItem(void *) {};
};

/*****************************************************************************
 Defined code pages sorted by ID:

Name                        ID
PC 437 ASCII ext.           437
PC 737 Greek                737
PC 775 DOS Baltic Rim       775
PC 850 Latin 1              850
PC 852 Latin 2              852
PC 855 Russian 2            855
PC 857 Turkish              857
PC 860 Portuguese           860
PC 861 Icelandic            861
PC 863 French               863
PC 865 Nordic               865
PC 866 Russian              866
PC 869 Greek 2              869
CP 1250 Win Latin 2         1250
CP 1251 Win Russian         1251
CP 1252 Win Latin 1         1252
CP 1253 Win Greek           1253
CP 1254 Win Turkish         1254
CP 1257 Win Baltic          1257
Mac Cyr. CP 10007           10007
ISO 8859-1 Latin 1          88791
ISO 8859-2 Latin 2          88792
ISO 8859-3 Latin 3          88593
ISO 8859-4 Latin 4          88594
ISO 8859-5 Russian          88595
ISO 8859-7 Greek            88597
ISO 8859-9                  88599
ISO Latin 1 (Linux)         885901
ISO Latin 1u(Linux)         885911
ISO 8859-14                 885914
ISO 8859-15 Icelan.         885915
ISO Latin 2 (Linux)         885920
ISO Latin 2u(Linux)         885921
ISO Latin 2 (Sun)           885922
ISO Latin 2+Euro (Linux)    885923
KOI-8r (Russian)            100000
KOI-8 with CRL/NMSU         100001
Mac OS Ukrainian            100072
Osnovnoj Variant Russian    885951
Alternativnyj Variant RU    885952
U-code Russian              885953
Mazovia (polish)            1000000
ISO 5427 ISO-IR-37 KOI-7    3604494
ECMA-Cyr.ISO-IR-111         17891342
JUS_I.B1.003-SERB ISOIR146  21364750
JUS_I.B1.003-MAC ISO-IR-147 21430286
Cyrillic ISO-IR-153         22216718

*****************************************************************************/

// PC437 doesn't need traslation, they are the first 256 chars.
CodePage TVCodePage::stPC437=
{ "PC 437 ASCII ext.",
  437,
 { 128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,
   144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,
   160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,
   176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,
   192,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207,
   208,209,210,211,212,213,214,215,216,217,218,219,220,221,222,223,
   224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,
   240,241,242,243,244,245,246,247,248,249,250,251,252,253,254,255 },
 "‚†„Ž‡€š‘’”™¤¥",
 "ƒ…ˆ‰Š‹Œ“•–—˜ ¡¢£á",0,0
};

CodePage TVCodePage::stPC737=
{ "PC 737 Greek",
  737,
 { 617,618,226,560,619,620,621,233,622,623,583,624,625,561,626,562,
   627,228,628,629,232,630,563,234,224,613,546,235,238,547,548,549,
   550,614,551,615,552,553,616,227,554,229,555,231,556,237,557,558,
   176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,
   192,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207,
   208,209,210,211,212,213,214,215,216,217,218,219,220,221,222,223,
   559,565,566,567,579,568,569,570,580,571,572,573,574,575,576,577,
   578,241,342,343,631,632,246,344,248,345,250,346,347,253,254,255 },
 "˜€™¯–›ƒœ„­”š‚ž† ˆ¥¡‰¢Š£‹¤Œ¦Ž§¨©‘«’à—ª‘®•¬“…áêâëãìåíæîéðçïäôèõ",
 "Ÿ",0,0
};

CodePage TVCodePage::stPC775=
{ "PC 775 DOS Baltic Rim",
  775,
 { 264,129,130,584,132,511,134,256,317,497,516,517,499,273,142,143,
   144,145,146,501,148,510,155,270,260,153,154,318,156,320,334,331,
   585,498,162,316,315,262,435,338,336,335,170,171,172,319,174,175,
   176,177,178,179,180,313,299,314,542,185,186,187,188,522,304,191,
   192,193,194,195,196,197,524,502,200,201,202,203,204,205,206,306,
   311,294,312,543,523,297,525,503,298,217,218,219,220,221,222,223,
   268,225,500,267,285,288,230,258,512,513,544,545,515,496,514, 39,
   341,241,434,343, 20, 21,246,429,248,249,250,346,347,253,254,255
 },
 "‡€‹Šš‚ƒ „Ž…•†ˆ­‰íŒ¡¥‘’“â”™˜—›¢à¤£ÐµÑ¶Ò·Ó¸Ô½Õ¾ÖÆ×ÇØÏçãäåéèëêìî",
 "áæ",0,0
};

CodePage TVCodePage::stPC850=
{ "PC 850 Latin 1",
  850,
 { 128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,
   144,145,146,147,148,149,150,151,152,153,154,318,156,320,334,159,
   160,161,162,163,164,165,166,167,168,335,170,171,172,173,174,175,
   176,177,178,179,180,263,289,275,336,185,186,187,188,155,157,191,
   192,193,194,195,196,197,284,287,200,201,202,203,204,205,206,331,
   321,323,290,280,276,337,265,291,281,217,218,219,220,338,277,223,
   268,225,292,278,285,288,230,332,333,271,293,279,261,272,339,340,
   341,241,342,343, 20, 21,246,344,248,345,250,346,347,253,254,255 },
 "‡€š‚ƒ¶„Ž…·†ˆÒ‰ÓŠÔ‹ØŒ×Þ‘’“â”™•ã–ê—ë› µ¡Ö¢à£é¤¥ÆÇÐÑäåçèìí",
 "˜ÏÕáæ",0,0
};

CodePage TVCodePage::stPC852=
{ "PC 852 Latin 2",
  852,
 { 128,129,130,131,132,282,256,135,317,137,354,353,140,273,142,264,
   144,266,257,147,148,327,325,270,260,153,154,305,326,319,334,294,
   160,161,162,163,313,311,306,298,314,312,  0,262,299,307,174,175,
   176,177,178,179,180,263,289,301,309,185,186,187,188,316,315,191,
   192,193,194,195,196,197,330,328,200,201,202,203,204,205,206,331,
   321,323,300,280,324,302,265,291,358,217,218,219,220,310,283,223,
   268,225,292,267,258,295,304,297,269,271,259,274,261,272,308,340,
   341,348,349,350,351, 21,246,344,248,345,352,286,303,296,254,255 },
 "‡€š‚ƒ¶„Ž…Þ†ˆ‰Ó‹ŠŒ×«’‘“â”™–•˜—Ÿ¬ µ¡Ö¢à£é¥¤§¦©¨­¸Ø·¾½ÇÆÐÑåÕîÝäãçæêèûëìíýü",
 "›œÏÒÔá",0,0
};

// PC855 Russian DOS code page
CodePage TVCodePage::stPC855=
{ "PC 855 Russian 2",
  855,
 { 413,412,415,414,604,605,408,407,606,612,607,610,608,416,609,611,
   418,417,420,419,422,421,424,423,426,425,428,427,405,379,401,375,
   598,360,381,361,397,371,384,363,599,589,396,370,383,362,174,175,
   176,177,178,179,180,603,597,387,365,185,186,187,188,388,366,191,
   192,193,194,195,196,197,389,590,200,201,202,203,204,205,206,331,
   390,367,391,591,392,592,600,593,393,217,218,219,220,368,406,223,
   380,601,594,602,595,394,596,395,369,385,364,382,588,403,377,411,
   341,402,376,386,409,399,373,404,378,400,374,398,372, 21,254,255 },
 "€‚ƒ„…†‡ˆ‰Š‹ŒŽ‘’“”•–—˜™š›œžŸ ¡¢£¤¥¦§¨©ª«¬­µ¶·¸½¾ÆÇÐÑÒÓÔÕÖ×ØÝÞàáâãäåæçèéêëìíîñòóôõö÷øùúûü",
 "",0,0
};

CodePage TVCodePage::stPC857=
{ "PC 857 Turkish",
  857,
 { 128,129,130,131,132,133,134,135,136,137,138,139,140,337,142,143,
   144,145,146,147,148,149,150,151,487,153,154,318,156,320,309,307,
   160,161,162,163,164,165,504,505,168,335,170,171,172,173,174,175,
   176,177,178,179,180,263,289,275,336,185,186,187,188,155,157,191,
   192,193,194,195,196,197,284,287,200,201,202,203,204,205,206,331,
   167,166,290,280,276,276,265,291,281,217,218,219,220,338,277,223,
   268,225,292,278,285,288,230,230,334,271,293,279,141,152,339,340,
   341,241,241,343, 20, 21,246,344,248,345,250,346,347,253,254,255 },
 "‡€š‚ƒ¶„Ž…·†ˆÒ‰ÓŠÔ‹ØŒ×˜‘’“â”™•ã–ê—ë›Ÿž µ¡Ö£é¤¥§¦ÆÇ¢àÞìäå",
 "Õáæçí",0,0
};

CodePage TVCodePage::stPC860=
{ "PC 860 Portuguese",
  860,
 { 128,129,130,131,284,133,263,135,136,290,138,265,140,141,287,289,
   144,275,276,147,285,149,271,151,277,288,154,155,156,279,158,268,
   160,161,162,163,164,165,166,167,168,278,170,171,172,173,174,175,
   176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,
   192,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207,
   208,209,210,211,212,213,214,215,216,217,218,219,220,221,222,223,
   224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,
   240,241,242,243,244,245,246,247,248,249,250,251,252,253,254,255 },
 "‡€š‚ƒ„Ž…‘ †ˆ‰Š’¡‹“Œ˜”™•©£–—¢Ÿ¤¥",
 "á",0,0
};

CodePage TVCodePage::stPC861=
{ "PC 861 Icelandic",
  861,
 { 128,129,130,131,132,133,134,135,136,137,138,323,322,333,142,143,
   144,145,146,147,148,332,150,272,261,153,154,318,156,320,158,159,
   160,161,162,163,263,265,268,271,168,169,170,171,172,173,174,175,
   176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,
   192,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207,
   208,209,210,211,212,213,214,215,216,217,218,219,220,221,222,223,
   224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,
   240,241,242,243,244,245,246,247,248,249,250,251,252,253,254,255 },
 "š‚„Ž†‡€Œ‹•‘’”™˜—› ¤¡¥¢¦£§",
 "ƒ…ˆ‰Š“–àáâãåäæçèéêëíîŸ",0,0
};

CodePage TVCodePage::stPC863=
{ "PC 863 French",
  863,
 { 128,129,130,131,289,133, 20,135,136,137,138,139,140,342,275, 21,
   144,276,290,147,280,281,150,151,331,292,154,155,156,279,158,159,
   338,340,162,163,345,344,347,339,291,169,170,171,172,173,174,175,
   176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,
   192,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207,
   208,209,210,211,212,213,214,215,216,217,218,219,220,221,222,223,
   224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,
   240,241,242,243,244,245,246,247,248,249,250,251,252,253,254,255 },
 "‡€š‚ƒ„…Žˆ’‰”Š‘‹•Œ¨“™–ž—",
 "˜¢£á",0,0
};

CodePage TVCodePage::stPC865=
{ "PC 865 Nordic",
  865,
 { 128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,
   144,145,146,147,148,149,150,151,152,153,154,318,156,320,158,159,
   160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,331,
   176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,
   192,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207,
   208,209,210,211,212,213,214,215,216,217,218,219,220,221,222,223,
   224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,
   240,241,242,243,244,245,246,247,248,249,250,251,252,253,254,255 },
 "‡€š›‚„Ž†‘’¤¥”™",
 "ƒ…ˆ‰Š‹Œ“•–—˜ ¡¢£¯á",0,0
};

CodePage TVCodePage::stPC869=
{ "PC 869 Greek 2",
  869,
 { 0x20,0x20,0x20,0x20,0x20,0x20,572,572,250,170,338, 96, 39,573,341,574,
   575,631,576,576,576,577,632,336,578,253,347,565,156,566,567,568,
   579,581,569,570,617,618,226,560,619,620,621,171,233,622,174,175,
   176,177,178,179,180,623,583,624,625,185,186,187,188,561,626,191,
   192,193,194,195,196,197,562,627,200,201,202,203,204,205,206,228,
   628,629,232,630,563,234,224,613,546,217,218,219,220,235,238,223,
   547,548,549,550,614,551,615,552,553,616,227,554,229,555,231,340,
   341,241,556,237,557, 21,558,564,248,345,559,580,582,571,254,255 },
 "›†žŸ ‘¢’£•û–ý˜Ö¤×¥Ø¦Ý§Þ¨à©áªâ¬ã­äµå¶æ·ç¸è½é¾êÆëÇìÏíÏîÐòÑóÒôÓöÔúÕ",
 "¡ü",0,0
};

CodePage TVCodePage::ISO8879_1=
{ "ISO 8859-1 Latin 1",
  88791,
 { 218,196,191,192,217,179,195,180,194,193,197,201,205,187,200,188,
   186,199,182,209,207,219,178,177,176,223,220,222,221,254,254,254,
    32,173,155,156, 36,157,338, 21,345,336,166,174,170,341,335,341,
   248,241,253,347,340,230, 20,249,344,346,167,175,172,171,343,168,
   275,263,289,287,142,143,146,128,276,144,290,280,277,265,291,281,
   323,165,278,268,292,288,153,334,320,279,271,293,154,272,333,225,
   133,160,131,284,132,134,145,135,138,130,136,137,141,161,140,139,
   321,164,149,162,147,285,148,246,318,151,163,150,129,261,332,152 },
 "àÀáÁâÂãÃäÄåÅæÆçÇèÈéÉêÊëËìÌíÍîÎïÏðÐñÑòÒóÓôÔõÕöÖøØùÙúÚûÛüÜýÝþÞ",
 "µßÿ",0,0
};

ushort TVCodePage::LowCrazyCharsRemaped[] =
{ 439,247,440,441,  4,442,443,444,445,176,177,178,219,220,223,221,
  222,446,447,242,243,448, 17, 16, 24, 25, 26, 27, 18, 29,449/*447*/,227, // 449 in lat1.sfm
   32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
   48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,
   64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
   80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95,
   96, 97, 98, 99,100,101,102,103,104,105,106,107,108,109,110,111,
  112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,450 };


CodePage TVCodePage::ISO8859_1_Lat1=
{ "ISO Latin 1 (Linux)",
  885901,
 { 451,452,453,192,454,179,218,195,455,217,196,193,191,180,194,197,
   456,457,458,200,459,186,201,204,460,188,205,202,187,185,203,206,
   255,173,155,156, 36,157,338, 21,345,336,166,174,170,341,335,341,
   248,241,253,347,340,230, 20,249,344,346,167,175,172,171,343,168,
   275,263,289,287,142,143,146,128,276,144,290,280,277,265,291,281,
   323,165,278,268,292,288,153,334,320,279,271,293,154,272,333,225,
   133,160,131,284,132,134,145,135,138,130,136,137,141,161,140,139,
   321,164,149,162,147,285,148,246,318,151,163,150,129,261,332,152 },
 "àÀáÁâÂãÃäÄåÅæÆçÇèÈéÉêÊëËìÌíÍîÎïÏðÐñÑòÒóÓôÔõÕöÖøØùÙúÚûÛüÜÿýÝþÞ",
 "µß",
 128,LowCrazyCharsRemaped
};

CodePage TVCodePage::ISO8859_1u_Lat1=
{ "ISO Latin 1u(Linux)",
  885911,
 { 275,263,289,287,142,143,146,128,276,144,290,280,277,265,291,281,
   323,165,278,268,292,288,153,334,320,279,271,293,154,272,333,225,
    32,173,155,156, 36,157,338, 21,345,336,166,174,170,341,335,341,
   248,241,253,347,340,230, 20,249,344,346,167,175,172,171,343,168,
   451,452,453,192,454,179,218,195,455,217,196,193,191,180,194,197,
   456,457,458,200,459,186,201,204,460,188,205,202,187,185,203,206,
   133,160,131,284,132,134,145,135,138,130,136,137,141,161,140,139,
   321,164,149,162,147,285,148,246,318,151,163,150,129,261,332,152 },
 "à€áâ‚ãƒä„å…æ†ç‡èˆé‰êŠë‹ìŒíîŽïðñ‘ò’ó“ô”õ•ö–ø˜ù™úšû›üœÿýþž",
 "µŸ",
 128,LowCrazyCharsRemaped
};

CodePage TVCodePage::ISO8879_2=
{ "ISO 8859-2 Latin 2",
  88792,
 { 218,196,191,192,217,179,195,180,194,193,197,201,205,187,200,188,
   186,199,182,209,207,219,178,177,176,223,220,222,221,254,254,254,
   255,313,351,319,331,356,270, 21,345,304,309,305,273,341,306,316,
   248,311,349,317,340,325,260,350,344,297,307,326,262,348,298,315,
   269,263,289,330,142,266,264,128,299,144,314,280,301,265,291,300,
   323,267,302,268,292,354,153,334,303,282,271,274,154,272,310,225,
   259,160,131,328,132,257,256,135,294,130,312,137,358,161,140,324,
   321,258,295,162,147,353,148,246,296,283,163,286,129,261,308,352 },
 "±¡³£µ¥¶¦¹©ºª»«¼¬¾®¿¯àÀáÁâÂãÃäÄåÅæÆçÇèÈéÉêÊëËìÌíÍîÎïÏðÐñÑòÒóÓôÔõÕöÖøØùÙúÚûÛüÜýÝþÞ",
 "",0,0
};

CodePage TVCodePage::ISO8859_2_Lat2=
{ "ISO Latin 2 (Linux)",
  885920,
 {451,452,453,192,454,179,218,195,455,217,196,193,191,180,194,197,
  456,457,458,200,459,186,201,204,460,188,205,202,187,185,203,206,
  255,313,351,319,331,356,270, 21,345,304,309,305,273,341,306,316,
  248,311,349,317,340,325,260,350,344,297,307,326,262,348,298,315,
  269,263,289,330,142,266,264,128,299,144,314,280,301,265,291,300,
  323,267,302,268,292,354,153,334,303,282,271,274,154,272,310,225,
  259,160,131,328,132,257,256,135,294,130,312,137,358,161,140,324,
  321,258,295,162,147,353,148,246,296,283,163,286,129,261,308,352 },
 "±¡³£µ¥¶¦¹©ºª»«¼¬¾®¿¯àÀáÁâÂãÃäÄåÅæÆçÇèÈéÉêÊëËìÌíÍîÎïÏðÐñÑòÒóÓôÔõÕöÖøØùÙúÚûÛüÜýÝþÞ",
 "",0,0
};

CodePage TVCodePage::ISO8859_2u_Lat2=
{ "ISO Latin 2u(Linux)",
  885921,
 {269,263,289,330,142,266,264,128,299,144,314,280,301,265,291,300,
  323,267,302,268,292,354,153,334,303,282,271,274,154,272,310,225,
  255,313,351,319,331,356,270, 21,345,304,309,305,273,341,306,316,
  248,311,349,317,340,325,260,350,344,297,307,326,262,348,298,315,
  451,452,453,192,454,179,218,195,455,217,196,193,191,180,194,197,
  456,457,458,200,459,186,201,204,460,188,205,202,187,185,203,206,
  259,160,131,328,132,257,256,135,294,130,312,137,358,161,140,324,
  321,258,295,162,147,353,148,246,296,283,163,286,129,261,308,352 },
 "±¡³£µ¥¶¦¹©ºª»«¼¬¾®¿¯à€áâ‚ãƒä„å…æ†ç‡èˆé‰êŠë‹ìŒíîŽïðñ‘ò’ó“ô”õ•ö–ø˜ù™úšû›üœýþž",
 "",0,0
};

// This code page have the Euro
CodePage TVCodePage::ISO8859_2_Sun=
{ "ISO Latin 2 (Sun)",
  885922,
 {255,313,351,319,331,356,270, 21,345,304,309,305,273,341,306,316,
  248,311,349,317,340,325,260,350,344,297,307,326,262,348,298,315,
  269,263,289,330,142,266,264,128,299,144,314,280,301,265,291,300,
  176,177,178,179,180,323,267,302,268,185,186,187,188,292,354,191,
  192,193,194,195,196,197,153,334,200,201,202,203,204,205,206,303,
  282,271,274,154,272,310,225,587,216,217,218,219,220,221,222,223,
  259,160,131,328,132,257,256,135,294,130,312,137,358,161,140,324,
  321,258,295,162,147,353,148,246,296,283,163,286,129,261,308,352 },
 "‘“ƒ•…–†–‰šŠ›‹œŒžŽŸà á¡â¢ã£ä¤å¥æ¦ç§è¨é©êªë«ì¬í­î®ï¯ðµñ¶ó¸ô½õ¾öÆøÏùÐúÑûÒüÓýÔþÕ",
 "Ö",0,0
};

CodePage TVCodePage::ISO8859_2e_Lat2=
{ "ISO Latin 2+Euro (Linux)",
  885923,
 {451,452,453,192,454,179,218,195,455,217,196,193,191,180,194,197,
  456,457,458,200,459,186,201,204,460,188,205,202,187,185,203,206,
  255,313,351,319,587,356,270, 21,345,304,309,305,273,341,306,316,
  248,311,349,317,340,325,260,350,344,297,307,326,262,348,298,315,
  269,263,289,330,142,266,264,128,299,144,314,280,301,265,291,300,
  323,267,302,268,292,354,153,334,303,282,271,274,154,272,310,225,
  259,160,131,328,132,257,256,135,294,130,312,137,358,161,140,324,
  321,258,295,162,147,353,148,246,296,283,163,286,129,261,308,352 },
 "±¡³£µ¥¶¦¹©ºª»«¼¬¾®¿¯àÀáÁâÂãÃäÄåÅæÆçÇèÈéÉêÊëËìÌíÍîÎïÏðÐñÑòÒóÓôÔõÕöÖøØùÙúÚûÛüÜýÝþÞ",
 "",0,0
};

CodePage TVCodePage::ISO8859_3=
{ "ISO 8859-3 Latin 3",
  88593,
 { 218,196,191,192,217,179,195,180,194,193,197,201,205,187,200,188,
   186,199,182,209,207,219,178,177,176,223,220,222,221,254,254,254,
   255,534,351,156,331,0x20,468, 21,345,487,309,504,470,341,0x20,316,
   248,535,253,347,340,230,469,250,344,337,307,505,471,171,0x20,315,
   275,263,289,0x20,142,480,464,128,276,144,290,280,277,265,291,281,
   0x20,165,278,268,292,586,153,334,466,279,271,293,154,508,472,225,
   133,160,131,0x20,132,481,465,135,138,130,136,137,141,161,140,139,
   0x20,164,149,162,147,486,148,246,467,151,163,150,129,509,473,352
 },
 "±¡¶¦¹©ºª»«¼¬¿¯àÀáÁâÂäÄåÅæÆçÇèÈéÉêÊëËìÌíÍîÎïÏñÑòÒóÓôÔõÕöÖøØùÙúÚûÛüÜýÝþÞ",
 "ßµ",0,0
};

CodePage TVCodePage::ISO8859_4=
{ "ISO 8859-4 Latin 4",
  88594,
 { 218,196,191,192,217,179,195,180,194,193,197,201,205,187,200,188,
   186,199,182,209,207,219,178,177,176,223,220,222,221,254,254,254,
   255,313,461,516,331,518,544, 21,345,304,496,510,536,341,306,339,
   248,311,349,517,340,519,545,350,344,297,497,511,537,462,298,463,
   585,263,289,287,142,143,146,522,299,144,314,280,542,265,291,498,
   323,514,500,512,292,288,153,334,320,524,271,293,154,520,502,225,
   584,160,131,284,132,134,145,523,294,130,312,137,543,161,140,499,
   321,515,501,513,147,285,148,246,318,525,163,150,129,521,503,352
 },
 "±¡³£µ¥¶¦¹©ºª»«¼¬¾®¿½àÀáÁâÂãÃäÄåÅæÆçÇèÈéÉêÊëËìÌíÍîÎïÏðÐñÑòÒóÓôÔõÕöÖøØùÙúÚûÛüÜýÝþÞ",
 "ß¢",0,0
};

// PC866 Russian DOS code page
CodePage TVCodePage::stPC866=
{ "PC 866 Russian",
  866,
 { 360,361,588,362,363,589,364,409,365,366,590,367,591,592,593,368,
   594,595,596,369,370,597,371,372,373,374,375,376,377,378,379,380,
   598,381,382,383,384,599,385,386,387,388,389,390,391,392,600,393,
   176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,
   192,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207,
   208,209,210,211,212,213,214,215,216,217,218,219,220,221,222,223,
   601,602,394,395,396,603,397,398,399,400,401,402,403,404,405,406,
   605,604,407,408,610,608,409,410,248,249,250,251,411,331,254,255 },
 " €¡¢‚£ƒ¤„¥…¦†§‡¨ˆ©‰ªŠ«‹¬Œ­®Ž¯àá‘â’ã“ä”å•æ–ç—è˜é™êšë›ìœíîžïŸñðóòõô÷ö",
 "",0,0
};

// ISO 8859-5 Russian ISO layout
CodePage TVCodePage::ISO8859_5=
{ "ISO 8859-5 Russian",
  88595,
 { 218,196,191,192,217,179,195,180,194,193,197,201,205,187,200,188,
   186,199,182,209,207,219,178,177,176,223,220,222,221,254,254,254,
    32,605,412,414,407,612,416,610,611,417,419,421,423,341,425,427,
   360,361,588,362,363,589,364,409,365,366,590,367,591,592,593,368,
   594,595,596,369,370,597,371,372,373,374,375,376,377,378,379,380,
   598,381,382,383,384,599,385,386,387,388,389,390,391,392,600,393,
   601,602,394,395,396,603,397,398,399,400,401,402,403,404,405,406,
   411,604,413,415,408,606,607,608,609,418,420,422,424, 21,426,428 },
 "ñ¡ò¢ó£ô¤õ¥ö¦÷§ø¨ù©úªû«ü¬þ®ÿ¯Ð°Ñ±Ò²Ó³Ô´ÕµÖ¶×·Ø¸Ù¹ÚºÛ»Ü¼Ý½Þ¾ß¿àÀáÁâÂãÃäÄåÅæÆçÇèÈéÉêÊëËìÌíÍîÎïÏ",
 "",0,0
};

// KOD OBMENA INFORMATSIEY - 8bit RUSSKIY
// ëïä ïâíåîá éîæïòíáôóéåù I think that's close to the original ;-)
CodePage TVCodePage::KOI_8r=
{ "KOI-8r (Russian)",
  100000,
 { 196,179,218,191,192,217,195,180,194,193,197,223,220,219,221,222,
   176,177,178,244,254,249,251,247,243,242,255,245,248,253,250,246,
   205,186,213,604,214,201,184,183,187,212,211,200,190,189,188,198,
   199,204,181,605,182,185,209,210,203,207,208,202,216,215,206,336,
   405,598,381,397,384,599,396,383,603,387,388,389,390,391,392,600,
   393,406,601,602,394,395,385,382,403,402,386,399,404,400,398,401,
   379,360,361,371,363,589,370,362,597,365,366,590,367,591,592,593,
   368,380,594,595,596,369,364,588,377,376,409,373,378,374,372,375 },
 "ÀàÁáÂâÃãÄäÅåÆæÇçÈèÉéÊêËëÌìÍíÎîÏïÐðÑñÒòÓóÔôÕõÖöØøÙùÚúÛûÜüÝýÞþßÿ£³",
 "",0,0
};

// KOI8 with CRL/NMSU extensions for SerboCroat
CodePage TVCodePage::KOI_8crl=
{ "KOI-8 with CRL/NMSU",
  100001,
 { 218,196,191,192,217,179,195,180,194,193,197,201,205,187,200,188, // grf
   186,199,182,209,207,219,178,177,176,223,220,222,221,254,254,254, // grf
   0x20,0x20,0x20,604,413,609,418,420,422,428,0x20,0x20,0x20,0x20,0x20,0x20,
   0x20,0x20,0x20,605,412,611,417,419,421,427,0x20,0x20,0x20,0x20,0x20,0x20,
   405,598,381,397,384,599,396,383,603,387,388,389,390,391,392,600,
   393,406,601,602,394,395,385,382,403,402,386,399,404,400,398,401,
   379,360,361,371,363,589,370,362,597,365,366,590,367,591,592,593,
   368,380,594,595,596,369,364,588,377,376,409,373,378,374,372,375 },
 "£³¤´¥µ¦¶§·¨¸©¹ÀàÁáÂâÃãÄäÅåÆæÇçÈèÉéÊêËëÌìÍíÎîÏïÐðÑñÒòÓóÔôÕõÖö×÷ØøÙùÚúÛûÜüÝýÞþßÿ",
 "",0,0
};

// ECMA-Cyrillic ISO-IR-111
CodePage TVCodePage::ISO_IR_111=
{ "ECMA-Cyr.ISO-IR-111",
  0x0111000E,
 { 218,196,191,192,217,179,195,180,194,193,197,201,205,187,200,188,
   186,199,182,209,207,219,178,177,176,223,220,222,221,254,254,254,
   255,413,415,604,408,606,607,608,609,418,420,422,424,341,426,428,
   411,412,414,605,407,612,610,416,611,417,419,421,423,331,425,427,
   405,598,381,397,384,599,396,383,603,387,388,389,390,391,392,600,
   393,406,601,602,394,395,385,382,403,402,386,399,404,400,398,401,
   379,360,361,371,363,589,370,362,597,365,366,590,367,591,592,593,
   368,380,594,595,596,369,364,588,377,376,409,373,378,374,372,375 },
 "¡±¢²£³¤´¥µ¦¶§·¨¸©¹ªº«»¬¼®¾¯¿ÀàÁáÂâÃãÄäÅåÆæÇçÈèÉéÊêËëÌìÍíÎîÏïÐðÑñÒòÓóÔôÕõÖö×÷ØøÙùÚúÛûÜüÝýÞþßÿ",
 "",0,0
};

// GOST_19768-74 ST_SEV_358-88 ISO-IR-153 /subset of ISO-8859-5/
CodePage TVCodePage::ISO_IR_153=
{ "Cyrillic ISO-IR-153",
  0x0153000E,
 { 218,196,191,192,217,179,195,180,194,193,197,201,205,187,200,188,
   186,199,182,209,207,219,178,177,176,223,220,222,221,254,254,254,
   255,605,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,341,0x20,0x20,
   360,361,588,362,363,589,364,409,365,366,590,367,591,592,593,368,
   594,595,596,369,370,597,371,372,373,374,375,376,377,378,379,380,
   598,381,382,383,384,599,385,386,387,388,389,390,391,392,600,393,
   601,602,394,395,396,603,397,398,399,400,401,402,403,404,405,406,
   0x20,604,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20 },
 "ñ¡Ð°Ñ±Ò²Ó³Ô´ÕµÖ¶×·Ø¸Ù¹ÚºÛ»Ü¼Ý½Þ¾ß¿àÀáÁâÂãÃäÄåÅæÆçÇèÈéÉêÊëËìÌíÍîÎïÏ",
 "",0,0
};


// Mac OS Cyrillic, cp10007
CodePage TVCodePage::CP10007=
{ "Mac Cyr. CP 10007",
  10007,
 { 360,361,588,362,363,589,364,409,365,366,590,367,591,592,593,368,
   594,595,596,369,370,597,371,372,373,374,375,376,377,378,379,380,
   431,248,155,156, 21,  7, 20,610,335,336,436,412,413,448,414,415,
   236,241,243,242,607,230,235,611,407,408,416,608,417,418,419,420,
   609,612,170,251,159,247,560,174,175,430,255,421,422,423,424,606,
    45,341,434,435, 96, 39,246,429,425,426,427,428,411,605,604,406,
   598,381,382,383,384,599,385,386,387,388,389,390,391,392,600,393,
    601,602,394,395,396,603,397,398,399,400,401,402,403,404,405,331 },
 "à€áâ‚ãƒä„å…æ†ç‡èˆé‰êŠë‹ìŒíîŽïðñ‘ò’ó“ô”õ•ö–÷—ø˜ù™úšû›üœýþžßŸ¬«¯®¹¸»º½¼¿¾ÌËÎÍÞÝÙØÛÚÀ·ÏÁ¶Æ",
 "´µÄ",0,0
};

// Mac OS Ukrainian
CodePage TVCodePage::CP100072=
{ "Mac OS Ukrainian",
  100072,
 { 360,361,588,362,363,589,364,409,365,366,590,367,591,592,593,368,
   594,595,596,369,370,597,371,372,373,374,375,376,377,378,379,380,
   431,248,437,156, 21,  7, 20,610,335,336,436,412,413,448,414,415,
   236,241,243,242,607,230,438,611,407,408,416,608,417,418,419,420,
   609,612,170,251,159,247,560,174,175,430,255,421,422,423,424,606,
    45,341,434,435, 96, 39,246,429,425,426,427,428,411,605,604,406,
   598,381,382,383,384,599,385,386,387,388,389,390,391,392,600,393,
   601,602,394,395,396,603,397,398,399,400,401,402,403,404,405,331 },
 "à€áâ‚ãƒä„å…æ†ç‡èˆé‰êŠë‹ìŒíîŽïðñ‘ò’ó“ô”õ•ö–÷—ø˜ù™úšû›üœýþžßŸ¶¢¬«¯®À·¹¸»º½¼¿¾ÏÁÌËÎÍÙØÛÚÞÝ",
 "§´µÄÆ",0,0
};

// Osnovnoj Variant Russian
CodePage TVCodePage::OVR=
{ "Osnovnoj Variant Russian",
  885951,
 { 218,196,191,192,217,179,195,180,194,193,197,201,205,187,200,188, // grf
   186,199,182,209,207,219,178,177,176,223,220,222,221,254,254,254, // grf
   0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
   360,361,588,362,363,589,364,409,365,366,590,367,591,592,593,368,
   594,595,596,369,370,597,371,372,373,374,375,376,377,378,379,380,
   598,381,382,383,384,599,385,386,387,388,389,390,391,392,600,393,
   601,602,394,395,396,603,397,398,399,400,401,402,403,404,405,406,
   605,604,340, 96,340, 96, 26, 27, 25, 24,246,241,411,331,0x20,0x20 },
 "Ð°Ñ±Ò²Ó³Ô´ÕµÖ¶×·Ø¸Ù¹ÚºÛ»Ü¼Ý½Þ¾ß¿àÀáÁâÂãÃäÄåÅæÆçÇèÈéÉêÊëËìÌíÍîÎïÏñð",
 "",0,0
};

// Alternativnyj Variant Russian
CodePage TVCodePage::AVR=
{ "Alternativnyj Variant RU",
  885952,
 { 360,361,588,362,363,589,364,409,365,366,590,367,591,592,593,368,
   594,595,596,369,370,597,371,372,373,374,375,376,377,378,379,380,
   598,381,382,383,384,599,385,386,387,388,389,390,391,392,600,393,
   218,196,191,192,217,179,195,180,194,193,197,201,205,187,200,188, // grfs
   186,199,182,209,207,219,178,177,176,223,220,222,221,254,254,254, // grfs
   0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
   601,602,394,395,396,603,397,398,399,400,401,402,403,404,405,406,
   605,604,340, 96,340, 96, 26, 27, 25, 24,246,241,411,331,0x20,0x20 },
 " €¡¢‚£ƒ¤„¥…¦†§‡¨ˆ©‰ªŠ«‹¬Œ­®Ž¯àá‘â’ã“ä”å•æ–ç—è˜é™êšë›ìœíîžïŸñð",
 "",0,0
};

// U-code Russian /subset of ISO-8859-5 and GOST_19768-74/
CodePage TVCodePage::U_CodeR=
{ "U-code Russian",
  885953,
 { 218,196,191,192,217,179,195,180,194,193,197,201,205,187,200,188, // grf
   186,199,182,209,207,219,178,177,176,223,220,222,221,254,254,254, // grf
   255,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
   360,361,588,362,363,589,364,409,365,366,590,367,591,592,593,368,
   594,595,596,369,370,597,371,372,373,374,375,376,377,378,379,380,
   598,381,382,383,384,599,385,386,387,388,389,390,391,392,600,393,
   601,602,394,395,396,603,397,398,399,400,401,402,403,404,405,406,
   0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20 },
 "Ð°Ñ±Ò²Ó³Ô´ÕµÖ¶×·Ø¸Ù¹ÚºÛ»Ü¼Ý½Þ¾ß¿àÀáÁâÂãÃäÄåÅæÆçÇèÈéÉêÊëËìÌíÍîÎïÏ",
 "",0,0
};

/***** 7 bits! ******/
ushort TVCodePage::tbKOI7[]=
{ 0,  1,  2,  3,  4,  5,  6,  7,
  8,  9, 10, 11, 12, 13, 14, 15,
 16, 17, 18, 19, 20, 21, 22, 23,
 24, 25, 26, 27, 28, 29, 30, 31,
 32, 33, 34, 35,331, 37, 38, 39,
 40, 41, 42, 43, 44, 45, 46, 47,
 48, 49, 50, 51, 52, 53, 54, 55,
 56, 57, 58, 59, 60, 61, 62, 63,
405,598,381,397,384,599,396,383,
603,387,388,389,390,391,392,600,
393,406,601,602,394,395,385,382,
403,402,386,399,404,400,398,401,
379,360,361,371,363,589,370,362,
597,365,366,590,367,591,592,593,
368,380,594,595,596,369,364,588,
377,376,409,373,378,374,372,127
};

CodePage TVCodePage::KOI_7=
{
 "ISO 5427 ISO-IR-37 KOI-7",
 0x0037000E,
 {
   32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
   48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,
   64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
  218,196,191,192,217,179,195,180,194,193,197,201,205,187,200,188,
  186,199,182,209,207,219,178,177,176,223,220,222,221,254,254,254,
   80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95,
   96, 97, 98, 99,100,101,102,103,104,105,106,107,108,109,110,111,
  112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127
 },
 "@`AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz[{\\|]}^~á¡â¢ã£ä¤å¥æ¦ç§è¨é©êªë«ì¬í­î®ï¯ðÐñÑòÒóÓôÔõÕöÖ÷×øØùÙúÚ",
 "_",
 128,TVCodePage::tbKOI7
};

ushort TVCodePage::tbISOIR147[]=
{   0,  1,  2,  3,  4,  5,  6,  7,
  8,  9, 10, 11, 12, 13, 14, 15,
 16, 17, 18, 19, 20, 21, 22, 23,
 24, 25, 26, 27, 28, 29, 30, 31,
 32, 33, 34, 35, 36, 37, 38, 39,
 40, 41, 42, 43, 44, 45, 46, 47,
 48, 49, 50, 51, 52, 53, 54, 55,
 56, 57, 58, 59, 60, 61, 62, 63,
364,360,361,371,363,589,370,362,
597,365,611,590,367,591,592,593,
368,417,594,595,596,369,588,419,
427,612,409,373,414,423,372, 95,
385,598,381,397,384,599,396,383,
603,387,609,389,390,391,392,600,
393,418,601,602,394,395,382,420,
428,606,386,399,415,424,398,127 };

CodePage TVCodePage::ISO_IR_147=
{
 "JUS_I.B1.003-MAC ISO-IR-147",
 0x0147000E,
 {
   32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
   48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,
   64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
  218,196,191,192,217,179,195,180,194,193,197,201,205,187,200,188,
  186,199,182,209,207,219,178,177,176,223,220,222,221,254,254,254,
   80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95,
   96, 97, 98, 99,100,101,102,103,104,105,106,107,108,109,110,111,
  112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127
 },
 "`@aAbBcCdDeEfFgGhHiIjJkKlLmMnNoOpPqQrRsStTuUvVwWxXyYzZ{[|\\}]~^á¡â¢ã£ä¤å¥æ¦ç§è¨é©êªë«ì¬í­î®ï¯ðÐñÑòÒóÓôÔõÕöÖ÷×øØùÙúÚ",
 "",
 128,TVCodePage::tbISOIR147
};

ushort TVCodePage::tbISOIR146[]=
{ 0,  1,  2,  3,  4,  5,  6,  7,
  8,  9, 10, 11, 12, 13, 14, 15,
 16, 17, 18, 19, 20, 21, 22, 23,
 24, 25, 26, 27, 28, 29, 30, 31,
 32, 33, 34, 35, 36, 37, 38, 39,
 40, 41, 42, 43, 44, 45, 46, 47,
 48, 49, 50, 51, 52, 53, 54, 55,
 56, 57, 58, 59, 60, 61, 62, 63,
364,360,361,371,363,589,370,362,
597,365,611,590,367,591,592,593,
368,417,594,595,596,369,588,419,
427,612,409,373,412,421,372, 95,
385,598,381,397,384,599,396,383,
603,387,609,389,390,391,392,600,
393,418,601,602,394,395,382,420,
428,606,386,399,413,422,398,127 };

CodePage TVCodePage::ISO_IR_146=
{
 "JUS_I.B1.003-SERB ISOIR146",
 0x0146000E,
 {
   32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
   48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,
   64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
  218,196,191,192,217,179,195,180,194,193,197,201,205,187,200,188,
  186,199,182,209,207,219,178,177,176,223,220,222,221,254,254,254,
   80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95,
   96, 97, 98, 99,100,101,102,103,104,105,106,107,108,109,110,111,
  112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127
 },
 "`@aAbBcCdDeEfFgGhHiIjJkKlLmMnNoOpPqQrRsStTuUvVwWxXyYzZ{[|\\}]~^á¡â¢ã£ä¤å¥æ¦ç§è¨é©êªë«ì¬í­î®ï¯ðÐñÑòÒóÓôÔõÕöÖ÷×øØùÙúÚ",
 "",
 128,TVCodePage::tbISOIR146
};

ushort TVCodePage::Low32CharsRemaped[]=
{ 197,196,191,192,217,179,195,  7,194,193,218,219,178,177,176,180,
   16, 17, 18,220,222,221,254, 23, 24, 25, 26, 27,223, 29, 30, 31};

// PC1250 Windows latin 2 code page
CodePage TVCodePage::stPC1250=
{ "CP 1250 Win Latin 2",
  1250,
 { 0x20,0x20, 44,0x20,429,430,431,432,0x20,433,304, 60,270,305,306,273,
   0x20, 96, 39,434,435,  7, 45,341,0x20,436,297, 62,260,326,298,262,
   255,350,351,319,331,313,338, 21,345,336,309,174,170,341,335,316,
   248,241,349,317,340,230, 20,250,344,311,307,175,327,348,325,315,
   269,263,289,330,142,266,264,128,299,144,314,280,301,265,291,300,
   323,267,302,268,292,354,153,334,303,283,271,274,154,272,310,225,
   259,160,131,328,132,257,256,135,294,130,312,137,358,161,140,324,
   321,258,295,162,147,353,148,246,296,282,163,286,129,261,308,352 },
 "šŠœŒŸžŽ³£¹¥ºª¾¼¿¯àÀáÁâÂãÃäÄåÅæÆçÇèÈéÉêÊëËìÌíÍîÎïÏðÐñÑòÒóÓôÔõÕöÖøØùÙúÚûÛüÜýÝþÞ",
 "µß",
 32,TVCodePage::Low32CharsRemaped
};

// PC1251 Russian Windows code page
CodePage TVCodePage::PC1251=
{ "CP 1251 Win Russian",
  1251,
 { 412,414, 44,415,429,430,431,432,0x20,433,417, 60,419,423,421,427,
   413, 96, 39,434,435,  7, 45,341,0x20,436,418, 62,420,424,422,428,
   255,425,426,611,331,437,338, 21,605,336,407,174,170,341,335,416,
   248,241,610,607,438,230, 20,250,604,411,408,175,609,612,606,608,
   360,361,588,362,363,589,364,409,365,366,590,367,591,592,593,368,
   594,595,596,369,370,597,371,372,373,374,375,376,377,378,379,380,
   598,381,382,383,384,599,385,386,387,388,389,390,391,392,600,393,
   601,602,394,395,396,603,397,398,399,400,401,402,403,404,405,406 },
 "€ƒšŠœŒžŽŸ¢¡¼£´¥ºª¿¯²³¸¨¾½àÀáÁâÂãÃäÄåÅæÆçÇèÈéÉêÊëËìÌíÍîÎïÏðÐñÑòÒóÓôÔõÕöÖ÷×øØùÙúÚûÛüÜýÝþÞÿß",
 "µ",
 32,Low32CharsRemaped
};

// PC1252 Windows latin 1 code page
CodePage TVCodePage::stPC1252=
{ "CP 1252 Win Latin 1",
  1252,
 { 0x20,0x20, 44,159,429,430,431,432, 94,433,304, 60,440,0x20,306,0x20,
   0x20, 96, 39,434,435,  7, 45,341,126,436,297, 62,441,0x20,298,450,
   255,173,155,156,331,157,338, 21,345,336,166,174,170,341,335,339,
   248,241,253,347,340,230, 20,250,344,346,167,175,172,171,343,168,
   275,263,289,287,142,143,146,128,276,144,290,280,277,265,291,281,
   323,165,278,268,292,288,153,334,320,279,271,293,154,272,333,225,
   133,160,131,284,132,134,145,135,138,130,136,137,141,161,140,139,
   322,164,149,162,147,285,148,246,318,151,163,150,129,261,332,152 },
 "œŒšŠžŽÿŸàÀáÁâÂãÃäÄåÅæÆçÇèÈéÉêÊëËìÌíÍîÎïÏðÐñÑòÒóÓôÔõÕöÖøØùÙúÚûÛüÜýÝþÞ",
 "ƒµß",
 32,Low32CharsRemaped
};

// PC1253 Windows greek code page
CodePage TVCodePage::stPC1253=
{ "CP 1253 Win Greek",
  1253,
 { 0x20,0x20, 44,159,429,430,431,432,0x20,433,0x20, 60,0x20,0x20,0x20,0x20,
   0x20, 96, 39,434,435,  7, 45,341,0x20,436,0x20, 62,0x20,0x20,0x20,0x20,
   255,564,572,156,331,157,338, 21,345,336,0x20,174,170,341,335,341,
   248,241,253,347,340,230, 20,250,573,574,575,175,576,171,577,578,
   581,617,618,226,560,619,620,621,233,622,623,583,624,625,561,626,
   562,627,0x20,228,628,629,232,630,563,234,631,632,565,566,567,568,
   582,224,613,546,235,238,547,548,549,550,614,551,615,552,553,616,
   227,554,555,229,231,556,237,557,558,559,579,580,569,570,571,0x20 },
 "µÌáÁâÂãÃäÄåÅæÆçÇèÈéÉêÊëËìÌíÍîÎïÏðÐñÑòÓóÓôÔõÕöÖ÷×øØùÙÜ¢Ý¸Þ¹ßºü¼ý¾þ¿úÚûÛ",
 "ƒÀà",
 32,Low32CharsRemaped
};

// PC1254 Windows
CodePage TVCodePage::stPC1254=
{ "CP 1254 Win Turkish",
  1254,
 { 0x20,0x20, 44,159,429,430,431,432, 94,433,304, 60,440,0x20,0x20,0x20,
   0x20, 96, 39,434,435,  7, 45,341,126,436,297, 62,441,0x20,0x20,450,
   255,173,155,156,331,157,338, 21,345,336,166,174,170,341,335,339,
   248,241,253,347,340,230, 20,250,344,346,167,175,172,171,343,168,
   275,263,289,287,142,143,146,128,276,144,290,280,277,265,291,281,
   504,165,278,268,292,288,153,334,320,279,271,293,154,487,309,225,
   133,160,131,284,132,134,145,135,138,130,136,137,141,161,140,139,
   505,164,149,162,147,285,148,246,318,151,163,150,129,337,307,152 },
 "àÀáÁâÂãÃäÄåÅæÆçÇèÈéÉêÊëËìÌíÍîÎïÏðÐñÑòÒóÓôÔõÕöÖøØùÙúÚûÛüÜýÝþÞšŠœŒÿŸ",
 "ƒµß",
 32,Low32CharsRemaped
};

// PC1257 Windows
CodePage TVCodePage::stPC1257=
{ "CP 1257 Win Baltic",
  1257,
 { 0x20,0x20, 44,0x20,429,430,431,432,0x20,433,0x20, 60,0x20,345,350,344,
   0x20, 96, 39,434,435,  7, 45,341,0x20,436,0x20, 62,0x20,339,349,0x20,
   255,0x20,155,156,331,0x20,338, 21,320,336,516,174,170,341,335,146,
   248,241,253,347,340,230, 20,250,318,346,517,175,172,171,343,145,
   313,522,585,264,142,143,314,496,299,144,273,542,510,512,498,544,
   304,267,514,268,500,288,153,334,524,319,270,502,154,316,306,225,
   311,523,584,256,132,134,312,497,294,130,262,543,511,513,499,545,
   297,258,515,162,501,285,148,246,525,317,260,503,129,315,298,352
 },
 "¸¨ºª¿¯àÀáÁâÂãÃäÄåÅæÆçÇèÈéÉêÊëËìÌíÍîÎïÏðÐñÑòÒóÓôÔõÕöÖ÷×øØùÙúÚûÛüÜýÝþÞ",
 "µß",
 32,Low32CharsRemaped
};

CodePage TVCodePage::ISO8859_7=
{ "ISO 8859-7 Greek",
  88597,
 { 218,196,191,192,217,179,195,180,194,193,197,201,205,187,200,188,
   186,199,182,209,207,219,178,177,176,223,220,222,221,254,254,254,
   255, 96, 39,156, 32, 32,338, 21,345,336, 32,174,170,341, 32,341,
   248,241,253,347,340,564,572,250,573,574,575,175,576,171,577,578,
   581,617,618,226,560,619,620,621,233,622,623,583,624,625,561,626,
   562,627, 32,228,628,629,232,630,563,234,631,632,565,566,567,568,
   582,224,613,546,235,238,547,548,549,550,614,551,615,552,553,616,
   227,554,555,229,231,556,237,557,558,559,579,580,569,570,571, 32 },
 "Ü¶Ý¸Þ¹ßºü¼ý¾þ¿úÚûÛáÁâÂãÃäÄåÅæÆçÇèÈéÉêÊëËìÌíÍîÎïÏðÐñÑòÓóÓôÔõÕöÖ÷×øØùÙ",
 "àÀ",0,0
};

// ISO 8859-9 (1989) Pure
CodePage TVCodePage::ISO8859_9=
{ "ISO 8859-9",
  88599,
 { 218,196,191,192,217,179,195,180,194,193,197,201,205,187,200,188,
   186,199,182,209,207,219,178,177,176,223,220,222,221,254,254,254,
   255,173,155,156,331,157,338, 21,345,336,166,174,170,341,335,339,
   248,241,253,347,340,230, 20,250,344,346,167,175,172,171,343,168,
   275,263,289,287,142,143,146,128,276,144,290,280,277,265,291,281,
   504,165,278,268,292,288,153,334,320,279,271,293,154,487,309,225,
   133,160,131,284,132,134,145,135,138,130,136,137,141,161,140,139,
   505,164,149,162,147,285,148,246,318,151,163,150,129,337,307,152 },
 "àÀáÁâÂãÃäÄåÅæÆçÇèÈéÉêÊëËìÌíÍîÎïÏðÐñÑòÒóÓôÔõÕöÖøØùÙúÚûÛüÜýÝþÞ",
 "µßÿ",0,0
};

// ISO/IEC 8859-14:1998
CodePage TVCodePage::ISO8859_14=
{ "ISO 8859-14",
  885914,
 { 218,196,191,192,217,179,195,180,194,193,197,201,205,187,200,188,
   186,199,182,209,207,219,178,177,176,223,220,222,221,254,254,254,
   255,478,479,156,480,481,482, 21,526,336,530,483,528,341,335,450,
   484,485,586,486,488,489, 20,490,527,491,531,492,529,532,533,493,
   275,263,289,287,142,143,146,128,276,144,290,280,277,265,291,281,
   474,165,278,268,292,288,153,494,320,279,271,293,154,272,476,225,
   133,160,131,284,132,134,145,135,138,130,136,137,141,161,140,139,
   475,164,149,162,147,285,148,495,318,151,163,150,129,261,477,152
 },
 "¢¡¥¤«¦¸¨ºª¼¬±°³²µ´¹·¿»¾½àÀáÁâÂãÃäÄåÅæÆçÇèÈéÉêÊëËìÌíÍîÎïÏðÐñÑòÒóÓôÔõÕöÖ÷×øØùÙúÚûÛüÜýÝþÞÿ¯",
 "ß",0,0
};

// ISO/IEC 8859-15:1998
CodePage TVCodePage::ISO8859_15=
{ "ISO 8859-15 Icelan.",
  885915,
 { 218,196,191,192,217,179,195,180,194,193,197,201,205,187,200,188,
   186,199,182,209,207,219,178,177,176,223,220,222,221,254,254,254,
   255,173,155,156,0x20,157,304, 21,297,336,166,174,170,341,335,339,
   248,241,253,347,306,230, 20,250,298,346,167,175,440,441,450,168,
   275,263,289,287,142,143,146,128,276,144,290,280,277,265,291,281,
   323,165,278,268,292,288,153,334,320,279,271,293,154,272,333,225,
   133,160,131,284,132,134,145,135,138,130,136,137,141,161,140,139,
   322,164,149,162,147,285,148,246,318,151,163,150,129,261,332,152 },
 "àÀáÁâÂãÃäÄåÅæÆçÇèÈéÉêÊëËìÌíÍîÎïÏðÐñÑòÒóÓôÔõÕöÖøØùÙúÚûÛüÜýÝþÞ",
 "µßÿ",0,0
};

// That's a special polish code page
// This is PC437 changing: 134,141,143,144,145,146,149,152,156,158,160-167
CodePage TVCodePage::stMazovia=
{ "Mazovia (polish)",
  1000000,
 { 128,129,130,131,132,133,311,135,136,137,138,139,140,256,142,313,
   314,312,317,147,148,264,150,151,270,153,154,155,319,157,260,159,
   273,316,162,268,258,267,262,315,168,169,170,171,172,173,174,175,
   176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,
   192,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207,
   208,209,210,211,212,213,214,215,216,217,218,219,220,221,222,223,
   224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,
   240,241,242,243,244,245,246,247,248,249,250,251,252,253,254,255 },
 "†•‘’œž˜¦ §¡¢£¤¥„Ž‡€š”™",
 "‚ƒ…ˆ‰Š‹Œ“–—á",0,0
};

/**[txh]********************************************************************

  Description:
  Protected member used to create the list of known code pages stored in
CodePages.

***************************************************************************/

void TVCodePage::CreateCodePagesCol()
{
 CodePages=new TVCodePageCol(43,3);

 #define a(v) CodePages->insert(&v)
 // Total: 47
 // Latin (27)
 a(stPC437);
 a(stPC775);
 a(stPC850);
 a(stPC852);
 a(stPC857);
 a(stPC860);
 a(stPC861);
 a(stPC863);
 a(stPC865);
 a(ISO8879_1);
 a(ISO8859_1_Lat1);
 a(ISO8859_1u_Lat1);
 a(ISO8879_2);
 a(ISO8859_2_Lat2);
 a(ISO8859_2u_Lat2);
 a(ISO8859_2e_Lat2);
 a(ISO8859_2_Sun);
 a(ISO8859_3);
 a(ISO8859_4);
 a(ISO8859_9);
 a(ISO8859_14);
 a(ISO8859_15);
 a(stPC1250);
 a(stPC1252);
 a(stPC1254);
 a(stPC1257);
 a(stMazovia);
 // Russian (16)
 a(stPC855);
 a(stPC866);
 a(ISO8859_5);
 a(KOI_8r);
 a(KOI_8crl);
 a(PC1251);
 a(ISO_IR_111);
 a(ISO_IR_153);
 a(CP10007);
 a(CP100072);
 a(OVR);
 a(AVR);
 a(U_CodeR);
 a(KOI_7);
 a(ISO_IR_147);
 a(ISO_IR_146);
 // Greek (4)
 a(stPC737);
 a(stPC869);
 a(stPC1253);
 a(ISO8859_7);
 #undef a
}

/**[txh]********************************************************************

  Description:
  Initializes the code page system selecting the indicated code pages. This
is only used iternally. @x{::SetCodePage}.
  
***************************************************************************/

TVCodePage::TVCodePage(int idApp, int idScr, int idInp)
{
 if (!CodePages)
    CreateCodePagesCol();
 FillTables(idApp);
 CreateOnTheFlyRemap(idApp,idScr);
 curScrCP=defScrCP=idScr;
 CreateOnTheFlyInpRemap(idInp,idApp);
 curInpCP=defInpCP=idInp;
 defAppCP=idApp;
 TGKey::SetCodePage(idInp);
 if (idApp!=curAppCP)
   {
    curAppCP=idApp; // After filling the tables
    RemapTVStrings(GetTranslate(curAppCP));
   }
}

/**[txh]********************************************************************

  Description:
  Protected member used to create a one to one table to convert from one
code page to another. Used to create the input to application and application
to screen on the fly convertion tables.

***************************************************************************/

void TVCodePage::CreateRemap(int idSource, int idDest, uchar *table)
{
 unsigned i;
 // Table to convert a value into an internal code.
 // That's what we need.
 ushort *toCode=GetTranslate(idSource),*aux;
 // Table to convert an internal value into a 0-255 value.
 // That's what we have. If something is missing we will remap to a similar.
 uchar *fromCode=new uchar[maxSymbolDefined];
 memset(fromCode,0,maxSymbolDefined*sizeof(uchar));
 CodePage *destCP=CodePageOfID(idDest);
 aux=destCP->Font;
 for (i=0; i<128; i++)
     if (aux[i]<maxSymbolDefined) // extra check, just in case I forgot to update the constant
        fromCode[aux[i]]=i+128;

 i=0;
 if (destCP->LowRemapNum)
   {
    unsigned to=destCP->LowRemapNum;
    aux=destCP->LowRemap;
    for (; i<to; i++)
        if (aux[i]<maxSymbolDefined)
           fromCode[aux[i]]=i;
   }
 for (; i<128; i++)
     fromCode[i]=i;

 // Adjust values found in source but not in dest (look for similars)
 for (i=1; i<256; i++)
    {
     unsigned val=toCode[i];
     if (fromCode[val])
        continue;
     while (!fromCode[val])
       { // Find an equivalent for val
        if (val<256)
           val=Similar[val];
        else
           val=Similar2[val-256];
       }
     fromCode[toCode[i]]=fromCode[val];
    }

 for (i=0; i<256; i++)
     table[i]=fromCode[toCode[i]];

 DeleteArray(fromCode);
}

/**[txh]********************************************************************

  Description:
  Protected member used to create the application to screen map when we have
to translate values on the fly. @x{::CreateRemap}.

***************************************************************************/

void TVCodePage::CreateOnTheFlyRemap(int idApp, int idScr)
{
 // Create on-the-fly remap table if needed
 if (idApp==idScr)
   {
    NeedsOnTheFlyRemap=0;
    return;
   }
 NeedsOnTheFlyRemap=1;
 CreateRemap(idApp,idScr,OnTheFlyMap);

 /* To debug the remapping table:
 fputs("Usando remapeo:\n",stderr);
 for (i=0; i<256; i++)
    {
     fprintf(stderr,"0x%02X,",OnTheFlyMap[i]);
     if (!((i+1)&0xF)) fputc('\n',stderr);
    }*/
}

/**[txh]********************************************************************

  Description:
  Protected member used to create the input to application map when we have
to translate values on the fly. @x{::CreateRemap}.

***************************************************************************/

void TVCodePage::CreateOnTheFlyInpRemap(int idInp, int idApp)
{
 // Unicode tables
 // Create a table to convert 8 bits inp. code page into 16 bits unicode
 ushort *internals=GetTranslate(idInp);
 unsigned i;
 for (i=0; i<256; i++)
     inpToUnicode[i]=UnicodeForInternalCode(internals[i]);

 if (!unicodeToInp || curInpCP!=idInp)
   {
    // Create a "partition tree" to convert a 16 bits unicode into 8 bits inp
    // code page
    delete unicodeToInp;
    unicodeToInp=NULL;
    if (idInp!=idApp)
      {
       unicodeToInp=new TVPartitionTree556();
       for (i=0; i<256; i++)
           unicodeToInp->add(appToUnicode[i],i);
      }
   }

 // Remap tables
 if (idInp==idApp)
   {
    NeedsOnTheFlyInpRemap=0;
    return;
   }
 NeedsOnTheFlyInpRemap=1;
 CreateRemap(idInp,idApp,OnTheFlyInpMap);
}

/**[txh]********************************************************************

  Description:
  Selects the current code page used for toupper, tolower, etc. operations
and internal encodings, the code page used for the screen and the one used
for input.@p
  If any of the arguments is -1 the current value is used. If the code pages
aren't the same the remap on the fly is enabled. The application code page
is used to remap the application, only if it changed.

***************************************************************************/

void TVCodePage::SetCodePage(int idApp, int idScr, int idInp)
{
 //fprintf(stderr,"idApp %d idScr %d idInp %d\n",idApp,idScr,idInp);
 if (idApp==-1)
    idApp=curAppCP;
 if (idScr==-1)
    idScr=curScrCP;
 if (idInp==-1)
    idInp=curInpCP;
 if (curAppCP!=idApp || curScrCP!=idScr)
    CreateOnTheFlyRemap(idApp,idScr);
 if (curAppCP!=idApp || curInpCP!=idInp)
    CreateOnTheFlyInpRemap(idInp,idApp);
 curScrCP=idScr;
 curInpCP=idInp;
 TGKey::SetCodePage(idInp);
 if (curAppCP!=idApp)
   {
    FillTables(idApp);
    curAppCP=idApp; // After filling the tables
    RemapTVStrings(GetTranslate(curAppCP));
   }
}

/**[txh]********************************************************************

  Description:
  Protected member used to create the toupper, tolower and isalpha tables.
Also creates tables to convert code page <-> Unicode.

***************************************************************************/

void TVCodePage::FillTables(int id)
{
 CodePage *p=CodePageOfID(id);
 int i;
 // Initialize toUpperTable, toLowerTable and AlphaTable:
 // 1) ASCII and defaults
 memset(AlphaTable,0,sizeof(AlphaTable));
 for (i=0; i<256; i++)
    {
     if (i>='a' && i<='z')
       {
        toUpperTable[i]=i-32;
        AlphaTable[i]=alphaChar | lowerChar;
       }
     else
        toUpperTable[i]=i;
     if (i>='A' && i<='Z')
       {
        toLowerTable[i]=i+32;
        AlphaTable[i]=alphaChar | upperChar;
       }
     else
        toLowerTable[i]=i;
     if (i>='0' && i<='9')
        AlphaTable[i]=digitChar;
    }
 // 2) For this code page
 uchar *s=(uchar *)p->UpLow;
 if (s)
    for (; *s; s+=2)
       {
        toLowerTable[*(s+1)]=*s;
        toUpperTable[*s]=*(s+1);
        AlphaTable[*s]=alphaChar | lowerChar;
        AlphaTable[*(s+1)]=alphaChar | upperChar;
       }
 s=(uchar *)p->MoreLetters;
 if (s)
    for (; *s; s++)
        AlphaTable[*s]=alphaChar;

 // Create a table to convert 8 bits app. code page into 16 bits unicode
 ushort *internals=GetTranslate(id);
 for (i=0; i<256; i++)
     appToUnicode[i]=UnicodeForInternalCode(internals[i]);

 // Create a "partition tree" to convert a 16 bits unicode into 8 bits app
 // code page
 if (!unicodeToApp || id!=curAppCP)
   {
    delete unicodeToApp;
    unicodeToApp=new TVPartitionTree556();
    for (i=0; i<256; i++)
        unicodeToApp->add(appToUnicode[i],i);
   }

 /* Dump tables
 fputs("uchar          TVCodePage::AlphaTable[256]=\n{\n",stderr);
 for (i=0; i<256; i++)
    {
     fprintf(stderr,"0x%02X,",AlphaTable[i]);
     if (!((i+1)&0xF)) fputc('\n',stderr);
    }
 fputs("};\nuchar          TVCodePage::toUpperTable[256]=\n{\n",stderr);
 for (i=0; i<256; i++)
    {
     fprintf(stderr,"0x%02X,",toUpperTable[i]);
     if (!((i+1)&0xF)) fputc('\n',stderr);
    }
 fputs("};\nuchar          TVCodePage::toLowerTable[256]=\n{\n",stderr);
 for (i=0; i<256; i++)
    {
     fprintf(stderr,"0x%02X,",toLowerTable[i]);
     if (!((i+1)&0xF)) fputc('\n',stderr);
    }
 fputs("};\n",stderr); */
}

/**[txh]********************************************************************

  Description:
  Deallocates memory used by TVCodePage.

***************************************************************************/

TVCodePage::~TVCodePage()
{
 CLY_destroy(CodePages);
 CodePages=NULL;
 if (unicodeToApp)
    delete unicodeToApp;
}

/**[txh]********************************************************************

  Description:
  Converts a code page id into an index in the code page collection.@*
  Important note: We should always default to 437 because:@*
1) That's the original encoding used by TV applications.@*
2) Some asiatic encodings, like code page 936 (simplified chinese, maybe
also 950 [traditional chinese], 932 [japanese Shift-JIS] and 949 [korean]),
behaves like 437 when we write to the video buffer.@*

  Return: The index of the code page with this id. If error the index for
PC 437 code page is returned.

***************************************************************************/

ccIndex TVCodePage::IDToIndex(int id)
{
 if (!CodePages) return 0;
 ccIndex c=CodePages->getCount();
 ccIndex i, i437=0;
 for (i=0; i<c; i++)
   {
    CodePage *p=(CodePage *)(CodePages->at(i));
    if (p->id==id)
       return i;
    if (p->id==PC437)
       i437=i;
   }
 return i437;
}

/**[txh]********************************************************************

  Description:
  Converts an index in the code page collection into a code page id. No
check of range is done.
  
  Return: The code page id for the indicated index.
  
***************************************************************************/

int TVCodePage::IndexToID(ccIndex index)
{
 if (!CodePages) return 0;
 CodePage *p=(CodePage *)(CodePages->at(index));
 return p->id;
}

/**[txh]********************************************************************

  Description:
  Used to get a map to translate code page symbols into internal symbols for
the indicated code page id.
  
  Return: A pointer to a static buffer containing the translation values. Or
NULL if not initialized. The index 256 indicates 128 if only the upper 128
values are really different from CP 437 or 256 if all the values are
different. In any case all the values are usable.
  
***************************************************************************/

ushort *TVCodePage::GetTranslate(int id)
{
 if (!CodePages)
    CreateCodePagesCol();
 CodePage *p=CodePageOfID(id);
 memcpy(CPTable+128,p->Font,sizeof(short)*128);
 CPTable[256]=128;
 int i=0;
 if (p->LowRemapNum)
   {
    CPTable[256]=0;
    memcpy(CPTable,p->LowRemap,sizeof(short)*p->LowRemapNum);
    i=p->LowRemapNum;
   }
 for (; i<128; i++)
     CPTable[i]=i;

 return CPTable;
}

/**[txh]********************************************************************

  Description:
  Fills a table with pairs Unicode/code in CP. The table is sorted and the
user must provide an array with at least 256 elements to fill.
  
***************************************************************************/

void TVCodePage::GetUnicodesForCP(int id, stIntCodePairs *unicodes)
{
 ushort *internals=GetTranslate(id);
 int i;
 for (i=0; i<256; i++)
    {
     unicodes[i].unicode=UnicodeForInternalCode(internals[i]);
     unicodes[i].code=i;
    }
 qsort(unicodes,256,sizeof(stIntCodePairs),compare);
 /*for (i=0; i<256; i++)
     printf("U+%04X => %d (0x%02X)\n",unicodes[i].unicode,unicodes[i].code,unicodes[i].code);*/
}

/**[txh]********************************************************************

  Description:
  Used to get a TStringCollection listing all the available code pages.
Useful to make the user choose one. This is a read-only value, don't modify
it.
  
  Return: The internal collection casted to TStringCollection.
  
***************************************************************************/

TStringCollection *TVCodePage::GetList(void)
{
 return CodePages;
}

/**[txh]********************************************************************

  Description:
  Remaps the desired character using the provided map. The original character
should be in CP437 encoding. @x{::GetTranslate}.
  
  Return: The remapped character. It can be a direct translation or the
closest found.
  
***************************************************************************/

uchar TVCodePage::RemapChar(uchar c, ushort *map)
{
 if (c<map[256])
    return c;
 int i;
 ushort v=c;
 for (i=map[256]; i<256; i++)
     if (map[i]==v)
        return i;

 while (v>126 || v<32)
   {
    v=Similar[v];
    if (v>=32 && v<127)
       return v;
    for (i=map[256]; i<256; i++)
        if (map[i]==v)
           return i;
   }
 return v;
}

/**[txh]********************************************************************

  Description:
  Remaps the @<var>{o} string using the provided map and storing it in the
@<var>{n} string. The string should be terminated by a 0. @x{::RemapChar}.
  
***************************************************************************/

void TVCodePage::RemapString(uchar *n, uchar *o, ushort *map)
{
 int i;
 for (i=0; o[i]; i++)
     n[i]=RemapChar(o[i],map);
}

/**[txh]********************************************************************

  Description:
  Remaps the @var{o} string using the provided map and storing it in the
@var{n} string. The number of items to remap is indicated with @var{len}.
@x{::RemapChar}.
  
***************************************************************************/

void TVCodePage::RemapNString(uchar *n, uchar *o, ushort *map, int len)
{
 for (; len; --len)
     n[len-1]=RemapChar(o[len-1],map);
}

/**[txh]********************************************************************

  Description:
  Used to remap a buffer from any of the supported code pages into another.
The process doesn't have to be reversible.@p
  Tabs, carriage returns, new lines and the null code aren't remapped.
  
***************************************************************************/

void TVCodePage::RemapBufferGeneric(int sourID, int destID, uchar *buffer, unsigned len,
                                    unsigned ops)
{
 unsigned i;
 if (!CodePages) return; // Just in case
 // Table to convert a value into an internal code
 ushort *toCode=GetTranslate(sourID);
 ushort *aux;
 if (ops & rbgDontRemapLow32)
   {
    for (i=0; i<32; i++)
        toCode[i]=i;
   }
 else
   {
    // Avoid conversions that will break the text:
    toCode[0]=0;
    toCode['\n']='\n';
    toCode['\r']='\r';
    toCode['\t']='\t';
   }

 // Table to convert an internal value into a 0-255 value
 uchar *fromCode=new uchar[maxSymbolDefined];
 memset(fromCode,0,maxSymbolDefined*sizeof(uchar));

 CodePage *destCP=CodePageOfID(destID);

 aux=destCP->Font;
 for (i=0; i<128; i++)
     if (aux[i]<maxSymbolDefined) // extra check, just in case I forgot to update the constant
        fromCode[aux[i]]=i+128;

 i=0;
 if (!(ops & rbgDontRemapLow32) && destCP->LowRemapNum)
   {
    unsigned to=destCP->LowRemapNum;
    aux=destCP->LowRemap;
    for (; i<to; i++)
        if (aux[i]<maxSymbolDefined)
           fromCode[aux[i]]=i;
   }
 for (; i<128; i++)
     fromCode[i]=i;
 // Avoid conversions that will break the text:
 fromCode[0]=0;
 fromCode['\n']='\n';
 fromCode['\r']='\r';
 fromCode['\t']='\t';

 // Adjust values found in source but not in dest (look for similars)
 for (i=1; i<256; i++)
    {
     unsigned val=toCode[i];
     if (fromCode[val])
        continue;
     while (!fromCode[val])
       { // Find an equivalent for val
        if (val<256)
           val=Similar[val];
        else
           val=Similar2[val-256];
       }
     fromCode[toCode[i]]=fromCode[val];
    }

 // Ok, now do it!
 for (i=0; i<len; i++)
     buffer[i]=fromCode[toCode[buffer[i]]];

 delete[] fromCode;
}

/**[txh]********************************************************************

  Description:
  Used to create a table to translate from sourID to destID. The provided
buffer should have at least 256 characters.

***************************************************************************/

void TVCodePage::FillGenericRemap(int sourID, int destID, uchar *buffer,
                                  unsigned ops)
{
 if (!CodePages)
    CreateCodePagesCol();
 int i;
 for (i=0; i<256; i++)
     buffer[i]=i;
 RemapBufferGeneric(sourID,destID,buffer,256,ops);
}

/**[txh]********************************************************************

  Description:
  Protected member used internally to remap all the TV stuff that isn't
ASCII.

***************************************************************************/

void TVCodePage::RemapTVStrings(ushort *map)
{
 #define C(cla,name) RemapString((uchar *)cla::name,(uchar *)cla::o##name,map)
 C(TRadioButtons,button);
 C(TMenuBox,frameChars);
 C(TFrame,frameChars);
 C(TFrame,closeIcon);
 C(TFrame,zoomIcon);
 C(TFrame,unZoomIcon);
 C(TFrame,dragIcon);
 C(TFrame,animIcon);
 C(THistory,icon);
 C(TMonoSelector,button);
 C(TStatusLine,hintSeparator);
 C(TCheckBoxes,button);
 C(TButton,shadows);
 C(TButton,markers);
 C(TDirListBox,pathDir);
 C(TDirListBox,firstDir);
 C(TDirListBox,middleDir);
 C(TDirListBox,lastDir);
 C(TDirListBox,graphics);
 #undef C

 #define C(cla,name) cla::name=RemapChar(cla::o##name,map)
 C(TIndicator,dragFrame);
 C(TIndicator,normalFrame);
 C(TColorSelector,icon);
 C(TColorSelector,mark);
 C(TInputLine,rightArrow);
 C(TInputLine,leftArrow);
 C(TMenuBox,rightArrow);
 C(TRadioButtons,check);
 C(TIndicator,modifiedStar);
 C(TListViewer,columnSeparator);
 C(TDeskTop,defaultBkgrnd);
 C(TView,noMoireFill);
 #undef C

 #define C(cla,name,len) RemapNString((uchar *)cla::name,(uchar *)cla::o##name,map,len)
 C(TView,specialChars,6);
 C(TScrollBar,vChars,5);
 C(TScrollBar,hChars,5);
 #undef C

 if (UserHook) UserHook(map);

 message(TProgram::deskTop,evBroadcast,cmUpdateCodePage,map);
}

/**[txh]********************************************************************

  Description:
  Sets the function to be called after changing the code page. This should
remap the special strings used by the application.
  
  Return: The previous function. If it's != NULL you should call it.
  
***************************************************************************/

TVCodePageCallBack TVCodePage::SetCallBack(TVCodePageCallBack cb)
{
 TVCodePageCallBack oldCB=UserHook;
 UserHook=cb;
 return oldCB;
}

/**[txh]********************************************************************

  Description:
  Protected member used to find the code page structure for a provided id.
  
  Return: A CodePage structure pointer.
  
***************************************************************************/

CodePage *TVCodePage::CodePageOfID(int id)
{
 return (CodePage *)(CodePages->at(IDToIndex(id)));
}

stIntCodePairs TVCodePage::InternalMap[]=
{
 { 0x0020,   32 },
 { 0x0021,   33 },
 { 0x0022,   34 },
 { 0x0023,   35 },
 { 0x0024,   36 },
 { 0x0025,   37 },
 { 0x0026,   38 },
 { 0x0027,   39 },
 { 0x0028,   40 },
 { 0x0029,   41 },
 { 0x002a,   42 },
 { 0x002b,   43 },
 { 0x002c,   44 },
 { 0x002d,   45 },
 { 0x002e,   46 },
 { 0x002f,   47 },
 { 0x0030,   48 },
 { 0x0031,   49 },
 { 0x0032,   50 },
 { 0x0033,   51 },
 { 0x0034,   52 },
 { 0x0035,   53 },
 { 0x0036,   54 },
 { 0x0037,   55 },
 { 0x0038,   56 },
 { 0x0039,   57 },
 { 0x003a,   58 },
 { 0x003b,   59 },
 { 0x003c,   60 },
 { 0x003d,   61 },
 { 0x003e,   62 },
 { 0x003f,   63 },
 { 0x0040,   64 },
 { 0x0041,   65 },
 { 0x0042,   66 },
 { 0x0043,   67 },
 { 0x0044,   68 },
 { 0x0045,   69 },
 { 0x0046,   70 },
 { 0x0047,   71 },
 { 0x0048,   72 },
 { 0x0049,   73 },
 { 0x004a,   74 },
 { 0x004b,   75 },
 { 0x004c,   76 },
 { 0x004d,   77 },
 { 0x004e,   78 },
 { 0x004f,   79 },
 { 0x0050,   80 },
 { 0x0051,   81 },
 { 0x0052,   82 },
 { 0x0053,   83 },
 { 0x0054,   84 },
 { 0x0055,   85 },
 { 0x0056,   86 },
 { 0x0057,   87 },
 { 0x0058,   88 },
 { 0x0059,   89 },
 { 0x005a,   90 },
 { 0x005b,   91 },
 { 0x005c,   92 },
 { 0x005d,   93 },
 { 0x005e,   94 },
 { 0x005f,   95 },
 { 0x0060,   96 },
 { 0x0061,   97 },
 { 0x0062,   98 },
 { 0x0063,   99 },
 { 0x0064,  100 },
 { 0x0065,  101 },
 { 0x0066,  102 },
 { 0x0067,  103 },
 { 0x0068,  104 },
 { 0x0069,  105 },
 { 0x006a,  106 },
 { 0x006b,  107 },
 { 0x006c,  108 },
 { 0x006d,  109 },
 { 0x006e,  110 },
 { 0x006f,  111 },
 { 0x0070,  112 },
 { 0x0071,  113 },
 { 0x0072,  114 },
 { 0x0073,  115 },
 { 0x0074,  116 },
 { 0x0075,  117 },
 { 0x0076,  118 },
 { 0x0077,  119 },
 { 0x0078,  120 },
 { 0x0079,  121 },
 { 0x007a,  122 },
 { 0x007b,  123 },
 { 0x007c,  124 },
 { 0x007d,  125 },
 { 0x007e,  126 },
 { 0x00a0,  255 },
 { 0x00a1,  173 },
 { 0x00a2,  155 },
 { 0x00a3,  156 },
 { 0x00a4,  331 },
 { 0x00a5,  157 },
 { 0x00a6,  338 },
 { 0x00a7,   21 },
 { 0x00a8,  345 },
 { 0x00a9,  336 },
 { 0x00aa,  166 },
 { 0x00ab,  174 },
 { 0x00ac,  170 },
 { 0x00ad,  341 },
 { 0x00ae,  335 },
 { 0x00af,  339 },
 { 0x00b0,  248 },
 { 0x00b1,  241 },
 { 0x00b2,  253 },
 { 0x00b3,  347 },
 { 0x00b4,  340 },
 { 0x00b5,  230 },
 { 0x00b6,   20 },
 { 0x00b7,  250 },
 { 0x00b8,  344 },
 { 0x00b9,  346 },
 { 0x00ba,  167 },
 { 0x00bb,  175 },
 { 0x00bc,  172 },
 { 0x00bd,  171 },
 { 0x00be,  343 },
 { 0x00bf,  168 },
 { 0x00c0,  275 },
 { 0x00c1,  263 },
 { 0x00c2,  289 },
 { 0x00c3,  287 },
 { 0x00c4,  142 },
 { 0x00c5,  143 },
 { 0x00c6,  146 },
 { 0x00c7,  128 },
 { 0x00c8,  276 },
 { 0x00c9,  144 },
 { 0x00ca,  290 },
 { 0x00cb,  280 },
 { 0x00cc,  277 },
 { 0x00cd,  265 },
 { 0x00ce,  291 },
 { 0x00cf,  281 },
 { 0x00d0,  323 },
 { 0x00d1,  165 },
 { 0x00d2,  278 },
 { 0x00d3,  268 },
 { 0x00d4,  292 },
 { 0x00d5,  288 },
 { 0x00d6,  153 },
 { 0x00d7,  334 },
 { 0x00d8,  320 },
 { 0x00d9,  279 },
 { 0x00da,  271 },
 { 0x00db,  293 },
 { 0x00dc,  154 },
 { 0x00dd,  272 },
 { 0x00de,  333 },
 { 0x00df,  225 },
 { 0x00e0,  133 },
 { 0x00e1,  160 },
 { 0x00e2,  131 },
 { 0x00e3,  284 },
 { 0x00e4,  132 },
 { 0x00e5,  134 },
 { 0x00e6,  145 },
 { 0x00e7,  135 },
 { 0x00e8,  138 },
 { 0x00e9,  130 },
 { 0x00ea,  136 },
 { 0x00eb,  137 },
 { 0x00ec,  141 },
 { 0x00ed,  161 },
 { 0x00ee,  140 },
 { 0x00ef,  139 },
 { 0x00f0,  322 },
 { 0x00f1,  164 },
 { 0x00f2,  149 },
 { 0x00f3,  162 },
 { 0x00f4,  147 },
 { 0x00f5,  285 },
 { 0x00f6,  148 },
 { 0x00f7,  246 },
 { 0x00f8,  318 },
 { 0x00f9,  151 },
 { 0x00fa,  163 },
 { 0x00fb,  150 },
 { 0x00fc,  129 },
 { 0x00fd,  261 },
 { 0x00fe,  332 },
 { 0x00ff,  152 },
 { 0x0100,  585 },
 { 0x0101,  584 },
 { 0x0102,  330 },
 { 0x0103,  328 },
 { 0x0104,  313 },
 { 0x0105,  311 },
 { 0x0106,  264 },
 { 0x0107,  256 },
 { 0x0108,  464 },
 { 0x0109,  465 },
 { 0x010a,  480 },
 { 0x010b,  481 },
 { 0x010c,  299 },
 { 0x010d,  294 },
 { 0x010e,  300 },
 { 0x010f,  324 },
 { 0x010f,  359 },
 { 0x0110,  323 },
 { 0x0111,  321 },
 { 0x0112,  496 },
 { 0x0113,  497 },
 { 0x0115,  329 },
 { 0x0116,  542 },
 { 0x0117,  543 },
 { 0x0118,  314 },
 { 0x0119,  312 },
 { 0x011a,  301 },
 { 0x011b,  358 },
 { 0x011c,  466 },
 { 0x011d,  467 },
 { 0x011e,  504 },
 { 0x011f,  505 },
 { 0x0120,  586 },
 { 0x0121,  486 },
 { 0x0122,  510 },
 { 0x0123,  511 },
 { 0x0124,  468 },
 { 0x0125,  469 },
 { 0x0126,  534 },
 { 0x0127,  535 },
 { 0x0128,  518 },
 { 0x0129,  519 },
 { 0x012a,  498 },
 { 0x012b,  499 },
 { 0x012c,  506 },
 { 0x012d,  507 },
 { 0x012e,  522 },
 { 0x012f,  523 },
 { 0x0130,  487 },
 { 0x0131,  337 },
 { 0x0134,  470 },
 { 0x0135,  471 },
 { 0x0136,  512 },
 { 0x0137,  513 },
 { 0x0138,  461 },
 { 0x0139,  266 },
 { 0x013a,  257 },
 { 0x013b,  544 },
 { 0x013c,  545 },
 { 0x013d,  327 },
 { 0x013d,  356 },
 { 0x013e,  325 },
 { 0x013e,  355 },
 { 0x0141,  319 },
 { 0x0142,  317 },
 { 0x0143,  267 },
 { 0x0144,  258 },
 { 0x0145,  514 },
 { 0x0146,  515 },
 { 0x0147,  302 },
 { 0x0148,  295 },
 { 0x014a,  462 },
 { 0x014b,  463 },
 { 0x014c,  500 },
 { 0x014d,  501 },
 { 0x0150,  354 },
 { 0x0151,  353 },
 { 0x0152,  440 },
 { 0x0153,  441 },
 { 0x0154,  269 },
 { 0x0155,  259 },
 { 0x0156,  516 },
 { 0x0157,  517 },
 { 0x0158,  303 },
 { 0x0159,  296 },
 { 0x015a,  270 },
 { 0x015b,  260 },
 { 0x015c,  472 },
 { 0x015d,  473 },
 { 0x015e,  309 },
 { 0x015f,  307 },
 { 0x0160,  304 },
 { 0x0161,  297 },
 { 0x0162,  310 },
 { 0x0163,  308 },
 { 0x0164,  305 },
 { 0x0165,  326 },
 { 0x0165,  357 },
 { 0x0166,  536 },
 { 0x0167,  537 },
 { 0x0168,  520 },
 { 0x0169,  521 },
 { 0x016a,  502 },
 { 0x016b,  503 },
 { 0x016c,  508 },
 { 0x016d,  509 },
 { 0x016e,  282 },
 { 0x016f,  283 },
 { 0x0170,  274 },
 { 0x0171,  286 },
 { 0x0172,  524 },
 { 0x0173,  525 },
 { 0x0174,  474 },
 { 0x0175,  475 },
 { 0x0176,  476 },
 { 0x0177,  477 },
 { 0x0178,  450 },
 { 0x0179,  273 },
 { 0x017a,  262 },
 { 0x017b,  316 },
 { 0x017c,  315 },
 { 0x017d,  306 },
 { 0x017e,  298 },
 { 0x0192,  159 },
 { 0x01a0,  538 },
 { 0x01a1,  539 },
 { 0x01af,  540 },
 { 0x01b0,  541 },
 { 0x02c7,  350 },
 { 0x02d8,  351 },
 { 0x02d9,  352 },
 { 0x02db,  349 },
 { 0x02dd,  348 },
 { 0x0385,  564 },
 { 0x0386,  572 },
 { 0x0388,  573 },
 { 0x0389,  574 },
 { 0x038a,  575 },
 { 0x038c,  576 },
 { 0x038e,  577 },
 { 0x038f,  578 },
 { 0x0390,  581 },
 { 0x0391,  617 },
 { 0x0392,  618 },
 { 0x0393,  226 },
 { 0x0394,  560 },
 { 0x0395,  619 },
 { 0x0396,  620 },
 { 0x0397,  621 },
 { 0x0398,  233 },
 { 0x0399,  622 },
 { 0x039a,  623 },
 { 0x039b,  583 },
 { 0x039c,  624 },
 { 0x039d,  625 },
 { 0x039e,  561 },
 { 0x039f,  626 },
 { 0x03a0,  562 },
 { 0x03a1,  627 },
 { 0x03a3,  228 },
 { 0x03a4,  628 },
 { 0x03a5,  629 },
 { 0x03a6,  232 },
 { 0x03a7,  630 },
 { 0x03a8,  563 },
 { 0x03a9,  234 },
 { 0x03aa,  631 },
 { 0x03ab,  632 },
 { 0x03ac,  565 },
 { 0x03ad,  566 },
 { 0x03ae,  567 },
 { 0x03af,  568 },
 { 0x03b0,  582 },
 { 0x03b1,  224 },
 { 0x03b2,  613 },
 { 0x03b3,  546 },
 { 0x03b4,  235 },
 { 0x03b5,  238 },
 { 0x03b6,  547 },
 { 0x03b7,  548 },
 { 0x03b8,  549 },
 { 0x03b9,  550 },
 { 0x03ba,  614 },
 { 0x03bb,  551 },
 { 0x03bc,  615 },
 { 0x03bd,  552 },
 { 0x03be,  553 },
 { 0x03bf,  616 },
 { 0x03c0,  227 },
 { 0x03c1,  554 },
 { 0x03c2,  555 },
 { 0x03c3,  229 },
 { 0x03c4,  231 },
 { 0x03c5,  556 },
 { 0x03c6,  237 },
 { 0x03c7,  557 },
 { 0x03c8,  558 },
 { 0x03c9,  559 },
 { 0x03ca,  579 },
 { 0x03cb,  580 },
 { 0x03cc,  569 },
 { 0x03cd,  570 },
 { 0x03ce,  571 },
 { 0x0401,  605 },
 { 0x0402,  412 },
 { 0x0403,  414 },
 { 0x0404,  407 },
 { 0x0405,  612 },
 { 0x0406,  610 },
 { 0x0407,  416 },
 { 0x0408,  611 },
 { 0x0409,  417 },
 { 0x040a,  419 },
 { 0x040b,  421 },
 { 0x040c,  423 },
 { 0x040e,  425 },
 { 0x040f,  427 },
 { 0x0410,  360 },
 { 0x0411,  361 },
 { 0x0412,  588 },
 { 0x0413,  362 },
 { 0x0414,  363 },
 { 0x0415,  589 },
 { 0x0416,  364 },
 { 0x0417,  409 },
 { 0x0418,  365 },
 { 0x0419,  366 },
 { 0x041a,  590 },
 { 0x041b,  367 },
 { 0x041c,  591 },
 { 0x041d,  592 },
 { 0x041e,  593 },
 { 0x041f,  368 },
 { 0x0420,  594 },
 { 0x0421,  595 },
 { 0x0422,  596 },
 { 0x0423,  369 },
 { 0x0424,  370 },
 { 0x0425,  597 },
 { 0x0426,  371 },
 { 0x0427,  372 },
 { 0x0428,  373 },
 { 0x0429,  374 },
 { 0x042a,  375 },
 { 0x042b,  376 },
 { 0x042c,  377 },
 { 0x042d,  378 },
 { 0x042e,  379 },
 { 0x042f,  380 },
 { 0x0430,  598 },
 { 0x0431,  381 },
 { 0x0432,  382 },
 { 0x0433,  383 },
 { 0x0434,  384 },
 { 0x0435,  599 },
 { 0x0436,  385 },
 { 0x0437,  386 },
 { 0x0438,  387 },
 { 0x0439,  388 },
 { 0x043a,  389 },
 { 0x043b,  390 },
 { 0x043c,  391 },
 { 0x043d,  392 },
 { 0x043e,  600 },
 { 0x043f,  393 },
 { 0x0440,  601 },
 { 0x0441,  602 },
 { 0x0442,  394 },
 { 0x0443,  395 },
 { 0x0444,  396 },
 { 0x0445,  603 },
 { 0x0446,  397 },
 { 0x0447,  398 },
 { 0x0448,  399 },
 { 0x0449,  400 },
 { 0x044a,  401 },
 { 0x044b,  402 },
 { 0x044c,  403 },
 { 0x044d,  404 },
 { 0x044e,  405 },
 { 0x044f,  406 },
 { 0x0451,  604 },
 { 0x0452,  413 },
 { 0x0453,  415 },
 { 0x0454,  408 },
 { 0x0455,  606 },
 { 0x0456,  607 },
 { 0x0457,  608 },
 { 0x0458,  609 },
 { 0x0459,  418 },
 { 0x045a,  420 },
 { 0x045b,  422 },
 { 0x045c,  424 },
 { 0x045e,  426 },
 { 0x045f,  428 },
 { 0x0490,  437 },
 { 0x0491,  438 },
 { 0x1e02,  478 },
 { 0x1e03,  479 },
 { 0x1e0a,  482 },
 { 0x1e0b,  483 },
 { 0x1e1e,  484 },
 { 0x1e1f,  485 },
 { 0x1e40,  488 },
 { 0x1e41,  489 },
 { 0x1e56,  490 },
 { 0x1e57,  491 },
 { 0x1e60,  492 },
 { 0x1e61,  493 },
 { 0x1e6a,  494 },
 { 0x1e6b,  495 },
 { 0x1e80,  526 },
 { 0x1e81,  527 },
 { 0x1e82,  530 },
 { 0x1e83,  531 },
 { 0x1e84,  532 },
 { 0x1e85,  533 },
 { 0x1ef2,  528 },
 { 0x1ef3,  529 },
 { 0x2017,  342 },
 { 0x201c,  434 },
 { 0x201d,  435 },
 { 0x201e,  429 },
 { 0x2020,  431 },
 { 0x2021,  432 },
 { 0x2022,    7 },
 { 0x2024,    7 },
 { 0x2026,  430 },
 { 0x2027,  249 },
 { 0x2030,  433 },
 { 0x203c,   19 },
 { 0x207f,  252 },
 { 0x20a7,  158 },
 { 0x20ac,  587 },
 { 0x2116,  411 },
 { 0x2122,  436 },
 { 0x2190,   27 },
 { 0x2191,   24 },
 { 0x2192,   26 },
 { 0x2193,   25 },
 { 0x2194,   29 },
 { 0x2195,   18 },
 { 0x21a8,   23 },
 { 0x21b5,  449 },
 { 0x2219,  249 },
 { 0x221a,  251 },
 { 0x221e,  236 },
 { 0x221f,   28 },
 { 0x2229,  239 },
 { 0x2248,  247 },
 { 0x2260,  448 },
 { 0x2261,  240 },
 { 0x2264,  243 },
 { 0x2265,  242 },
 { 0x22c5,    7 },
 { 0x2302,  127 },
 { 0x2310,  169 },
 { 0x2320,  244 },
 { 0x2321,  245 },
 { 0x2409,  442 },
 { 0x240a,  445 },
 { 0x240b,  447 },
 { 0x240c,  443 },
 { 0x240d,  444 },
 { 0x2423,   32 }, // Should be fixed
 { 0x2424,  446 },
 { 0x2500,  196 },
 { 0x2502,  179 },
 { 0x250c,  218 },
 { 0x2510,  191 },
 { 0x2514,  192 },
 { 0x2518,  217 },
 { 0x251c,  195 },
 { 0x2524,  180 },
 { 0x252c,  194 },
 { 0x2534,  193 },
 { 0x253c,  197 },
 { 0x2550,  205 },
 { 0x2551,  186 },
 { 0x2552,  213 },
 { 0x2553,  214 },
 { 0x2554,  201 },
 { 0x2555,  184 },
 { 0x2556,  183 },
 { 0x2557,  187 },
 { 0x2558,  212 },
 { 0x2559,  211 },
 { 0x255a,  200 },
 { 0x255b,  190 },
 { 0x255c,  189 },
 { 0x255d,  188 },
 { 0x255e,  198 },
 { 0x255f,  199 },
 { 0x2560,  204 },
 { 0x2561,  181 },
 { 0x2562,  182 },
 { 0x2563,  185 },
 { 0x2564,  209 },
 { 0x2565,  210 },
 { 0x2566,  203 },
 { 0x2567,  207 },
 { 0x2568,  208 },
 { 0x2569,  202 },
 { 0x256a,  216 },
 { 0x256b,  215 },
 { 0x256c,  206 },
 { 0x2574,  455 },
 { 0x2575,  452 },
 { 0x2576,  453 },
 { 0x2577,  454 },
 { 0x2578,  460 },
 { 0x2579,  457 },
 { 0x257a,  458 },
 { 0x257b,  459 },
 { 0x2580,  223 },
 { 0x2584,  220 },
 { 0x2588,  219 },
 { 0x258c,  221 },
 { 0x2590,  222 },
 { 0x2591,  176 },
 { 0x2592,  177 },
 { 0x2593,  178 },
 { 0x25a0,  254 },
 { 0x25ac,   22 },
 { 0x25b2,   30 },
 { 0x25b6,   16 },
 { 0x25bc,   31 },
 { 0x25c0,   17 },
 { 0x25c6,    4 },
 { 0x25cb,    9 },
 { 0x25d8,    8 },
 { 0x25d9,   10 },
 { 0x25e6,    8 },
 { 0x2609,   15 },
 { 0x263a,    1 },
 { 0x263b,    2 },
 { 0x263c,   15 },
 { 0x2640,   12 },
 { 0x2642,   11 },
 { 0x2660,    6 },
 { 0x2663,    5 },
 { 0x2665,    3 },
 { 0x2666,    4 },
 { 0x266a,   13 },
 { 0x266b,   14 },
 { 0x2764,    3 },
 { 0xf800,  451 }, // Approximation
 { 0xf801,  451 },
 { 0xf803,  456 },
 { 0xf804,  456 }, // Approximation
 { 0xfffd,  439 }
};

stIntCodePairs TVCodePage::InternalMapBrokenLinux[]=
{
 { 0x2501,  205 }, // (*1)
 { 0x2503,  186 }, // (*1)
 { 0x250f,  201 }, // (*1)
 { 0x2513,  187 }, // (*1)
 { 0x2517,  200 }, // (*1)
 { 0x251b,  188 }, // (*1)
 { 0x2523,  204 }, // (*1)
 { 0x252b,  185 }, // (*1)
 { 0x2533,  203 }, // (*1)
 { 0x253b,  202 }, // (*1)
 { 0x254b,  206 }, // (*1)
};

// Notes:
// (*1) Linux lat1 says this. I think this is wrong and in fact XFree86 10x20
//      font author interprets Unicode tables as me.
//      The 205 symbol is Unicode 0x2550, of course it can be used as a default
//      for 0x2501. 0x2550 is double line and 0x2501 is one line that is the
//      double in width. The same is for all the other cases.


const int TVCodePage::providedUnicodes=sizeof(TVCodePage::InternalMap)/sizeof(stIntCodePairs);
const int TVCodePage::providedUnicodesBL=sizeof(TVCodePage::InternalMapBrokenLinux)/sizeof(stIntCodePairs);

/**[txh]********************************************************************

  Description:
  Finds which unicode is represented by the specified internal code.
Currently that's an slow search because isn't used very much.
  
  Return: The first unicode found.
  
***************************************************************************/

uint16 TVCodePage::UnicodeForInternalCode(uint16 value)
{
 int i;
 for (i=0; i<providedUnicodes; i++)
     if (InternalMap[i].code==value)
        return InternalMap[i].unicode;
 return 0;
}

/**[txh]********************************************************************

  Description:
  Finds which internal code can render an Unicode value.
  
  Return: The internal or -1 if none can do it.
  
***************************************************************************/

int TVCodePage::InternalCodeForUnicode(ushort unicode)
{
 if (!unicode) return 0;
 stIntCodePairs s={unicode,0};
 void *res=bsearch(&s,InternalMap,providedUnicodes,sizeof(stIntCodePairs),compare);
 if (!res)
    res=bsearch(&s,InternalMapBrokenLinux,providedUnicodesBL,sizeof(stIntCodePairs),compare);
 return res ? ((stIntCodePairs *)res)->code : -1;
}

/**[txh]********************************************************************

  Description:
  Creates a new code page from an arry containing the unicodes for each
symbol. Use an id over than 0x7FFF8000 to avoid collisions. This is used by
the Linux driver when the unicodes maps doesn't match with known cp.
  
***************************************************************************/

void TVCodePage::CreateCPFromUnicode(CodePage *cp, int id, const char *name,
                                     ushort *unicodes)
{
 // Create a code page
 strcpy(cp->Name,name);
 cp->id=id;
 int i;
 for (i=128; i<256; i++)
    {
     int v=unicodes[i];
     if (v==0xFFFF)
        cp->Font[i-128]=0;
     else
       {
        v=InternalCodeForUnicode(v);
        cp->Font[i-128]=v==-1 ? 0 : v;
       }
    }
 // Currently we lack an Unicode toupper/lower and isalpha mechanism
 cp->UpLow=cp->MoreLetters=NULL;
 cp->LowRemapNum=128;
 cp->LowRemap=new ushort[128];
 for (i=0; i<128; i++)
    {
     int v=unicodes[i];
     if (v==0xFFFF)
        cp->LowRemap[i]=0;
     else
       {
        v=InternalCodeForUnicode(v);
        cp->LowRemap[i]=v==-1 ? 0 : v;
       }
    }
}

/**[txh]********************************************************************

  Description:
  Adds a custom code page to the list. You can create a new one from an
array containing the Unicodes for each symbol. @x{::CreateCPFromUnicode}.

  Return: The index of the new code page in the collection.
  
***************************************************************************/

ccIndex TVCodePage::AddCodePage(CodePage *cp)
{
 if (!CodePages) // Drivers could need to add custom cps before init.
    CreateCodePagesCol();
 return CodePages->insert(cp);
}

/**[txh]********************************************************************

  Description:
  This is a helper for the TVFontCollection class the parameters of this
function could change in the future.@*
  Basically it finds a replacement for a missing symbol in a font. It allows
using incomplete font.
  
  Return: The replacement or -1 if none known.
  
***************************************************************************/

int TVCodePage::LookSimilarInRange(int code, int last)
{
 while (code>last && !(code>=' ' && code<='~'))
   {
    if (code<256)
       code=Similar[code];
    else
       code=Similar2[code-256];
   }
 return code>last ? -1 : code;
}

/*****************************************************************************
 Helper functions used by TView. They convert the application code page values
to Unicode and viceversa.
*****************************************************************************/

/**[txh]********************************************************************

  Description:
  Converts an Unicode value to application code page.
  
  Return: application code page value, 0 is the fallback.
  
***************************************************************************/

char TVCodePage::convertU16_2_CP(uint16 val)
{
 uint16 ret=unicodeToApp->search(val);
 return ret==0xFFFF ? 0 : (uchar)ret;
}

/**[txh]********************************************************************

  Description:
  Converts an application code page value to Unicode.
  
  Return: The Unicode that represents this value.
  
***************************************************************************/

uint16 TVCodePage::convertCP_2_U16(char val)
{
 return appToUnicode[(uchar)val];
}

/**[txh]********************************************************************

  Description:
  Converts an Unicode value to input code page.
  
  Return: application code page value, 0 is the fallback.
  
***************************************************************************/

char TVCodePage::convertU16_2_InpCP(uint16 val)
{
 if (!unicodeToInp)
    return convertU16_2_CP(val);
 uint16 ret=unicodeToInp->search(val);
 return ret==0xFFFF ? 0 : (uchar)ret;
}

/**[txh]********************************************************************

  Description:
  Converts an input code page value to Unicode.
  
  Return: The Unicode that represents this value.
  
***************************************************************************/

uint16 TVCodePage::convertInpCP_2_U16(char val)
{
 return inpToUnicode[(uchar)val];
}

/**[txh]********************************************************************

  Description:
  Converts a buffer containing Unicode/Attribute to Application Code Page/
Attribute.
  
  Return: The destination buffer.
  
***************************************************************************/

void *TVCodePage::convertBufferU16_2_CP(void *dest, const void *orig,
                                        unsigned count)
{
 uint16 *o=(uint16 *)orig;
 uchar *d=(uchar *)dest;
 while (count--)
   {
    uint16 uni=unicodeToApp->search(*(o++));
    *(d++)=uni==0xFFFF ? 0 : (uchar)uni;
    *(d++)=(uchar)*(o++);
   }
 return dest;
}

/**[txh]********************************************************************

  Description:
  Converts a buffer containing Application Code Page/Attribute to
Unicode/Attribute.
  
  Return: The destination buffer.
  
***************************************************************************/

void *TVCodePage::convertBufferCP_2_U16(void *dest, const void *orig,
                                        unsigned count)
{
 uint16 *d=(uint16 *)dest;
 uchar *o=(uchar *)orig;
 while (count--)
   {
    *(d++)=appToUnicode[*(o++)];
    *(d++)=(uint16)*(o++);
   }
 return dest;
}

/**[txh]********************************************************************

  Description:
  Converts a string containing Unicode to Application Code Page. The len is
as returned by strlen and the destination must be len+1 bytes for the EOS.
  
  Return: The destination string.
  
***************************************************************************/

void *TVCodePage::convertStrU16_2_CP(void *dest, const void *orig,
                                     unsigned len)
{
 uint16 *o=(uint16 *)orig;
 uchar *d=(uchar *)dest;
 while (len--)
   {
    uint16 uni=unicodeToApp->search(*(o++));
    *(d++)=uni==0xFFFF ? 0 : (uchar)uni;
   }
 *d=0;
 return dest;
}

/**[txh]********************************************************************

  Description:
  Converts a string containing Application Code Page to Unicode. The
destination must be (len+1)*2 bytes for the EOS.
  
  Return: The destination string.
  
***************************************************************************/

void *TVCodePage::convertStrCP_2_U16(void *dest, const void *orig,
                                     unsigned len)
{
 uint16 *d=(uint16 *)dest;
 uchar *o=(uchar *)orig;
 while (len--)
    *(d++)=appToUnicode[*(o++)];
 *d=0;
 return dest;
}



