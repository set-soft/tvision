/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *

Modified by Robert H”hne to be used for RHIDE.
Modified by Salvador E. Tropea.

 *
 *
 */

#if defined( Uses_MsgBox ) && !defined( __MsgBox )
#define __MsgBox

#if !defined( __STDARG_H )
#include <stdarg.h>
#endif  // __STDARG_H

class TRect;
class TValidator;

ushort CLY_EXPORT messageBox( const char *msg, ushort aOptions );
ushort CLY_EXPORT messageBox( ushort aOptions, const char *msg, ... )
                __attribute__ ((format (printf, 2, 3)));

ushort CLY_EXPORT messageBoxRect( const TRect &r, const char *msg, ushort aOptions );
ushort CLY_EXPORT messageBoxRect( const TRect &r, ushort aOptions, const char *msg, ... )
                __attribute__ ((format (printf, 3, 4)));

ushort CLY_EXPORT inputBox( const char *Title, const char *aLabel, char *s, int limit,
                            TValidator *v=NULL );

ushort CLY_EXPORT inputBoxRect( const TRect &bounds, const char *title,
                                const char *aLabel, char *s, int limit,
                                TValidator *v=NULL );

const ushort

//  Message box classes

    mfWarning      = 0x0000,       // Display a Warning box
    mfError        = 0x0001,       // Dispaly a Error box
    mfInformation  = 0x0002,       // Display an Information Box
    mfConfirmation = 0x0003,       // Display a Confirmation Box

// Message box button flags

    mfYesButton    = 0x0100,       // Put a Yes button into the dialog
    mfNoButton     = 0x0200,       // Put a No button into the dialog
    mfOKButton     = 0x0400,       // Put an OK button into the dialog
    mfCancelButton = 0x0800,       // Put a Cancel button into the dialog

// Special flag: Avoid translating this message, is already translated.
    mfDontTranslate = 0x4000,
// Special flag: For the "Don't show it next time" field
    mfDontShowAgain = 0x8000,

    mfYesNoCancel  = mfYesButton | mfNoButton | mfCancelButton,
                                    // Standard Yes, No, Cancel dialog
    mfOKCancel     = mfOKButton | mfCancelButton;
                                    // Standard OK, Cancel dialog

class CLY_EXPORT MsgBoxText
{

public:

//    static const char * yesText;
//    static const char * noText;
//    static const char * okText;
//    static const char * cancelText;
//    static const char * warningText;
//    static const char * errorText;
//    static const char * informationText;
//    static const char * confirmText;
};

#endif  // Uses_MsgBox


