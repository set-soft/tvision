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
    bfBroadcast = 0x04,

    cmRecordHistory = 60;

#endif  // __BUTTON_TYPE

#if defined( Uses_TDialog )

#include <dialog.h>

#endif

#if defined( Uses_TInputLine )

#include <inputln.h>

#endif

#if defined( Uses_TButton )

#include <button.h>

#endif

#if defined( Uses_TSItem )

#include <sitem.h>

#endif

#if defined( Uses_TCluster )

#include <cluster.h>

#endif

#if defined( Uses_TRadioButtons )

#include <radiobtn.h>

#endif

#if defined( Uses_TCheckBoxes )

#include <checkbox.h>

#endif

#if defined( Uses_TListBox )

#include <listbox.h>

#endif

#if defined( Uses_TStaticText )

#include <sttctext.h>

#endif

#if defined( Uses_TParamText )

#include <parmtext.h>

#endif

#if defined( Uses_TLabel )

#include <label.h>

#endif

#if defined( Uses_THistoryViewer )

#include <histvwer.h>

#endif

#if defined( Uses_THistoryWindow )

#include <histwind.h>

#endif

#if defined( Uses_THistory )

#include <history.h>

#endif


