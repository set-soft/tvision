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
  cmNewColorIndex          = 75,
  cmTryColors              = 76,
  cmUpdateColorsChanged    = 77;

#endif  // __COLOR_COMMAND_CODES 

class TColorItem;
class TColorGroup;

CLY_EXPORT TColorItem& operator + ( TColorItem& i1, TColorItem& i2 );
CLY_EXPORT TColorGroup& operator + ( TColorGroup& g, TColorItem& i );
CLY_EXPORT TColorGroup& operator + ( TColorGroup& g1, TColorGroup& g2 );

#if defined( Uses_TColorItem )

#include <tv/clritem.h>

#endif

#if defined( Uses_TColorGroup )

#include <tv/clrgroup.h>

#endif

#if defined( Uses_TColorSelector )

#include <tv/clrselct.h>

#endif

#if defined( Uses_TMonoSelector )

#include <tv/mnoselct.h>

#endif

#if defined( Uses_TColorDisplay )

#include <tv/clrdisp.h>

#endif

#if defined( Uses_TColorGroupList )

#include <tv/clrgrpli.h>

#endif

#if defined( Uses_TColorItemList )

#include <tv/clriteml.h>

#endif

#if defined( Uses_TColorDialog )

#include <tv/clrdlg.h>

#endif


