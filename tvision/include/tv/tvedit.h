/***************************************************************/
/*                                                             */
/*  this header-file was modified by Robert Hoehne             */
/*  to use it with DJ's port of gcc                            */
/*                                                             */
/***************************************************************/
/*----------------------------------------------------------*/
/*                                                          */
/*   Turbo Vision 1.0                                       */
/*   Copyright (c) 1991 by Borland International            */
/*                                                          */
/*   Turbo Vision TVEDIT header file                        */
/*----------------------------------------------------------*/

#if defined( Uses_TEditorApp ) && !defined( __TEditorApp )
#define __TEditorApp

class TMenuBar;
class TStatusLine;
class TEditWindow;
class TDialog;

class CLY_EXPORT TEditorApp : public TApplication
{

public:

    TEditorApp();

    virtual void handleEvent( TEvent& event );
    static TMenuBar *initMenuBar( TRect );
    static TStatusLine *initStatusLine( TRect );
    virtual void outOfMemory();

// private:

    virtual TEditWindow *openEditor( const char *fileName, Boolean visible );

protected:

    virtual void fileOpen();
    void fileNew();
    void changeDir();
    virtual void dosShell();
    void showClip();
    void tile();
    void cascade();
};

extern TEditWindow *clipWindow;

ushort execDialog( TDialog *d, void *data );
TDialog *createFindDialog();
TDialog *createReplaceDialog();
ushort doEditDialog( int dialog, ... );

#endif
