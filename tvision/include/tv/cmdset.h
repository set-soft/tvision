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

#if defined( Uses_TCommandSet ) && !defined( __TCommandSet )
#define __TCommandSet

class TCommandSet
{

public:

    TCommandSet();
    TCommandSet( const TCommandSet& );
    ~TCommandSet();

    Boolean has( int cmd );

    void disableCmd( int cmd );
    void disableCmd( int cmdStart, int cmdEnd ); // SET: Added
    void enableCmd( int cmd );
    void enableCmd( int cmdStart, int cmdEnd ); // SET: Added
    void enableAllCommands();
    void operator += ( int cmd );
    void operator -= ( int cmd );

    void disableCmd( const TCommandSet& );
    void enableCmd( const TCommandSet& );
    void operator += ( const TCommandSet& );
    void operator -= ( const TCommandSet& );

    Boolean isEmpty();

    TCommandSet& operator = (const TCommandSet& );

    TCommandSet& operator &= ( const TCommandSet& );
    TCommandSet& operator |= ( const TCommandSet& );

    friend TCommandSet operator & ( const TCommandSet&, const TCommandSet& );
    friend TCommandSet operator | ( const TCommandSet&, const TCommandSet& );

    friend int operator == ( const TCommandSet& tc1, const TCommandSet& tc2 );
    friend int operator != ( const TCommandSet& tc1, const TCommandSet& tc2 );

private:
    uint32 *cmds;

};

inline void TCommandSet::operator += ( int cmd )
{
    enableCmd( cmd );
}

inline void TCommandSet::operator -= ( int cmd )
{
    disableCmd( cmd );
}

inline void TCommandSet::operator += ( const TCommandSet& tc )
{
    enableCmd( tc );
}

inline void TCommandSet::operator -= ( const TCommandSet& tc )
{
    disableCmd( tc );
}

inline int operator != ( const TCommandSet& tc1, const TCommandSet& tc2 )
{
    return !operator == ( tc1, tc2 );
}

#endif  // Uses_TCommandSet

