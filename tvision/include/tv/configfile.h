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
 union
 {
  long integer;
  char *string;
  TVConfigFileTreeNode *content;
 };
 char *name;
 TVConfigFileTreeNode *next;
};

class TVConfigFile
{
public:
 TVConfigFile(const char *file);
 ~TVConfigFile() { FreeTree(); };

 int Search(const char *key, char *&p, long &n);

 int ErrorLine;
 int ErrorStatus;
 static const char *Errors[];

protected:
 TVConfigFileTreeNode tree;
 FILE *f;
 char *s, *line;
 size_t sLine;

 int   EatSpaces();
 int   GetLine();
 TVConfigFileTreeNode
      *NewBranch(const char *name, int len);
 char *GetString();
 long  GetInteger();
 int   ReadBranch(TVConfigFileTreeNode *parent);
 int   ReadBase(TVConfigFileTreeNode *p);
 int   SearchInBranch(TVConfigFileTreeNode *b, char *key, char *&p, long &n);
 int   Read(const char *file);
 void  FreeList(TVConfigFileTreeNode *p);
 void  FreeTree();

 static int   IsWordChar(int val);
 static char *newStrL(const char *start, int len);
};

// This is a special case to configure the library
class TVMainConfigFile
{
public:
 TVMainConfigFile();
 ~TVMainConfigFile();
 static Boolean Search(const char *key, long &val);
 static char   *Search(const char *key);

protected:
 static TVConfigFile *config;
 static char *TestForFileIn(const char *where);
};
#endif

