/**[txh]********************************************************************

  Copyright (c) 2003 by Salvador E. Tropea.
  Covered by the GPL license.
  Description:
  tlink have a really useless command line. This wrapper avoids all the
complexities.
  Also: I failed to create a macro that expands to something that creates
a response file. Looks like it only works outside macros. This program
just collects the files from the specified directories and creates the
library with all of them.
  
***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#define MAX_FILE 256

int main(int argc, char *argv[])
{
 FILE *f;
 int i,ret;
 char *s;
 char b[MAX_FILE];
 DIR *d;
 struct dirent *de;

 if (argc<3)
   {
    printf("tlink wrapper\n");
    printf("Copyright (c) 2003 by Salvador E. Tropea. GPL.\n");
    printf("Usage: blink lib list_obj_files\n");
    return 1;
   }
 f=fopen("bcclink.lnk","wt");
 if (!f)
   {
    printf("Can't create response file\n");
    return 2;
   }
 for (i=2; i<argc; i++)
    {
     d=opendir(argv[i]);
     if (!d)
       {
        printf("Can't open directory %s\n",argv[i]);
        return 4;
       }
     while ((de=readdir(d))!=NULL)
       {
        //printf("%s\n",de->d_name);
        s=de->d_name;
        if (!strstr(s,".obj"))
           continue;
        fprintf(f,"+%s\\%s &\n",argv[i],s);
       }
     closedir(d);
    }
 fprintf(f,"+");
 fclose(f);

 unlink(argv[1]);
 s=(char *)malloc(strlen(argv[1])+32);
 sprintf(s,"tlib %s /E /P256 @bcclink.lnk",argv[1]);
 ret=system(s);
 if (!ret)
    unlink("bcclink.lnk");
 return ret;
}
