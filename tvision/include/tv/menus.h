/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *

Modified by Robert H”hne to be used for RHIDE.
Modified by Salvador E. Tropea: Added += operators.

 *
 *
 */

class TSubMenu;
class TMenuItem;
class TStatusDef;
class TStatusItem;

TSubMenu& operator + ( TSubMenu& s, TMenuItem& i );
TSubMenu& operator + ( TSubMenu& s1, TSubMenu& s2 );
TStatusDef& operator + ( TStatusDef& s1, TStatusItem& s2 );
TStatusDef& operator + ( TStatusDef& s1, TStatusDef& s2 );

#if defined( Uses_TMenuItem )
 #include <tv/menuitem.h>
#endif

#if defined( Uses_TSubMenu )
 #include <tv/submenu.h>
#endif

#if defined( Uses_TMenu )
 #include <tv/menu.h>
#endif

#if defined( Uses_TMenuView )
 #include <tv/menuview.h>
#endif

#if defined( Uses_TMenuBar )
 #include <tv/menubar.h>
#endif

#if defined( Uses_TMenuBox )
 #include <tv/menubox.h>
#endif

#if defined( Uses_TStatusItem )
 #include <tv/statsitm.h>
#endif

#if defined( Uses_TStatusDef )
 #include <tv/statsdef.h>
#endif

#if defined( Uses_TStatusLine )
 #include <tv/statslin.h>
#endif

// SET: The following 4 operator are from TVTools
// They are simple and adds more coherence.
#if defined( Uses_TSubMenu ) && !defined( TSubMenu_operators_defined  )
#define TSubMenu_operators_defined
inline void operator += ( TSubMenu& s1, TSubMenu& s2 )
{
    s1 = s1 + s2;
}
#endif

#if defined( Uses_TSubMenu ) && defined( Uses_TMenuItem ) && \
    !defined( TSubMenu_Item_operators_defined  )
#define TSubMenu_Item_operators_defined
inline void operator += ( TSubMenu& i1, TMenuItem& i2 )
{
    i1 = i1 + i2;
}
#endif

#if defined( Uses_TStatusDef ) && !defined( TStatusDef_operators_defined )
#define TStatusDef_operators_defined
inline void operator += ( TStatusDef& s1, TStatusDef& s2 )
{
    s1 = s1 + s2;
}
#endif


#if defined( Uses_TStatusDef ) && defined( Uses_TStatusItem ) && \
    !defined( TStatusDef_Item_operators_defined )
#define TStatusDef_Item_operators_defined
inline void operator += ( TStatusDef& s1, TStatusItem& s2 )
{
    s1 = s1 + s2;
}
#endif // Uses_TStatusItem

