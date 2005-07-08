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

#if defined( Uses_TNSCollection ) && !defined( __TNSCollection )
#define __TNSCollection
// Avoid replacing free by MSS's macro
#include <tv/no_mss.h>

class CLY_EXPORT TNSCollection : public TObject
{

public:

    TNSCollection( ccIndex aLimit, ccIndex aDelta );
    ~TNSCollection();

    virtual void shutDown();

    void *at( ccIndex index )
    {
      if( index < 0 || index >= count )
          error(1,0);
      return items[index];
    }
    virtual ccIndex indexOf( void *item );

    void atFree( ccIndex index );
    void atRemove( ccIndex index );
    void remove( void *item );
    void removeAll();
    void free( void *item );
    void freeAll();

    virtual void atInsert( ccIndex index, void *item );
    void atPut( ccIndex index, void *item );
    void atReplace( ccIndex index, void *item ); // SET: Why not? ;-)
    virtual ccIndex insert( void *item );

    virtual void error( ccIndex code, ccIndex info );

    void *firstThat( ccTestFunc Test, void *arg );
    void *lastThat( ccTestFunc Test, void *arg );
    void forEach( ccAppFunc action, void *arg );

    void pack();
    virtual void setLimit( ccIndex aLimit );

    ccIndex getCount()
        { return count; }

    // SET: I added it to create collections with static strings inserted
    // in it. I think that's a common case and strduping each insertion is
    // a real waste.
    void setOwnerShip(Boolean option) { shouldDelete=option; }
    // SET: That's quite natural. I did it slower than at() but also smaller
    // I added it after error became virtual which makes it really bloated.
    void *operator[](ccIndex i);

protected:

    TNSCollection();

    void **items;
    ccIndex count;
    ccIndex limit;
    ccIndex delta;
    Boolean shouldDelete;

private:

    virtual void freeItem( void *item );

};

#include <tv/yes_mss.h>

#endif  // Uses_TNSCollection

