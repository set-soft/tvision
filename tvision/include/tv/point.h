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

#if defined( Uses_TPoint ) && !defined( __TPoint )
#define __TPoint

class TPoint
{

public:

    TPoint& operator+=( const TPoint& adder );
    TPoint& operator-=( const TPoint& subber );
    friend TPoint operator - ( const TPoint& one, const TPoint& two);
    friend TPoint operator + ( const TPoint& one, const TPoint& two);
    friend int operator == ( const TPoint& one, const TPoint& two);
    friend int operator != ( const TPoint& one, const TPoint& two);

    int x,y;

};

inline TPoint& TPoint::operator += ( const TPoint& adder )
{
    x += adder.x;
    y += adder.y;
    return *this;
}

inline TPoint& TPoint::operator -= ( const TPoint& subber )
{
    x -= subber.x;
    y -= subber.y;
    return *this;
}

inline ipstream& operator >> ( ipstream& is, TPoint& p )
    { return is >> p.x >> p.y; }
inline ipstream& operator >> ( ipstream& is, TPoint*& p )
    { return is >> p->x >> p->y; }

inline opstream& operator << ( opstream& os, TPoint& p )
    { return os << p.x << p.y; }
inline opstream& operator << ( opstream& os, TPoint* p )
    { return os << p->x << p->y; }

#endif  // Uses_TPoint

