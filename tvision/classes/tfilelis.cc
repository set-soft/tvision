/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *

Modified by Robert H”hne to be used for RHIDE.
Modified by Vadim Beloborodov to be used on WIN32 console
Modified by Salvador E. Tropea to exclude .. in parent dir and
exlude some particular files by configuration.

 *
 *
 */
#include <tv/configtv.h>
 
#define Uses_string
#define Uses_stdio
#ifdef TVComp_MSC
 #include <io.h>
 #define Uses_glob
#else
 #define Uses_unistd
#endif
#define Uses_stdlib
#define Uses_ctype
#define Uses_HaveLFNs
#define Uses_dirent
#define Uses_getcwd
#define Uses_AllocLocal

#define Uses_TGKey
#define Uses_MsgBox
#define Uses_TFileList
#define Uses_TRect
#define Uses_TSearchRec
#define Uses_TEvent
#define Uses_TGroup
#define Uses_TStreamableClass
#define Uses_TKeys
#define Uses_TKeys_Extended
#define Uses_sys_stat

#if defined(TVCompf_djgpp)
 // DJGPP
 #define Uses_fnmatch
 #include <dir.h>
#elif defined(TVComp_BCPP) || defined(TVCompf_Cygwin) || defined(TVOS_UNIX) && !defined(TVOSf_QNX4)
 // UNIX, Win32/BC++, Win32/Cygwin
 #define Uses_glob
#endif

#include <tv.h>

#include <errno.h>
#if defined(TVComp_BCPP)
#include <dos.h>
#endif

TFileList::TFileList( const TRect& bounds,
          TScrollBar *aScrollBar) :
    TSortedListBox( bounds, 2, aScrollBar )
{
 if (CLY_HaveLFNs())
    setNumCols(1);
}

TFileList::~TFileList()
{
 if ( list() )
   CLY_destroy ( list() );
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

  if ((TGKey::getShiftState() & (kbLeftShiftDown | kbRightShiftDown)) ||
      *s=='.')
    sR.attr=FA_DIREC;
  else
    sR.attr=0;
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

void TFileList::handleEvent( TEvent & event )
{
  TSearchRec trec, *tp;

  TSortedListBox::handleEvent( event );
  if( event.what == evKeyDown )
      {
      if( event.keyDown.keyCode == kbLeft )
          {
          clearEvent( event );
          /* Move to .. */
          trec.attr = FA_DIREC;
          strcpy( trec.name, ".." );
          message( owner, evBroadcast, cmFileFocused, &trec );
          message( owner, evBroadcast, cmFileDoubleClicked, &trec );
          }
      else if( event.keyDown.keyCode == kbRight )
          {
          clearEvent( event );
          /* Enter dir */
          tp = list()->at(focused);
          if( tp->attr & FA_DIREC )
              message( owner, evBroadcast, cmFileDoubleClicked, tp );
          }
      }
}

void TFileList::readDirectory( const char *dir, const char *wildCard )
{
  char path[PATH_MAX];
  strcpy( path, dir );
  strcat( path, wildCard );
  readDirectory( path );
}

void TFileList::setState( ushort aState, Boolean enable )
{
  TSortedListBox::setState( aState, enable );
  if ( aState == sfFocused && enable )
     {
     message( owner, evBroadcast, cmFileFocused, list()->at(focused) );
     }
}

/******** struct DirSearchRec ********/
#ifdef TVCompf_djgpp
// DJGPP
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
#if defined(TVOS_Win32) && !defined(TVComp_BCPP) && !defined(TVCompf_Cygwin)
// MingW
struct DirSearchRec : public TSearchRec
{
	void readFf_blk(_finddata_t  &s)
	{
		attr = 0;
		if (s.attrib & _A_ARCH)
			attr = FA_ARCH;
		if (s.attrib & _A_SUBDIR)
			attr |= FA_DIREC;
		strcpy(name, s.name);
		size = s.size;
		time = s.time_write;
	}
};

#else
// Linux, BC++/Win32 and CygWin
struct DirSearchRec : public TSearchRec
{
  /* SS: changed */
  void readFf_blk(const char *filename, struct stat &s)
  {
    attr = FA_ARCH;
    if (S_ISDIR(s.st_mode)) attr |= FA_DIREC;
    strcpy(name, filename);
    size = s.st_size;
    time = s.st_mtime;
  }
};
#endif
#endif
/******** end of struct DirSearchRec ********/


// SET: Helper routine to exclude some special files
static
int ExcludeSpecialName(const char *name)
{
 int len=strlen(name);
 if ((TFileCollection::sortOptions & fcolHideEndTilde) && name[len-1]=='~')
    return 1;
 if ((TFileCollection::sortOptions & fcolHideEndBkp) && len>4 &&
     strcasecmp(name+len-4,".bkp")==0)
    return 1;
 if ((TFileCollection::sortOptions & fcolHideStartDot) && name[0]=='.')
    return 1;
 return 0;
}

/******** void readDirectory( const char *aWildCard ) ********/
#ifdef TVCompf_djgpp
// DOS+DJGPP target
// this is really faster than the glob methode
void TFileList::readDirectory( const char *aWildCard )
{
  DIR *dir;
  struct dirent *de;
  DirSearchRec *p;
  TFileCollection *fileList = new TFileCollection( 10, 10 );
  AllocLocalStr(wildcard,strlen(aWildCard)+1);
  strcpy(wildcard,aWildCard);
  char *slash = strrchr(wildcard,DIRSEPARATOR);
  char *path, pathAux[4];
  // SET: Added code to remove .. in the root directory
  int removeParent=0;
  
  if (slash)
    {
     *slash = 0;
     path = wildcard;
     if (strlen(path) == 2 && path[1] == ':')
       {
        path = pathAux;
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
  if (dir)
    {
     while ((de = readdir(dir)))
       {
        struct ffblk &ff = dir->ff;
        char *name=de->d_name;
        if (!(ff.ff_attrib & FA_DIREC) && fnmatch(slash,name,0)) continue;
        if (strcmp(name,".")==0) continue;
        if (removeParent && strcmp(name,"..")==0) continue;
        // SET: Special exclusions:
        if (ExcludeSpecialName(name)) continue;
        p = new DirSearchRec;
        strcpy(p->name,name);
        p->attr = ff.ff_attrib;
        p->size = ff.ff_fsize;
        p->time = ((unsigned long)(ff.ff_fdate)) << 16 | (unsigned short)ff.ff_ftime;
        p->time = _file_time_stamp(p->time);
        fileList->insert(p);
       }
     closedir(dir);
    }
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
#if defined(TVOS_Win32) && !defined(TVComp_BCPP) && !defined(TVCompf_Cygwin)
// Win32+MingW
void TFileList::readDirectory( const char *aWildCard )
{
  long dir;
  _finddata_t de;
  DirSearchRec *p;
  TFileCollection *fileList = new TFileCollection( 10, 10 );
  AllocLocalStr(wildcard,strlen(aWildCard)+1);
  strcpy(wildcard,aWildCard);
  char *slash = strrchr(wildcard,DIRSEPARATOR);
  char *path, pathAux[4];
  // SET: Added code to remove .. in the root directory
  int removeParent=0;
  char dirpath[PATH_MAX];
  
  if (slash)
    {
     *slash = 0;
     path = wildcard;
     if (strlen(path) == 2 && path[1] == ':')
       {
        path = pathAux;
        strcpy(path,wildcard);
        strcat(path,DIRSEPARATOR_);
       }
     strcpy(dirpath,path);
     strcat(dirpath,DIRSEPARATOR_"*");
     if (strlen(path) == 3 && path[1] == ':')
        removeParent=1;
     *slash = DIRSEPARATOR;
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
     strcpy(dirpath,path);
     strcat(dirpath,DIRSEPARATOR_"*");
    }
  //find all directories
  dir = _findfirst( dirpath, &de ) ;
  if (dir!=-1)
    {
     do
       {
        if (!(de.attrib & _A_SUBDIR)) continue;
        if (strcmp(de.name,".")==0) continue;
        if (removeParent && strcmp(de.name,"..")==0) continue;
        // SET: Special exclusions:
        if (ExcludeSpecialName(de.name)) continue;
        p = new DirSearchRec;
        p->readFf_blk( de );
        fileList->insert(p);
       }
     while (_findnext(dir,&de)==0);
     _findclose(dir);
    }

  //find all files
  dir = _findfirst( wildcard, &de ) ;
  if (dir!=-1)
    {
     do
       {
        if (de.attrib & _A_SUBDIR) continue;
        // SET: Special exclusions:
        if (ExcludeSpecialName(de.name)) continue;
        p = new DirSearchRec;
        p->readFf_blk( de );
        fileList->insert(p);
       }
     while (_findnext(dir,&de)==0);
     _findclose(dir);
    }

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

#if defined(TVOSf_QNX4)
void TFileList::readDirectory( const char *aWildCard )
{
   DIR *dp;
   DirSearchRec *p;
   char dir[PATH_MAX];
   char file[PATH_MAX];
   char path[PATH_MAX];
   dirent *de;
   struct stat s;

   strcpy( path, aWildCard );
   if (!CLY_IsWild(path))
      strcat(path, "*");
   CLY_fexpand( path );
   CLY_ExpandPath(path, dir, file);
   TFileCollection *fileList = new TFileCollection( 5, 5 );

   /* read regular archive files */

   sprintf(path, "%s.", dir);
   if ((dp = opendir(path)) != NULL)
   {
      while ((de = readdir(dp)) != NULL)
      {
         /* is it a regular file (not a directory) ? */
         sprintf(path, "%s%s", dir, de->d_name);
         if (stat(path, &s) == 0 && S_ISREG(s.st_mode))
         {
            if (ExcludeSpecialName(de->d_name))
               continue;
            if (fnmatch(file, de->d_name, 0)==0)
            {
               if ((p = new DirSearchRec) == NULL)
                  break;
               p->readFf_blk(de->d_name, s);
               fileList->insert( p );
            }
         }
      }
      closedir(dp);
   }

   /* read directories */

   sprintf(path, "%s.", dir);
   if ((dp = opendir(path)) != NULL)
   {
      while ((de = readdir(dp)) != NULL)
      {
         /* we don't want these directories */

         if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0)
            continue;

         /* is it a directory ? */
         sprintf(path, "%s%s", dir, de->d_name);
         if (stat(path, &s) == 0 && S_ISDIR(s.st_mode))
         {
            if ((p = new DirSearchRec) == NULL)
               break;
            p->readFf_blk(de->d_name, s);
            fileList->insert( p );
         }
      }
      closedir(dp);
   }

   if( strlen( dir ) > 1 )
   {
      p = new DirSearchRec;
      if( p != 0 )
      {
         sprintf(path, "%s..", dir);
         if (stat(path, &s) == 0)
            p->readFf_blk("..", s);
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

   newList(fileList);

   if( list()->getCount() > 0 )
      message( owner, evBroadcast, cmFileFocused, list()->at(0) );
   else
   {
      static DirSearchRec noFile;
      message( owner, evBroadcast, cmFileFocused, &noFile );
   }
}

#else

// Linux, BC++/Win32 and CygWin
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
  if (!CLY_IsWild(path)) strcat(path, "*");
  CLY_fexpand( path );
  CLY_ExpandPath(path, dir, file);
  TFileCollection *fileList = new TFileCollection( 10, 10 );

  /* find all filenames that match our wildcards */

  /*
   * The use of 'glob' function was proposed by:
   * Rainer Keuchel <r_keuchel@smaug.netwave.de>
   * Date: 18 Jan 1997 22:52:12 +0000
   */
  #ifdef TVOSf_Linux
  # define __gl_options GLOB_PERIOD
  #else
  # define __gl_options 0
  #endif
  if (glob(path, __gl_options, NULL, &gl) == 0)
    {
     for (int i = 0; i < (int)gl.gl_pathc; i++)
        {
         /* is this a regular file ? */
         if (stat(gl.gl_pathv[i], &s) == 0 && S_ISREG(s.st_mode))
           {
            /* strip directory part */
            if ((np = strrchr(gl.gl_pathv[i], '/')) != NULL)
               np++;
            else
               np = gl.gl_pathv[i];
            // SET: Special exclusions:
            if (ExcludeSpecialName(np)) continue;
            p = new DirSearchRec;
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
        if (stat(path, &s) == 0 && S_ISDIR(s.st_mode) &&
            // SET: Special exclusions:
            !ExcludeSpecialName(de->d_name))
          {
           p = new DirSearchRec;
           p->readFf_blk(de->d_name, s);
           fileList->insert( p );
          }
       }
     closedir(dp);
    }

  if ( strlen( dir ) > 1 )
    {
     p = new DirSearchRec;
     if (p)
       {
        sprintf(path, "%s..", dir);
        if (stat(path, &s)==0)
           p->readFf_blk("..", s);
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
  #if 0 // I think this will never hapen (new doesn't return 0)
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
#endif // TVOSf_QNX4
#endif // Linux, BC++/Win32 and CygWin
#endif // !DOS+DJGPP
/******** end of void readDirectory ********/

#if !defined( NO_STREAM )
TStreamable *TFileList::build()
{
    return new TFileList( streamableInit );
}
#endif // NO_STREAM

