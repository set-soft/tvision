/**[txh]********************************************************************

  Copyright (c) 2003-2004 by Salvador Eduardo Tropea.
  Covered by the GPL license.
  Description:
  This program generates a .imk file containing the dependencies for a
project.
  
***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <limits.h>
#include <unistd.h>
#include <time.h>
#include <ctype.h>

#define ONE_DEP_BY_LINE 1

const int maxLine=1024;
unsigned maxCol=78;

struct node;
static char *projectBase;
static int   projectBaseL;
static int   IncludeCounter=0;

struct stMak
{
 node *base, *last;
 char *objDir;
 char *mainTarget;
 char *baseDir;
};

struct node
{
 char *name;
 node *deps;
 node *ldep;
 node *next;
 stMak *subprj;
};

struct stIncDir
{
 char *var;
 char *dir;
 int   ldir;
};

stIncDir incDirs[]=
{
{"INCLUDE_DIR",      "../include", 0},
{"CLASSES_DIR",      "../classes", 0},
{"EXTRA_SRC_DIR",    "..",         0},
{0,0}
};

char *srcDirs[]=
{
 "../classes",
 "../classes/dos",
 "../classes/linux",
 "../classes/qnx4",
 "../classes/qnxrtp",
 "../classes/unix",
 "../classes/win32",
 "../classes/wingr",
 "../classes/winnt",
 "../classes/x11",
 "../names",
 "../stream",
 "../compat",
 "..",
 NULL
};

static
void AddFileName(const char *name, stMak &mk)
{
 //printf("`%s'\n",name);
 if (mk.base)
   {
    mk.last->next=new node;
    mk.last=mk.last->next;
   }
 else
    mk.base=mk.last=new node;
 mk.last->name=strdup(name);
 mk.last->next=mk.last->deps=mk.last->ldep=NULL;
 mk.last->subprj=NULL;
}

static
int PrConvertExt(FILE *d, const char *file)
{
 char *s=strrchr(file,'.');
 if (!s)
    return fprintf(d,"%s",file);
 int l=s-file;
 fwrite(file,l,1,d);
 if (strcmp(s,".o")==0)
    l+=fprintf(d,"$(ExOBJ)");
 else if (strcmp(s,".exe")==0)
    l+=fprintf(d,"$(ExEXE)");
 else if (strcmp(s,".a")==0)
    l+=fprintf(d,"$(ExLIB)");
 else if (strcmp(s,".env")==0)
    l+=fprintf(d,".env");
 else if (strcmp(s,".imk")==0)
    l+=fprintf(d,".imk");
 else if (strcmp(s,".h")==0)
    l+=fprintf(d,".h");
 else if (strcmp(s,".c")==0)
    l+=fprintf(d,".c");
 else if (strcmp(s,".cc")==0)
    l+=fprintf(d,".cc");
 else if (strcmp(s,".cpp")==0)
    l+=fprintf(d,".cpp");
 else
   {
    fprintf(stderr,"Unknown extension: '%s'\n",s);
    exit(15);
   }
 return l;
}

static
void ExtractSources(FILE *f, stMak &mk)
{
 char buffer[maxLine];
 char found=0;
 do
   {
    if (fgets(buffer,maxLine,f))
       if (strncmp(buffer,"PROJECT_ITEMS",13)==0)
         {
          found=1;
          break;
         }
   }
 while (!feof(f));
 if (!found)
   {
    fprintf(stderr,"Can't find project items\n");
    exit(1);
   }
 char *s=buffer+14;
 do
   {
    char *fName=strtok(s," \t\\\n");
    while (fName)
      {
       if (*fName)
          AddFileName(fName,mk);
       fName=strtok(NULL," \t\\\n");
      }
    fgets(buffer,maxLine,f);
    s=buffer;
   }
 while (!feof(f) && buffer[0]=='\t');
}

static
void AddDependency(const char *name, node *p)
{
 node *nNode=new node;
 nNode->next=NULL;
 if (p->deps)
   {
    node *r=p->deps, *a=NULL;
    // Force the list to be sorted, it reduces the output of diff
    while (r && strcmp(r->name,name)<0)
      {
       a=r;
       r=r->next;
      }
    if (!r)
      {// @ the end
       p->ldep->next=nNode;
       p->ldep=nNode;
      }
    else if (!a) // @ the beggining
      {
       nNode->next=p->deps;
       p->deps=nNode;
      }
    else // in the middle
      {
       a->next=nNode;
       nNode->next=r;
      }
   }
 else
    p->deps=p->ldep=nNode;
 nNode->name=strdup(name);
 nNode->deps=nNode->ldep=NULL;
 nNode->subprj=NULL;
}

static
void ExtractDeps(FILE *f, node *p)
{
 char buffer[maxLine];
 char bName[maxLine];
 char found=0;
 int depNum;
 do
   {
    if (fgets(buffer,maxLine,f) &&
        sscanf(buffer,"DEPS_%d=%s ",&depNum,bName)==2 &&
        depNum)
      {
       if (strcmp(bName,p->name))
         {
          fprintf(stderr,"Error: unsorted deps? (%d,%s looking for %s)\n",depNum,bName,
                  p->name);
          exit(2);
         }
       found=1;
       break;
      }
   }
 while (!feof(f));
 if (!found)
   {
    fprintf(stderr,"Failed to find %s dependencies\n",p->name);
    exit(3);
   }
 char *s=strchr(buffer,'=')+1+strlen(p->name);
 do
   {
    char *fName=strtok(s," \t\\\n");
    while (fName)
      {
       if (*fName)
          AddDependency(fName,p);
       fName=strtok(NULL," \t\\\n");
      }
    fgets(buffer,maxLine,f);
    s=buffer;
   }
 while (!feof(f) && buffer[0]=='\t');
}

static
int PrintDep(FILE *d, int l, const char *s)
{
 if (ONE_DEP_BY_LINE)
   {
    fputs("\\\n\t",d);
    l=8+PrConvertExt(d,s)+1;
    fputc(' ',d);
   }
 else
   {
    if (l+strlen(s)+2>maxCol)
      {
       fputs("\\\n\t",d);
       l=8;
      }
    l+=PrConvertExt(d,s)+1;
    fputc(' ',d);
   }
 return l;
}

static
int PrintDepDir(FILE *d, int l, const char *s,int lenDir, const char *dir)
{
 if (ONE_DEP_BY_LINE)
   {
    fputs("\\\n\t",d);
    l=8+fprintf(d,"%s/",dir)+PrConvertExt(d,s)+1;
    fputc(' ',d);
   }
 else
   {
    if (l+strlen(s)+3+lenDir>maxCol)
      {
       fputs("\\\n\t",d);
       l=8;
      }
    l+=fprintf(d,"%s/",dir)+PrConvertExt(d,s)+1;
    fputc(' ',d);
   }
 return l;
}

static
int AddFixedDeps(FILE *d, int l)
{
 //l=PrintDep(d,l,"rhide.env");
 //l=PrintDep(d,l,"common.imk");
 l=PrintDep(d,l,"$(MAKEFILE_DEPS)");
 return l;
}

static
char *SearchSrc(char *toStat)
{
 struct stat st;
 if (stat(toStat,&st))
   {
    int i;
    char buf[PATH_MAX];
    for (i=0; srcDirs[i]; i++)
       {
        strcpy(buf,srcDirs[i]);
        strcat(buf,"/");
        strcat(buf,toStat);
        if (stat(buf,&st)==0)
           return strdup(buf);
       }
   }
 return toStat;
}

static
int IsAbsolute(const char *s)
{
 #if defined(__DJGPP__)
 return *s=='/' || (isalpha(*s) && s[1]==':');
 #else
 return *s=='/';
 #endif
}

static
void GenerateDepFor(node *p, FILE *d, stMak &mk)
{
 char *baseName=strdup(p->name);
 char *s=strrchr(baseName,'.');
 char *ext=s+1;
 *s=0;

 // Already compiled item?
 if (strcmp(ext,"o")==0 || strcmp(ext,"a")==0)
    return;

 int isExample=mk.baseDir && strstr(mk.baseDir,"example"), l;
 if (isExample)
   {
    l=fprintf(d,"%s/%s$(ExOBJ):: %s ",mk.objDir,baseName,p->name);
   }
 else
   {
    char *relName=SearchSrc(p->name);
    l=fprintf(d,"%s/%s$(ExOBJ):: %s ",mk.objDir,baseName,relName);
    if (relName!=p->name)
       free(relName);
   }
 node *c=p->deps;
 int isCompat=mk.baseDir && strstr(mk.baseDir,"compat");
 while (c)
   {
    s=c->name;
    if (strstr(s,p->name)!=NULL)
      {// RHIDE 1.5 duplicates the source as dependency
       c=c->next;
       continue;
      }
    char *toStat;
    struct stat st;

    if (mk.baseDir && !isCompat && !IsAbsolute(s))
      {
       toStat=new char[strlen(mk.baseDir)+strlen(s)+1];
       strcpy(toStat,mk.baseDir);
       strcat(toStat,s);
      }
    else
       toStat=strdup(s);
    int foundOnVPath=0;
    if (stat(toStat,&st))
      {
       //fprintf(stderr,"Buscando %s\n",toStat);
       // RHIDE 1.5 CVS filters the VPATH part, now I added it to common.imk.
       int i;
       char buf[PATH_MAX];
       for (i=0; !foundOnVPath && incDirs[i].var; i++)
          {
           strcpy(buf,incDirs[i].dir);
           strcat(buf,"/");
           strcat(buf,toStat);
           if (stat(buf,&st)==0)
              foundOnVPath=1;
          }
       for (i=0; !foundOnVPath && srcDirs[i]; i++)
          {
           strcpy(buf,srcDirs[i]);
           strcat(buf,"/");
           strcat(buf,toStat);
           if (stat(buf,&st)==0)
              foundOnVPath=1;
          }
       if (!foundOnVPath)
         {
          fprintf(stderr,"Can't stat %s dependency\n",toStat);
          exit(12);
         }
      }
    free(toStat);
    if (!foundOnVPath)
      {
       int i;
       if (strchr(s,'/'))
         {
          for (i=0; incDirs[i].var; i++)
              if (strncmp(s,incDirs[i].dir,incDirs[i].ldir)==0)
                {
                 s=(char *)malloc(3+strlen(incDirs[i].var)+1+strlen(s+incDirs[i].ldir));
                 sprintf(s,"$(%s)%s",incDirs[i].var,c->name+incDirs[i].ldir);
                 break;
                }
          if (s==c->name)
            {
             // This is some bug in RHIDE 1.5 CVS: some paths are emitted as absolute,
             // maybe is because the program is loading an old project.
             if (strncmp(s,projectBase,projectBaseL)==0)
               {
                char *sub=s+projectBaseL;
                char *toTest=(char *)malloc(strlen(sub)+3+1);
                sprintf(toTest,"../%s",sub);
                for (i=0; incDirs[i].var; i++)
                    if (strncmp(toTest,incDirs[i].dir,incDirs[i].ldir)==0)
                      {
                       s=(char *)malloc(3+strlen(incDirs[i].var)+1+strlen(toTest+incDirs[i].ldir));
                       sprintf(s,"$(%s)%s",incDirs[i].var,toTest+incDirs[i].ldir);
                       //fprintf(stderr,"%s Da: %s\n",c->name,s);
                       break;
                      }
                free(toTest);
               }
             if (s==c->name)
               {
                fprintf(stderr,"Unknown include dir: %s\n",c->name);
                exit(4);
               }
            }
         }
      }
    l=PrintDep(d,l,s);
    if (s!=c->name)
       free(s);
    c=c->next;
   }
 l=AddFixedDeps(d,l);
 fputc('\n',d);
 if (strcmp(ext,"c")==0)
    fputs("\t$(RHIDE_COMPILE_C)",d);
 else if (strcmp(ext,"cc")==0)
    fputs("\t$(RHIDE_COMPILE_CC)",d);
 else if (strcmp(ext,"cpp")==0)
    fputs("\t$(RHIDE_COMPILE_CC)",d);
 else if (strcmp(ext,"s")==0)
    fputs("\t$(RHIDE_COMPILE_ASM_FORCE)",d);
 else
   {
    fprintf(stderr,"\nUnknown extension `%s'\n",ext);
    exit(5);
   }
 fputs("\n\n",d);
 free(baseName);
}

static
int ExtractVar(FILE *f, const char *var, char *&dest, char ret)
{
 char buffer[maxLine];
 int l=strlen(var);
 do
   {
    if (fgets(buffer,maxLine,f))
      {
       if (strncmp(buffer,var,l)==0)
         {
          char *s=strtok(buffer+l+1,"\n");
          dest=s ? strdup(s) : strdup("");
          return 1;
         }
       else
         {
          if (strncmp(buffer,"include",7)==0)
            {
             char *s=strtok(buffer+8,"\n");
             FILE *inc=fopen(s,"rt");
             if (!inc)
               {
                fprintf(stderr,"Can't open include %s\n",s);
                exit(13);
               }
             int retVal=ExtractVar(inc,var,dest,1);
             fclose(inc);
             if (retVal)
                return 1;
            }
         }
      }
   }
 while (!feof(f));
 if (ret)
    return 0;
 fprintf(stderr,"Unable to find %s var\n",var);
 exit(7);
}

static
void ExtractObjDir(FILE *f, stMak &mk)
{
 mk.objDir=".";
 ExtractVar(f,"vpath_obj",mk.objDir,1);
}

/*static
void ExtractTVDir(FILE *f)
{
 if (incDirs[0].dir)
    return;
 ExtractVar(f,"TVISION_INC",incDirs[0].dir,0);
}*/

static
void ExtractTarget(FILE *f, stMak &mk)
{
 ExtractVar(f,"MAIN_TARGET",mk.mainTarget,0);
}

static
int CollectTargets(FILE *d, const char *name, stMak &mk, int l);

static
int ListTargetItems(FILE *d, int l, stMak &mk)
{
 node *p=mk.base;
 int lenObjDir=strlen(mk.objDir);
 while (p)
   {
    if (strstr(p->name,".gpr")==NULL)
      {
       char *s=strdup(p->name);
       char *ext=strrchr(s,'.');
       if (strcmp(ext,".o")==0 || strcmp(ext,".a")==0)
         {
          l=PrintDep(d,l,s);
         }
       else
         {
          strcpy(ext,".o");
          l=PrintDepDir(d,l,s,lenObjDir,mk.objDir);
         }
       free(s);
      }
    else
      {
       l=CollectTargets(d,p->name,*p->subprj,l);
      }
    p=p->next;
   }
 return l;
}

static
int CollectTargets(FILE *d, const char *name, stMak &mk, int l)
{
 if (mk.mainTarget && *mk.mainTarget)
   {// Simple, it have a lib
    l=PrintDep(d,l,mk.mainTarget);
   }
 else
   {// More complex
    l=ListTargetItems(d,l,mk);
   }
 return l;
}

static
void GenerateTarget(FILE *d, stMak &mk)
{
 if (!*mk.mainTarget)
    return;
 int l=PrConvertExt(d,mk.mainTarget)+3;
 fputs(":: ",d);
 l=ListTargetItems(d,l,mk);
 l=AddFixedDeps(d,l);
 char *ext=strrchr(mk.mainTarget,'.');
 if (ext) ext++; else ext="exe";
 fputc('\n',d);
 if (strcmp(ext,"exe")==0)
    fputs("\t$(RHIDE_COMPILE_LINK)",d);
 else if (strcmp(ext,"a")==0)
    fputs("\t$(RHIDE_COMPILE_ARCHIVE)",d);
 else
   {
    fprintf(stderr,"\nUnknown extension `%s'\n",ext);
    exit(8);
   }
 fputs("\n\n",d);
}

static
void GenerateAll(FILE *f, stMak &mk)
{
 GenerateTarget(f,mk);
 node *p=mk.base;
 while (p)
   {
    if (p->subprj)
      {
       if (0)
          GenerateAll(f,*p->subprj);
       else
         {
          char *s=strdup(p->name);
          // This is some bug in RHIDE 1.5 CVS: some paths are emitted as absolute,
          // maybe is because the program is loading an old project.
          if (strncmp(s,projectBase,projectBaseL)==0)
            {
             char *sub=s+projectBaseL;
             char *toTest=(char *)malloc(strlen(sub)+3+1);
             sprintf(toTest,"../%s",sub);
             free(s);
             s=toTest;
            }
          char *ext=strrchr(s,'.');
          *ext=0;
          fprintf(f,"INCLUDE%02d=%s$(ExIMK)\n\n",IncludeCounter++,s);
          free(s);
         }
      }
    else
       GenerateDepFor(p,f,mk);
    p=p->next;
   }
}

static
int ListTargetOItems(FILE *d, int l, stMak &mk)
{
 node *p=mk.base;
 int lenObjDir=strlen(mk.objDir);
 while (p)
   {
    if (strstr(p->name,".gpr")==NULL)
      {
       char *ext=strrchr(p->name,'.');
       if (strcmp(ext,".a"))
         {
          char *s=strdup(p->name);
          if (strcmp(ext,".o"))
             strcpy(strrchr(s,'.'),".o");
          l=PrintDepDir(d,l,s,lenObjDir,mk.objDir);
          free(s);
         }
      }
    else
      {
       if (!p->subprj->mainTarget || !*p->subprj->mainTarget)
          l=ListTargetOItems(d,l,*p->subprj);
      }
    p=p->next;
   }
 return l;
}

static
void CollectOTargets(FILE *d, const char *name, stMak &mk)
{

}


static
void GenerateObjs(FILE *f, stMak &mk)
{
 int l=fprintf(f,"OBJFILES=");
 ListTargetOItems(f,l,mk);
 fputs("\n\n",f);
}

static
int ListTargetAItems(FILE *d, int l, stMak &mk)
{
 node *p=mk.base;
 if (mk.mainTarget && *mk.mainTarget)
   {
    l=PrintDep(d,l,mk.mainTarget);
   }
 else
   {
    while (p)
      {
       if (p->subprj)
          ListTargetAItems(d,l,*p->subprj);
       else
         {
          char *ext=strrchr(p->name,'.');
          if (strcmp(ext,".a")==0)
             l=PrintDep(d,l,p->name);
         }
       p=p->next;
      }
   }
 return l;
}

static
void GenerateLibs(FILE *f, stMak &mk)
{
 int l=fprintf(f,"LIBRARIES=");
 node *p=mk.base;
 while (p)
   {
    if (p->subprj)
       l=ListTargetAItems(f,l,*p->subprj);
    else
      {
       char *ext=strrchr(p->name,'.');
       if (strcmp(ext,".a")==0)
          l=PrintDep(f,l,p->name);
      }
    p=p->next;
   }
 fputs("\n\n",f);
}

static
void ExtractBaseDir(const char *mak, stMak &mk)
{
 char *s=strrchr(mak,'/');
 if (s)
   {
    int l=s-mak+1;
    mk.baseDir=new char[l+1];
    memcpy(mk.baseDir,mak,l);
    mk.baseDir[l]=0;
   }
}

static
void ProcessMakefile(const char *mak, stMak &mk, int level)
{
 //printf("\n\n\nParsing %s makefile\n\n\n",mak);
 ExtractBaseDir(mak,mk);
 FILE *f;
 f=fopen(mak,"rt");
 if (!f)
   {
    fprintf(stderr,"Can't open %s\n",mak);
    exit(8);
   }
 ExtractObjDir(f,mk);
 fseek(f,0,SEEK_SET);
 long pos=ftell(f);
 ExtractTarget(f,mk);
 ExtractSources(f,mk);
 // RHIDE 1.4.7 have a different order than 1.4.9 and 1.5
 fseek(f,pos,SEEK_SET);
 //ExtractTVDir(f);
 int i;
 if (!incDirs[0].ldir)
    for (i=0; incDirs[i].var; i++)
        incDirs[i].ldir=strlen(incDirs[i].dir);
 node *p;
 //printf("Should dig:\n");
 p=mk.base;
 while (p)
   {
    char *s=strstr(p->name,".gpr");
    if (s)
      {
       //printf("%s\n",p->name);
       p->subprj=new stMak;
       memset(p->subprj,0,sizeof(stMak));
       char *subMak=strdup(p->name);
       s=strstr(subMak,".gpr");
       strcpy(s,".mak");
       ProcessMakefile(subMak,*p->subprj,level+1);
       free(subMak);
      }
    p=p->next;
   }
 p=mk.base;
 while (p)
   {
    char *ext=strrchr(p->name,'.');
    if (strcmp(ext,".gpr") && strcmp(ext,".a") && strcmp(ext,".o"))
       ExtractDeps(f,p);
    p=p->next;
   }
 // Generation
 if (level)
    return;

 char timeBuf[32];
 time_t now;
 time(&now);
 struct tm *brkT=localtime(&now);
 strftime(timeBuf,32,"%Y-%m-%d %H:%M",brkT);
 //fprintf(stdout,"#!/usr/bin/make\n# Automatically generated from RHIDE projects, don't edit\n# %s\n#\n\n",timeBuf);
 // Don't know why I needed the time, so I'm disabling it.
 fprintf(stdout,"#!/usr/bin/make\n# Automatically generated from RHIDE projects, don't edit\n#\n\n");

 // Write the body to a temporal
 char bNameT[12];
 strcpy(bNameT,"mkXXXXXX");
 int hT=mkstemp(bNameT);
 FILE *fT=fdopen(hT,"wt");
 if (hT==-1 || !fT)
   {
    fprintf(stderr,"Unable to create temporal\n");
    exit(31);
   }
 GenerateAll(fT,mk);
 fclose(fT);

 // Write the variables now
 if (mk.mainTarget && *mk.mainTarget)
   {
    char *ext=strrchr(mk.mainTarget,'.');
    if (!ext) ext=".exe";
    if (strcmp(ext,".a")==0 || strcmp(ext,".exe")==0)
      {
       GenerateObjs(stdout,mk);
       GenerateLibs(stdout,mk);
      }
   }

 // Now copy the body
 fT=fopen(bNameT,"rt");
 if (!fT)
   {
    fprintf(stderr,"Unable to copy temporal\n");
    exit(32);
   }
 char buffer[maxLine];
 while (!feof(fT))
   {
    if (fgets(buffer,maxLine,fT))
       fputs(buffer,stdout);
   }
 fclose(fT);
 unlink(bNameT);
}

static
void SetUpCurDir()
{
 char buf[PATH_MAX];
 getcwd(buf,PATH_MAX);
 char *mk=strstr(buf,"/makes");
 if (!mk)
   {
    fprintf(stderr,"We aren't in make directory\n");
    exit(30);
   }
 mk[1]=0;
 projectBase=strdup(buf);
 projectBaseL=strlen(projectBase);
}

int main(int argc, char *argv[])
{
 if (argc!=2)
   {
    printf("Usage: %s file.mak\n",argv[0]);
    return 1;
   }
 stMak mak;
 memset(&mak,0,sizeof(mak));
 SetUpCurDir();
 ProcessMakefile(argv[1],mak,0);
 //ExtractSources();
 return 0;
}

