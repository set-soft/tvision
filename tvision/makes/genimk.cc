/**[txh]********************************************************************

  Copyright (c) 2003 by Salvador Eduardo Tropea
  Covered by the GPL license.
  Description:
  This program generates the .imk files from the .gpr files. To achieve it
first creates the .mak files and then calls the extrimk.exe program.
  It also generates a simplified version called .umk without extra
dependencies, just the needed to get the executables.
  
***************************************************************************/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <ctype.h>

const int maxLine=256;

static
void ChangeExt(char *s, const char *newE)
{
 int l=strlen(s);
 int lE=strlen(newE);
 strcpy(s+(l-lE),newE);
}

// Needed to force a make of the wrong files.
static
void RemoveGen(int argc, char *argv[])
{
 int i;

 for (i=2; i<argc; i++)
    {
     if (strstr(argv[i],".gpr"))
       {
        char *gpr=argv[i];
        char *imk=strdup(gpr);
        char *umk=strdup(gpr);
        ChangeExt(imk,".imk");
        ChangeExt(umk,".umk");
        unlink(imk);
        unlink(umk);
        free(imk);
        free(umk);
       }
    }
}

int main(int argc, char *argv[])
{
 if (argc<3)
   {
    printf("Usage: genimk output main.gpr [other.gprs...]\n");
    return 1;
   }
 const char *dest=argv[1];
 const char *mainGpr=argv[2];
 char *mainMak=strdup(mainGpr);
 ChangeExt(mainMak,".mak");
 printf("\nGenerating %s for %s (%s)\n",dest,mainGpr,mainMak);

 int i;
 unsigned lenTot=20;
 for (i=0; i<argc; i++)
     lenTot+=2+strlen(argv[i]);

 char *b=new char[lenTot];
 for (i=2; i<argc; i++)
    {
     if (strstr(argv[i],".gpr"))
       {
        char *gpr=argv[i];
        char *mak=strdup(gpr);
        ChangeExt(mak,".mak");

        struct stat stGpr;
        if (stat(gpr,&stGpr)!=0)
          {
           fprintf(stderr,"%s: Can't stat %s\n",argv[0],gpr);
           return 20;
          }

        struct stat stMak;
        if (stat(mak,&stMak)!=0 || stMak.st_mtime<stGpr.st_mtime)
          {
           printf("%s => %s\n",gpr,mak);
           sprintf(b,"gprexp %s",gpr);
           system(b);
           sprintf(b,"gpr2mak %s",gpr);
           system(b);
           if (stat(mak,&stMak)!=0)
             {
              fprintf(stderr,"%s: Can't create .mak file for %s\n",argv[0],gpr);
              return 21;
             }
          }

        free(mak);
       }
    }
 for (i=2; i<argc; i++)
    {
     if (strstr(argv[i],".gpr"))
       {
        char *gpr=argv[i];
        char *mak=strdup(gpr);
        char *imk=strdup(gpr);
        char *umk=strdup(gpr);
        ChangeExt(mak,".mak");
        ChangeExt(imk,".imk");
        ChangeExt(umk,".umk");

        struct stat stImk;
        printf("%s => %s\n",mak,imk);
        sprintf(b,"./extrimk.exe %s > %s",mak,imk);
        int ret=system(b);
        if (ret)
          {
           RemoveGen(argc,argv);
           return ret>256 ? ret>>8 : ret;
          }
        if (stat(mak,&stImk)!=0)
          {
           RemoveGen(argc,argv);
           fprintf(stderr,"%s: Can't create .imk file for %s\n",argv[0],mak);
           return 22;
          }

        printf("%s => %s\n",imk,umk);
        FILE *ori=fopen(imk,"rt");
        if (!ori)
          {
           RemoveGen(argc,argv);
           fprintf(stderr,"%s: Can't open .imk file for %s\n",argv[0],mak);
           return 23;
          }
        FILE *des=fopen(umk,"wt");
        if (!des)
          {
           RemoveGen(argc,argv);
           fprintf(stderr,"%s: Can't create .umk file for %s\n",argv[0],mak);
           return 24;
          }
        char b[maxLine], *sep;
        int skip=0;
        while (fgets(b,maxLine,ori))
          {
           if ((isalpha(b[0]) || b[0]=='.') && (sep=strchr(b,':'))!=0)
             {
              char *ext=sep-1;
              for (;*ext!='$' && ext>b; ext--);
              if (*ext!='$')
                {
                 fprintf(stderr,"%s: Parser error (ext): %s\n",argv[0],b);
                 RemoveGen(argc,argv);
                 return 25;
                }
              if (ext[4]=='O')
                {// That's an object, only the first is relevant.
                 for (;*sep && *sep!='\\'; sep++);
                 if (*sep) *sep=' ';
                 skip=1;
                 fputs(b,des);
                 continue;
                }
             }
           if (skip)
             {
              char *slash=b;
              for (;*slash && *slash!='\\'; slash++);
              if (!*slash)
                 skip=0;
             }
           else
              fputs(b,des);
          }
        fclose(des);
        fclose(ori);

        free(mak);
        free(imk);
        free(umk);
       }
    }

 printf("\n");
 return 0;
}

