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


