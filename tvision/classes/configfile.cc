/**[txh]********************************************************************

  Copyright 2002-2009 by Salvador Eduardo Tropea (SET)
  This file is covered by the GPL license.

  Module: Config File
  Include: TVConfigFile
  Comments:
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
 __("Can't open file"), // -1
 __("Comment inside section name"), // -2
 __("Unterminated section name"), // -3
 __("Empty section name"), // -4
 __("Syntax error"), // -5
 __("Missing open section {"), // -6
 __("Missing close section }"), // -7
 __("Missing equal sign"), // -8
 __("Missing value after equal"), // -9
 __("Unterminated string"), // -10
 __("Non branch in base"), // -11
 __("Trying to overwrite a value with a section"), // -12
 __("Trying to overwrite a section with a value")  // -13
};

/**[txh]********************************************************************

  Description:
  Initializes the class and reads the contents of the specified file to a
tree in memory.
  
***************************************************************************/

TVConfigFile::TVConfigFile()
{
 tree.next=NULL;
 sLine=0;
 line=NULL;
 ErrorStatus=0;
}

/**[txh]********************************************************************

  Description:
  Searchs for the indicated key in the tree. Separate nested sections using
forward slashes.
  
  Return: not 0 if the key was found, the return value is in @<var>{p} if
the key is an string. Otherwise @<var>{p} is NULL and the value is in
@<var>{n}.
  
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
 ssize_t read=CLY_getline(&line,&sLine,f);
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
 char *ret=new char[len+1], *ori, *dest;
 for (ori=start, dest=ret; *ori && *ori!='"'; ori++, dest++)
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
 return ret;//newStrL(start,s-start-1);
}

long TVConfigFile::GetInteger()
{
 char *end;
 long value;
 value=strtol(s,&end,0);
 s=end;
 return value;
}

int TVConfigFile::ReadBranch(TVConfigFileTreeNode *&base)
{
 int cant=0, ret;
 char *start, *end;
 TVConfigFileTreeNode *aux, *last=base;

 while (last && last->next) last=last->next;

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

          aux=SearchOnlyInBranch(base,start,s-start);
          if (aux)
            { // We already have it
             if (aux->type!=tBranch) return -12;
             s++;
             ret=ReadBranch(aux->content);
            }
          else
            { // New one
             aux=NewBranch(start,s-start);
             aux->priority=fromFile;
             //printf("New Branch 2: %s\n",aux->name);
             if (base)
               {
                last->next=aux;
                last=aux;
               }
             else
                base=last=aux;
             s++;
             ret=ReadBranch(aux->content);
            }
          if (ret<0) return ret;

          cant+=ret;
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
          int lenVar=s-start-1;
          // Be sure we can get the value
          char *string=NULL;
          long integer=0;
          if (*s=='"')
            {
             string=GetString();
             if (!string) return -10;
            }
          else if (isdigit(*s))
            {
             integer=GetInteger();
            }
          else
             return -10;
          // Is already there?
          int newOne=0;
          aux=SearchOnlyInBranch(base,start,lenVar);
          if (aux)
            { // We already have it
             if (aux->type==tBranch) return -13;
             if (aux->priority>fromFile)
                aux=NULL;
             else
                if (aux->type==tString) delete[] aux->string;
            }
          else
            {
             aux=new TVConfigFileTreeNode;
             newOne=1;
            }

          if (aux)
            {
             if (string)
               {
                aux->string=string;
                aux->type=tString;
               }
             else
               {
                aux->integer=integer;
                aux->type=tInteger;
               }
             if (newOne)
                aux->name=newStrL(start,end-start);
             aux->next=NULL;
             aux->priority=fromFile;
             /*if (aux->type==tString)
                printf("New String: %s=\"%s\"\n",aux->name,aux->string);
             else
                printf("New Integer: %s=%d\n",aux->name,aux->integer);*/
             if (newOne)
               {
                if (base)
                  {
                   last->next=aux;
                   last=aux;
                  }
                else
                   base=last=aux;
               }
             cant++;
            }
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

int TVConfigFile::ReadBase(TVConfigFileTreeNode *&base)
{
 int cant=0, ret;
 char *start;
 TVConfigFileTreeNode *aux, *last=base;

 while (last && last->next) last=last->next;

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
          aux=SearchOnlyInBranch(base,start,s-start);
          if (aux)
            { // We already have it
             if (aux->type!=tBranch) return -11;
             s++;
             ret=ReadBranch(aux->content);
            }
          else
            { // New one
             aux=NewBranch(start,s-start);
             aux->priority=fromFile;
             //printf("New Branch 1: %s\n",aux->name);
             if (base)
               {
                last->next=aux;
                last=aux;
               }
             else
                base=last=aux;
             s++;
             ret=ReadBranch(aux->content);
            }
          if (ret<0) return ret;
          cant+=ret;
          EatSpaces();
         }
       if (*s && *s!='#' && *s!='[')
          return -5;
      }
    while (*s && *s!='#');
   }
 return cant;
}

TVConfigFileTreeNode *TVConfigFile::SearchOnlyInBranch(TVConfigFileTreeNode *b,
                                                       char *name, int len)
{
 if (!b) return NULL;
 AllocLocalStr(key,len+1);
 memcpy(key,name,len);
 key[len]=0;
 return SearchOnlyInBranch(b,key);
}

TVConfigFileTreeNode *TVConfigFile::SearchOnlyInBranch(TVConfigFileTreeNode *b,
                                                       char *name)
{
 if (!b) return NULL;
 while (b)
   {
    if (strcmp(b->name,name)==0)
       return b;
    b=b->next;
   }
 return NULL;
}

int TVConfigFile::SearchInBranch(TVConfigFileTreeNode *b, char *key, char *&p, long &n)
{
 if (!key || !b) return 0;
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
 int ret=ReadBase(tree.next);

 // Free the line
 free(line);
 line=NULL;
 sLine=0;

 fclose(f);
 return ret;
}

int TVConfigFile::Add(const char *key, TVConfigFileTreeNode *node)
{
 int len=strlen(key);
 AllocLocalStr(b,len+1);
 strcpy(b,key);

 // Create/navigate the "directories"
 TVConfigFileTreeNode *where=tree.next, **parent=&tree.next, *p;
 char *val=strtok(b,"/");
 while (val)
   {
    p=SearchOnlyInBranch(where,val);
    if (p)
      {
       parent=&p->content;
       where=p->content;
      }
    else
      {
       TVConfigFileTreeNode *aux=NewBranch(val,len);
       aux->priority=node->priority;
       if (where)
         {// At the end
          TVConfigFileTreeNode *p=where;
          while (p->next) p=p->next;
          p->next=aux;
         }
       else
          *parent=aux;
       parent=&aux->content;
       where=NULL;
      }
    val=strtok(NULL,"/");
   }
 // Now put the value
 p=SearchOnlyInBranch(where,node->name);
 if (p)
   {
    if (p->type==tBranch)
      {
       ErrorStatus=-13;
       return 0;
      }
    if (p->priority>node->priority) return 0;
    if (p->type==tString)
       delete[] p->string;
    node->next=p->next;
    memcpy(p,node,sizeof(TVConfigFileTreeNode));
   }
 else
   {
    if (where)
      {// At the end
       TVConfigFileTreeNode *b=where;
       while (b->next) b=b->next;
       b->next=node;
      }
    else
       *parent=node;
   }
 return 1;
}

/**[txh]********************************************************************

  Description:
  Adds a new variable to the tree. The type of the variable is integer. The
@<var>{name} variable is added to the section indicated by @<var>{key}.
Possible priority values are justHint (25), fromFile (50) or fromApplication
(75). It is used to determine if a current value should be replaced by the
new value. The name is copied.
  
  Return: !=0 if the variable was added.
  Example: TVConfigFile::AddInt("TV/X11","ScreenWidth",90,
TVConfigFile::fromFile)
  
***************************************************************************/

int TVConfigFile::AddInt(const char *key, const char *name, long value, int priority)
{
 TVConfigFileTreeNode *node=new TVConfigFileTreeNode;
 node->type=tInteger;
 node->priority=priority;
 node->integer=value;
 node->name=newStr(name);
 node->next=NULL;
 if (!Add(key,node))
   {
    DeleteArray(node->name);
    delete node;
    return 0;
   }
 return 1;
}

/**[txh]********************************************************************

  Description:
  Adds a new variable to the tree. The type of the variable is string. The
@<var>{name} variable is added to the section indicated by @<var>{key}.
Possible priority values are justHint (25), fromFile (50) or fromApplication
(75). It is used to determine if a current value should be replaced by the
new value. The name and the content are copied.
  
  Return: !=0 if the variable was added.
  Example: TVConfigFile::AddInt("TV/X11","ExtProgVideoMode","A_Program",
TVConfigFile::fromFile)
  
***************************************************************************/

int TVConfigFile::AddString(const char *key, const char *name,
                            const char *value, int priority)
{
 TVConfigFileTreeNode *node=new TVConfigFileTreeNode;
 node->type=tString;
 node->priority=priority;
 node->string=newStr(value);
 node->name=newStr(name);
 node->next=NULL;
 if (!Add(key,node))
   {
    DeleteArray(node->name);
    DeleteArray(node->string);
    delete node;
    return 0;
   }
 return 1;
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
    delete[] p->name;
    delete p;
    p=aux;
   }
}

void TVConfigFile::FreeTree()
{
 FreeList(tree.next);
}

void TVConfigFile::PrintIndent(int indent, FILE *f)
{
 int i;
 for (i=indent; i; --i) fputc(' ',f);
}

void TVConfigFile::PrintBranch(TVConfigFileTreeNode *base, int indent, FILE *f)
{
 while (base)
   {
    switch (base->type)
      {
       case tBranch:
            PrintIndent(indent,f);
            fprintf(f,"[%s]\n",base->name);
            PrintIndent(indent,f);
            fputs("{\n",f);
            PrintBranch(base->content,indent+1,f);
            PrintIndent(indent,f);
            fputs("}\n",f);
            break;
       case tString:
            PrintIndent(indent,f);
            fprintf(f,"%s=\"%s\"\n",base->name,base->string);
            break;
       case tInteger:
            PrintIndent(indent,f);
            fprintf(f,"%s=%ld\n",base->name,base->integer);
            break;               
      }
    base=base->next;
   }
}

/**[txh]********************************************************************

  Description:
  Prints the current tree to the specified file. It's main purpose is to
debug programs, but you can use it to generate valid configuration files
from data found in memory.
    
***************************************************************************/

void TVConfigFile::Print(FILE *f)
{
 PrintBranch(tree.next,0,f);
}

// Inline members just documented here.
/**[txh]**
  Function: ~TVConfigFile
  Prototype: ~TVConfigFile()
  Description:
  This is the destructor and just releases the tree that contains the
variables.
*********/
/**[txh]**
  Function: Load
  Prototype: int Load(const char *file)
  Description: Reads the specified configuration file.
  Return: The read status, 0 means no error. The @<var>{Errors} data member
contains descriptions for the errors.
*********/

/*****************************************************************************
 That's a special case used to configure the library.
*****************************************************************************/

TVConfigFile *TVMainConfigFile::config=NULL;
const char *TVMainConfigFile::userConfigFile=NULL;
const char *configFileName="tvrc";
#if CLY_HiddenDifferent
const char *configFileNameH=".tvrc";
#endif

/**[txh]********************************************************************

  Class: TVMainConfigFile
  Description:
  This is the constructor for TVMainConfigFile. This class is a special case
of TVConfigFile used for the Turbo Vision configuration file. Note you can
store application specific information in the configuration file using a
section different than TV.
  
***************************************************************************/

TVMainConfigFile::TVMainConfigFile()
{
 config=new TVConfigFile();
}

/**[txh]********************************************************************

  Description:
  Loads the configuration file. First we try using the application provided
name, then we search in the current directory, the path indicated by the
HOME environment variable, the path indicated by the HOMEDIR environment
variable, /etc and /dev/env/DJDIR/etc (%DJDIR%/etc). The default name for
the file is tvrc, if the OS uses a point in the name to make it a hidden file
the library also tries with .tvrc.
  
  Return: The error status. 0 is OK.
  
***************************************************************************/

int TVMainConfigFile::Load()
{
 // Load the configuration file
 char *name=NULL;
 if (userConfigFile)
   {
    struct stat st;
    if (stat(userConfigFile,&st)==0 && S_ISREG(st.st_mode))
       name=newStr(userConfigFile);
   }
 if (!name)
   {
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
               {// DJGPP trick
                name=TestForFileIn("/dev/env/DJDIR/etc");
               }
            }
         }
      }
   }
 if (name)
   {
    //printf("Loading configuration from %s\n",name);
    config->Load(name);
    delete[] name;
   }
 //config->Print(stderr);
 return config->ErrorStatus;
}

/**[txh]********************************************************************

  Description:
  Destroys the global TVConfigFile that contains the configuration.
  
***************************************************************************/

TVMainConfigFile::~TVMainConfigFile()
{
 delete config;
 config=NULL;
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

/**[txh]********************************************************************

  Description:
  Looks for the specified key in the TV section. If found the result is
stored in @<var>{val}, but only if that's an integer.
  
  Return: True if the key exists, even when that's an string.
  
***************************************************************************/

Boolean TVMainConfigFile::Search(const char *key, long &val)
{
 if (!config || !key) return False;
 AllocLocalStr(b,strlen(key)+4);
 strcpy(b,"TV/");
 strcat(b,key);
 char *v;
 return config->Search(b,v,val) ? True : False;
}

/**[txh]********************************************************************

  Description:
  Looks for the specified @var{variable} in the TV section using the
specified @var{section} as subsection. If found the result is
stored in @<var>{val}, but only if that's an integer.
  
  Return: True if the key exists, even when that's an string.
  
***************************************************************************/

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

/**[txh]********************************************************************

  Description:
  Looks for the specified key in the TV section. If found the result is
returned but only if that's a string.
  
  Return: The string or NULL.
  
***************************************************************************/

char *TVMainConfigFile::Search(const char *key)
{
 if (!config || !key) return NULL;
 AllocLocalStr(b,strlen(key)+4);
 strcpy(b,"TV/");
 strcat(b,key);
 long n; char *v=NULL;
 config->Search(b,v,n);
 return v;
}

/**[txh]********************************************************************

  Description:
  Looks for the specified @var{variable} in the TV section using the
specified @var{section} as subsection. If found the result is returned but
only if that's a string.
  
  Return: The string or NULL.
  
***************************************************************************/

char *TVMainConfigFile::Search(const char *section, const char *variable)
{
 if (!config || !section || !variable) return NULL;
 AllocLocalStr(b,strlen(section)+strlen(variable)+5);
 strcpy(b,"TV/");
 strcat(b,section);
 strcat(b,"/");
 strcat(b,variable);
 long n; char *v=NULL;
 config->Search(b,v,n);
 return v;
}

/**[txh]********************************************************************

  Description:
  Adds a new variable of integer type to the TV subtree in the specified
@<var>{section} and using the specified @<var>{name}. The priority for this
variable is TVConfigFile::fromApplication. @x{TVConfigFile::AddInt}.
  
  Return: !=0 if added.
  
***************************************************************************/

int TVMainConfigFile::Add(const char *section, const char *name, long value)
{
 int lSection=section ? strlen(section)+1 : 0;
 AllocLocalStr(b,lSection+4);
 strcpy(b,"TV");
 if (section)
   {
    b[2]='/';
    strcpy(b+3,section);
   }
 return config->AddInt(b,name,value,TVConfigFile::fromApplication);
}

/**[txh]********************************************************************

  Description:
  Adds a new variable of string type to the TV subtree in the specified
@<var>{section} and using the specified @<var>{name}. The priority for this
variable is TVConfigFile::fromApplication. The string is copied.
@x{TVConfigFile::AddInt}.
  
  Return: !=0 if added.
  
***************************************************************************/

int TVMainConfigFile::Add(const char *section, const char *name,
                          const char *value)
{
 int lSection=section ? strlen(section)+1 : 0;
 AllocLocalStr(b,lSection+4);
 strcpy(b,"TV");
 if (section)
   {
    b[2]='/';
    strcpy(b+3,section);
   }
 return config->AddString(b,name,value,TVConfigFile::fromApplication);
}

// Inline members just documented here.
/**[txh]**
  Function: Print
  Prototype: void TVMainConfigFile::Print(FILE *f)
  Class: TVMainConfigFile
  Description:
  Prints the current tree to the specified file. It's main purpose is to
debug programs, but you can use it to generate valid configuration files
from data found in memory.
*********/
/**[txh]**
  Function: SetUserConfigFile
  Prototype: void TVMainConfigFile::SetUserConfigFile(const char *name)
  Class: TVMainConfigFile
  Description: Specifies a name and path for the configuration file. Use it
if your application needs special settings different than the rest of the
TV applications.
*********/

