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

#if defined( Uses_TRect ) && !defined( __TRect )
#define __TRect

class TRect
{

public:

    TRect( int ax, int ay, int bx, int by );
    TRect( TPoint p1, TPoint p2 );
    TRect();

    void move( int aDX, int aDY );
    void grow( int aDX, int aDY );
    void intersect( const TRect& r );
    void Union( const TRect& r );
    Boolean contains( const TPoint& p ) const;
    Boolean operator == ( const TRect& r ) const;
    Boolean operator != ( const TRect& r ) const;
    Boolean isEmpty();

    TPoint a, b;

};

inline TRect::TRect( int ax, int ay, int bx, int by)
{
    a.x = ax;
    a.y = ay;
    b.x = bx;
    b.y = by;
}

inline TRect::TRect( TPoint p1, TPoint p2 )
{
    a = p1;
    b = p2;
}

inline TRect::TRect()
{
}

inline void TRect::move( int aDX, int aDY )
{
    a.x += aDX;
    a.y += aDY;
    b.x += aDX;
    b.y += aDY;
}

inline void TRect::grow( int aDX, int aDY )
{
    a.x -= aDX;
    a.y -= aDY;
    b.x += aDX;
    b.y += aDY;
}

inline void TRect::intersect( const TRect& r )
{
    a.x = max( a.x, r.a.x );
    a.y = max( a.y, r.a.y );
    b.x = min( b.x, r.b.x );
    b.y = min( b.y, r.b.y );
}

inline void TRect::Union( const TRect& r )
{
    a.x = min( a.x, r.a.x );
    a.y = min( a.y, r.a.y );
    b.x = max( b.x, r.b.x );
    b.y = max( b.y, r.b.y );
}

inline Boolean TRect::contains( const TPoint& p ) const
{
    return Boolean(
        p.x >= a.x && p.x < b.x && p.y >= a.y && p.y < b.y
        );
}

inline Boolean TRect::operator == ( const TRect& r) const
{
    return Boolean( a == r.a && b == r.b );
}

inline Boolean TRect::operator != ( const TRect& r ) const
{
    return Boolean( !(*this == r) );
}

inline Boolean TRect::isEmpty()
{
    return Boolean( a.x >= b.x || a.y >= b.y );
}

inline ipstream& operator >> ( ipstream& is, TRect& r )
    { return is >> r.a >> r.b; }
inline ipstream& operator >> ( ipstream& is, TRect*& r )
    { return is >> r->a >> r->b; }

inline opstream& operator << ( opstream& os, TRect& r )
    { return os << r.a << r.b; }
inline opstream& operator << ( opstream& os, TRect* r )
    { return os << r->a << r->b; }

#endif  // Uses_TRect

