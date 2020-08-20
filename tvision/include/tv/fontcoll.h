/**[txh]********************************************************************

  Copyright 1996-2003 by Salvador Eduardo Tropea (SET)
  This file is covered by the GPL license.

  Module: TVFontCollection

***************************************************************************/

#if defined(Uses_TVFontCollection) && !defined(TVFontCollection_Included)
#define TVFontCollection_Included 1

struct TVBitmapFont
{
 int first, last; // Currently is barely supported. The range includes both ends.
 unsigned lines;       // Height
 unsigned width;       // Width in pixels
 // --- Not in disk:
 int wBytes;      // Width in bytes. To avoid computing it all the time
 uchar *fontFull; // last-first+1 chars
 uchar *font;     // 256 according to the code page
};

struct TVBitmapFontDesc
{
 const char *name;
 const char *file;
 TStringCollection *sizes;
};

class CLY_EXPORT TVBitmapFontDescCol : public TStringCollection
{
public:
 TVBitmapFontDescCol() : TStringCollection(8,4) {}
 virtual void  freeItem(void *item);
 virtual void *keyOf(void *item);
};

class CLY_EXPORT TVBitmapFontDescLBox : public TSortedListBox
{
public:
 TVBitmapFontDescLBox(const TRect& bounds, ushort aNumCols,
                      TScrollBar *aHScrollBar, TScrollBar *aVScrollBar,
                      Boolean aCenterOps=False) :
  TSortedListBox(bounds,aNumCols,aHScrollBar,aVScrollBar,aCenterOps) {}
 virtual void getText(char *dest, ccIndex item, short maxChars);
};

struct TVBitmapFontSize
{
 unsigned w,h;
};

class CLY_EXPORT TVBitmapFontSizeCol : public TStringCollection
{
public:
 TVBitmapFontSizeCol() : TStringCollection(3,3) {}
 virtual int compare(void *key1, void *key2);
 // do not use the default delete[]
 virtual void freeItem(void *item) { delete (TVBitmapFontSize *)item; }
};

class CLY_EXPORT TVBitmapFontSizeLBox : public TSortedListBox
{
public:
 TVBitmapFontSizeLBox(const TRect& bounds, ushort aNumCols,
                      TScrollBar *aHScrollBar, TScrollBar *aVScrollBar,
                      Boolean aCenterOps=False) :
  TSortedListBox(bounds,aNumCols,aHScrollBar,aVScrollBar,aCenterOps) {}
 virtual void getText(char *dest, ccIndex item, short maxChars);
};

class CLY_EXPORT TVFontCollection : public TNSCollection
{
public:
 TVFontCollection(const char *file, int cp);
 ~TVFontCollection();

 virtual void freeItem(void *item);
 static void ReduceOne(uchar *dest, uchar *ori, int height, int wBytes, int num=256);
 static void EnlargeOne(uchar *dest, uchar *ori, int height, int wBytes, int num=256);
 uchar *GetFont(int width, int height);
 uchar *GetFontFull(int width, int height, int &first, int &last);
 void   SetCodePage(int id);
 int    GetError() { return error; }
 const char *GetFileName() { return fileName; }
 const char *GetFontName() { return fontName; }
 static TVBitmapFontDescCol *CreateListOfFonts(const char *dir, unsigned wmin=0,
          unsigned wmax=256, unsigned hmin=0, unsigned hmax=256);
 static void Size2Str(char *buffer, unsigned w, unsigned h);
 static void Str2Size(const char *buffer, unsigned &w, unsigned &h);

protected:
 static Boolean CheckForLines(void *item, void *arg);
 static void    CreateFont(void *item, void *arg);
 const static char Signature[];
 const static char SFTExtension[];
 static int      CheckSignature(FILE *f);
 static char    *ReadName(FILE *f);
 static void     ReadVersionNum(FILE *f, int *version, int *numfonts);
 static unsigned ReadFontInfo(FILE *f, int version, TVBitmapFont *p);
 static void     Swap(int *value);
 static void     Swap(unsigned *value) { Swap((int *)value); };
 static void     Swap(uint16 *value);
 // !=0 if the file was created with the reverse endian style.
 // Only used during load.
 static int      invertEndian;
 int   error;
 char *fileName;
 char *fontName;
};

#endif
