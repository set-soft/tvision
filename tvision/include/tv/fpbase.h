/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *

Modified by Robert H”hne to be used for RHIDE.
Modified to compile with gcc v3.x by Salvador E. Tropea, with the help of
Andris Pavenis and Christoph Bauer.

SET: I introduced an important change: now buf is a pointer to a filebuf
and not a filebuf. This is much more convenient to make the code
compilable with gcc 3.x without breaking compatibility.

SET: Added a filebuf wrapper to get access to protected members needed
to open associate a filebuf with a C stream/file handle. Andris proposed
this idea. This is needed for gcc 3.1 and we don't know how long this
trick will be used.

 *
 *
 */

/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   class fpbase                                                          */
/*                                                                         */
/*   Base class for handling streamable objects on file streams            */
/*                                                                         */
/* ------------------------------------------------------------------------*/

#if defined( Uses_fpbase ) && !defined( __fpbase )
#define __fpbase

#ifdef CLY_DefineSpecialFileBuf
class CLY_filebuf: public std::filebuf
{
public:
 CLY_filebuf() : std::filebuf() {};
 CLY_filebuf(FILE *f, std::ios_base::openmode mode)
   { open(f,mode); };
 CLY_filebuf(int h, std::ios_base::openmode mode)
   { open(h,mode); };

 CLY_filebuf *open(FILE *f, std::ios_base::openmode);
 CLY_filebuf *open(int h, std::ios_base::openmode);
 std::filebuf *open(const char *file, std::ios_base::openmode mode)
   { return std::filebuf::open(file,mode); };
};
#endif

class fpbase : virtual public pstream
{

public:

    fpbase();
    fpbase( const char *, CLY_OpenModeT, int = CLY_FBOpenProtDef );
    fpbase( int );
    fpbase( int, char *, int );
    ~fpbase();

    void open( const char *, CLY_OpenModeT, int = CLY_FBOpenProtDef );
    #ifdef CLY_HaveFBAttach
    void attach( int );
    #endif
    void close();
    void setbuf( char *, int );
    CLY_std(filebuf) * rdbuf();

private:

    CLY_filebuf *buf;

};

#endif  // Uses_fpbase

