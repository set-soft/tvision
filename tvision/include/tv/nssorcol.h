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

#if defined( Uses_TNSSortedCollection ) && !defined( __TNSSortedCollection )
#define __TNSSortedCollection

class CLY_EXPORT TNSSortedCollection: public virtual TNSCollection
{

public:

    TNSSortedCollection( ccIndex aLimit, ccIndex aDelta) :
        TNSCollection( aLimit, aDelta ), duplicates(False)
            { delta = aDelta; setLimit( aLimit ); }

    virtual Boolean search( void *key, ccIndex& index );

    virtual ccIndex indexOf( void *item );
    virtual ccIndex insert( void *item );

    Boolean duplicates;
    virtual void *keyOf( void *item );

    void reSort(); // SET: Sort it again

protected:

    TNSSortedCollection() : duplicates(False) {}

private:

    virtual int compare( void *key1, void *key2 ) = 0;

};

#endif  // Uses_TNSSortedCollection

