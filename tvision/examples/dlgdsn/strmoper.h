/***************************************************************************

    strmoper.h - Stream operations
    ---------------------------------------------------------------------
    May, 2000
    Copyright (C) 2000 by Warlei Alves
    walves@usa.net
    
    Modified by Salvador E. Tropea to compile without warnings.
    
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 
#define dialogFileSig     "FreeDsgn Dialog File\x1a"
#define menuFileSig       "FreeDsgn Menu File\x1a"
#define statusLineFileSig "FreeDsgn StatusLine File\x1a"
#define constanstFileSig  "FreeDsgn Constants File\x1a"
#define helpCtxFileSig    "FreeDsgn Help Contexts File\x1a"
#define AppDataFileSig    "FreeDsgn App Data File\x1a"
#define hintsDataFileSig  "FreeDsgn Hints File\x1a"

ifpstream * openFile(const char * FileName, char * Signature);
ofpstream * initFile(const char * FileName, char * current, char * Signature);
char * getFileName(const char * aTitle, const char * ext, int Mode);

