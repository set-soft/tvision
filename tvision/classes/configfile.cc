/**[txh]********************************************************************

  Copyright 2002 by Salvador Eduardo Tropea (SET)
  This file is covered by the GPL license.

  Module: TVConfigFile
  Include: configfile.h
  Description:
  This module provides the TVConfigFile class. This class can load a
configuration file containing sections and variables. Each section have
a name which is indicated using brackets [Name]. As sections can be nested
and my text editor can indent using the braces as reference (also jump to
the start/end of a block and highlight the pair) the section's body must
be enclosed using braces. Sections can be nested conforming an structure
that resembles a file system. Inside a section you can define variables. A
variable name must be composed of a-zA-Z_0-9 characters. After the name, and
in the same line, an equal sign must be put. The value after the equal sign
can be a number (decimal, octal or hexadecimal as in C) or a string enclosed
using double quotes. You can use spaces around the equal sign and after the
value but you must finish a definition in one line. Comments are started
with a # symbol and ends at the end of the line.@p
  The information from a configuration file is stored in memory as a tree.
You can search in the tree some key indicating it as a you indicate a path
and file name. Just think it as a particular file system, just use forward
slashes.

***************************************************************************/

#define Uses_getline
#define Uses_stdio
#define Uses_ctype
#define Uses_string
#define Uses_stdlib
#define Uses_TVConfigFile
#define Uses_sys_stat
#define Uses_AllocLocal
#include <tv.h>

// Currently I don't want to expose these things:
// Type of nodes
const char tBranch=0, tInteger=1, tString=2;

const char *TVConfigFile::Errors[]=
{
 __("Can't open file"),
 __("Comment inside section name"),
 __( "Unterminated section name"),
 __( "Empty section name"),
 __( "Syntax error"),
 __( "Missing open section {"),
 __( "Missing close section }"),
 __( "Missing equal sign"),
 __( "Missing value after equal"),
 __( "Unterminated string")
};

/**[txh]********************************************************************

  Description:
  Initializes the class and reads the contents of the specified file to a
tree in memory.
  
***************************************************************************/

TVConfigFile::TVConfigFile(const char *file)
{
 tree.next=NULL;
 sLine=0;
 line=NULL;
 ErrorStatus=Read(file);
}

/**[txh]********************************************************************

  Description:
  Searchs for the indicated key in the tree. Separate nested sections using
forward slashes.
  
  Return: not 0 if the key was found, the return value is in @var{p} if
the key is an string. Otherwise @var{p} is NULL and the value is in @var{n}.
  
***************************************************************************/

int TVConfigFile::Search(const char *key, char *&p, long &n)
{
 if (!key) return 0;
 p=NULL;
 char *copy=newStr(key);
 strtok(copy,"/");
 int ret=SearchInBranch(tree.next,copy,p,n);
 delete[] copy;
 return ret;
}


int TVConfigFile::EatSpaces()
{
 for (; *s && *s!='\n' && isspace(*s) && *s!='#'; s++);
 return !*s || *s=='#';
}

int TVConfigFile::GetLine()
{
 ssize_t read=getline(&line,&sLine,f);
 if (read==-1) return -1;
 s=line;
 ErrorLine++;
 // Eat the EOL
 if (read && line[read-1]=='\n')
   {
    line[read-1]=0;
    return read-1;
   }
 return read;
}

char *TVConfigFile::newStrL(const char *start, int len)
{
 char *ret=new char[len+1];
 memcpy(ret,start,len);
 ret[len]=0;
 return ret;
}

TVConfigFileTreeNode *TVConfigFile::NewBranch(const char *name, int len)
{
 TVConfigFileTreeNode *aux=new TVConfigFileTreeNode;
 aux->type=tBranch;
 aux->content=NULL;
 aux->next=NULL;
 aux->name=newStrL(name,len);
 return aux;
}

int TVConfigFile::IsWordChar(int val)
{
 return isalnum(val) || val=='_';
}

char *TVConfigFile::GetString()
{
 char *start=++s;
 int len;
 for (len=1; *s && *s!='"'; len++, s++)
     if (*s=='\\' && s[1]) s++;
 if (*s!='"')
    return 0;
 s++;
 char *ret=new char[len], *ori, *dest;
 for (ori=start, dest=ret; *ori; ori++, dest++)
     if (*ori=='\\')
       {
        ori++;
        switch (*ori)
          {
           case 'n':
                *dest='\n';
                break;
           case 'r':
                *dest='\r';
                break;
           case 't':
                *dest='\t';
                break;
           default:
                *dest=*ori;
          }
       }
     else
       *dest=*ori;
 *dest=0;
 return newStrL(start,s-start-1);
}

long TVConfigFile::GetInteger()
{
 char *end;
 long value;
 value=strtol(s,&end,0);
 s=end;
 return value;
}

int TVConfigFile::ReadBranch(TVConfigFileTreeNode *parent)
{
 TVConfigFileTreeNode *p=NULL, *aux;
 int cant=0;
 char *start, *end;
 // Look for the open section character
 do
   {
    if (!EatSpaces())
      {
       if (*s!='{') return -6;
       s++;
       break;
      }
   }
 while (GetLine()!=-1);

 do
   {
    if (EatSpaces()) continue;
    do
      {
       if (*s=='[')
         {
          start=++s;
          if (*s==']') return -4;
          for (; *s && *s!=']' && *s!='#'; s++);
          if (*s=='#') return -2;
          if (!*s) return -3;
          aux=NewBranch(start,s-start);
          //printf("New Branch 2: %s\n",aux->name);
          if (p)
             p->next=aux;
          else
             parent->content=aux;
          s++;
          int ret=ReadBranch(aux);
          if (ret<0) return ret;
          cant+=ret;
          p=aux;
          EatSpaces();
         }
       if (*s=='}')
         {
          s++;
          return cant;
         }
       if (IsWordChar(*s))
         {
          start=s;
          for (; IsWordChar(*s); s++);
          end=s;
          if (EatSpaces() || *s!='=') return -8;
          s++;
          if (EatSpaces()) return -9;
          if (*s=='"')
            {
             char *string;
             if (!(string=GetString())) return -10;
             aux=new TVConfigFileTreeNode;
             aux->string=string;
             aux->type=tString;
            }
          else if (isdigit(*s))
            {
             aux=new TVConfigFileTreeNode;
             aux->integer=GetInteger();
             aux->type=tInteger;
            }
          else
             return -5;
          aux->name=newStrL(start,end-start);
          aux->next=NULL;
          /*if (aux->type==tString)
             printf("New String: %s=\"%s\"\n",aux->name,aux->string);
          else
             printf("New Integer: %s=%d\n",aux->name,aux->integer);*/
          if (p)
             p->next=aux;
          else
             parent->content=aux;
          cant++;
          p=aux;
          EatSpaces();
         }
       else
         if (*s && *s!='[' && *s!='#')
            return -5;
      }
    while (*s && *s!='#');
   }
 while (GetLine()!=-1);
 return -7;
}

int TVConfigFile::ReadBase(TVConfigFileTreeNode *p)
{
 int cant=0;
 char *start;
 TVConfigFileTreeNode *aux;

 while (GetLine()!=-1)
   {
    if (EatSpaces()) continue;
    do
      {
       if (*s=='[')
         {
          start=++s;
          if (*s==']') return -4;
          for (; *s && *s!=']' && *s!='#'; s++);
          if (*s=='#') return -2;
          if (!*s) return -3;
          aux=NewBranch(start,s-start);
          //printf("New Branch 1: %s\n",aux->name);
          p->next=aux;
          s++;
          int ret=ReadBranch(aux);
          if (ret<0) return ret;
          cant+=ret;
          p=aux;
          EatSpaces();
         }
       if (*s && *s!='#' && *s!='[')
          return -5;
      }
    while (*s && *s!='#');
   }
 return cant;
}

int TVConfigFile::SearchInBranch(TVConfigFileTreeNode *b, char *key, char *&p, long &n)
{
 if (!key) return 0;
 while (b)
   {
    if (strcmp(b->name,key)==0)
      {
       switch (b->type)
         {
          case tBranch:
               return SearchInBranch(b->content,strtok(NULL,"/"),p,n);
          case tString:
               p=b->string;
               break;
          case tInteger:
               n=b->integer;
               break;               
         }
       return 1;
      }
    b=b->next;
   }
 return 0;
}

int TVConfigFile::Read(const char *file)
{
 ErrorLine=0;
 f=fopen(file,"rt");
 if (!f)
    return -1;
 TVConfigFileTreeNode *ant=NULL, *p=&tree;

 // Move to the end of the current list
 while (p)
   {
    ant=p;
    p=p->next;
   }
 p=ant;
 int ret=ReadBase(p);

 // Free the line
 free(line);
 line=NULL;
 sLine=0;

 fclose(f);
 return ret;
}

void TVConfigFile::FreeList(TVConfigFileTreeNode *p)
{
 TVConfigFileTreeNode *start=p,*aux;
 // First pass is for recursion
 while (p)
   {
    if (p->type==tBranch)
       FreeList(p->content);
    p=p->next;
   }
 p=start;
 // Second pass frees it
 while (p)
   {
    aux=p->next;
    if (p->type==tString)
       delete[] p->string;
    delete p;
    p=aux;
   }
}

void TVConfigFile::FreeTree()
{
 FreeList(tree.next);
}


/*****************************************************************************
 That's a special case used to configure the library.
*****************************************************************************/

TVConfigFile *TVMainConfigFile::config=NULL;
const char *configFileName="tvrc";
#if CLY_HiddenDifferent
const char *configFileNameH=".tvrc";
#endif

TVMainConfigFile::TVMainConfigFile()
{
 // Load the configuration file
 char *name=NULL;
 name=TestForFileIn(".");
 if (!name)
   {
    char *home=getenv("HOME");
    if (home)
       name=TestForFileIn(home);
    if (!name)
      {
       home=getenv("HOMEDIR");
       if (home)
          name=TestForFileIn(home);
       if (!name)
         {
          name=TestForFileIn("/etc");
          if (!name)
            {// ToDo change by /dev/env...
             char *djgpp=getenv("DJGPP");
             if (djgpp)
               {
                AllocLocalStr(b,strlen(djgpp)+5);
                strcpy(b,djgpp);
                strcat(b,"/etc");
                name=TestForFileIn(b);
               }
            }
         }
      }
   }
 if (name)
   {
    config=new TVConfigFile(name);
    delete[] name;
   }
}

TVMainConfigFile::~TVMainConfigFile()
{
 delete config;
}

char *TVMainConfigFile::TestForFileIn(const char *where)
{
 AllocLocalStr(b,strlen(where)+strlen(configFileName)+3);
 struct stat st;

 strcpy(b,where);
 strcat(b,"/");
 strcat(b,configFileName);
 if (stat(b,&st)==0 && S_ISREG(st.st_mode))
    return newStr(b);

 #if CLY_HiddenDifferent
 strcpy(b,where);
 strcat(b,"/");
 strcat(b,configFileNameH);
 if (stat(b,&st)==0 && S_ISREG(st.st_mode))
    return newStr(b);
 #endif

 return NULL;
}

Boolean TVMainConfigFile::Search(const char *key, long &val)
{
 if (!config || !key) return False;
 AllocLocalStr(b,strlen(key)+4);
 strcpy(b,"TV/");
 strcat(b,key);
 char *v;
 return config->Search(b,v,val) ? True : False;
}

Boolean TVMainConfigFile::Search(const char *section, const char *variable, long &val)
{
 if (!config || !section || !variable) return False;
 AllocLocalStr(b,strlen(section)+strlen(variable)+5);
 strcpy(b,"TV/");
 strcat(b,section);
 strcat(b,"/");
 strcat(b,variable);
 char *v;
 return config->Search(b,v,val) ? True : False;
}

char *TVMainConfigFile::Search(const char *key)
{
 if (!config || !key) return False;
 AllocLocalStr(b,strlen(key)+4);
 strcpy(b,"TV/");
 strcat(b,key);
 long n; char *v;
 config->Search(b,v,n);
 return v;
}


