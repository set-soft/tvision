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

#if defined( Uses_TResourceFile ) && !defined( __TResourceFile )
#define __TResourceFile

class TResourceCollection;
class fpstream;

class CLY_EXPORT TResourceFile: public TObject
{

public:

    TResourceFile( fpstream *aStream );
    ~TResourceFile();
    short count();
    void remove( const char *key );
    void flush();
    void *get( const char *key );
    const char *keyAt( short i );
    void put( TStreamable *item, const char *key );
    fpstream *switchTo( fpstream *aStream, Boolean pack );

protected:

    fpstream *stream;
    Boolean modified;
    CLY_StreamPosT basePos;
    CLY_StreamPosT indexPos;
    TResourceCollection *index;
};

#endif  // Uses_TResourceFile

