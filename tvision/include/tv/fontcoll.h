/**[txh]********************************************************************

  Copyright 1996-2002 by Salvador Eduardo Tropea (SET)
  This file is covered by the GPL license.

  Module: TVFontCollection

***************************************************************************/

#if defined(Uses_TVFontCollection) && !defined(TVFontCollection_Included)
#define TVFontCollection_Included 1

struct TVBitmapFont
{
 int first, last; // Currently unsupported
 int lines;
 uchar *fontFull; // 354 chars
 uchar *font;     // 256 according to the code page
};

class TVFontCollection : public TNSCollection
{
public:
 TVFontCollection(const char *file, int cp);
 ~TVFontCollection();

 virtual void freeItem(void *item);
 static void ReduceOne(uchar *dest, uchar *ori, int height, int num=256);
 static void EnlargeOne(uchar *dest, uchar *ori, int height, int num=256);
 uchar *GetFont(int height);
 void   SetCodePage(int id);
 int    GetError() { return error; }
 const char *GetFileName() { return fileName; }
 const char *GetFontName() { return fontName; }

protected:
 static Boolean CheckForLines(void *item, void *arg);
 static void    CreateFont(void *item, void *arg);
 TVBitmapFont *firstThat(ccTestFunc Test, int height)
   { return (TVBitmapFont *)(TNSCollection::firstThat(CheckForLines,(void *)&height)); };
 FILE *f;
 const static char Signature[];
 int   CheckSignature();
 char *ReadName();
 int   error;
 char *fileName;
 char *fontName;
};

#endif
