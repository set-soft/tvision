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
#define Uses_TVCodePage
#define Uses_TVFontCollection
#include <tv.h>

// This small test program creates raw fonts from the specified sft file
// In Linux the generated fonts can be loaded using consolechars -f file
int main(int argc, char *argv[])
{
 new TVCodePage(TVCodePage::ISOLatin1Linux,TVCodePage::ISOLatin1Linux);

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

 FILE *F;
 char b[PATH_MAX];
 int i;
 uchar *data;

 for (i=13; i<18; i++)
    {
     data=p->GetFont(i);
     if (!data) continue;
     sprintf(b,"rawfont.%03d",i);
     F=fopen(b,"wb");
     if (F)
       {
        printf("Writing %s\n",b);
        fwrite(data,256*i,1,F);
        fclose(F);
       }
    }
 return 0;
}
