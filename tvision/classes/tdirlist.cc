/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *

Modified by Robert H”hne to be used for RHIDE.
Modified by Vadim Beloborodov to be used on WIN32 console
Modified by Salvador E. Tropea. Changed to sort directory names. Added
incremental directory search.
 *
 *
 */
// SET: Moved the standard headers here because according to DJ
// they can inconditionally declare symbols like NULL
#include <tv/configtv.h>
#define Uses_string
#define Uses_sys_stat
#define Uses_TDirListBox
#define Uses_TEvent
#define Uses_TDirCollection
#define Uses_TChDirDialog
#define Uses_TDirEntry
#define Uses_TButton
#define Uses_TStreamableClass
#define Uses_TStringCollection
#define Uses_TKeys
#define Uses_TVCodePage
#if defined(TVCompf_djgpp) || defined(TVComp_BCPP)
 #define Uses_dir // getdisk()
#endif
#include <tv.h>

TDirListBox::TDirListBox( const TRect& bounds, TScrollBar *aScrollBar ) :
    TListBox( bounds, 1, aScrollBar ),
    cur( 0 )
{
    *dir = EOS;
    incPos = 0;
    lastFocusedInSearch = -1;
}

TDirListBox::~TDirListBox()
{ 
    if( list() )
        CLY_destroy( list() );
}

void TDirListBox::getText( char *text, ccIndex item, short maxChars )
{
    strncpy( text, list()->at(item)->text(), maxChars );
    text[maxChars] = '\0';
}

void TDirListBox::handleEvent( TEvent& event )
{
    if( event.what == evMouseDown && event.mouse.doubleClick )
        {
        event.what = evCommand;
        event.message.command = cmChangeDir;
        putEvent( event );
        clearEvent( event );
        }
    else if( event.what == evKeyDown  )
        {// SET: Incremental directory search
        if( event.keyDown.keyCode != kbEnter &&
            ( event.keyDown.charScan.charCode != 0 ||
              event.keyDown.keyCode == kbBack ) )
           {
           if( lastFocusedInSearch != focused )
              incPos = 0;
           if( event.keyDown.keyCode == kbBack )
              {
              if( incPos > 0 )
                  incPos--;
              }
           else
              incremental[incPos++] = event.keyDown.charScan.charCode;
           incremental[incPos] = 0;
           TDirCollection *dirs = list();
           ccIndex c = dirs->getCount();
           ccIndex newFocus = -1;
           if( focused < c && cur < c )
              {
              int offset = strlen( dirs->at( cur )->dir() ) + 1;
              if( focused > cur )
                 // From focused
                 for( int i = focused; i < c; i++ )
                     {
                     if( strncasecmp( dirs->at( i )->dir() + offset,
                         incremental, incPos ) == 0 )
                        {
                        newFocus = i;
                        break;                        
                        }
                     }
              // From beginning
              for( int i = cur + 1; newFocus < 0 && i < c; i++ )
                  {
                  if( strncasecmp( dirs->at( i )->dir() + offset,
                      incremental, incPos ) == 0 )
                     newFocus = i;
                  }
              if( newFocus < 0 )
                 {
                 if( event.keyDown.keyCode != kbBack )
                    incPos--;
                 if( TVCodePage::isAlNum( event.keyDown.charScan.charCode ) )
                    {
                    clearEvent( event );
                    return;
                    }
                 }
              else
                 {
                 focusItem( newFocus );
                 lastFocusedInSearch = newFocus;
                 clearEvent( event );
                 return;
                 }
              }
           }
        TListBox::handleEvent( event );
        }
    else
        TListBox::handleEvent( event );
}

Boolean TDirListBox::isSelected( ccIndex item )
{
    return Boolean( item == cur );
}

#ifndef CLY_HaveDriveLetters
// Mostly POSIX systems where drive letter doesn't exist

void TDirListBox::showDrives( TDirCollection * )
{
}

#else
// Mostly DOS and Win32

#if defined(TVCompf_djgpp) || defined(TVComp_BCPP)
#include <dos.h>
#else
#include <io.h>
#include <direct.h> //_getdrive()
#define getdisk() (_getdrive()-1)
#endif

void TDirListBox::showDrives( TDirCollection *dirs )
{
    Boolean isFirst = True;
    char oldc[5];
    strcpy( oldc, "0:"DIRSEPARATOR_ );
    for( char c = 'a'; c <= 'z'; c++ )
        {
        if( c < 'c' || driveValid( c ) )
            {
            if( oldc[0] != '0' )
                {
                char s[ 16 ];
                if( isFirst )
                    {
                    strcpy( s, firstDir );
                    s[ strlen(firstDir) ] = oldc[0];
                    s[ strlen(firstDir)+1 ] = EOS;
                    isFirst = False;
                    }
                else
                    {
                    strcpy( s, middleDir );
                    s[ strlen(middleDir) ] = oldc[0];
                    s[ strlen(middleDir)+1 ] = EOS;
                    }
                dirs->insert( new TDirEntry( s, oldc ) );
                }
            if( c == getdisk() + 'a' )
                cur = dirs->getCount();
            oldc[0] = c;
            }
        }
    if( oldc[0] != '0' )
        {
        char s[ 16 ];
        strcpy( s, lastDir );
        s[ strlen(lastDir) ] = oldc[0];
        s[ strlen(lastDir)+1 ] = EOS;
        dirs->insert( new TDirEntry( s, oldc ) );
        }
}
#endif // CLY_HaveDriveLetters


/*****************************************************************************
  char *SkipDriveName( const char *dir )
  This is a helper to skip the characters used by the drive letter in the
specified dir string.
  Added by SET to make the code maintainable.
*****************************************************************************/

#ifndef CLY_HaveDriveLetters
// Mostly POSIX systems where drive letter doesn't exist

// SET: Here we have to "skip" the drive name
#if !defined(TVOSf_QNX4)
static
char *SkipDriveName( char *dir )
{
    return dir + 1;
}
#else
// Special case "//nodenumber/"
static
char *SkipDriveName( char *dir )
{
    char *end;

    if( (*dir == '/') && (dir[1] == '/') )
       {
       end = strchr( dir+2, DIRSEPARATOR );
       if( !end )
          end = dir + 1; // fallback to usual UNIX path.
       else
          end += 1; // split the //nodenumber/ string.
       }
    else
       end = dir + 1; // usual UNIX path.
    return end;
}
#endif // TVOSf_QNX4

#else
// Mostly DOS and Win32

static
char *SkipDriveName( char *dir )
{
    return dir + 3;
}
#endif // CLY_HaveDriveLetters

/*****************************************************************************
  End of char *SkipDriveName( const char *dir )
*****************************************************************************/

/*****************************************************************************
  TStringCollection *ListDirectory( char *path, char *end )
  This is a helper to collect the directories found in path using "end" as
base to construct a mask.
  Added by SET to make the code maintainable.
*****************************************************************************/

#if defined(TVOS_UNIX) || defined(TVCompf_Cygwin)
// POSIX solution using opendir and stat
#include <dirent.h>

static
TStringCollection *ListDirectory( char *path, char *end )
{
    *end = 0;
    TStringCollection *col = new TStringCollection( 10, 10 );

    DIR *d = opendir( path );
    if( d )
       {
       struct dirent *ent;
       while( (ent = readdir(d)) != 0 )
          {
          struct stat st;
          strcpy( end, ent->d_name );
          stat( path, &st );
          if( S_ISDIR(st.st_mode) && strcmp(ent->d_name,".")!=0 &&
              strcmp(ent->d_name,"..")!=0 )
              col->insert( newStr( ent->d_name ) );
          }
       closedir( d );
       }
    return col;
}

#else
#if !defined(TVCompf_djgpp) && !defined(TVComp_BCPP)
// MSVC Win32 solution. It uses _findfirst

static
TStringCollection *ListDirectory( char *path, char *end )
{
    strcpy( end, "*" );
    TStringCollection *col=new TStringCollection( 10, 10 );

    _finddata_t ff;
    long res = _findfirst( path, &ff );
    if( res != -1 )
       {
       do
         {
         if( (ff.attrib & _A_SUBDIR) != 0 && ff.name[0] != '.' )
            col->insert( newStr( ff.name ) );
         }
       while( _findnext( res, &ff )==0 );
       _findclose( res );
       }
}

#else // DJGPP and BC++
// DJGPP and BC++ solution. It uses findfirst

#if 0
extern "C" unsigned short ffattrib(struct ffblk *);
extern "C" char *ffname(struct ffblk *);
#define A(s) ffattrib(&s)
#define N(s) ffname(&s)
#else
#define A(s) s.ff_attrib
#define N(s) s.ff_name
#endif

static
TStringCollection *ListDirectory( char *path, char *end )
{
    strcpy( end, "*" );
    TStringCollection *col=new TStringCollection(10,10);

    ffblk ff;
    int res = findfirst( path, &ff, FA_DIREC );
    while( res == 0 )
        {
        if( (A(ff) & FA_DIREC) != 0 && N(ff)[0] != '.' )
            col->insert( newStr( N(ff) ) );
        res = findnext( &ff );
        }
    return col;
}

#undef A
#undef N
#endif
#endif // DJGPP

/*****************************************************************************
  End of TStringCollection *ListDirectory( char *path, char *end )
*****************************************************************************/

void TDirListBox::showDirs( TDirCollection *dirs )
{
    const int indentSize = 2;
    int indent = indentSize;

    char buf[PATH_MAX*2];
    memset( buf, ' ', sizeof( buf ) );
    char *name = buf + PATH_MAX;

    // The first ramification of the tree
    char *org = name - strlen(pathDir);
    strcpy( org, pathDir );

    char *curDir = dir;
    char *end = SkipDriveName( dir );
    char hold = *end;
    *end = EOS;         // mark end of drive name
    strcpy( name, curDir );
    dirs->insert( new TDirEntry( org, name ) );

    *end = hold;        // restore full path
    curDir = end;
    while( (end = strchr( curDir, DIRSEPARATOR )) != 0 )
        {
        *end = EOS;
        strncpy( name, curDir, size_t(end-curDir) );
        name[size_t(end-curDir)] = EOS;
        dirs->insert( new TDirEntry( org - indent, dir ) );
        *end = DIRSEPARATOR;
        curDir = end+1;
        indent += indentSize;
        }

    cur = dirs->getCount() - 1;

    end = strrchr( dir, DIRSEPARATOR );
    char path[PATH_MAX];
    strncpy( path, dir, size_t(end-dir+1) );
    end = path + unsigned(end-dir)+1;

    // SET: Insert them sorted
    // SET: 1) collect them
    TStringCollection *col = ListDirectory( path, end );

    // SET: 2) insert sorted
    Boolean isFirst = True;
    int cnt = col->getCount();
    for( int j = 0; j < cnt; j++ )
        {
        char *s = (char *) col->at(j);
        if( isFirst )
           {
           memcpy( org, firstDir, strlen(firstDir)+1 );
           isFirst = False;
           }
        else
           memcpy( org, middleDir, strlen(middleDir)+1 );
        strcpy( name, s );
        strcpy( end, s );
        dirs->insert( new TDirEntry( org - indent, path ) );
        }

    CLY_destroy(col);

    char *p = dirs->at(dirs->getCount()-1)->text();
    char *i = strchr( p, graphics[0] );
    if( i == 0 )
        {
        i = strchr( p, graphics[1] );
        if( i != 0 )
            *i = graphics[0];
        }
    else
        {
        *(i+1) = graphics[2];
        *(i+2) = graphics[2];
        }
}

void TDirListBox::newDirectory( const char *str )
{
    strcpy( dir, str );
    TDirCollection *dirs = new TDirCollection( 5, 5 );
    #ifdef CLY_HaveDriveLetters
    #if defined(TVCompf_djgpp) || defined(TVComp_BCPP)
    // SET: Old programs created for original TV can pass backslashes here
    // and the code in showDirs assumes they are all forward.
    for (int i=0; dir[i]; i++)
       {
        if (dir[i]=='\\')
           dir[i]='/';
       }
    #endif
    const char *drives = _("Drives");
    dirs->insert( new TDirEntry( drives, drives ) );
    if( strcmp( dir, drives ) == 0 )
        showDrives( dirs );
    else
    #endif // CLY_HaveDriveLetters
        showDirs( dirs );
    newList( dirs );
    focusItem( cur );
}

void TDirListBox::setState( ushort nState, Boolean enable )
{
    TListBox::setState( nState, enable );
    if( (nState & sfFocused) != 0 )
        message(owner, evCommand, cmDirSelection, (void *)enable);
}

#if !defined( NO_STREAM )
TStreamable *TDirListBox::build()
{
    return new TDirListBox( streamableInit );
}
#endif // NO_STREAM


