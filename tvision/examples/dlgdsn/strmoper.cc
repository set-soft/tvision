/***************************************************************************

    strmoper.cc - Stream operations
    ---------------------------------------------------------------------
    May, 2000
    Copyright (C) 2000 by Warlei Alves
    walves@usa.net
    
    Modified by Salvador E. Tropea to compile without warnings.
    For gcc 2.95.x and then 3.0.1.
    
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 
#define Uses_stdio
#define Uses_string
// didn't find std::ios (AP)
#define Uses_iostream

#define Uses_MsgBox
#define Uses_TFileDialog
#define Uses_TProgram
#define Uses_TApplication
#define Uses_TDeskTop
#define Uses_ofpstream
#define Uses_ifpstream

#include <tv.h>
#include "strmoper.h"

UsingNamespaceStd

bool fileExists(const char * FileName)
{
   FILE * test;
   
   bool rst = ((test = fopen(FileName, "r")) != 0);
   if (rst) fclose(test);
   return rst;
}

// SET: This function was originaly declared as const char * and returned a
// pointer to a temporal variable in the stack. Nicola Asuni found it and
// suggested a partial workaround. I then modified the function to return
// a char * using newStr to allocate it. I also modified all the points
// where this function is used (at least all that I found) to release the
// returned value.
char * getFileName(const char * aTitle, const char * ext, int Mode)
{
   TFileDialog *d = 0;
   int cmd = cmCancel;
   
   if (Mode == 0)
     d = (TFileDialog *) TProgram::application->validView( new
       TFileDialog(ext, aTitle, _("File"), fdOpenButton, 100) );
   else
     d = (TFileDialog *) TProgram::application->validView( new
       TFileDialog(ext, aTitle, _("File"), fdOKButton, 100) );
   if (d != 0) cmd = TProgram::deskTop->execView( d );
   if ( cmd != cmCancel )
   {
      char fileName[PATH_MAX];
      d->getFileName( fileName );
      delete d;
      return newStr( fileName );
   }
   else return NULL;
}


ifpstream * openFile(const char * FileName, char * Signature)
{
   if (!fileExists(FileName)) return 0;
   ifpstream * rst = new ifpstream( FileName, ios::in|ios::binary );
   char buf[50];
   if (rst)
   {
      rst->readBytes(&buf, strlen(Signature) + 1);
      if (strcmp(buf, Signature) != 0)
      {
          delete rst;
          return 0;
      }
   }
   return rst;
}

ofpstream * initFile(const char * FileName, char * current, char * Signature)
{
   if ( fileExists(FileName) &&
      ( current == 0 || strcmp(FileName, current) != 0 ) )
   {
      if ( messageBox(__("The file already exists. Overwrite it?"),
                    mfYesNoCancel) != cmYes ) return 0;
   }
   ofpstream * rst = new ofpstream( FileName, ios::in|ios::binary );
   if (rst) rst->writeBytes(Signature, strlen(Signature) + 1);
   return rst;
}

