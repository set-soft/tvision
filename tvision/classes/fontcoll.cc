/**[txh]********************************************************************

  Copyright 1996-2002 by Salvador Eduardo Tropea (SET)
  This file is covered by the GPL license.

  Module: TVFontCollection
  Description:
  This module handles collections of 8xN bitmaped fonts useful for text
consoles. The collection is created from a fonts files. Each item is a font
of certain height. The collection can return interpolated and extrapolated
fonts in a range of +/- one from the available fonts. The fonts file should
contain fonts with all the supported symbols.
  
***************************************************************************/

#define Uses_stdlib
#define Uses_string
#define Uses_TVCodePage
#define Uses_TVFontCollection
#include <tv.h>

//#define TEST

const int SizeInDisk1=3*sizeof(int);
const int SizeInDisk2=SizeInDisk1+sizeof(int);
const char TVFontCollection::Signature[]="SET's editor font\x1A";

struct SizeFont
{
 int width,height;
};

static
int isWordChar(int i)
{
 return i=='_' || TVCodePage::isAlpha(i);
}

/**[txh]********************************************************************

  Description:
  A specialized free to free the memory used by the font structure.
  
***************************************************************************/

void TVFontCollection::freeItem(void *item)
{
 TVBitmapFont *p=(TVBitmapFont *)item;
 if (p)
   {
    DeleteArray(p->font);
    DeleteArray(p->fontFull);
    delete p;
   }
}

/**[txh]********************************************************************

  Description:
  Internally used during searchs to look for a font that matchs a specified
size.
  
***************************************************************************/

Boolean TVFontCollection::CheckForLines(void *item, void *arg)
{
 TVBitmapFont *p=(TVBitmapFont *)item;
 SizeFont *s=(SizeFont *)arg;
 if (p && p->lines==s->height && p->width==s->width)
    return True;
 return False;
}

/**[txh]********************************************************************

  Description:
  Creates a font of the specified height from another of the same height
plus one. The destination buffer should be large enough (num*height bytes).
Special care is taked for letters.
  
***************************************************************************/

void TVFontCollection::ReduceOne(uchar *dest, uchar *ori, int height,
                                 int wBytes, int num)
{
 int i;
 unsigned sizeDest=wBytes*height;
 unsigned sizeOri=sizeDest+wBytes;
 // height is the dest size, ori is one more

 for (i=0; i<num; i++)
    {
     if (isWordChar(i) && ori[0])
       { // first pixel is used
        memcpy(dest,ori,sizeDest);
       }
     else
       { // first free
        memcpy(dest,ori+wBytes,sizeDest);
       }

     ori+=sizeOri;
     dest+=sizeDest;
    }
}

/**[txh]********************************************************************

  Description:
  Creates a font of the specified height from another of the same height
minus one. The destination buffer should be large enough (num*height bytes).
The last line is filled with 0.
  
***************************************************************************/

void TVFontCollection::EnlargeOne(uchar *dest, uchar *ori, int height,
                                  int wBytes, int num)
{
 int i;
 unsigned sizeDest=wBytes*height;
 unsigned sizeOri=sizeDest-wBytes;
 // height is the dest size, ori is one less

 for (i=0; i<num; i++)
    {
     memcpy(dest,ori,sizeDest);
     memset(dest+sizeOri,0,wBytes);

     ori+=sizeOri;
     dest+=sizeDest;
    }
}

/**[txh]********************************************************************

  Description:
  Returns a newly allocated block of memory containing a font of the
specified height.
  
  Return: NULL if the font isn't available.
  
***************************************************************************/

uchar *TVFontCollection::GetFont(int width, int height)
{
 int oneMore=0,oneLess=0;
 SizeFont sz={width,height};

 TVBitmapFont *p=(TVBitmapFont *)firstThat(CheckForLines,&sz);
 // If we can't find a font of the right size look for 1 more and one less
 if (!p)
   {
    sz.height++;
    p=(TVBitmapFont *)firstThat(CheckForLines,&sz);
    if (p)
       oneMore=1;
    else
      {
       sz.height-=2;
       p=(TVBitmapFont *)firstThat(CheckForLines,&sz);
       if (p)
          oneLess=1;
      }
   }

 if (!p)
    return NULL;

 unsigned size=256*height*p->wBytes;
 uchar *fontShape=new uchar[size];
 if (oneMore)
    ReduceOne(fontShape,p->font,height,p->wBytes);
 else if (oneLess)
    EnlargeOne(fontShape,p->font,height,p->wBytes);
 else
    memcpy(fontShape,p->font,size);

 return fontShape;
}

/**[txh]********************************************************************

  Description:
  Internally used to check if we opened a fonts file.
  
***************************************************************************/

int TVFontCollection::CheckSignature()
{
 char buf[sizeof(Signature)];
 fread(buf,sizeof(Signature)-1,1,f);
 buf[sizeof(Signature)-1]=0;
 return strcmp(Signature,buf)==0;
}

/**[txh]********************************************************************

  Description:
  Internally used to get the font's name.
  
***************************************************************************/

char *TVFontCollection::ReadName()
{
 uint16 strLen;
 fread(&strLen,2,1,f);
 char *aux=new char[strLen];
 strLen--;
 fread(aux,strLen,1,f);
 aux[strLen]=0;
 return aux;
}

/**[txh]********************************************************************

  Description:
  Internally used to create a font for a desired code page.
  
***************************************************************************/

void TVFontCollection::CreateFont(void *item, void *arg)
{
 TVBitmapFont *f=(TVBitmapFont *)item;
 ushort *map=(ushort *)arg;
 DeleteArray(f->font);
 unsigned size1=f->lines*f->wBytes;
 unsigned size=256*size1;
 f->font=new uchar[size];
 int i;
 uchar *dest=f->font;
 memset(f->font,0,size);
 for (i=0; i<256; i++,dest+=size1)
     memcpy(dest,&f->fontFull[map[i]*size1],size1);

 #if 0
 // This code stores the generated font to disk, is used for debug
 // purposes
 FILE *F;
 char b[PATH_MAX],*t;
 t=getenv("TMP");
 if (!t) t="/tmp";
 sprintf(b,"%s/font.%03d",t,f->lines);
 F=fopen(b,"wb");
 fwrite(f->font,size,1,F);
 fclose(F);
 #endif
}

/**[txh]********************************************************************

  Description:
  Sets the encoding of the fonts returned by GetFont.
  
***************************************************************************/

void TVFontCollection::SetCodePage(int id)
{
 forEach(CreateFont,TVCodePage::GetTranslate(id));
}

TVFontCollection::~TVFontCollection()
{
 DeleteArray(fontName);
 DeleteArray(fileName);
}

/**[txh]********************************************************************

  Description:
  Creates a font collection from the specified file and using the specified
code page. You must check the error status with GetError before using the
collection.
  
***************************************************************************/

TVFontCollection::TVFontCollection(const char *file, int cp) :
  TNSCollection(2,2)
{
 error=0;
 if (!file)
   {
    error=1;
    return;
   }

 f=fopen(file,"rb");
 if (!f)
   {
    error=2;
    return;
   }

 if (!CheckSignature())
   {
    fclose(f);
    error=3;
    return;
   }

 int version;
 fread(&version,sizeof(version),1,f);
 int numfonts;
 fread(&numfonts,sizeof(numfonts),1,f);

 int i;
 TVBitmapFont *p;
 uchar *fData;
 unsigned size;

 fontName=ReadName();
 fileName=newStr(file);

 for (i=0; i<numfonts; i++)
    {
     p=new TVBitmapFont;
     if (version==1)
       {
        fread(p,SizeInDisk1,1,f);
        p->width=8;
        p->wBytes=1;
       }
     else
       {
        fread(p,SizeInDisk2,1,f);
        p->wBytes=(p->width+7)/8;
       }
     size=(p->last-p->first+1)*p->lines*p->wBytes;
     fData=new uchar[size];
     fread(fData,size,1,f);
     p->fontFull=fData;
     p->font=0;
     insert(p);
    }
 fclose(f);
 SetCodePage(cp);
}


