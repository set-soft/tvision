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

#if !defined( __COLOR_COMMAND_CODES )
#define __COLOR_COMMAND_CODES

const ushort
  cmColorForegroundChanged = 71,
  cmColorBackgroundChanged = 72,
  cmColorSet               = 73,
  cmNewColorItem           = 74,
  cmNewColorIndex          = 75;

#endif  // __COLOR_COMMAND_CODES 

class TColorItem;
class TColorGroup;

TColorItem& operator + ( TColorItem& i1, TColorItem& i2 );
TColorGroup& operator + ( TColorGroup& g, TColorItem& i );
TColorGroup& operator + ( TColorGroup& g1, TColorGroup& g2 );

#if defined( Uses_TColorItem )

#include <clritem.h>

#endif

#if defined( Uses_TColorGroup )

#include <clrgroup.h>

#endif

#if defined( Uses_TColorSelector )

#include <clrselct.h>

#endif

#if defined( Uses_TMonoSelector )

#include <mnoselct.h>

#endif

#if defined( Uses_TColorDisplay )

#include <clrdisp.h>

#endif

#if defined( Uses_TColorGroupList )

#include <clrgrpli.h>

#endif

#if defined( Uses_TColorItemList )

#include <clriteml.h>

#endif

#if defined( Uses_TColorDialog )

#include <clrdlg.h>

#endif


