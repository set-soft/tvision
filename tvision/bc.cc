/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#include <unistd.h>
#ifdef __DJGPP__
#include <dpmi.h>
#include <dir.h>
#include <fcntl.h> // _use_lfn
#endif
#include <string.h>
#include <limits.h>
#include <ctype.h>
#include <sys/stat.h>

#include <tv.h>

unsigned short win_ver()
{
#ifdef __DJGPP__
  __dpmi_regs r;
  r.x.ax = 0x160a;
  __dpmi_int(0x2f,&r);
  if (r.x.ax == 0) return r.x.bx;
#endif
  return 0;
}

#ifdef __DJGPP__
int getcurdir(int drive, char *buffer)
{
  if (!drive) getwd(buffer);
  else
  {
    int d = getdisk();
    setdisk(drive-1);
    getwd(buffer);
    setdisk(d);
  }
  strcpy(buffer,buffer+3);
  return 0;
}
#else
int getcurdir(int , char *buffer)
{
  getcwd(buffer, PATH_MAX);
  return 0;
}
#endif

#if !defined( DJGPP ) || ( DJGPP < 2 )

extern "C" long __filelength(int fhandle)
{
  long oldval;
  long retval;
  oldval = lseek(fhandle,0,1);
  if (oldval == -1L) return -1L;
  retval = lseek(fhandle,0,2);
  if (retval == -1L) return -1L;
  if (lseek(fhandle,oldval,0) == -1L) return -1L;
  return retval;
}

#endif

/*
 * Expands a path into its directory and file components.
 */

void expandPath(const char *path, char *dir, char *file)
{
  char *tag = strrchr(path, '/');

  if (tag != NULL)
  {
    if (file)
      strcpy(file, tag + 1);
    if (dir)
    {
      strncpy(dir, path, tag - path + 1);
      dir[tag - path + 1] = '\0';
    }
  }
  else
  {
    /* there is only the file name */
    if (file)
      strcpy(file, path);
    if (dir)
      dir[0] = '\0';
  }
}

#ifdef __DJGPP__
#include <sys/stat.h> // for _fixpath
#endif

void fexpand( char *rpath )
{
  char path[PATH_MAX];
  _fixpath(rpath,path);
  strcpy(rpath,path);
}

Boolean driveValid( char drive )
{
#ifdef __DJGPP__
  __dpmi_regs r;
  r.x.ax = 0x1C00;
  r.x.dx = uctoupper(drive) - 'A' + 1;
  __dpmi_int(0x21,&r);
  if (r.h.al != 0xFF) return True;
#endif
  return False;
}

Boolean isDir( const char *str )
{
  struct stat s;
  
  if (stat(str, &s) == 0 && S_ISDIR(s.st_mode)) return True;
  return False;
}

Boolean pathValid( const char *path )
{
  char dir[PATH_MAX];
  char name[PATH_MAX];
  
  expandPath(path, dir, name);
#ifdef __DJGPP__
  if (strlen(dir) <= 3)
    return driveValid(dir[0]);
#endif
  if (*dir && dir[strlen(dir)-1] == '/') strcat(dir, ".");
  else strcat(dir, "/.");
#ifdef __DJGPP__
  if (driveValid(dir[0]) == False)
    return False;
#endif
  return isDir(dir);
}

Boolean validFileName( const char *fileName )
{
#ifdef __DJGPP__
  static const char * const illegalChars1 = ";,=+<>|\"[]/";
  static const char * const illegalChars2 = "<>|/\"";
  static const char * illegalChars;
  // SET: Added a check for LFNs, in LFN drives ;,[]=+ are valid
  if (_use_lfn(fileName))
     illegalChars=illegalChars2;
  else
     illegalChars=illegalChars1;
#else
  static const char * const illegalChars = "<>|/\"";
#endif
  const char *slash = strrchr(fileName,'/');
  if (!slash)
  {
    if (strpbrk(fileName,illegalChars) != 0)
      return False;
    return True;
  }
  if (strpbrk(slash+1,illegalChars) != 0)
    return False;
  return pathValid(fileName);
}

void getCurDir( char *dir )
{
  getcwd(dir, PATH_MAX);
  if (dir[strlen(dir)-1] != '/')
    strcat(dir,"/");
}

Boolean isWild( const char *f )
{
    return Boolean( strpbrk( f, "?*" ) != 0 );
}

#ifndef __DJGPP__

/* 
   Here now the very usefull function _fixpath() from DJGPP's
   libc 'fixpath.c'
   I have modified it to be used on unix systems (like linux).
*/

/* Copyright (C) 1996 DJ Delorie, see COPYING.DJ for details */
/* Copyright (C) 1995 DJ Delorie, see COPYING.DJ for details */

__inline__ static int
is_slash(int c)
{
  return c == '/';
}

__inline__ static int
is_term(int c)
{
  return c == '/' || c == '\0';
}

/* Takes as input an arbitrary path.  Fixes up the path by:
   1. Removing consecutive slashes
   2. Removing trailing slashes
   3. Making the path absolute if it wasn't already
   4. Removing "." in the path
   5. Removing ".." entries in the path (and the directory above them)
 */
void
_fixpath(const char *in, char *out)
{
  const char	*ip = in;
  char		*op = out;

  /* Convert relative path to absolute */
  if (!is_slash(*ip))
  {
    getcurdir(0,op);
    op += strlen(op);
  }

  /* Step through the input path */
  while (*ip)
  {
    /* Skip input slashes */
    if (is_slash(*ip))
    {
      ip++;
      continue;
    }

    /* Skip "." and output nothing */
    if (*ip == '.' && is_term(*(ip + 1)))
    {
      ip++;
      continue;
    }

    /* Skip ".." and remove previous output directory */
    if (*ip == '.' && *(ip + 1) == '.' && is_term(*(ip + 2)))
    {
      ip += 2;
      /* Don't back up over root '/' */
      if (op > out )
      /* This requires "/" to follow drive spec */
	while (!is_slash(*--op));
      continue;
    }

    /* Copy path component from in to out */
    *op++ = '/';
    while (!is_term(*ip)) *op++ = *ip++;
  }

  /* If root directory, insert trailing slash */
  if (op == out) *op++ = '/';

  /* Null terminate the output */
  *op = '\0';

}

/*
  End of modified code from DJGPP's libc 'fixpath.c'
*/

int __file_exists(const char *fname)
{
  return (access(fname,R_OK) == 0);
}

#endif // __DJGPP__
