/***************************************************************************

    propdlgs.h - Functions prototypes for property editors
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
 
#if !defined(_PROPDLGS_HPP_)
#define _PROPDLGS_HPP_

class TDialog;

// String maker functions - convert properties to its string representation
char * OptionsStr(ushort aOptions);
char * EventMaskStr(ushort aEventMask);
char * StateStr(ushort aState);
char * GrowModeStr(ushort aGrowMode);
char * DragModeStr(ushort aDragMode);
char * HelpCtxStr(ushort aHelpCtx);
char * CommandStr(ushort aCommand);
char * ButtonFlagStr(ushort aButtonFlag);
char * WindowFlagStr(ushort aWindowFlag);
char * ValidatorStr(ushort aWindowFlag);
char * ItemsStr(void * aItemPtr);
char * CharPtrStr(void * aItemPtr);

// Properties editors
TDialog * OptionsEditor();
TDialog * EventMaskEditor();
TDialog * StateEditor();
TDialog * GrowModeEditor();
TDialog * DragModeEditor();
TDialog * HelpCtxEditor();
TDialog * CommandEditor();
TDialog * ButtonFlagEditor();
TDialog * WindowFlagEditor();
TDialog * ValidatorEditor();
TDialog * ItemsEditor();
TDialog * CharPtrEditor();
TDialog * LinkEditor();
TDialog * ScrollEditor();

#endif // _PROPDLGS_HPP_

