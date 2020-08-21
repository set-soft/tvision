/*---------------------------------------------------------*/
/*                                                         */
/*   MakeRez.cpp                                           */
/*   Creates a MyDialog (derives from TDialog) Resource    */
/*                                                         */
/*   based on Turbo Vision Hello World Demo Source File    */
/*---------------------------------------------------------*/
#define Uses_TObject
#define Uses_TKeys
#define Uses_TApplication
#define Uses_TEvent
#define Uses_TRect
#define Uses_TDialog
#define Uses_TStaticText
#define Uses_TButton
#define Uses_TMenuBar
#define Uses_TSubMenu
#define Uses_TMenuItem
#define Uses_TStatusLine
#define Uses_TStatusItem
#define Uses_TStatusDef
#define Uses_TDeskTop
#define Uses_TStreamableClass
#define Uses_TResourceFile
#define Uses_TResourceCollection
#define Uses_MsgBox
#define Uses_fpstream
#define Uses_string

#include <tv.h>
#include "tprogbar.h"

__link(RWindow);
__link(RGroup);
__link(RDialog);
__link(RFrame);
__link(RResourceCollection);
__link(RCluster);
__link(RButton);
__link(RProgressBar);

#include <stdlib.h>

const char rezFileName[] = "MY.REZ";

void makerez()
{
   fpstream *ofps=0;


   CLY_std(cout)<< "Creating "<< rezFileName << CLY_std(endl);

   ofps = new fpstream( rezFileName, CLY_IOSOut | CLY_IOSBin );
   if( !ofps->good() )
    {
      CLY_std(cerr)<< rezFileName <<": init failed..."<<CLY_std(endl);
      exit(1);
    }

   TResourceFile *myRez;
   myRez = new TResourceFile( ofps );
   if( !myRez )
   {
     CLY_std(cerr)<< "Resource file init failed..."<<CLY_std(endl);
     exit(1);
   }

   TDialog *pd = new TDialog(TRect(0,0,60,15),"Example Progress Bar");
   pd->options |= ofCentered;
   pd->flags &= ~wfClose;
   TProgressBar *pbar = new TProgressBar(TRect(2,2,pd->size.x-2,3),300);
   pd->insert(new TButton(TRect(10,pd->size.y-3,pd->size.x-10,pd->size.y-1),"~C~ancel",cmCancel,bfDefault));

   if( !pbar )
   {
    CLY_std(cerr)<< "Progress Bar init failed..."<<CLY_std(endl);
    exit(1);
   }

   if( !pd )
   {
    CLY_std(cerr)<< "Dialog Box init failed..."<<CLY_std(endl);
    exit(1);
   }

   /*
      We need a separate TProgressBar stream because you need that object
      visible outside the dialog box in order to update it via
               TProgressBar::update(double progress);
   */
   myRez->put( pbar, "theProgressBar");

   // Here is the stream for the dialog box.
   myRez->put( pd, "theDialogBox");

   TObject::CLY_destroy( pd );
   TObject::CLY_destroy( pbar );
   TObject::CLY_destroy( myRez);
}

int main()
{
    makerez();
    return 0;
}
