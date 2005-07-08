/***************************************************************************

  Copyright 2002 by Salvador Eduardo Tropea (SET)
  This file is covered by the GPL license.
  For more information read configfile.cc

***************************************************************************/

#if defined(Uses_TVConfigFile) && !defined(TVConfigFile_Included)
#define  TVConfigFile_Included

// Structure for a node
struct TVConfigFileTreeNode
{
 char type;
 char priority;
 union
 {
  long integer;
  char *string;
  TVConfigFileTreeNode *content;
 };
 char *name;
 TVConfigFileTreeNode *next;
};

class CLY_EXPORT TVConfigFile
{
public:
 TVConfigFile();
 ~TVConfigFile() { FreeTree(); };
 int Load(const char *file) { ErrorStatus=Read(file); return ErrorStatus; };

 // Priorities 0 to 100
 enum Priority { justHint=25, fromFile=50, fromApplication=75 };

 int Search(const char *key, char *&p, long &n);
 int AddInt(const char *key, const char *name, long value, int priority);
 int AddString(const char *key, const char *name, const char *value, int priority);
 // Just for debug purposes
 void Print(FILE *f);

 int ErrorLine;
 int ErrorStatus;
 static const char *Errors[];

protected:
 TVConfigFileTreeNode tree;
 FILE *f;
 char *s, *line;
 size_t sLine;

 void  PrintBranch(TVConfigFileTreeNode *base, int indent, FILE *f);
 void  PrintIndent(int indent, FILE *f);
 int   EatSpaces();
 int   GetLine();
 TVConfigFileTreeNode
      *NewBranch(const char *name, int len);
 char *GetString();
 long  GetInteger();
 int   ReadBranch(TVConfigFileTreeNode *&base);
 int   ReadBase(TVConfigFileTreeNode *&base);
 int   SearchInBranch(TVConfigFileTreeNode *b, char *key, char *&p, long &n);
TVConfigFileTreeNode *
       SearchOnlyInBranch(TVConfigFileTreeNode *b, char *name, int len);
TVConfigFileTreeNode *
       SearchOnlyInBranch(TVConfigFileTreeNode *b, char *name);
 int   Read(const char *file);
 void  FreeList(TVConfigFileTreeNode *p);
 void  FreeTree();
 int   Add(const char *key, TVConfigFileTreeNode *node);

 static int   IsWordChar(int val);
 static char *newStrL(const char *start, int len);
};

// This is a special case to configure the library
class CLY_EXPORT TVMainConfigFile
{
public:
 TVMainConfigFile();
 ~TVMainConfigFile();
 static int     Load();
 static Boolean Search(const char *key, long &val);
 static Boolean Search(const char *section, const char *variable, long &val);
 static char   *Search(const char *key);
 static char   *Search(const char *section, const char *variable);
 static int     Add(const char *key, const char *name, long value);
 static int     Add(const char *key, const char *name, const char *value);
 static void    Print(FILE *f) { config->Print(f); };
 static void    SetUserConfigFile(const char *name) { userConfigFile=name; };

protected:
 static TVConfigFile *config;
 static char *TestForFileIn(const char *where);
 static const char *userConfigFile;
};
#endif

