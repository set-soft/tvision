/**[txh]********************************************************************

  Copyright 1996-2002 by Salvador Eduardo Tropea (SET)
  This file is covered by the GPL license.

  Module: TVCodePage

***************************************************************************/

#if defined(Uses_TVCodePage) && !defined(TVCodePage_Included)
#define TVCodePage_Included 1

class TVCodePageCol;
class TStringCollection;
struct stIntCodePairs;

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
 static int     InternalCodeForUnicode(ushort unicode);
 static TVCodePageCallBack SetCallBack(TVCodePageCallBack map);

 // Arbitrary names for the supported code pages
 // Just to avoid using the magics, look in codepage.cc for more information
 enum
 {
  PC437=437, PC737=737, PC775=775, PC850=850, PC852=852, PC855=855, PC857=857,
  PC860=860, PC861=861, PC863=863, PC865=865, PC866=866, PC869=869, CP1250=1250,
  CP1251=1251, CP1252=1252, CP1253=1253, CP1254=1254, CP1257=1257, MacCyr=10007,
  ISOLatin1=88791, ISOLatin2=88792, ISOLatin3=88593, ISOLatin4=88594,
  ISORussian=88595, ISOGreek=88597, ISO9=88599, ISOLatin1Linux=885901,
  ISOLatin1uLinux=885911, ISO14=885914, ISOIceland=885915, KOI8r=100000,
  KOI8_CRL_NMSU=100001, MacOSUkrainian=100072, OsnovnojVariantRussian=885951,
  AlternativnyjVariantRU=885952, UcodeRussian=885953, Mazovia=1000000,
  ISO5427=3604494, ECMACyr=17891342, ISOIR146=21364750, ISOIR147=21430286,
  ISOIR153=22216718
 };

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
 static TVCodePageCallBack  UserHook;
 static stIntCodePairs      InternalMap[];
 static const int           providedUnicodes;
 // CodePage structures
 static CodePage stPC437;
 static CodePage stPC775;
 static CodePage stPC850;
 static CodePage stPC852;
 static CodePage stPC857;
 static CodePage stPC860;
 static CodePage stPC861;
 static CodePage stPC863;
 static CodePage stPC865;
 static CodePage ISO8879_1;
 static CodePage ISO8859_1_Lat1;
 static CodePage ISO8859_1u_Lat1;
 static CodePage ISO8879_2;
 static CodePage ISO8859_3;
 static CodePage ISO8859_4;
 static CodePage ISO8859_9;
 static CodePage ISO8859_14;
 static CodePage ISO8859_15;
 static CodePage stPC1250;
 static CodePage stPC1252;
 static CodePage stPC1254;
 static CodePage stPC1257;
 static CodePage stMazovia;
 static CodePage stPC855;
 static CodePage stPC866;
 static CodePage ISO8859_5;
 static CodePage KOI_8r;
 static CodePage KOI_8crl;
 static CodePage PC1251;
 static CodePage ISO_IR_111;
 static CodePage ISO_IR_153;
 static CodePage CP10007;
 static CodePage CP100072;
 static CodePage OVR;
 static CodePage AVR;
 static CodePage U_CodeR;
 static CodePage KOI_7;
 static CodePage ISO_IR_147;
 static CodePage ISO_IR_146;
 static CodePage stPC737;
 static CodePage stPC869;
 static CodePage stPC1253;
 static CodePage ISO8859_7;
 static ushort LowCrazyCharsRemaped[];
 static ushort Low32CharsRemaped[];
 static ushort tbKOI7[];
 static ushort tbISOIR147[];
 static ushort tbISOIR146[];
};

#endif

