/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *

Modified by Robert H”hne to be used for RHIDE.
Modified by Salvador Eduardo Tropea to add more functionality + i18n support.
Modified by Vadim Beloborodov to be used on WIN32 console
 *
 *
 */
#define Uses_string
#define Uses_ctype
#include <stdio.h>
#define Uses_HaveLFNs
#define Uses_TFileDialog
#define Uses_MsgBox
#define Uses_TRect
#define Uses_TFileInputLine
#define Uses_TButton
#define Uses_T1Label
#define Uses_TFileList
#define Uses_THistory
#define Uses_TScrollBar
#define Uses_TEvent
#define Uses_TFileInfoPane
#define Uses_opstream
#define Uses_ipstream
#define Uses_TStreamableClass
#include <tv.h>

#include <errno.h>

// File dialog flags
const int
    ffOpen        = 0x0001,
    ffSaveAs      = 0x0002;

const ushort
    cmOpenDialogOpen    = 100,
    cmOpenDialogReplace = 101;

TFileDialog::TFileDialog( const char *aWildCard,
                          const char *aTitle,
                          const char *inputName,
                          ushort aOptions,
                          uchar histId
                        ) :
    TWindowInit( &TFileDialog::initFrame ),
    TDialog( TRect( 15, 1, 64, 21 ), aTitle ),
    directory( 0 )
{ // SET: Enlarged the window 1 line and added 1 line to the list moving
  // labels 1 line up
    options |= ofCentered;
    // SET: Allow it to grow
    growMode = gfGrowAll;
    flags   |= wfGrow | wfZoom;
    strcpy( wildCard, aWildCard );

    fileName = new TFileInputLine( TRect( 3, 2, 31, 3 ), PATH_MAX );
    fileName->setDataFromStr( wildCard );
    fileName->growMode=gfGrowHiX;
    insert( fileName );

    insert( new T1Label( 2, 1, inputName, fileName ) );
    THistory *his=new THistory(TRect(31,2,34,3),fileName,histId);
    // SET: This and more settings to make it grow nicely
    his->growMode=gfGrowLoX | gfGrowHiX;
    insert(his);
    
    int longNames=CLY_HaveLFNs(); // SET
    TScrollBar *sb = longNames ?
                     new TScrollBar( TRect( 34, 5, 35, 16 ) ) :
                     new TScrollBar( TRect( 3, 15, 34, 16 ) );
    insert( sb );
    insert(fileList=new TFileList(TRect(3,5,34,longNames ? 16 : 15),sb));
    fileList->growMode=gfGrowHiX | gfGrowHiY;

    insert( new T1Label( 2, 4, __("~F~iles"), fileList ) );

    ushort opt = bfDefault;
    TRect r( 35, 2, 46, 4 );
    
    TButton *bt;
    #define AddButton(flag,name,command)\
    if (aOptions & flag) {\
        bt=new TButton(r,name,command,opt); \
        bt->growMode=gfGrowLoX | gfGrowHiX; \
        insert(bt); opt=bfNormal; r.a.y+=2; r.b.y+=2; }

    AddButton(fdOpenButton,__("~O~pen"),cmFileOpen)
    AddButton(fdOKButton,__("~O~K"),cmFileOpen)
    AddButton(fdAddButton,__("~A~dd"),cmFileOpen)
    AddButton(fdSelectButton,__("~S~elect"),cmFileSelect)
    AddButton(fdReplaceButton,__("~R~eplace"),cmFileReplace)
    AddButton(fdClearButton,__("~C~lear"),cmFileClear)

    bt=new TButton(r,aOptions & fdDoneButton ? __("Done") : __("Cancel"),
                   cmCancel,bfNormal);
    bt->growMode=gfGrowLoX | gfGrowHiX;
    insert(bt);
    r.a.y += 2;
    r.b.y += 2;

    if( (aOptions & fdHelpButton) != 0 )
        {
        bt=new TButton(r,__("~H~elp"),cmHelp,bfNormal);
        bt->growMode=gfGrowLoX | gfGrowHiX;
        insert(bt);
        r.a.y += 2;
        r.b.y += 2;
        }

    TFileInfoPane *fip=new TFileInfoPane(TRect(1,16,48,19));
    //fip->growMode=gfGrowHiX | gfGrowHiY;
    fip->growMode=gfGrowHiX | gfGrowHiY | gfGrowLoY;
    insert(fip);

    selectNext( False );
    if( (aOptions & fdNoLoadDir) == 0 )
        readDirectory();
    else
        setUpCurDir(); // SET: We must setup the current directory anyways
}

// SET: Avoid a size smaller than the starting one
void TFileDialog::sizeLimits(TPoint& min, TPoint& max)
{
 TDialog::sizeLimits(min,max);
 min.x=64-15;
 min.y=21-1;
}

TFileDialog::~TFileDialog()
{
    DeleteArray((char *)directory);
}

void TFileDialog::shutDown()
{
    fileName = 0;
    fileList = 0;
    TDialog::shutDown();
}

static void trim( char *dest, const char *src )
{
    const char *end = NULL;
    while( *src != EOS && ucisspace( *src ) )
        src++;
    {
/* when a filename contains spaces (not tested) */
      end = src + strlen(src) - 1;
      if (end < src) end = src;
      else
      {
        while (end > src && ucisspace(*end)) end--;
      }
    }
    while( *src != EOS && src <= end )
        *dest++ = *src++;
    *dest = EOS;
}

void TFileDialog::getFileName( char *s )
{
  char buf[PATH_MAX];
  const char *file=(const char *)fileName->getData();

  trim( buf, file );
  if ( CLY_IsRelativePath( buf ) )
  {
    strcpy( buf, directory );
    trim( buf + strlen(buf), file );
  }
  CLY_fexpand( buf );
  strcpy( s, buf );
}

void TFileDialog::handleEvent(TEvent& event)
{
    TDialog::handleEvent(event);
    if( event.what == evCommand )
        switch( event.message.command )
            {
            case cmFileOpen:
            case cmFileReplace:
            case cmFileClear:
            case cmFileSelect:
                {
                endModal(event.message.command);
                clearEvent(event);
                }
                break;
            default:
                break;
            }
    // SET: From TV 2.0
    else if( event.what == evBroadcast && event.message.command == cmFileDoubleClicked )
        {
        event.what = evCommand;
        event.message.command = cmOK;
        putEvent( event );
        clearEvent( event );
        }

}

void TFileDialog::readDirectory()
{
    fileList->readDirectory( wildCard );
    setUpCurDir();
}

void TFileDialog::setUpCurDir()
{
    DeleteArray((char *)directory);
    char curDir[PATH_MAX];
    CLY_GetCurDirSlash(curDir);
    directory = newStr( curDir );
}

void TFileDialog::setData( void *rec )
{
    TDialog::setData( rec );
    if( *(char *)rec != EOS && CLY_IsWild( (char *)rec ) )
        {
        valid( cmFileInit );
        fileName->select();
        }
}

void TFileDialog::getData( void *rec )
{
    getFileName( (char *)rec );
}

Boolean TFileDialog::checkDirectory( const char *str )
{
    if( CLY_PathValid( str ) )
        return True;
    else
        {
        messageBox( __("Invalid drive or directory"), mfError | mfOKButton );
        fileName->select();
        return False;
        }
}

Boolean TFileDialog::valid(ushort command)
{
    char fName[PATH_MAX];
    char name[PATH_MAX];
    char dir[PATH_MAX];

    if (!TDialog::valid(command))
        return False;

    if ((command == cmValid) || (command == cmCancel))
        return True;

    getFileName( fName );
    if (command != cmFileClear)
    {
        if(CLY_IsWild(fName))
        {
            CLY_ExpandPath(fName, dir, name);
            if (checkDirectory(dir))
            {
                DeleteArray((char *)directory);
                directory = newStr(dir);
                strcpy(wildCard, name);
                if (command != cmFileInit)
                    fileList->select();
                fileList->readDirectory(directory, wildCard);
            }
        }
        else if (CLY_IsDir(fName))
        {
            if (checkDirectory(fName))
            {
                delete (char *)directory;
                strcat(fName, DIRSEPARATOR_);
                directory = newStr(fName);
                if (command != cmFileInit)
                    fileList->select();
                fileList->readDirectory(directory, wildCard);
            }
        }
        else if (CLY_ValidFileName(fName))
            return True;
        else
        {
            messageBox( __("Invalid file name."), mfError | mfOKButton );
            return False;
        }
    }
    else
       return True;
    return False; // To shut up GCC's warning
}

#if !defined( NO_STREAM )
void TFileDialog::write( opstream& os )
{
    TDialog::write( os );
    os.writeString( wildCard );
    os << fileName << fileList;
}

void *TFileDialog::read( ipstream& is )
{
    TDialog::read( is );
    is.readString( wildCard, sizeof(wildCard) );
    is >> fileName >> fileList;
    readDirectory();
    return this;
}

TStreamable *TFileDialog::build()
{
    return new TFileDialog( streamableInit );
}
#endif // NO_STREAM

