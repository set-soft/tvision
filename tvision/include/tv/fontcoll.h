/**[txh]********************************************************************

  Copyright 1996-2002 by Salvador Eduardo Tropea (SET)
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

class TVBitmapFontDescCol : public TStringCollection
{
public:
 TVBitmapFontDescCol() : TStringCollection(8,4) {}
 virtual void  freeItem(void *item);
 virtual void *keyOf(void *item);
};

class TVBitmapFontDescLBox : public TSortedListBox
{
public:
 TVBitmapFontDescLBox(const TRect& bounds, ushort aNumCols,
                      TScrollBar *aHScrollBar, TScrollBar *aVScrollBar) :
  TSortedListBox(bounds,aNumCols,aHScrollBar,aVScrollBar) {}
 virtual void getText(char *dest, ccIndex item, short maxChars);
};

class TVFontCollection : public TNSCollection
{
public:
 TVFontCollection(const char *file, int cp);
 ~TVFontCollection();

 virtual void freeItem(void *item);
 static void ReduceOne(uchar *dest, uchar *ori, int height, int wBytes, int num=256);
 static void EnlargeOne(uchar *dest, uchar *ori, int height, int wBytes, int num=256);
 uchar *GetFont(int width, int height);
 void   SetCodePage(int id);
 int    GetError() { return error; }
 const char *GetFileName() { return fileName; }
 const char *GetFontName() { return fontName; }
 static TVBitmapFontDescCol *CreateListOfFonts(const char *dir, unsigned wmin,
          unsigned wmax, unsigned hmin, unsigned hmax);
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
 int   error;
 char *fileName;
 char *fontName;
};

#endif
