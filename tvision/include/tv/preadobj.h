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

/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   class TPReadObjects                                                   */
/*                                                                         */
/*   Maintains a database of all objects that have been read from the      */
/*   current persistent stream.                                            */
/*                                                                         */
/*   Used by ipstream when it reads a pointer from a stream to determine   */
/*   the address of the object being referred to.                          */
/*                                                                         */
/* ------------------------------------------------------------------------*/

#if defined( Uses_TPReadObjects ) && !defined( __TPReadObjects )
#define __TPReadObjects

class CLY_EXPORT TPReadObjects : public TNSCollection
{

    friend class ipstream;

public:

    void removeAll() { curId = 0; TNSCollection::removeAll(); }

private:

    TPReadObjects();
    ~TPReadObjects();

    void registerObject( const void *adr );
    const void *find( P_id_type id );

    P_id_type curId;

};

#endif  // Uses_TPReadObjects

