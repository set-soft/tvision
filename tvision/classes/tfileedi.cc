/* Modified by Robert Hoehne and Salvador Eduardo Tropea for the gcc port */
/* Modified by Vadim Beloborodov to be used on WIN32 console */
/* Modified to compile with gcc v3.x by Salvador E. Tropea, with the help of
   Andris Pavenis. */
/* Modified by Salvador E. Tropea to avoid using C++ streams (just C code) */
/*----------------------------------------------------------*/
/*                                                          */
/*   Turbo Vision 1.0                                       */
/*   Copyright (c) 1991 by Borland International            */
/*                                                          */
/*----------------------------------------------------------*/
#include <tv/configtv.h>

#define Uses_filelength
#define Uses_limits
#define Uses_string
#define Uses_fcntl
#ifdef TVComp_MSC
 #include <io.h>
#else
 #define Uses_unistd
#endif
#define Uses_stdio
#define Uses_sys_stat

#define Uses_TGroup
#define Uses_TEditor
#define Uses_TFileEditor
#define Uses_TEvent
#define Uses_opstream
#define Uses_ipstream
#define Uses_TStreamableClass
#define Uses_IOS_BIN
#include <tv.h>

UsingNamespaceStd

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
    DeleteArray(buffer);
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
    int crfound = 0;
    char tmp[PATH_MAX];
    FILE *f=fopen(fileName,"rb");
    if( !f )
        {
        setBufLen( 0 );
        return True;
        }
    else
        {
        #ifdef TVOS_DOS
        int i;
/* check for a unix text file (is a heuristic, because only 1024 chars checked */
        {
          char tmpbuf[1024];
          long fsize=filelength(fileno(f));
          if (fsize > 1024) fsize = 1024;
          fread(tmpbuf,fsize,1,f);
          for (i=0;i<1024;i++)
          {
            if (tmpbuf[i] == '\r') crfound = 1;
            else if (tmpbuf[i] == '\n') break;
          }
          if (crfound)
          {
            fseek(f,0,SEEK_SET);
          }
          else
          {
            int readhandle,writehandle;
            fclose(f);
            readhandle = open(fileName,O_RDONLY|O_BINARY);
            tmpnam(tmp);
            writehandle = open(tmp,O_WRONLY|O_CREAT|O_TRUNC|O_TEXT,0600);
            while ((fsize = ::read(readhandle,tmpbuf,1024)) > 0)
              ::write(writehandle,tmpbuf,fsize);
            close(readhandle);
            close(writehandle);
            f=fopen(tmp,"rb");
          }
        }
        #endif
        long fSize=filelength(fileno(f));;
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
               fread( &buffer[bufSize - (uint32)(fSize)], INT_MAX, 1, f );
               fread( &buffer[bufSize - (uint32)(fSize) + INT_MAX],
                                (uint32)(fSize - INT_MAX), 1, f );

            }
            else
               fread( &buffer[bufSize - (uint32)(fSize)], (uint32)(fSize), 1, f );
            int error=ferror(f);
            if( fclose(f) || error )
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
        CLY_fexpand( fileName );
        message( owner, evBroadcast, cmUpdateTitle, 0 );
        res = saveFile();
        if( isClipboard() == True )
            *fileName = EOS;
        }
    return res;
}

static void writeBlock( FILE *f, char *buf, unsigned len )
{
    while( len > 0 )
        {
        int l = len < INT_MAX ? len : INT_MAX;
        fwrite( buf, l, 1, f );
        buf += l;
        len -= l;
        }
}

// SET: from my editor:
static
int edTestForFile(const char *name)
{
 struct stat st;

 if (stat(name,&st)==0)
    return S_ISREG(st.st_mode);
 return 0;
}

Boolean TFileEditor::saveFile()
{
    // SET: That's similar to what I use in TCEdit and is partially a hack
    // to avoid fnsplit & fnmerge in Linux (originally from Robert).
    if ((editorFlags & efBackupFiles) &&
        edTestForFile(fileName)) // Forget about it if that's a new file
      {
       char *dot,*slash;
       int flen = strlen(fileName);
       char backupName[PATH_MAX];
       strcpy(backupName,fileName);
       dot = strrchr(backupName,'.');
       slash = strrchr(backupName,DIRSEPARATOR);
       if (dot < slash) // directory has a dot but not the filename
         dot = NULL;
       if (!dot)
         dot = backupName + flen;
       strcpy(dot,backupExt);
       unlink( backupName );
       rename( fileName, backupName );
      }

    FILE *f=fopen(fileName,"wb");

    if( !f )
        {
        editorDialog( edCreateError, fileName );
        return False;
        }
    else
        {
        writeBlock( f, buffer, curPtr );
        writeBlock( f, buffer+curPtr+gapLen, bufLen-curPtr );

        int error=ferror(f); // SET: Is that needed? Or fclose will inform the error?
        if( fclose(f) || error )
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

#ifndef NO_STREAM

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

#endif

// SET: Changed to lower case because it looks much better when using LFNs
const char *TFileEditor::backupExt = ".bak";
