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

#if !defined( __BUTTON_TYPE )
#define __BUTTON_TYPE

const ushort
    bfNormal    = 0x00,
    bfDefault   = 0x01,
    bfLeftJust  = 0x02,
    bfBroadcast = 0x04;

#endif  // __BUTTON_TYPE

#if defined( Uses_TDialog )

#include <tv/dialog.h>

#endif

#if defined( Uses_TInputLine )

#include <tv/inputln.h>

#endif

#if defined( Uses_TButton )

#include <tv/button.h>

#endif

#if defined( Uses_TSItem )

#include <tv/sitem.h>

#endif

#if defined( Uses_TCluster )

#include <tv/cluster.h>

#endif

#if defined( Uses_TRadioButtons )

#include <tv/radiobtn.h>

#endif

#if defined( Uses_TCheckBoxes )

#include <tv/checkbox.h>

#endif

#if defined( Uses_TListBox )

#include <tv/listbox.h>

#endif

#if defined( Uses_TStaticText )

#include <tv/sttctext.h>

#endif

#if defined( Uses_TParamText )

#include <tv/parmtext.h>

#endif

#if defined( Uses_TLabel )

#include <tv/label.h>

#endif

#if defined( Uses_THistoryViewer )

#include <tv/histvwer.h>

#endif

#if defined( Uses_THistoryWindow )

#include <tv/histwind.h>

#endif

#if defined( Uses_THistory )

#include <tv/history.h>

#endif


