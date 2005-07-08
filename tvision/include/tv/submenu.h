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

#if defined( Uses_TSubMenu ) && !defined( __TSubMenu )
#define __TSubMenu

class CLY_EXPORT TSubMenu : public TMenuItem
{

public:

    TSubMenu( const char *, ushort, ushort = hcNoContext );

};

#endif  // Uses_TSubMenu

