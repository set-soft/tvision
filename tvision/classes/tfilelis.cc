/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *

Modified by Robert H”hne to be used for RHIDE.

 *
 *
 */
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define Uses_MsgBox
#define Uses_TFileList
#define Uses_TRect
#define Uses_TSearchRec
#define Uses_TEvent
#define Uses_TGroup
#define Uses_TStreamableClass
#include <tv.h>

#include <errno.h>
#include <ctype.h>
#ifdef __DJGPP__
#include <dir.h>
#endif
#include <sys/stat.h>
#include <dirent.h>
#include <fnmatch.h>
#include <glob.h>

TFileList::TFileList( const TRect& bounds,
          TScrollBar *aScrollBar) :
    TSortedListBox( bounds, 2, aScrollBar )
{
 if (TV_HaveLFNs())
    numCols = 1;
}

TFileList::~TFileList()
{
 if ( list() )
   destroy ( list() );
}

void TFileList::focusItem( ccIndex item )
{
  TSortedListBox::focusItem( item );
  message( owner, evBroadcast, cmFileFocused, list()->at(item) );
}

// SET: From TV 2.0
void TFileList::selectItem( ccIndex item )
{
    message( owner, evBroadcast, cmFileDoubleClicked, list()->at(item) );
}

void TFileList::getData( void * )
{
}

void TFileList::setData( void * )
{
}

uint32 TFileList::dataSize()
{
  return 0;
}

void* TFileList::getKey( const char *s )
{
  static TSearchRec sR;

  if ( (shiftState & 0x03) != 0 || *s == '.' )
    sR.attr = FA_DIREC;
  else
    sR.attr = 0;
  strcpy( sR.name, s );
  return &sR;
}

void TFileList::getText( char *dest, ccIndex item, short maxChars )
{
  TSearchRec *f = (TSearchRec *)(list()->at(item));

  strncpy( dest, f->name, maxChars );
  dest[maxChars] = '\0';
    if( f->attr & FA_DIREC )
  strcat( dest, DIRSEPARATOR_ );
}

/* SET: Moved (TV 2.0)
void TFileList::handleEvent( TEvent & event )
{
  if ( event.what == evMouseDown && event.mouse.doubleClick )
  {
    event.what = evCommand;
    event.message.command = cmOK;
    putEvent( event );
    clearEvent( event );
  }
  else
    TSortedListBox::handleEvent( event );
}*/

void TFileList::readDirectory( const char *dir, const char *wildCard )
{
  char path[PATH_MAX];
  strcpy( path, dir );
  strcat( path, wildCard );
  readDirectory( path );
}

#ifdef __DJGPP__
typedef struct TSearchRec DirSearchRec;

struct __dj_DIR {
  int num_read;
  char *name;
  int flags;
  struct ffblk ff;
  struct dirent de;
};

extern "C" size_t _file_time_stamp(unsigned);

#else

struct DirSearchRec : public TSearchRec
{
  /* SS: changed */

  void readFf_blk(char *filename, struct stat &s)
  {
    attr = FA_ARCH;
    if (S_ISDIR(s.st_mode)) attr |= FA_DIREC;
    strcpy(name, filename);
    size = s.st_size;
    time = s.st_mtime;
  }

};

#endif

#ifdef __DJGPP__ // this is really faster then the glob methode
void TFileList::readDirectory( const char *aWildCard )
{
  DIR *dir;
  struct dirent *de;
  DirSearchRec *p;
  TFileCollection *fileList = new TFileCollection( 5, 5 );
  char *wildcard = (char *)alloca(strlen(aWildCard)+1);
  strcpy(wildcard,aWildCard);
  char *slash = strrchr(wildcard,'/');
  char *path;
  // SET: Added code to remove .. in the root directory
  int removeParent=0;
  
  if (slash)
  {
    *slash = 0;
    path = wildcard;
    if (strlen(path) == 2 && path[1] == ':')
    {
      path = (char *)alloca(4);
      strcpy(path,wildcard);
      strcat(path,"/");
    }
    slash++;
    if (strlen(path) == 3 && path[1] == ':')
       removeParent=1;
  }
  else
  {
    slash = wildcard;
    path = ".";
    char *cwd=getcwd(0,PATH_MAX);
    if (cwd)
      {
       if (strlen(cwd)==3 && cwd[1] == ':')
          removeParent=1;
       free(cwd);
      }
  }
  dir = opendir(path);
  if (dir) while ((de = readdir(dir)))
  {
    struct ffblk &ff = dir->ff;
    if (!(ff.ff_attrib & FA_DIREC) && fnmatch(slash,de->d_name,0)) continue;
    if (strcmp(de->d_name,".") == 0) continue;
    if (removeParent && strcmp(de->d_name,"..")==0) continue;
    p = new DirSearchRec;
    strcpy(p->name,de->d_name);
    p->attr = ff.ff_attrib;
    p->size = ff.ff_fsize;
    p->time = ((unsigned long)(ff.ff_fdate)) << 16 | (unsigned short)ff.ff_ftime;
    p->time = _file_time_stamp(p->time);
    fileList->insert(p);
  }
  if (dir) closedir(dir);
  newList(fileList);
  if (list()->getCount() > 0)
    message( owner, evBroadcast, cmFileFocused, list()->at(0) );
  else
  {
    static DirSearchRec noFile;
    message( owner, evBroadcast, cmFileFocused, &noFile );
  }
}

#else

void TFileList::readDirectory( const char *aWildCard )
{
  /* SS: changed */

  DIR *dp;
  DirSearchRec *p;
  char dir[PATH_MAX];
  char file[PATH_MAX];
  char path[PATH_MAX];
  char *np;
  dirent *de;
  glob_t gl;
  struct stat s;

  strcpy( path, aWildCard );
  if (!isWild(path)) strcat(path, "*");
  fexpand( path );
  expandPath(path, dir, file);
  TFileCollection *fileList = new TFileCollection( 5, 5 );

  /* find all filenames that match our wildcards */

  /*
   * The use of 'glob' function was proposed by:
   * Rainer Keuchel <r_keuchel@smaug.netwave.de>
   * Date: 18 Jan 1997 22:52:12 +0000
   */
#ifdef __linux__
#define __gl_options GLOB_PERIOD
#else
#define __gl_options 0
#endif
  if (glob(path, __gl_options, NULL, &gl) == 0)
  {
    for (int i = 0; i < (int)gl.gl_pathc; i++)
    {
      /* is this a regular file ? */
  
      if (stat(gl.gl_pathv[i], &s) == 0 && S_ISREG(s.st_mode))
      {
        if ((p = new DirSearchRec) == NULL) break;
  
        /* strip directory part */
  
        if ((np = strrchr(gl.gl_pathv[i], '/')) != NULL) np++;
        else np = gl.gl_pathv[i];
        p->readFf_blk(np, s);
        fileList->insert( p );
      }
    }
    globfree(&gl);
  }
  /* now read all directory names */

  sprintf(path, "%s.", dir);
  if ((dp = opendir(path)) != NULL)
  {
    while ((de = readdir(dp)) != NULL)
    {
      /* we don't want these directories */

      if (strcmp(de->d_name, ".") == 0 ||
        strcmp(de->d_name, "..") == 0) continue;

      /* is it a directory ? */

      sprintf(path, "%s%s", dir, de->d_name);
      if (stat(path, &s) == 0 && S_ISDIR(s.st_mode))
      {
        if ((p = new DirSearchRec) == NULL) break;
        p->readFf_blk(de->d_name, s);
        fileList->insert( p );
      }
    }
    closedir(dp);
  }

  if ( strlen( dir ) > 1 )
  {
    p = new DirSearchRec;
    if ( p != 0 )
    {
      sprintf(path, "%s..", dir);
      if (stat(path, &s) == 0)
      {
        p->readFf_blk("..", s);
      }
      else
      {
        strcpy( p->name, ".." );
        p->size = 0;
        p->time = 0x210000uL;
        p->attr = FA_DIREC;
      }
      fileList->insert( p );
    }
  }
#if 0 // I think this will never hapen
  if ( p == 0 )
    messageBox( tooManyFiles, mfOKButton | mfWarning );
#endif
  newList(fileList);
  if ( list()->getCount() > 0 )
    message( owner, evBroadcast, cmFileFocused, list()->at(0) );
  else
  {
    static DirSearchRec noFile;
    message( owner, evBroadcast, cmFileFocused, &noFile );
  }
}

#endif

#if !defined( NO_STREAM )
TStreamable *TFileList::build()
{
    return new TFileList( streamableInit );
}
#endif // NO_STREAM

