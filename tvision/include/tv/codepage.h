/**[txh]********************************************************************

  Copyright 1996-2002 by Salvador Eduardo Tropea (SET)
  This file is covered by the GPL license.

  Module: TVCodePage

***************************************************************************/

#if defined(Uses_TVCodePage) && !defined(TVCodePage_Included)
#define TVCodePage_Included 1

class TVCodePageCol;
class TStringCollection;

// This is the internal structure used to describe a code page
typedef struct
{
 char Name[28];       // Descriptive name
 int id;              // Unique ID to identify it
 ushort Font[128];    // High 128 symbols
 char *UpLow;         // Lowecase/Uppercase pairs
 char *MoreLetters;   // Other symbols that should be treat as letters but doesn't
                      // have lowecase/uppercase pair.
 int LowRemapNum;     // Most code pages are plain ASCII in the first 128 symbols
 ushort *LowRemap;    // and we don't define them. This information is used when
                      // symbols under 128 needs special treatment.
} CodePage;

typedef void (*TVCodePageCallBack)(ushort *map);

const unsigned rbgDontRemapLow32=1, rbgOnlySelected=2;

class TVCodePage
{
public:
 TVCodePage(int id);
 ~TVCodePage();
 static ccIndex IDToIndex(int id);
 static int     IndexToID(ccIndex index);
 static ushort *GetTranslate(int id);
 static void    SetCodePage(int id);
 static TStringCollection
               *GetList(void);
 static uchar   RemapChar(uchar c, ushort *map);
 static void    RemapString(uchar *n, uchar *o, ushort *map);
 static void    RemapNString(uchar *n, uchar *o, ushort *map, int len);
 static void    RemapBufferGeneric(int sourID, int destID, uchar *buffer, unsigned len,
                                   unsigned ops);
 static char    toUpper(char val)
  { return (char)toUpperTable[(uchar)val]; }
 static char    toLower(char val)
  { return (char)toLowerTable[(uchar)val]; }
 static int     isAlpha(char val)
  { return AlphaTable[(uchar)val]; }
 static TVCodePageCallBack SetCallBack(TVCodePageCallBack map);

 enum { ISOLatin1Linux=885901 };

protected:
 static CodePage *CodePageOfID(int id);
 static void      RemapTVStrings(ushort *map);
 static void      FillTables(int id);

 static TVCodePageCol *CodePages;
 static ushort CPTable[257];
 static int    CurrentCP;
 static uchar  toUpperTable[256];
 static uchar  toLowerTable[256];
 static uchar  AlphaTable[256];
 static uchar  Similar[];
 static ushort Similar2[];
 static TVCodePageCallBack UserHook;
};

#endif

