/**[txh]********************************************************************

  Copyright (c) 2003 by Salvador Eduardo Tropea
  Covered by the GPL license.

  Description:
  Small program to help configuring packages using Turbo Vision.
  
***************************************************************************/

#define Uses_getcwd
#define Uses_stdio
#define Uses_string
#define Uses_limits
#define Uses_stdlib
#include <compatlayer.h>

#define VERSION "1.0.0"

#if defined(TVOS_DOS) || defined(TVOS_Win32) || defined(TVOSf_QNX4)
 // Currently only static linking is supported for DOS and Win32.
 #define ONLY_STATIC 1
#else
 #define ONLY_STATIC 0
#endif

static char *curdir=TVCONFIG_REF_DIR;
static int   lcurdir;

static
void StdInc(void)
{
 puts(TVCONFIG_RHIDE_STDINC);
}

/*static
char *FixRelative(const char *s)
{
 if (strncmp(s,"../../",6)==0)
   {
    char *ret=(char *)malloc(lcurdir+strlen(s)+1-5);
    sprintf(ret,"%s%s",curdir,s+5);
    return ret;
   }
 return strdup(s);
}*/

static
void Include(void)
{
 char *copy=strdup(TVCONFIG_INCLUDE);
 char *s=strtok(copy," ");

 while (s)
   {
    if (*s && *s!='.')
      {
       printf("-I%s ",s);
      }
    s=strtok(NULL," ");
   }
}

static
void SLibs(void)
{
 char *copy=strdup(TVCONFIG_RHIDE_OS_LIBS);
 char *s=strtok(copy," ");

 if (ONLY_STATIC)
    printf("-lrhtv ");
 else
    printf("-Wl,-dn -lrhtv -Wl,-dy ");

 while (s)
   {
    if (*s && *s!='.')
      {
       printf("-l%s ",s);
      }
    s=strtok(NULL," ");
   }
}

static
void DLibs(void)
{
 if (ONLY_STATIC)
    SLibs();
 else
   {
    char *copy=strdup(TVCONFIG_RHIDE_OS_LIBS);
    char *s=strtok(copy," ");

#if !defined(TVOSf_QNXRtP)
    printf("-lrhtv ");
#else
    printf("-lrhtv -lncursesS");
#endif /* TVOSf_QNXRtP */
    if (strstr(TVCONFIG_RHIDE_OS_LIBS,"tvfintl"))
       printf("-ltvfintl");
   }
}

static
void DirLibs(void)
{
 char *copy=strdup(TVCONFIG_TVOBJ);
 char *s=strtok(copy," ");

 while (s)
   {
    if (*s && *s!='.')
      {
       printf("-L%s ",s);
      }
    s=strtok(NULL," ");
   }
}

static
void Usage(void)
{
 fputs("Turbo Vision configuration tool v" VERSION "\n",stderr);
 fputs("Copyright (c) 2003 by Salvador E. Tropea.\n",stderr);
 fputs("That's free software covered by the GPL license.\n",stderr);
 fprintf(stderr,"Usage: rhtv-config.exe OPTION\n");
 fprintf(stderr,"Available options: [Only one can be specified]\n");
 fprintf(stderr,"\t--stdinc\n");
 fprintf(stderr,"\t--include\n");
 fprintf(stderr,"\t--slibs   [for linking static]\n");
 fprintf(stderr,"\t--dlibs   [for linking dynamic]\n");
 fprintf(stderr,"\t--dir-libs\n");
 fprintf(stderr,"\t--cflags\n");
 fprintf(stderr,"\t--cppflags\n");
 fprintf(stderr,"\t--version\n");
}

static
void UnknowOp(const char *s)
{
 fprintf(stderr,"Unknown option: %s\n\n",s);
 Usage();
}

static
void SetUpCurDir(void)
{
 lcurdir=strlen(curdir);
}

int main(int argc, char *argv[])
{
 char *op;
 if (argc!=2)
   {
    Usage();
    return 1;
   }
 op=argv[1];
 if (op[0]!='-' || op[1]!='-')
   {
    UnknowOp(op);
    return 2;
   }
 SetUpCurDir();
 op+=2;
 if (strcmp(op,"stdinc")==0)
    StdInc();
 else if (strcmp(op,"include")==0)
    Include();
 else if (strcmp(op,"slibs")==0)
    SLibs();
 else if (strcmp(op,"dlibs")==0)
    DLibs();
 else if (strcmp(op,"dir-libs")==0)
    DirLibs();
 else if (strcmp(op,"cflags")==0)
    fputs(TVCONFIG_CFLAGS,stdout);
 else if (strcmp(op,"cppflags")==0)
    fputs(TVCONFIG_CXXFLAGS,stdout);
 else if (strcmp(op,"version")==0)
    fputs("Turbo Vision configuration tool v" VERSION,stdout);
 else
   {
    UnknowOp(op);
    return 2;
   }
 printf("\n");
 return 0;
}

