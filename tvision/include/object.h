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

#if defined( Uses_TObject ) && !defined( __TObject )
#define __TObject

#include <stddef.h>

class TObject
{

public:

    virtual ~TObject();

    static void destroy( TObject * );
    // SET: It also sets the pointer to 0
    #define destroy0(a) destroy(a); a=0
    virtual void shutDown();

private:

};

inline void TObject::destroy( TObject *o )
{
    if( o != 0 )
        o->shutDown();
    delete o;
}

inline void destroy( TObject *o )
{
  if (o) 
  {
    o->shutDown();
    delete o;
  }
}

#endif  // Uses_TObject

