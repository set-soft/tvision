/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
             \ /\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\ /
 -----=====<<<|  FREEDSGN  - A Turbo Vision App Design Helper  |>>>=====-----
 -----=====<<<| Version 0.1 - copyright (c) 1999  Warlei Alves |>>>=====-----
             / \/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/ \

                          w a l v e s @ u s a . n e t

 -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>

#define Uses_TApplication
#define Uses_TProgram
#define Uses_TDesktop
#define Uses_TRect
#define Uses_TView
#define Uses_MsgBox
#define Uses_TMenuBar
#define Uses_TSubMenu
#define Uses_TMenuItem
#define Uses_TStatusLine
#define Uses_TStatusItem
#define Uses_TStatusDef
#define Uses_TKeys
#define Uses_TEditor
#define Uses_TDialog
#define Uses_TCheckBoxes
#define Uses_TRadioButtons
#define Uses_TSItem
#define Uses_TFileDialog
#define Uses_TChDirDialog
#define Uses_ofpstream
#define Uses_ifpstream

#include <tv.h>

// Inclusoes locais =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#include "dsgobjs.h"
#include "consted.h"
#include "strmoper.h"

class TFreeDsgnApp: public TApplication
{
public:
    TFreeDsgnApp();
    virtual void handleEvent(TEvent& event);
    static TMenuBar * initMenuBar( TRect r );
    static TStatusLine * initStatusLine( TRect r );
    virtual void outOfMemory();
//  virtual void idle();
private:
    void viewBuffer(TMemoData& r);
    TDialog * ToolDlg;
    TDDialog * EditDialog;
    TWindow * ObjEdit;
    TWindow * srcWindow;
    TEditor * srcBuffer;
    TConstCollection * PrjCommands;
    TConstCollection * PrjHelpCtxs;
    TConstEdit * CommandEditor;
    TConstEdit * HelpCtxEditor;
    TWindow * HintsEditor;
    TWindow * MenusEditor;
    TWindow * StatusDefEditor;
    TWindow * TextViewer;
};

TFreeDsgnApp::TFreeDsgnApp():
              TProgInit(&TFreeDsgnApp::initStatusLine,
                        &TFreeDsgnApp::initMenuBar,
                        &TApplication::initDeskTop), TApplication()
{
   PrjCommands = CommandsList();
   PrjHelpCtxs = HelpCtxsList();
   InitDlgEditor(0);
   EditDialog = DialogEditor();
   ToolDlg = ToolDialog();
   ObjEdit = ObjectEditor();
   getPalette().data[0x3f] = 0x4f;
   getPalette().data[0x3e] = 0x38;
   CommandEditor = HelpCtxEditor = 0;
   HintsEditor = MenusEditor = StatusDefEditor = srcWindow = 0;
}

void TFreeDsgnApp::viewBuffer(TMemoData& rec)
{
   TRect r = deskTop->getExtent();
   srcWindow = new TWindow(r, "Dialog function", 0);
   TScrollBar * vs = new TScrollBar(
      TRect(r.b.x - 1, r.a.y + 1, r.b.x, r.b.y - 1) );
   TScrollBar * hs = new TScrollBar(
      TRect(r.a.x + 1, r.b.y - 1, r.b.x - 1, r.b.y) );
   srcWindow->insert(vs);
   srcWindow->insert(hs);
   r.grow(-1, -1);
   srcBuffer = new TEditor(r, hs, vs, 0, rec.length);
   srcWindow->insert(srcBuffer);
   srcBuffer->insertText(rec.buffer, rec.length, False);
   deskTop->insert(srcWindow);
}

void TFreeDsgnApp::handleEvent(TEvent& event)
{
  TDialog * d;
  TWindow * w;
  TRect r;
  const char * str;

  if (( event.what == evCommand ) && ( event.message.command == cmQuit ))
  {
     if ( messageBox( __("Are you sure?"),
         mfConfirmation | mfYesButton | mfCancelButton ) != cmYes )
         clearEvent(event);
  }
  
  TApplication::handleEvent(event);

  if (event.what == evBroadcast)
  {
     switch(event.message.command)
     {
        case cmClosingWindow:
           w = (TWindow *) event.message.infoPtr;
           if (w == CommandEditor) CommandEditor = 0;
           if (w == HelpCtxEditor) HelpCtxEditor = 0;
           if (w == HintsEditor) HintsEditor = 0;
           if (w == MenusEditor) MenusEditor = 0;
           if (w == StatusDefEditor) StatusDefEditor = 0;
           if (w == EditDialog) EditDialog = 0;
           if (w == ObjEdit) ObjEdit = 0;
           if (w == ToolDlg) ToolDlg = 0;
           if (w == srcWindow) srcWindow = 0;
           break;
        default:;
     }
  }
  else
  if (event.what == evCommand )
  {
     switch(event.message.command)
     {
        case cmNewProject:
             InitDlgEditor(0);
             break;
        case cmShowGrid:
           SetGrid(!GetGrid());
           break;
/*      case cmStatic...cmVoid:
           EditDialog->setCurrentClass(event.message.command);
           break; */
        case cmDialogEditor:
           if (!EditDialog) break;
           if (!indexOf(EditDialog)) insert(EditDialog);
           EditDialog->select();
           break;
        case cmObjEditor:
           if (!ObjEdit) break;
           if (!indexOf(ObjEdit)) insert(ObjEdit);
           ObjEdit->select();
           break;
        case cmObjects:
           if (!ToolDlg) break;
           if (!indexOf(ToolDlg)) insert(ToolDlg);
           ToolDlg->select();
           break;
        case cmDlgRun:
           if (EditDialog) EditDialog->dlgRun();
           break;
//      case cmCalculator:
//         break;
        case cmCommandEdit:
           if (CommandEditor)
              CommandEditor->select();
           else
           {
              CommandEditor = new TConstEdit();
              CommandEditor->Edit(PrjCommands);
           }
           break;
        case cmHelpCtxEdit:
           if (HelpCtxEditor)
              HelpCtxEditor->select();
           else
           {
              HelpCtxEditor = new TConstEdit();
              HelpCtxEditor->Edit(PrjHelpCtxs);
           }
           break;
        case cmViewAppWindow:
           w = new TAppWindow();
           r = w->getExtent();
//         r.b.y = r.a.y + 1;
           ((TAppWindow *)w)->setMenuBar(initMenuBar(r));
//         r = w->getExtent();
//         r.a.y = r.b.y - 1;
           ((TAppWindow *)w)->setStatusLine(initStatusLine(r));
           application->execView(w);
           TObject::CLY_destroy(w);
        break;
        case cmChangeDir:
           d = (TDialog *) validView( new TChDirDialog(0, 0) );
           if (d != 0)
           {
              deskTop->execView(d);
              TObject::CLY_destroy(d);
           }
           break;
        case cmOpenObj:
           str = getFileName(_("Open a dialog"), "*.fd*", 0);
           if (str != 0) InitDlgEditor(str);
           delete[] str;
           break;
        case cmSaveObj:
           if (EditDialog) EditDialog->Save(cmYes);
           break;
        case cmSaveObjAs:
           if (EditDialog)
           {
              str = getFileName(_("Save dialog as"), "*.fdg", 1);
              if (str) EditDialog->saveToFile(str);
              delete[] str;
           }
           break;
        case cmWriteObjFunc:
           if (srcWindow != 0) break;
           TMemoData r;
           strcpy(r.buffer, "");
           buildCode(EditDialog, r.buffer);
           r.length = strlen(r.buffer);
           viewBuffer(r);
           break;
        case cmSave:
           if (srcWindow)
           {
              str = getFileName(_("Save dialog as"), "*.src", 1);
              if (str)
              {
                 ofpstream * s = new ofpstream(str);
                 s->writeBytes(srcBuffer->buffer, srcBuffer->bufLen);
                 s->close();
                 delete s;
                 delete[] str;
              }
           }
           break;
        default: return;
     }
  }
  clearEvent(event);
}

TMenuBar * TFreeDsgnApp::initMenuBar( TRect r )
{
  TMenuItem& menuBuild = *new TSubMenu( __("~B~uild obj code..."), kbNoKey, hcNoContext) +
    *new TMenuItem( __("As ~f~unction"), cmWriteObjFunc, kbNoKey, hcWriteFunc) +
    *new TMenuItem( __("As f~i~le"), cmWriteObjUnit, kbNoKey, hcWriteUnit);

  TSubMenu& menuOptions = *new TSubMenu( __("~O~ptions"), kbNoKey, hcNoContext) +
    *new TMenuItem( __("~G~rid On/Off"), cmShowGrid, kbNoKey, hcNoContext);
    
  TSubMenu& menuProject = *new TSubMenu( __("~P~roject"), kbNoKey, hcNoContext) +
    *new TMenuItem( __("~V~iew..."), cmViewProject, kbNoKey, hcViewProject) +
    *new TMenuItem( __("~M~enu editor..."), cmMenuEditor, kbNoKey, hcMenuEdit) +
    *new TMenuItem( __("~S~tatus bar..."), cmStatusEditor, kbNoKey, hcStatusEdit) +
    *new TMenuItem( __("H~i~nts..."), cmHintsEditor, kbNoKey, hcHindEdit) +
    *new TMenuItem( __("~C~ommands..."), cmCommandEdit, kbNoKey, hcCommandEdit) +
    *new TMenuItem( __("~H~elp contexts..."), cmHelpCtxEdit, kbNoKey, hcHelpCtxEdit) +
    newLine() +
    *new TMenuItem( __("~A~pp options..."), cmAppOptions, kbNoKey, hcAppOptions) +
    *new TMenuItem( __("~G~enerate APP code..."), cmBuildApp, kbNoKey, hcAppBuild) +
    newLine() +
    *new TMenuItem( __("App ~W~indow"), cmViewAppWindow, kbNoKey, hcViewAppWindow);

  TSubMenu& menuEdit = *new TSubMenu( __("~E~dit"), kbNoKey, hcNoContext ) +
//  *new TMenuItem( __("~U~ndelete"), cmUndo, kbAltBack, hcUndo, "Alt+BkSpc") +
    *new TMenuItem( __("~C~opy"), cmCopy, kbShiftIns, hcCopy, "Shift+Ins") +
    *new TMenuItem( __("C~u~t"), cmCut, kbShiftDel, hcCut, "Shift+Del") +
    *new TMenuItem( __("~P~aste"), cmPaste, kbCtrlIns, hcPaste, "Ctrl+Ins") +
    newLine() +
    menuBuild;
    
  TSubMenu& menuFile = *new TSubMenu(__("~F~ile"), kbNoKey, hcNoContext ) +
    *new TMenuItem( __("~N~ew project..."), cmNewProject, kbNoKey, hcNewProject ) +
    *new TMenuItem( __("~O~pen project..."), cmOpenProject, kbNoKey, hcOpenProject ) +
    *new TMenuItem( __("~S~ave project..."), cmSaveProject, kbNoKey, hcSaveProject ) +
    *new TMenuItem( __("Sa~v~e project as..."), cmSaveProjectAs, kbNoKey, hcSavePrjAs ) +
    newLine() +
    *new TMenuItem( __("O~p~en object..."), cmOpenObj, kbNoKey, hcOpenObj ) +
    *new TMenuItem( __("Save ~o~bject..."), cmSaveObj, kbNoKey, hcSaveObj ) +
    *new TMenuItem( __("Save object ~a~s..."), cmSaveObjAs, kbNoKey, hcSaveObjAs ) +
    *new TMenuItem( __("Save ~s~ource..."), cmSave, kbNoKey, hcNoContext ) +
    newLine() +
    *new TMenuItem( __("~C~hange directory..."), cmChangeDir, kbNoKey, hcChangeDir ) +
    *new TMenuItem( __("~Q~uit"), cmQuit, kbNoKey, hcQuit );

  r.b.y = r.a.y + 1;
  return new TMenuBar( r, menuFile +
                          menuEdit +
                          menuProject +
                          menuOptions
                      );
}

TStatusLine * TFreeDsgnApp::initStatusLine( TRect r )
{
#define CommomHide() *new TStatusItem(__("~F1~ Help"), kbF1, cmHelp) + \
                     *new TStatusItem(0, kbAltQ, cmQuit) + \
                     *new TStatusItem(0, kbAltF3, cmClose) + \
                     *new TStatusItem(0, kbF5, cmZoom) + \
                     *new TStatusItem(0, kbCtrlF5, cmResize) + \
                     *new TStatusItem(0, kbF6, cmNext) + \
                     *new TStatusItem(0, kbShiftF6, cmPrev)
//  r = deskTop->getExtent();
  r.a.y = r.b.y - 1;
  return new TStatusLine(r,
    *new TStatusDef( 0, 0xffff ) +
    CommomHide() +
    *new TStatusItem(__("~F7~ Objects"), kbF7, cmObjects ) +
    *new TStatusItem(__("~F8~ Dialog"), kbF8, cmDialogEditor ) +
    *new TStatusItem(__("~F9~ Obj Editor"), kbF9, cmObjEditor ) +
    *new TStatusItem(__("~^F9~ Run dialog"), kbCtrlF9, cmDlgRun ) +
    *new TStatusDef(hcDragging, hcDragging) +
    *new TStatusItem(__("~PgUp/PgDn/Home/End~ to limits"), 0, 0) +
    *new TStatusItem(__("~\x18\x19\x1a\x1b~ Move"), 0, 0 ) +
    *new TStatusItem(__("~^+[\x18\x19\x1a\x1b]~ tab move"), 0, 0 ) +
    *new TStatusItem(__("~Shift+[\x18\x19\x1a\x1b]~ Resize"), 0, 0 ) +
    *new TStatusItem(__("~ENTER~ put"), 0, 0 ) +
    *new TStatusItem(__("~ESC~ cancel"), 0, 0 )
  );
}

void TFreeDsgnApp::outOfMemory()
{
  messageBox( __("Not enough memory for this operation."), mfError | mfOKButton );
}


int main()
{
  const char * _LocaleDir = TVIntl::bindTextDomain("mtv", NULL);
  if (_LocaleDir)
  {
     char * LocaleDir = strdup(_LocaleDir);
     setlocale(LC_ALL, "");                 // Utilizando ambiente LANG
     TVIntl::bindTextDomain("mtv", LocaleDir);      // Ajusta o caminho
     TVIntl::textDomain("mtv");                     // Confirma o arquivo mo
     free(LocaleDir);
  }
  TFreeDsgnApp *app = new TFreeDsgnApp();
 if (app)
  {
    app->run();
    delete app;
  }
  return 0;
}
