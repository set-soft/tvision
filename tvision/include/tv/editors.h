/***************************************************************/
/*                                                             */
/*  this header-file was modified by Robert Hoehne             */
/*  to use it with DJ's port of gcc                            */
/*                                                             */
/***************************************************************/
/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   EDITORS.H                                                             */
/*                                                                         */
/*   Copyright (c) Borland International 1991                              */
/*   All Rights Reserved.                                                  */
/*                                                                         */
/*   defines the classes TIndicator, TEditor, TMemo, TFileEditor,          */
/*   and TEditWindow                                                       */
/*                                                                         */
/* ------------------------------------------------------------------------*/

#if !defined( __EDIT_COMMAND_CODES )
#define __EDIT_COMMAND_CODES

const int
  ufUpdate = 0x01,
  ufLine   = 0x02,
  ufView   = 0x04;

const int
  smExtend = 0x01,
  smDouble = 0x02;

const unsigned
  sfSearchFailed = UINT_MAX;

const int
  cmSave        = 80,
  cmSaveAs      = 81,
  cmFind        = 82,
  cmReplace     = 83,
  cmSearchAgain = 84;

const int
  cmCharLeft    = 500,
  cmCharRight   = 501,
  cmWordLeft    = 502,
  cmWordRight   = 503,
  cmLineStart   = 504,
  cmLineEnd     = 505,
  cmLineUp      = 506,
  cmLineDown    = 507,
  cmPageUp      = 508,
  cmPageDown    = 509,
  cmTextStart   = 510,
  cmTextEnd     = 511,
  cmNewLine     = 512,
  cmBackSpace   = 513,
  cmDelChar     = 514,
  cmDelWord     = 515,
  cmDelStart    = 516,
  cmDelEnd      = 517,
  cmDelLine     = 518,
  cmInsMode     = 519,
  cmStartSelect = 520,
  cmHideSelect  = 521,
  cmIndentMode  = 522,
  cmUpdateTitle = 523,
  cmInsertText  = 524;

const int
  edOutOfMemory   = 0,
  edReadError     = 1,
  edWriteError    = 2,
  edCreateError   = 3,
  edSaveModify    = 4,
  edSaveUntitled  = 5,
  edSaveAs        = 6,
  edFind          = 7,
  edSearchFailed  = 8,
  edReplace       = 9,
  edReplacePrompt = 10;

const int
  efCaseSensitive   = 0x0001,
  efWholeWordsOnly  = 0x0002,
  efPromptOnReplace = 0x0004,
  efReplaceAll      = 0x0008,
  efDoReplace       = 0x0010,
  efBackupFiles     = 0x0100;

const int
  maxLineLength = 256;

// SET: Added these old constants here
const unsigned
  cmOpen       = 100,
  cmNew        = 101,
  cmChangeDrct = 102,
  cmDosShell   = 103,
  cmCalculator = 104,
  cmShowClip   = 105,
  cmMacros     = 106;

#endif  // __EDIT_COMMAND_CODES

#if defined( Uses_TIndicator ) && !defined( __TIndicator )
#define __TIndicator

class far TRect;

class TIndicator : public TView
{

public:

    TIndicator( const TRect& );

    virtual void draw();
    virtual TPalette& getPalette() const;
    virtual void setState( ushort, Boolean );
    void setValue( const TPoint&, Boolean );

protected:

    TPoint location;
    Boolean modified;

#ifndef NO_STREAM

private:

    static const char near dragFrame;
    static const char near normalFrame;

    virtual const char *streamableName() const
        { return name; }

protected:

    TIndicator( StreamableInit );

public:

    static const char * const near name;
    static TStreamable *build();

#endif
};

#ifndef NO_STREAM

inline ipstream& operator >> ( ipstream& is, TIndicator& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, TIndicator*& cl )
    { return is >> (void *&)cl; }

inline opstream& operator << ( opstream& os, TIndicator& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, TIndicator* cl )
    { return os << (TStreamable *)cl; }

#endif // NO_STREAM

#endif  // Uses_TIndicator


#if defined( Uses_TEditor ) && !defined( __TEditor )
#define __TEditor

typedef ushort (*TEditorDialog)( int, ... );
ushort defEditorDialog( int dialog, ... );

class TRect;
class TScrollBar;
class TIndicator;
struct TEvent;

class TEditor : public TView
{

public:

    TEditor( const TRect&, TScrollBar *, TScrollBar *, TIndicator *, uint32 );
    virtual ~TEditor();

    virtual void shutDown();

    char bufChar( uint32 );
    uint32 bufPtr( uint32 );
    virtual void changeBounds( const TRect& );
    virtual void convertEvent( TEvent& );
    Boolean cursorVisible();
    void deleteSelect();
    virtual void doneBuffer();
    virtual void draw();
    virtual TPalette& getPalette() const;
    virtual void handleEvent( TEvent& );
    virtual void initBuffer();
    Boolean insertBuffer( char *, uint32, uint32, Boolean, Boolean );
    virtual Boolean insertFrom( TEditor * );
    Boolean insertText( const void *, uint32, Boolean );
    void scrollTo( int, int );
    Boolean search( const char *, ushort );
    virtual Boolean setBufSize( uint32 );
    void setCmdState( ushort, Boolean );
    void setSelect( uint32, uint32, Boolean);
    virtual void setState( ushort, Boolean );
    void trackCursor( Boolean );
    void undo();
    virtual void updateCommands();
    virtual Boolean valid( ushort );

    int charPos( uint32, uint32 );
    uint32 charPtr( uint32, int );
    Boolean clipCopy();
    void clipCut();
    void clipPaste();
    void deleteRange( uint32, uint32, Boolean );
    void doUpdate();
    void doSearchReplace();
    void drawLines( int, int, uint32 );
    virtual void formatLine(void *, uint32, int, ushort );
    void find();
    uint32 getMousePtr( TPoint );
    Boolean hasSelection();
    void hideSelect();
    Boolean isClipboard();
    uint32 lineEnd( uint32 );
    uint32 lineMove( uint32, int );
    uint32 lineStart( uint32 );
    void lock();
    void newLine();
    uint32 nextChar( uint32 );
    uint32 nextLine( uint32 );
    uint32 nextWord( uint32 );
    uint32 prevChar( uint32 );
    uint32 prevLine( uint32 );
    uint32 prevWord( uint32 );
    void replace();
    void setBufLen( uint32 );
    void setCurPtr( uint32, uchar );
    void startSelect();
    void toggleInsMode();
    void unlock();
    void update( uchar );
    void checkScrollBar( const TEvent&, TScrollBar *, int& );

    TScrollBar *hScrollBar;
    TScrollBar *vScrollBar;
    TIndicator *indicator;
    char *buffer;
    uint32 bufSize;
    uint32 bufLen;
    uint32 gapLen;
    uint32 selStart;
    uint32 selEnd;
    uint32 curPtr;
    TPoint curPos;
    TPoint delta;
    TPoint limit;
    int drawLine;
    uint32 drawPtr;
    uint32 delCount;
    uint32 insCount;
    Boolean isValid;
    Boolean canUndo;
    Boolean modified;
    Boolean selecting;
    Boolean overwrite;
    Boolean autoIndent;

    static TEditorDialog editorDialog;
    static ushort editorFlags;
    static char findStr[maxFindStrLen];
    static char replaceStr[maxReplaceStrLen];
    static TEditor * clipboard;
    static uint32 tabSize;
    uchar lockCount;
    uchar updateFlags;
    int keyState;

#ifndef NO_STREAM

private:

    virtual const char *streamableName() const
        { return name; }

protected:

    TEditor( StreamableInit );
    virtual void write( opstream& );
    virtual void *read( ipstream& );

public:

    static const char * const name;
    static TStreamable *build();

#endif
};

#ifndef NO_STREAM
inline ipstream& operator >> ( ipstream& is, TEditor& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, TEditor*& cl )
    { return is >> (void *&)cl; }

inline opstream& operator << ( opstream& os, TEditor& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, TEditor* cl )
    { return os << (TStreamable *)cl; }
#endif // NO_STREAM

#endif  // Uses_TEditor

#if defined( Uses_TMemo ) && !defined( __TMemo )
#define __TMemo

struct TEvent;

struct TMemoData
{
    uint32 length;
    char buffer[65536];
};

class TMemo : public TEditor
{

public:

    TMemo( const TRect&, TScrollBar *, TScrollBar *, TIndicator *, uint32 );
    virtual void getData( void *rec );
    virtual void setData( void *rec );
    virtual uint32 dataSize();
    virtual TPalette& getPalette() const;
    virtual void handleEvent( TEvent& );

#ifndef NO_STREAM

private:

    virtual const char *streamableName() const
        { return name; }

protected:

    TMemo( StreamableInit );
    virtual void write( opstream& );
    virtual void *read( ipstream& );

public:

    static const char * const name;
    static TStreamable *build();

#endif  // NO_STREAM
};

#ifndef NO_STREAM
inline ipstream& operator >> ( ipstream& is, TMemo& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, TMemo*& cl )
    { return is >> (void *&)cl; }

inline opstream& operator << ( opstream& os, TMemo& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, TMemo* cl )
    { return os << (TStreamable *)cl; }
#endif  // NO_STREAM

#endif  // Uses_TMemo


#if defined( Uses_TFileEditor ) && !defined( __TFileEditor )
#define __TFileEditor

//#include <dir.h>

class TRect;
class TScrollBar;
class TIndicator;
struct TEvent;

class TFileEditor : public TEditor
{

public:

    char fileName[PATH_MAX];
    TFileEditor( const TRect&,
                 TScrollBar *,
                 TScrollBar *,
                 TIndicator *,
                 const char *
               );
    virtual void doneBuffer();
    virtual void handleEvent( TEvent& );
    virtual void initBuffer();
    Boolean loadFile();
    Boolean save();
    Boolean saveAs();
    Boolean saveFile();
    virtual Boolean setBufSize( uint32 );
    virtual void shutDown();
    virtual void updateCommands();
    virtual Boolean valid( ushort );

private:

    static const char * backupExt;

#ifndef NO_STREAM
    virtual const char *streamableName() const
        { return name; }

protected:

    TFileEditor( StreamableInit );
    virtual void write( opstream& );
    virtual void *read( ipstream& );

public:

    static const char * const name;
    static TStreamable *build();
#endif // NO_STREAM
};

#ifndef NO_STREAM
inline ipstream& operator >> ( ipstream& is, TFileEditor& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, TFileEditor*& cl )
    { return is >> (void *&)cl; }

inline opstream& operator << ( opstream& os, TFileEditor& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, TFileEditor* cl )
    { return os << (TStreamable *)cl; }
#endif // NO_STREAM

#endif  // Uses_TFileEditor


#if defined( Uses_TEditWindow ) && !defined( __TEditWindow )
#define __TEditWindow

class TFileEditor;

class TEditWindow : public TWindow
{

public:

    TEditWindow( const TRect&, const char *, int );
    virtual void close();
    virtual const char *getTitle( short );
    virtual void handleEvent( TEvent& );
    virtual void sizeLimits( TPoint& min, TPoint& max );

    TFileEditor *editor;

private:

    static const char * clipboardTitle;
    static const char * untitled;

#ifndef NO_STREAM
    virtual const char *streamableName() const
        { return name; }

protected:

    TEditWindow( StreamableInit );
    virtual void write( opstream& );
    virtual void *read( ipstream& );

public:

    static const char * const name;
    static TStreamable *build();
#endif // NO_STREAM
};

#ifndef NO_STREAM
inline ipstream& operator >> ( ipstream& is, TEditWindow& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, TEditWindow*& cl )
    { return is >> (void *&)cl; }

inline opstream& operator << ( opstream& os, TEditWindow& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, TEditWindow* cl )
    { return os << (TStreamable *)cl; }
#endif // NO_STREAM

#endif  // Uses_TEditWindow


#if defined( Uses_TFindDialogRec ) && !defined( __TFindDialogRec )
#define __TFindDialogRec

struct TFindDialogRec
{
    TFindDialogRec( const char *str, ushort flgs )
        {
        strcpy( find, str );
        options = flgs;
        }
    char find[maxFindStrLen];
    ushort options;
};

#endif  // Uses_TFindDialogRec

#if defined( Uses_TReplaceDialogRec ) && !defined( __TReplaceDialogRec )
#define __TReplaceDialogRec

struct TReplaceDialogRec
{
    TReplaceDialogRec( const char *str, const char *rep, ushort flgs )
        {
        strcpy( find, str );
        strcpy( replace, rep );
        options = flgs;
        }
    char find[maxFindStrLen];
    char replace[maxReplaceStrLen];
    ushort options;
};

#endif  // Uses_TReplaceDialogRec

