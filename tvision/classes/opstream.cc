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
#include <stdio.h> // fprintf in writeData

#define Uses_TStreamable
#define Uses_TStreamableClass
#define Uses_TPWrittenObjects
#define Uses_TStreamableTypes
#define Uses_pstream
#define Uses_opstream
#define Uses_TPWrittenObjects
#include <tv.h>

opstream::opstream()
{
    objs = new TPWrittenObjects();
}

opstream::opstream( streambuf * sb )
{
    objs = new TPWrittenObjects();
    pstream::init( sb );
}

opstream::~opstream()
{
    destroy(objs);
}

opstream& opstream::seekp( streampos pos )
{
    objs->removeAll();
    bp->seekoff( pos, ios::beg );
    return *this;
}

opstream& opstream::seekp( streamoff pos, ios::seek_dir dir )
{
    objs->removeAll();
    bp->seekoff( pos, dir );
    return *this;
}

streampos opstream::tellp()
{
    return bp->seekoff( 0, ios::cur, ios::out );
}

opstream& opstream::flush()
{
    bp->sync();
    return *this;
}

void opstream::writeByte( uchar ch )
{
    bp->sputc( ch );
}

void opstream::writeBytes( const void *data, size_t sz )
{
    bp->sputn( (char *)data, sz );
}

void opstream::writeWord( ushort sh )
{
    bp->sputn( (char *)&sh, sizeof( ushort ) );
}

void opstream::writeString( const char *str )
{
    if( str == 0 )
        {
        writeByte( 0xFF );
        return;
        }
    int len = strlen( str );
    if (len > 0xfd)
    {
      writeByte( 0xfe );
      writeBytes( &len, sizeof(len) );
    }
    else
    {
      writeByte( (uchar)len );
    }
    writeBytes( str, len );
}

opstream& operator << ( opstream& ps, signed char ch )
{
    ps.writeByte( ch );
    return ps;
}

opstream& operator << ( opstream& ps, char ch )
{
    ps.writeByte( ch );
    return ps;
}

opstream& operator << ( opstream& ps, unsigned char ch )
{
    ps.writeByte( ch );
    return ps;
}

opstream& operator << ( opstream& ps, signed short sh )
{
    ps.writeWord( sh );
    return ps;
}

opstream& operator << ( opstream& ps, unsigned short sh )
{
    ps.writeWord( sh );
    return ps;
}

opstream& operator << ( opstream& ps, signed int i )
{
    ps.writeBytes( &i, sizeof(i) );
    return ps;
}

opstream& operator << ( opstream& ps, unsigned int i )
{
    ps.writeBytes( &i, sizeof(i) );
    return ps;
}
opstream& operator << ( opstream& ps, signed long l )
{
    ps.writeBytes( &l, sizeof(l) );
    return ps;
}

opstream& operator << ( opstream& ps, unsigned long l )
{
    ps.writeBytes( &l, sizeof(l) );
    return ps;
}

opstream& operator << ( opstream& ps, float f )
{
    ps.writeBytes( &f, sizeof(f) );
    return ps;
}

opstream& operator << ( opstream& ps, double d )
{
    ps.writeBytes( &d, sizeof(d) );
    return ps;
}

opstream& operator << ( opstream& ps, TStreamable& t )
{
    ps.writePrefix( t );
    ps.writeData( t );
    ps.writeSuffix( t );
    return ps;
}

opstream& operator << ( opstream& ps, TStreamable *t )
{
    P_id_type index;
    if( t == 0 )
	ps.writeByte( pstream::ptNull );
    else if( (index = ps.find( t )) != P_id_notFound )
	{
	ps.writeByte( pstream::ptIndexed );
	ps.writeWord( index );
	}
    else
	{
	ps.writeByte( pstream::ptObject );
	ps << *t;
	}
    return ps;
}

void opstream::writePrefix( const TStreamable& t )
{
    writeByte( '[' );
    writeString( t.streamableName() );
}


void opstream::writeData( TStreamable& t )
{
    if( types->lookup( t.streamableName() ) == 0 )
    {
        fprintf(stderr,_("type not registered: %s\n"),t.streamableName());
	error( peNotRegistered, t );
    }
    else
	{
	registerObject( &t );
	t.write( *this );
	}
}

void opstream::writeSuffix( const TStreamable& )
{
    writeByte( ']' );
}

P_id_type opstream::find( const void *adr )
{
    return objs->find( adr );
}

void opstream::registerObject( const void *adr )
{
    objs->registerObject( adr );
}

