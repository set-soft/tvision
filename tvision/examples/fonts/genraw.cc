/**[txh]********************************************************************

  Copyright (c) 2002 by Salvador E. Tropea
  This code is Public Domain and you can use it for any purpose. Note that
when you link to another libraries, obviously needed, the code can (and
currently will) be affected by the libraries license.
  Currently my TV port is GPL so this code becomes GPL, but if you link
with another Turbo Vision port or the library changes license things are
different.

  Description:
  This examples shows how to use sft files for bitmaped fonts.
  It can be used as a fonts generator and supports over than 40 encodings.
Current sft files can be used to generate more than 200 fonts (different
sizes and encodings).
  For more sft files look in other directories or in the setedit project.
  
***************************************************************************/

#define Uses_stdlib
#define Uses_string
#define Uses_TVCodePage
#define Uses_TVFontCollection
#include <tv.h>

// This small test program creates raw fonts from the specified sft file
// In Linux the generated fonts can be loaded using consolechars -f file
// Note that some SFT files can contain fonts with more than 8 pixels of
// width.

void WriteRaw(uchar *data, int w, int h, const char *name)
{
 unsigned wSize=(w+7)/8;
 FILE *f=fopen(name,"wb");
 if (f)
   {
    printf("Writing %s\n",name);
    fwrite(data,256*h*wSize,1,f);
    fclose(f);
   }
}

void WriteHeader(uchar *data, int w, int h, char *name)
{
 FILE *f=fopen(name,"wb");
 int c,x,y;
 int wSize=(w+7)/8;

 if (f)
   {
    fprintf(f,"// Font %s size %d,%d\n",name,w,h);
    char *dot=strchr(name,'.');
    if (dot) *dot='_';
    fprintf(f,"uchar *FontData_%s[]=\n{\n",name);
    for (c=0; c<256; c++)
       {
        for (y=0; y<h; y++)
            for (x=0; x<wSize; data++, x++)
                fprintf(f,"0x%02X,",*data);
        fprintf(f,"\n");
       }
    fprintf(f,"};\n");
    fclose(f);
   }
}

int main(int argc, char *argv[])
{
 new TVCodePage(TVCodePage::ISOLatin1Linux,TVCodePage::ISOLatin1Linux,
                TVCodePage::ISOLatin1Linux);

 if (argc<2 || argc>3)
   {
    printf("Usage: %s sft_file [encoding]\nGenerates raw fonts\n",argv[0]);
    return 2;
   }

 char *end;
 int encoding=TVCodePage::ISOLatin1Linux;
 if (argc==3)
    encoding=strtol(argv[2],&end,0);

 TVFontCollection *p=new TVFontCollection(argv[1],encoding);
 if (p->GetError())
   {
    printf("Error!\n");
    return 1;
   }

 char b[PATH_MAX];
 int w,h;
 uchar *data;

 for (w=8; w<11; w++)
    {
     for (h=13; h<21; h++)
        {
         data=p->GetFont(w,h);
         // Not all are available, you must check. In fact only a few are
         // available
         if (!data) continue;
         sprintf(b,"raw%03d.%03d",w,h);
         #if 0
         WriteRaw(data,w,h,b);
         #else
         // Optional version to generated a C++ header
         WriteHeader(data,w,h,b);
         #endif
         DeleteArray(data);
        }
    }
 return 0;
}
