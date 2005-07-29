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

#if defined( Uses_TFileViewer ) && !defined( __TFileViewer )
#define __TFileViewer

#include <tv.h>

const ushort hlChangeDir = cmChangeDir;

const ushort
          hcFileViewer     = 599,
          cmSaveFile       = 598,
          cmFileViewAnswer = 599; // I hope this conflicts with
                                  // nothing else

class CLY_EXPORT TLineCollection : public TCollection
{

public:

    TLineCollection(ccIndex lim, ccIndex delta) : TCollection(lim, delta) {}
    virtual void  freeItem(void *p) { delete (char *)p; }

#if !defined( NO_STREAM )
private:

    virtual void *readItem( ipstream& ) { return 0; }
    virtual void writeItem( void *, opstream& ) {}
#endif // NO_STREAM
    friend class TFileViewer;

};

class CLY_EXPORT TFileViewer : public TScroller
{

public:

    char *fileName;
    Boolean isValid;
    TFileViewer( const TRect& bounds,
                 TScrollBar *aHScrollBar,
                 TScrollBar *aVScrollBar,
                 const char *aFileName
               );
    ~TFileViewer();
#if !defined( NO_STREAM )
    TFileViewer( StreamableInit ) : TScroller(streamableInit) { };
#endif // NO_STREAM
    void draw();
    void readFile( const char *fName );
    virtual void saveFile(const char *fName);
    virtual void handleEvent(TEvent &);
    void setState( ushort aState, Boolean enable );
    void scrollDraw();
    virtual void changeBounds(const TRect &);
    Boolean valid( ushort command );
    void insertLine(const char *line);
    const char * const operator [] (int index);
    int Count() { return count; }

private:

/* I have moved the fileLines in the private section, because
   I'm using now a faster way for the FileViewer in which the user
   is not allowed to insert or delete a line from the fileLines.
   For inserting a line use the TFileViewer::insertLine.
   For accessing the lines use TFileViewer::operator []
*/
    TLineCollection *fileLines;
    char *buffer;
    ssize_t real_bufsize;

protected:

    const char * const getBuffer() { return buffer; }
    ssize_t getBufsize() { return real_bufsize; }

    int width;
    int count;

#if !defined( NO_STREAM )
private:
    virtual const char *streamableName() const
        { return name; }

protected:
    virtual void write( opstream& );
    virtual void *read( ipstream& );

public:

    static const char * const name;
    static TStreamable *build();
#endif // NO_STREAM
};

class CLY_EXPORT TFileWindow : public TWindow
{

public:

    TFileWindow( const char *fileName );

};

#endif

