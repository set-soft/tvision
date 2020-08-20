/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *
 * Modified by Robert H”hne to be used for RHIDE.
 *
 * SET: Moved the standard headers here because according to DJ
 * they can inconditionally declare symbols like NULL.
 * Reworked code for endian stuff (readShort, readInt, readLong, read8,
 * read16, read32 and read64).
 * Added 16 bit strings read members.
 *
 * Modified to compile with gcc v3.x by Salvador E. Tropea, with the help of
 * Andris Pavenis and Christoph Bauer.
 *
 * JASC: Endian compatible files
 *
 */
#include <assert.h>
#define Uses_string
#define Uses_stdio // EOF
#define Uses_TStreamable
#define Uses_TStreamableClass
#define Uses_TStreamableTypes
#define Uses_pstream
#define Uses_ipstream
#define Uses_TPReadObjects
#define Uses_PubStreamBuf
#include <tv.h>

ipstream::ipstream( CLY_streambuf *sb )
{
    objs = new TPReadObjects();
    pstream::init( sb );
}

ipstream::~ipstream()
{
    objs->shouldDelete = False;
    CLY_destroy((TObject *)objs);
}

CLY_StreamPosT ipstream::tellg()
{
    return bp->CLY_PubSeekOff( 0, CLY_IOSCur, CLY_IOSIn );
}

ipstream& ipstream::seekg( CLY_StreamPosT pos )
{
    objs->removeAll();
    bp->CLY_PubSeekOff( pos, CLY_IOSBeg );
    clear(); //is added by V.Bugrov for clear eof bit
    return *this;
}

ipstream& ipstream::seekg( CLY_StreamOffT off, CLY_IOSSeekDir dir )
{
    objs->removeAll();
    bp->CLY_PubSeekOff( off, dir );
    clear(); //is added by V.Bugrov for clear eof bit
    return *this;
}

uchar ipstream::readByte()
{
    // Added modified code by V. Bugrov
    int result = bp->sbumpc();
    if (result == EOF)
       setstate(CLY_IOSEOFBit);
    return (uchar)result; // This cast is safe here
}


/*
 *  readShort, readInt and readLong:
 *  These are platform dependent, reads the size and byte order of the native
 *  platform.
 *  Created by SET to be compatible with original code and V. Bugrov ideas.
 */

#define DefineReadDep(name,type)\
type ipstream::read##name()\
{\
 type temp; size_t i;\
 i=bp->sgetn((char *)&temp,sizeof(type));\
 if (i<sizeof(type))\
    setstate(CLY_IOSEOFBit);\
 return temp;\
}
DefineReadDep(Short,ushort);
DefineReadDep(Int,uint);
DefineReadDep(Long,ulong);

/*
 *  read16, read32 and read64:
 *  These are platform independent, reads a fixed size in little endian order
 *  and if the platform is big endian swaps bytes.
 *  Created by SET to be compatible with original code and JASC + V. Bugrov
 *  ideas.
 */

#ifdef TV_BIG_ENDIAN
#define Swap(a,b) t=v[a]; v[a]=v[b]; v[b]=t

static inline
void Swap16(char *v)
{
 char t;
 Swap(0,1);
}

static inline
void Swap32(char *v)
{
 char t;
 Swap(0,3);
 Swap(1,2);
}

static inline
void Swap64(char *v)
{
 char t;
 Swap(0,7);
 Swap(1,6);
 Swap(2,5);
 Swap(3,4);
}
#else
static inline
void Swap16(char *) {}
static inline
void Swap32(char *) {}
static inline
void Swap64(char *) {}
#endif

#define DefineRead(name,type)\
type ipstream::read##name()\
{\
 type temp; size_t i;\
 i=bp->sgetn((char *)&temp,sizeof(type));\
 if (i<sizeof(type))\
    setstate(CLY_IOSEOFBit);\
 Swap##name((char *)&temp);\
 return temp;\
}
DefineRead(16,uint16);
DefineRead(32,uint32);
DefineRead(64,uint64);

void ipstream::readBytes( void *data, size_t sz )
{
    size_t i = bp->sgetn( (char *)data, sz );
    if (i < sz)
      {
       setstate(CLY_IOSEOFBit);
       // SET: Fill with 0s. Avoid using unitialized memory.
       memset( ((char *)data) + i, 0, sz - i);
      }
}

char *ipstream::readString()
{
    int len = readByte();
    if( len == 0xFF )
        return 0;
    if( len == 0xfe )
        // SET: Read a fixed ammount of bytes in all platforms
        len = read32();
    char *buf = new char[len+1];
    if( buf == 0 )
        return 0;
    readBytes( buf, len );
    buf[len] = EOS;
    return buf;
}

char *ipstream::readString( char *buf, unsigned maxLen )
{
    assert( buf != 0 );
    char *tmp = readString();
    if (tmp)
    {
      strncpy(buf, tmp, maxLen-1);
      buf[maxLen-1] = 0;
      delete [] tmp;
    }
    else
      *buf = 0;
    return buf;
}

uint16 *ipstream::readString16()
{
    int len = readByte();
    if( len == 0xFF )
        return 0;
    if( len == 0xfe )
        len = read32();
    uint16 *buf = new uint16[len+1];
    if( buf == 0 )
        return 0;
    readBytes( buf, len*2 );
    buf[len] = EOS;
    return buf;
}

uint16 *ipstream::readString16( uint16 *buf, unsigned maxLen )
{
    assert( buf != 0 );
    uint16 *tmp = readString16();
    if (tmp)
    {
      maxLen--;
      unsigned i;
      for (i=0; tmp[i] && i<maxLen; i++)
          buf[i] = tmp[i];
      buf[maxLen] = 0;
      delete [] tmp;
    }
    else
      *buf = 0;
    return buf;
}

/* Operators moved to headers by JASC */

CLY_EXPORT ipstream& operator >> ( ipstream& ps, TStreamable& t )
{
    const TStreamableClass *pc = ps.readPrefix();
    ps.readData( pc, &t );
    ps.readSuffix();
    return ps;
}

CLY_EXPORT ipstream& operator >> ( ipstream& ps, void *&t )
{
    char ch = ps.readByte();
    switch( ch )
        {
        case pstream::ptNull:
            t = 0;
            break;
        case pstream::ptIndexed:
            {
            P_id_type index = ps.readWord();
            t = (void *)ps.find( index );
            assert( t != 0 );
            break;
            }
        case pstream::ptObject:
            {
            const TStreamableClass *pc = ps.readPrefix();
            t = ps.readData( pc, 0 );
            ps.readSuffix();
            break;
            }
        default:
            ps.error( pstream::peInvalidType );
            break;
        }
    return ps;
}

ipstream::ipstream()
{
    objs = new TPReadObjects();
}

const TStreamableClass *ipstream::readPrefix()
{
    char ch = readByte();
    assert( ch == '[' );    // don't combine this with the previous line!
                            // We must always do the read, even if we're
                            // not checking assertions

    char name[128];
    readString( name, sizeof name );
    const TStreamableClass *ret = types->lookup( name );
    assert( ret != NULL );
    return ret;
}

void *ipstream::readData( const TStreamableClass *c, TStreamable *mem )
{
    if( mem == 0 )
        mem = c->build();

    registerObject( (char *)mem - c->delta );   // register the actual address
                                        // of the object, not the address
                                        // of the TStreamable sub-object
    return mem->read( *this );
}

void ipstream::readSuffix()
{
    char ch = readByte();
    assert( ch == ']' );    // don't combine this with the previous line!
                            // We must always do the write, even if we're
                            // not checking assertions

}

const void *ipstream::find( P_id_type id )
{
    return objs->find( id );
}

void ipstream::registerObject( const void *adr )
{
    objs->registerObject( adr );
}

