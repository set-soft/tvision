/***************************************************************************

    fdconsts.h - Commom application constants
    ---------------------------------------------------------------------
    May, 2000
    Copyright (C) 2000 by Warlei Alves
    walves@usa.net
    
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 
#if !defined(_FDCONSTS_HPP_)
#define _FDCONSTS_HPP_

//---> FreeDsg constants

const unsigned short
   cmFirst         = 100,
   cmOpenProject   = cmFirst + 1,
   cmSaveProject   = cmFirst + 2,
   cmSaveProjectAs = cmFirst + 3,
   cmOpenObj       = cmFirst + 4,
   cmSaveObj       = cmFirst + 5,
   cmSaveObjAs     = cmFirst + 6,

   cmWriteObjFunc  = cmFirst + 7,
   cmWriteObjUnit  = cmFirst + 8,
   
   cmShowGrid      = cmFirst + 9,

   cmViewProject   = cmFirst + 10,
   cmMenuEditor    = cmFirst + 11,
   cmStatusEditor  = cmFirst + 12,
   cmHintsEditor   = cmFirst + 13,
   cmCommandEdit   = cmFirst + 14,
   cmHelpCtxEdit   = cmFirst + 15,
   cmAppOptions    = cmFirst + 16,
   cmBuildApp      = cmFirst + 17,
   cmViewAppWindow = cmFirst + 18,

   cmDialogEditor  = cmFirst + 19,
   cmObjects       = cmFirst + 20,
   cmObjEditor     = cmFirst + 21,
   cmNewProject    = cmFirst + 22,

   cmEditEvents    = cmFirst + 0xff,
   cmItemDropped   = cmEditEvents + 1,
   cmResizeObj     = cmEditEvents + 2,
   cmValueChanged  = cmEditEvents + 3,
   cmDlgRun        = cmEditEvents + 4,

   hcFirst         = 1000,
   hcOpenProject   = hcFirst + 1,
   hcSaveProject   = hcFirst + 2,
   hcSavePrjAs     = hcFirst + 3,
   hcChangeDir     = hcFirst + 4,
   hcOpenObj       = hcFirst + 5,
   hcSaveObj       = hcFirst + 6,
   hcSaveObjAs     = hcFirst + 7,
   hcQuit          = hcFirst + 8,

   hcUndo          = hcFirst + 9,
   hcCopy          = hcFirst + 10,
   hcCut           = hcFirst + 11,
   hcPaste         = hcFirst + 12,

   hcWriteCode     = hcFirst + 13,
   hcWriteFunc     = hcFirst + 14,
   hcWriteUnit     = hcFirst + 15,
   
   hcHideShowGrid  = hcFirst + 16,
   
   hcViewProject   = hcFirst + 17,
   hcMenuEdit      = hcFirst + 18,
   hcStatusEdit    = hcFirst + 19,
   hcHindEdit      = hcFirst + 20,
   hcCommandEdit   = hcFirst + 21,
   hcHelpCtxEdit   = hcFirst + 22,
   hcAppOptions    = hcFirst + 23,
   hcAppBuild      = hcFirst + 24,
   hcViewAppWindow = hcFirst + 25,
   hcNewProject    = hcFirst + 26,

// Status defs contexts
   hcDefs          = hcFirst + 1000,
   
   hcCommandEditor = hcDefs + 1,
   hcHelpCtxEditor = hcDefs + 2,
   hcDlgTring      = hcDefs + 3,
   hcMenuEditor    = hcDefs + 4,
   hcStatusEditor  = hcDefs + 5,
   hcDlgEditor     = hcDefs + 6,
   hcObjWindow     = hcDefs + 7,
   hcObjEditor     = hcDefs + 8;

#endif // _FDCONSTS_HPP_

