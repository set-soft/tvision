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

class CLY_EXPORT TObject
{

public:

    virtual ~TObject();

    static void CLY_destroy( TObject * );
    virtual void shutDown();

private:

};

inline void TObject::CLY_destroy( TObject *o )
{
    if( o != 0 )
        o->shutDown();
    delete o;
}

inline void CLY_destroy( TObject *o )
{
  if (o) 
  {
    o->shutDown();
    delete o;
  }
}

// SET: It also sets the pointer to 0
#define destroy0(o) \
{ \
 CLY_destroy(o);\
 o=0;\
}

#endif  // Uses_TObject

