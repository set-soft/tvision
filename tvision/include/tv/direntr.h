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

#if defined( Uses_TDirEntry ) && !defined( __TDirEntry )
#define __TDirEntry

class TDirEntry
{

public:

    TDirEntry( const char *txt, const char *dir, int anOffset=0 );
    ~TDirEntry();
    char *dir() { return directory; }
    char *text() { return displayText; }
    int   offset() { return nameOffset; }

private:

    char *displayText;
    char *directory;
    int   nameOffset;

};

inline TDirEntry::TDirEntry( const char *txt, const char *dir,
                             int anOffset ) :
    displayText( newStr( txt ) ),
    directory( newStr( dir ) ),
    nameOffset( anOffset )
{
}

inline TDirEntry::~TDirEntry()
{
    delete[] displayText;
    delete[] directory;
}

#endif  // Uses_TDirEntry

