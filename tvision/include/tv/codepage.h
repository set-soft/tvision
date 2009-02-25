/**[txh]********************************************************************

  Copyright 1996-2003 by Salvador Eduardo Tropea (SET)
  This file is covered by the GPL license.

  Module: TVCodePage

***************************************************************************/

#if defined(Uses_TVCodePage) && !defined(TVCodePage_Included)
#define TVCodePage_Included 1

class TVCodePageCol;
class TStringCollection;
struct stIntCodePairs
{
 uint16 unicode,code;
};

// This is the internal structure used to describe a code page
struct CodePage
{
 char Name[28];       // Descriptive name
 int id;              // Unique ID to identify it
 ushort Font[128];    // High 128 symbols
 const char *UpLow;       // Lowecase/Uppercase pairs
 const char *MoreLetters; // Other symbols that should be treat as letters but doesn't
                      // have lowecase/uppercase pair.
 int LowRemapNum;     // Most code pages are plain ASCII in the first 128 symbols
 ushort *LowRemap;    // and we don't define them. This information is used when
                      // symbols under 128 needs special treatment.
};

typedef void (*TVCodePageCallBack)(ushort *map);

const unsigned rbgDontRemapLow32=1, rbgOnlySelected=2;

class CLY_EXPORT TVCodePage
{
public:
 TVCodePage(int idApp, int idScr, int idInp);
 ~TVCodePage();
 static ccIndex IDToIndex(int id);
 static int     IndexToID(ccIndex index);
 static ushort *GetTranslate(int id);
 static void    SetCodePage(int idApp, int idScr, int idInp);
 // Helpers to call SetCodePage
 static void    SetCodePage(int idCP) { SetCodePage(idCP,idCP,idCP); };
 static void    SetScreenCodePage(int idCP) { SetCodePage(curAppCP,idCP,curInpCP); };
 static void    SetInputCodePage(int idCP) { SetCodePage(curAppCP,curScrCP,idCP); };
 static TStringCollection
               *GetList(void);
 static uchar   RemapChar(uchar c, ushort *map);
 static void    RemapString(uchar *n, uchar *o, ushort *map);
 static void    RemapNString(uchar *n, uchar *o, ushort *map, int len);
 static void    RemapBufferGeneric(int sourID, int destID, uchar *buffer, unsigned len,
                                   unsigned ops);
 static void    FillGenericRemap(int sourID, int destID, uchar *buffer, unsigned ops=0);
 static char    toUpper(char val)
  { return (char)toUpperTable[(uchar)val]; }
 static char    toLower(char val)
  { return (char)toLowerTable[(uchar)val]; }
 static int     isAlpha(char val)
  { return AlphaTable[(uchar)val] & alphaChar; }
 static int     isNumber(char val)
  { return AlphaTable[(uchar)val] & digitChar; }
 static int     isAlNum(char val)
  { return AlphaTable[(uchar)val] & (alphaChar | digitChar); }
 static int     isLower(char val)
  { return AlphaTable[(uchar)val] & lowerChar; }
 static int     isUpper(char val)
  { return AlphaTable[(uchar)val] & upperChar; }
 static int     InternalCodeForUnicode(uint16 unicode);
 static uint16  UnicodeForInternalCode(uint16 code);
 static void    GetUnicodesForCP(int id, stIntCodePairs *unicodes);
 static TVCodePageCallBack SetCallBack(TVCodePageCallBack map);

 static Boolean OnTheFlyRemapNeeded() { return NeedsOnTheFlyRemap ? True : False; }
 static uchar   OnTheFlyRemap(uchar val) { return OnTheFlyMap[val]; }
 static Boolean OnTheFlyRemapInpNeeded() { return NeedsOnTheFlyInpRemap ? True : False; }
 static uchar   OnTheFlyInpRemap(uchar val) { return OnTheFlyInpMap[val]; }
 static void    CreateCPFromUnicode(CodePage *cp, int id, const char *name,
                                    ushort *unicodes);
 static ccIndex AddCodePage(CodePage *cp);
 static void    GetCodePages(int &idScr, int &idApp, int &idInp)
                { idApp=curAppCP; idScr=curScrCP; idInp=curInpCP; }
 static int     GetAppCodePage() { return curAppCP; }
 static int     GetScrCodePage() { return curScrCP; }
 static int     GetInpCodePage() { return curInpCP; }
 static void    GetDefaultCodePages(int &idScr, int &idApp, int &idInp)
                { idApp=defAppCP; idScr=defScrCP; idInp=defInpCP; }
 static int     LookSimilarInRange(int code, int last);
 // TView helpers
 static void   *convertBufferU16_2_CP(void *dest, const void *orig, unsigned count);
 static void   *convertBufferCP_2_U16(void *dest, const void *orig, unsigned count);
 static void   *convertStrU16_2_CP(void *dest, const void *orig, unsigned len);
 static void   *convertStrCP_2_U16(void *dest, const void *orig, unsigned len);
 static char    convertU16_2_CP(uint16 val);
 static uint16  convertCP_2_U16(char val);
 static char    convertU16_2_InpCP(uint16 val);
 static uint16  convertInpCP_2_U16(char val);

 // Arbitrary names for the supported code pages
 // Just to avoid using the magics, look in codepage.cc for more information
 enum
 {
  PC437=437, PC737=737, PC775=775, PC850=850, PC852=852, PC855=855, PC857=857,
  PC860=860, PC861=861, PC863=863, PC865=865, PC866=866, PC869=869, CP1250=1250,
  CP1251=1251, CP1252=1252, CP1253=1253, CP1254=1254, CP1257=1257, MacCyr=10007,
  ISOLatin1=88791, ISOLatin2=88792, ISOLatin3=88593, ISOLatin4=88594,
  ISORussian=88595, ISOGreek=88597, ISO9=88599, ISOLatin1Linux=885901,
  ISOLatin1uLinux=885911, ISO14=885914, ISOIceland=885915,
  ISOLatin2uLinux=885921, ISOLatin2Linux=885920, ISOLatin2Sun=885922,
  ISOLatin2eLinux=885923, KOI8r=100000, KOI8_CRL_NMSU=100001,
  MacOSUkrainian=100072, OsnovnojVariantRussian=885951,
  AlternativnyjVariantRU=885952, UcodeRussian=885953, Mazovia=1000000,
  ISO5427=3604494, ECMACyr=17891342, ISOIR146=21364750, ISOIR147=21430286,
  ISOIR153=22216718,
  LinuxACM=0x7FFF0000, LinuxSFM=0x7FFF0001
 };
 // Be careful with this table is public just to simplify the code.
 static stIntCodePairs      InternalMap[];
 static stIntCodePairs      InternalMapBrokenLinux[];
 static const int           providedUnicodes;
 static const int           providedUnicodesBL;

protected:
 static CodePage *CodePageOfID(int id);
 static void      RemapTVStrings(ushort *map);
 static void      FillTables(int id);
 static void      CreateOnTheFlyRemap(int idApp, int idScr);
 static void      CreateOnTheFlyInpRemap(int idInp, int idApp);
 static void      CreateRemap(int idSource, int idDest, uchar *table);
 static void      CreateCodePagesCol();
 static void      ChangeDefaultCodePages(int idScr, int idApp)
                  { defAppCP=idApp; defScrCP=idScr; }
 static void      SetDefaultCodePages(int idScr, int idApp, int idInp)
                  { defAppCP=idApp; defScrCP=idScr; defInpCP=idInp; }

 enum { alphaChar=1, lowerChar=2, upperChar=4, digitChar=8 };
 static TVCodePageCol *CodePages;
 static ushort CPTable[257];
 static int    curAppCP,curScrCP,curInpCP;
 static int    defAppCP,defScrCP,defInpCP;
 static uchar  toUpperTable[256];
 static uchar  toLowerTable[256];
 static uchar  AlphaTable[256];
 static uchar  Similar[];
 static ushort Similar2[];
 static char   NeedsOnTheFlyRemap;
 static uchar  OnTheFlyMap[256];
 static char   NeedsOnTheFlyInpRemap;
 static uchar  OnTheFlyInpMap[256];
 static TVCodePageCallBack  UserHook;
 static uint16 appToUnicode[256];
 static TVPartitionTree556 *unicodeToApp;
 static uint16 inpToUnicode[256];
 static TVPartitionTree556 *unicodeToInp;
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
 static CodePage ISO8859_2_Lat2;
 static CodePage ISO8859_2u_Lat2;
 static CodePage ISO8859_2_Sun;
 static CodePage ISO8859_2e_Lat2;
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
 static uchar  High32FramesSwap[];

 friend class TScreen;
 friend class TDisplay;
};

#endif

