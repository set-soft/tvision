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

#if !defined( __FILE_CMDS )
#define __FILE_CMDS

const ushort

//  Commands

    cmFileOpen     = 1001,   // Returned from TFileDialog when Open pressed
    cmFileReplace  = 1002,   // Returned from TFileDialog when Replace pressed
    cmFileClear    = 1003,   // Returned from TFileDialog when Clear pressed
    cmFileInit     = 1004,   // Used by TFileDialog internally
    cmChangeDir    = 1005,   // Used by TChDirDialog internally
    cmRevert       = 1006,   // Used by TChDirDialog internally
    cmFileSelect   = 1007,   // Returned from TFileDialog when Select pressed
    cmDirSelection = 1008,   // New: Used by TChDirDialog internally and
                             // TDirListBox externally

//  Messages

    cmFileFocused = 102,    // A new file was focused in the TFileList
    cmFileDoubleClicked     // A file was selected in the TFileList
            = 103;

#endif  // __FILE_CMDS

#if defined( Uses_TSearchRec )

#include <tv/searchrc.h>

#endif

#if defined( Uses_TFileInputLine )

#include <tv/filinpln.h>

#endif

#if defined( Uses_TFileCollection )

#include <tv/filecoll.h>

#endif

#if defined( Uses_TSortedListBox )

#include <tv/sortlibo.h>

#endif

#if defined( Uses_TFileList )

#include <tv/filelist.h>

#endif

#if defined( Uses_TFileInfoPane )

#include <tv/filinfpn.h>

#endif

#if defined( Uses_TFileDialog )

#include <tv/filedlg.h>

#endif

#if defined( Uses_TDirEntry )

#include <tv/direntr.h>

#endif

#if defined( Uses_TDirCollection )

#include <tv/dircoll.h>

#endif

#if defined( Uses_TDirListBox )

#include <tv/dirlibo.h>

#endif

#if defined( Uses_TChDirDialog )

#include <tv/chdirdlg.h>

#endif


