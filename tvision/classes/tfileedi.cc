/* Modified by Robert Hoehne and Salvador Eduardo Tropea for the gcc port */
/*----------------------------------------------------------*/
/*                                                          */
/*   Turbo Vision 1.0                                       */
/*   Copyright (c) 1991 by Borland International            */
/*                                                          */
/*----------------------------------------------------------*/
#include <limits.h>
#include <string.h>
#include <fstream.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

#define Uses_TGroup
#define Uses_TEditor
#define Uses_TFileEditor
#define Uses_TEvent
#define Uses_opstream
#define Uses_ipstream
#define Uses_TStreamableClass
#include <tv.h>

inline uint32 min( uint32 u1, uint32 u2 )
{
    return u1 < u2 ? u1 : u2;
}

TFileEditor::TFileEditor( const TRect& bounds,
                          TScrollBar *aHScrollBar,
                          TScrollBar *aVScrollBar,
                          TIndicator *aIndicator,
                          const char *aFileName
                        ) :
    TEditor( bounds, aHScrollBar, aVScrollBar, aIndicator, 4096 )
{
    if( aFileName == 0 )
        fileName[0] = EOS;
    else
        {
        strcpy( fileName, aFileName );
        if( isValid )
            isValid = loadFile();
        }
}

void TFileEditor::doneBuffer()
{
    delete buffer;
}

void TFileEditor::handleEvent( TEvent& event )
{
    TEditor::handleEvent(event);
    switch( event.what )
        {
        case evCommand:
            switch( event.message.command )
                {
                case cmSave:
                    save();
                    break;
                case cmSaveAs:
                    saveAs();
                    break;
                default:
                    return;
                }
            break;
        default:
            return;
        }
    clearEvent(event);
}

void TFileEditor::initBuffer()
{
    buffer = new char[bufSize];
}

Boolean TFileEditor::loadFile()
{
    int crfound = 0,i;
    char tmp[PATH_MAX];
    ifstream f( fileName, ios::in | ios::bin );
    if( !f )
        {
        setBufLen( 0 );
        return True;
        }
    else
        {
/* check for a unix text file (is a heuristic, because only 1024 chars checked */
        {
          char tmpbuf[1024];
          long fsize = filelength( f.rdbuf()->fd() );
          if (fsize > 1024) fsize = 1024;
          f.read( tmpbuf, fsize);
          for (i=0;i<1024;i++)
          {
            if (tmpbuf[i] == 13) crfound = 1;
            else if (tmpbuf[i] == 10) break;
          }
          if (crfound)
          {
            f.seekg(0);
          }
          else
          {
            int readhandle,writehandle;
            f.close();
            readhandle = open(fileName,O_RDONLY|O_BINARY);
            tmpnam(tmp);
            writehandle = open(tmp,O_WRONLY|O_CREAT|O_TRUNC|O_TEXT);
            while ((fsize = ::read(readhandle,tmpbuf,1024)) > 0)
              ::write(writehandle,tmpbuf,fsize);
            close(readhandle);
            close(writehandle);
            f.open(tmp,ios::in | ios::bin);
          }
        }
        long fSize = filelength( f.rdbuf()->fd() );
        if( setBufSize((uint32)(fSize)) == False )
            {
            editorDialog( edOutOfMemory );
            if (!crfound) remove(tmp);
            return False;
            }
        else
            {
            if ( fSize > INT_MAX )
            {
               f.read( &buffer[bufSize - (uint32)(fSize)], INT_MAX );
               f.read( &buffer[bufSize - (uint32)(fSize) + INT_MAX],
                                (uint32)(fSize - INT_MAX) );

            }
            else
               f.read( &buffer[bufSize - (uint32)(fSize)], (uint32)(fSize) );
            f.close();
            if( !f )
                {
                editorDialog( edReadError, fileName );
                if (!crfound) remove(tmp);
                return False;
                }
            else
                {
                setBufLen((uint32)(fSize));
                if (!crfound) remove(tmp);
                return True;
                }
            }
        }
}

Boolean TFileEditor::save()
{
    if( *fileName == EOS )
        return saveAs();
    else
        return saveFile();
}

Boolean TFileEditor::saveAs()
{
    Boolean res = False;
    if( editorDialog( edSaveAs, fileName ) != cmCancel )
        {
        fexpand( fileName );
        message( owner, evBroadcast, cmUpdateTitle, 0 );
        res = saveFile();
        if( isClipboard() == True )
            *fileName = EOS;
        }
    return res;
}

static void writeBlock( ofstream& f, char *buf, unsigned len )
{
    while( len > 0 )
        {
        int l = len < INT_MAX ? len : INT_MAX;
        f.write( buf, l );
        buf += l;
        len -= l;
        }
}

Boolean TFileEditor::saveFile()
{
    char drive[MAXDRIVE];
    char dir[MAXDIR];
    char file[MAXFILE];
    char ext[MAXEXT];

    if( (editorFlags & efBackupFiles) != 0 )
        {
        fnsplit( fileName, drive, dir, file, ext );
        char backupName[MAXPATH];
        fnmerge( backupName, drive, dir, file, backupExt );
        unlink( backupName );
        rename( fileName, backupName );
        }

    ofstream f( fileName, ios::out | ios::bin );

    if( !f )
        {
        editorDialog( edCreateError, fileName );
        return False;
        }
    else
        {
        writeBlock( f, buffer, curPtr );
        writeBlock( f, buffer+curPtr+gapLen, bufLen-curPtr );

        if( !f )
            {
            editorDialog( edWriteError, fileName );
            return False;
            }
        else
            {
            modified = False;
            update(ufUpdate);
            }
        }
    return True;
}

Boolean TFileEditor::setBufSize( uint32 newSize )
{
   newSize = (newSize + 0x0FFF) & 0xFFFFF000L;
    if( newSize != bufSize )
        {
        char *temp = buffer;
        if( (buffer = new char[newSize]) == 0 )
            {
            delete temp;
            return False;
            }
        uint32 n = bufLen - curPtr + delCount;
        memcpy( buffer, temp, min( newSize, bufSize ) );
        memmove( &buffer[newSize - n], &temp[bufSize - n], n );
        delete temp;
        bufSize = newSize;
        gapLen = bufSize - bufLen;
        }
    return True;
}

void TFileEditor::shutDown()
{
    setCmdState(cmSave, False);
    setCmdState(cmSaveAs, False);
    TEditor::shutDown();
}

void TFileEditor::updateCommands()
{
    TEditor::updateCommands();
    setCmdState(cmSave, True);
    setCmdState(cmSaveAs, True);
}

Boolean TFileEditor::valid( ushort command )
{
    if( command == cmValid )
        return isValid;
    else
        {
        if( modified == True )
            {
            int d;
            if( *fileName == EOS )
                d = edSaveUntitled;
            else
                d = edSaveModify;

            switch( editorDialog( d, fileName ) )
                {
                case cmYes:
                    return save();
                case cmNo:
                    modified = False;
                    return True;
                case cmCancel:
                    return False;
                }
            }
        }
    return True;
}

void TFileEditor::write( opstream& os )
{
    TEditor::write( os );
    os.writeString( fileName );
    os << selStart << selEnd << curPtr;
}

void *TFileEditor::read( ipstream& is )
{
    TEditor::read( is );
    is.readString( fileName, sizeof( fileName ) );
    if( isValid )
        {
        isValid = loadFile();
        uint32 sStart, sEnd, curs;
        is >> sStart >> sEnd >> curs;
        if( isValid && sEnd <= bufLen )
            {
            setSelect( sStart, sEnd, Boolean(curs == sStart) );
            trackCursor( True );
            }
        }
    return this;
}

TStreamable *TFileEditor::build()
{
    return new TFileEditor( streamableInit );
}

TFileEditor::TFileEditor( StreamableInit ) : TEditor( streamableInit )
{
}

// SET: Changed to lower case because it looks much better when using LFNs
const char *TFileEditor::backupExt = ".bak";
