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
// SET: Moved the standard headers here because according to DJ
// they can inconditionally declare symbols like NULL
#include <assert.h>
#include <string.h>


#define Uses_TStreamable
#define Uses_TStreamableClass
#define Uses_TStreamableTypes
#define Uses_pstream
#define Uses_ipstream
#define Uses_TPReadObjects
#include <tv.h>

ipstream::ipstream( streambuf *sb )
{
    objs = new TPReadObjects();
    pstream::init( sb );
}

ipstream::~ipstream()
{
    objs->shouldDelete = False;
    destroy(objs);
}

streampos ipstream::tellg()
{
    return bp->seekoff( 0, ios::cur, ios::in );
}

ipstream& ipstream::seekg( streampos pos )
{
    objs->removeAll();
    bp->seekoff( pos, ios::beg );
    clear(); //is added by V.Bugrov for clear eof bit
    return *this;
}

ipstream& ipstream::seekg( streamoff off, ios::seek_dir dir )
{
    objs->removeAll();
    bp->seekoff( off, dir );
    clear(); //is added by V.Bugrov for clear eof bit
    return *this;
}

uchar ipstream::readByte()
{
    // Added modified code by V. Bugrov
    int result = bp->sbumpc();
    if (result == EOF)
       setstate(ios::eofbit);
    return (uchar)result; // This cast is safe here
}

ushort ipstream::readWord()
{
    // Added modified code by V. Bugrov here.
    ushort temp, i;
    i = bp->sgetn( (char *)&temp, sizeof( ushort ) );
    if (i < sizeof(ushort))
       setstate(ios::eofbit);
    return temp;
}

void ipstream::readBytes( void *data, size_t sz )
{
    // Added modified code by V. Bugrov here.
    size_t i = bp->sgetn( (char *)data, sz );
    if (i < sz)
       setstate(ios::eofbit);
}

char *ipstream::readString()
{
    uchar _len = readByte();
    if( _len == 0xFF )
        return 0;
    int len = _len;
    if (len == 0xfe)
      readBytes( &len, sizeof(len) );
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
      delete tmp;
    }
    else
      *buf = 0;
    return buf;
}

ipstream& operator >> ( ipstream& ps, signed char &ch )
{
    ch = ps.readByte();
    return ps;
}

ipstream& operator >> ( ipstream& ps, char &ch )
{
    ch = ps.readByte();
    return ps;
}

ipstream& operator >> ( ipstream& ps, unsigned char &ch )
{
    ch = ps.readByte();
    return ps;
}

ipstream& operator >> ( ipstream& ps, signed short &sh )
{
    sh = ps.readWord();
    return ps;
}

ipstream& operator >> ( ipstream& ps, unsigned short &sh )
{
    sh = ps.readWord();
    return ps;
}

ipstream& operator >> ( ipstream& ps, signed int &i )
{
    ps.readBytes(&i,sizeof(i));
    return ps;
}

ipstream& operator >> ( ipstream& ps, unsigned int &i )
{
    ps.readBytes(&i,sizeof(i));
    return ps;
}

ipstream& operator >> ( ipstream& ps, signed long &l )
{
    ps.readBytes( &l, sizeof(l) );
    return ps;
}

ipstream& operator >> ( ipstream& ps, unsigned long &l )
{
    ps.readBytes( &l, sizeof(l) );
    return ps;
}

ipstream& operator >> ( ipstream& ps, float &f )
{
    ps.readBytes( &f, sizeof(f) );
    return ps;
}

ipstream& operator >> ( ipstream& ps, double &d )
{
    ps.readBytes( &d, sizeof(d) );
    return ps;
}

ipstream& operator >> ( ipstream& ps, TStreamable& t )
{
    const TStreamableClass *pc = ps.readPrefix();
    ps.readData( pc, &t );
    ps.readSuffix();
    return ps;
}

ipstream& operator >> ( ipstream& ps, void *&t )
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
    return types->lookup( name );
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

