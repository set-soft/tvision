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
// SET: Moved the standard headers here because according to DJ
// they can inconditionally declare symbols like NULL
#include <string.h>

#define Uses_TDirListBox
#define Uses_TEvent
#define Uses_TDirCollection
#define Uses_TChDirDialog
#define Uses_TDirEntry
#define Uses_TButton
#define Uses_TStreamableClass
#include <tv.h>

TDirListBox::TDirListBox( const TRect& bounds, TScrollBar *aScrollBar ) :
    TListBox( bounds, 1, aScrollBar ),
    cur( 0 )
{
    *dir = EOS;
}

TDirListBox::~TDirListBox()
{ 
   if ( list() )
      destroy( list() );
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
    else
        TListBox::handleEvent( event );
}

Boolean TDirListBox::isSelected( ccIndex item )
{
    return Boolean( item == cur );
}

#ifndef __DJGPP__

void TDirListBox::showDrives( TDirCollection * )
{
}

#else

#include <dir.h> // getdisk()
#include <dos.h>

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
#endif // __DJGPP__

#if 0
extern "C" unsigned short ffattrib(struct ffblk *);
extern "C" char *ffname(struct ffblk *);
#define A(s) ffattrib(&s)
#define N(s) ffname(&s)
#else
#define A(s) s.ff_attrib
#define N(s) s.ff_name
#endif

#ifndef __DJGPP__
#include <dirent.h>
#include <sys/stat.h>

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
    char *end = dir + 1;
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
    *end = 0;
    
    Boolean isFirst = True;
    DIR *d=opendir(path);
    if (d)
      {
       struct dirent *ent;
       while( (ent=readdir(d))!=0 )
           {
           struct stat st;
           strcpy(end,ent->d_name);
           stat(path,&st);
           if( S_ISDIR(st.st_mode) && strcmp(ent->d_name,".")!=0 &&
               strcmp(ent->d_name,"..")!=0)
               {
               if( isFirst )
                   {
                   memcpy( org, firstDir, strlen(firstDir)+1 );
                   isFirst = False;
                   }
               else
                   memcpy( org, middleDir, strlen(middleDir)+1 );
               strcpy( name, ent->d_name );
               strcpy( end, ent->d_name );
               dirs->insert( new TDirEntry( org - indent, path ) );
               }
           }
       closedir(d);
      }

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

#else

void TDirListBox::showDirs( TDirCollection *dirs )
{
    const int indentSize = 2;
    int indent = indentSize;

    char buf[PATH_MAX*2];
    memset( buf, ' ', sizeof( buf ) );
    char *name = buf + PATH_MAX;

    char *org = name - strlen(pathDir);
    strcpy( org, pathDir );

    char *curDir = dir;
    char *end = dir + 3;
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
    strcpy( end, "*" );

    Boolean isFirst = True;
    ffblk ff;
    int res = findfirst( path, &ff, FA_DIREC );
    while( res == 0 )
        {
        if( (A(ff) & FA_DIREC) != 0 && N(ff)[0] != '.' )
            {
            if( isFirst )
                {
                memcpy( org, firstDir, strlen(firstDir)+1 );
                isFirst = False;
                }
            else
                memcpy( org, middleDir, strlen(middleDir)+1 );
            strcpy( name, N(ff) );
            strcpy( end, N(ff) );
            dirs->insert( new TDirEntry( org - indent, path ) );
            }
        res = findnext( &ff );
        }

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

#endif // __DJGPP__

void TDirListBox::newDirectory( const char *str )
{
    strcpy( dir, str );
    TDirCollection *dirs = new TDirCollection( 5, 5 );
#ifdef __DJGPP__
    char *drives = _("Drives");
    dirs->insert( new TDirEntry( drives, drives ) );
    if( strcmp( dir, drives ) == 0 )
        showDrives( dirs );
    else
#endif // __DJGPP__
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


