/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *

Modified by Robert H”hne to be used for RHIDE.
Modified by Vadim Beloborodov to be used on WIN32 console
Modified by Salvador E. Tropea: added i18n support. Horizontal scroll bar.

 *
 *
 */
// SET: Moved the standard headers here because according to DJ
// they can inconditionally declare symbols like NULL
#define Uses_ctype
#define Uses_stdio
#define Uses_string
#define Uses_unistd
#define Uses_chdir

#define Uses_MsgBox
#define Uses_TChDirDialog
#define Uses_TRect
#define Uses_TInputLine
#define Uses_T1Label
#define Uses_THistory
#define Uses_TScrollBar
#define Uses_TDirListBox
#define Uses_TButton
#define Uses_TEvent
#define Uses_TDirEntry
#define Uses_TDirCollection
#define Uses_TChDirDialog
#define Uses_opstream
#define Uses_ipstream
#define Uses_TStreamableClass
#include <tv.h>

TChDirDialog::TChDirDialog( ushort opts, ushort histId ) :
    TWindowInit( &TChDirDialog::initFrame )
    , TDialog( TRect( 16, 2, 64, 21 ), __("Change Directory") )
{
    options |= ofCentered;

    dirInput = new TInputLine( TRect( 3, 3, 30, 4 ), FILENAME_MAX );
    insert( dirInput );
    insert( new T1Label( 2, 2, __("Directory ~n~ame"), dirInput ));
    insert( new THistory( TRect( 30, 3, 33, 4 ), dirInput, histId ) );

    TScrollBar *sbv = new TScrollBar( TRect( 32, 6, 33, 16 ) );
    insert( sbv );
    // SET: Looks like original authors never had deep directory structures ...
    // So I added a horizontal scroll bar.
    TScrollBar *sbh = new TScrollBar( TRect( 3, 16, 32, 17 ) );
    sbh->setRange( 0, PATH_MAX );
    sbh->setStep( 28, 1 );
    insert( sbh );
    dirList = new TDirListBox( TRect( 3, 6, 32, 16 ), sbv, sbh );
    insert( dirList );
    insert( new T1Label( 2, 5, __("Directory ~t~ree"), dirList ));

    okButton = new TButton( TRect( 35, 6, 45, 8 ), __("~O~K"), cmOK, bfDefault );
    insert( okButton );
    chDirButton = new TButton( TRect( 35, 9, 45, 11 ), __("~C~hdir"), cmChangeDir, bfNormal );
    insert( chDirButton );
    insert( new TButton( TRect( 35, 12, 45, 14 ), __("~R~evert"), cmRevert, bfNormal ) );
    if( (opts & cdHelpButton) != 0 )
        insert( new TButton( TRect( 35, 15, 45, 17 ), __("Help"), cmHelp, bfNormal ) );
    if( (opts & cdNoLoadDir) == 0 )
        setUpDialog();
    selectNext( False );
}

static inline
int changeDir( const char *path )
{
#if 0 // DJGPP can handle chdir also over drives
    if( path[1] == ':' )
        setdisk( uctoupper(path[0]) - 'A' );
#endif
    return chdir( path );
}

uint32 TChDirDialog::dataSize()
{
    return 0;
}

void TChDirDialog::shutDown()
{
    dirList = 0;
    dirInput = 0;
    okButton = 0;
    chDirButton = 0;
    TDialog::shutDown();
}

void TChDirDialog::getData( void * )
{
}

void TChDirDialog::handleEvent( TEvent& event )
{
    TDialog::handleEvent( event );
    switch( event.what )
        {
        case evCommand:
            {
            char curDir[PATH_MAX];
            switch( event.message.command )
                {
                case cmRevert:
                    CLY_GetCurDirSlash(curDir);
                    break;
                case cmChangeDir:
                    {
                    TDirEntry *p = dirList->list()->at( dirList->focused );
                    strcpy( curDir, p->dir() );
                    #ifdef CLY_HaveDriveLetters
                    if( strcmp( curDir, _("Drives") ) == 0 )
                        // Go and get the drive names
                        break;
                    if( !driveValid( curDir[0] ) )
                        // Invalid drive, don't change
                        return;
                    #endif
                    // Ensure it have a DIRSEPARATOR at the end
                    if( curDir[strlen(curDir)-1] != DIRSEPARATOR )
                        strcat( curDir, DIRSEPARATOR_ );
                    // Go and get the directories
                    break;
                    }
                case cmDirSelection:
                    chDirButton->makeDefault((Boolean)(event.message.infoPtr!=NULL));
                    return; // Do not use break here ! 
                default:
                    return;
                }
            dirList->newDirectory( curDir );
            #if CLY_HaveDriveLetters
            // Let the last dirsep only if it is X:/
            int len = strlen( curDir );
            if( len > 3 && curDir[len-1] == DIRSEPARATOR )
                curDir[len-1] = EOS;
            #endif
            dirInput->setDataFromStr(curDir);
            dirInput->drawView();
            dirList->select();
            clearEvent( event );
            }
        default:
            break;
        }
}

void TChDirDialog::setData( void * )
{
}

void TChDirDialog::setUpDialog()
{
    if( dirList != 0 )
        {
        char curDir[PATH_MAX];
        CLY_GetCurDirSlash( curDir );
        dirList->newDirectory( curDir );
        if( dirInput != 0 )
            {
            #if CLY_HaveDriveLetters
            int len = strlen( curDir );
            if( len > 3 && curDir[len-1] == DIRSEPARATOR )
                curDir[len-1] = EOS;
            #endif
            dirInput->setDataFromStr(curDir);
            dirInput->drawView();
            }
        }
}

Boolean TChDirDialog::valid( ushort command )
{
  if ( command != cmOK )
    return True;
  if( changeDir( (const char *)dirInput->getData() ) != 0 )
  {
    messageBox( __("Invalid directory"), mfError | mfOKButton );
    return False;
  }
  return True;
}

#if !defined( NO_STREAM )

void TChDirDialog::write( opstream& os )
{
    TDialog::write( os );
    os << dirList << dirInput << okButton << chDirButton;
}

void *TChDirDialog::read( ipstream& is )
{
    TDialog::read( is );
    is >> dirList >> dirInput >> okButton >> chDirButton;
    setUpDialog();
    return this;
}

TStreamable *TChDirDialog::build()
{
    return new TChDirDialog( streamableInit );
}

#endif // NO_STREAM

