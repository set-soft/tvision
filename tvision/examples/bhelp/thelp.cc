/**[txh]********************************************************************

  Copyright (c) 1999 by Andreas F. Gorup von Besanez <besanez@compuserve.de>
  Copyright (c) 2002 by Jan Van Buggenhout <Chipzz@ULYSSIS.Org>
  Copyright (c) 2002-2003 by Salvador E. Tropea <set@ieee.org>

  Covered by the GPL license.

  Description:
  This program reads the help files used by Turbo C++ and Borland C++
compilers.
  
***************************************************************************/

#define Uses_stdlib
#define Uses_stdio
#define Uses_access
#define Uses_string
#define Uses_TKeys
#define Uses_ifpstream
#define Uses_IOS_BIN
#define Uses_TCollection
#define Uses_THelpWindow
#define Uses_TApplication
#define Uses_TDialog
#define Uses_TButton
#define Uses_TListBox
#define Uses_TPalette
#define Uses_TDeskTop
#define Uses_getopt
#define Uses_MsgBox
#include <tv.h>
#include "bhelp.h"

class THelp:public TApplication
{
public:
  THelp();
  virtual TPalette& getPalette() const;
  static TDeskTop *initDeskTop( TRect r );
  virtual void getEvent(TEvent& event);
  virtual void run();
  static void ParseCommandLine(int argc, char *argv[]);

  static const char *fileName;
};

const char *THelp::fileName="tchelp.tch";

THelp::THelp():TProgInit(0, 0, &THelp::initDeskTop)
{
}

#define cpAppColor \
 "\x71\x70\x78\x74\x20\x28\x24\x17\x1F\x1A\x31\x31\x1E\x71\x1F" \
 "\x37\x3F\x3A\x13\x13\x3E\x21\x3F\x70\x7F\x7A\x13\x13\x70\x7F\x7E" \
 "\x70\x7F\x7A\x13\x13\x70\x70\x7F\x7E\x20\x2B\x2F\x78\x2E\x70\x30" \
 "\x3F\x3E\x1F\x2F\x1A\x20\x72\x31\x31\x30\x2F\x3E\x31\x13\x38\x00" \
 "\x17\x1F\x1A\x71\x71\x1E\x17\x1F\x1E\x20\x2B\x2F\x78\x2E\x10\x30" \
 "\x3F\x3E\x70\x2F\x7A\x20\x12\x31\x31\x30\x2F\x3E\x31\x13\x38\x00" \
 "\x37\x3F\x3A\x13\x13\x3E\x30\x3F\x3E\x20\x2B\x2F\x78\x2E\x30\x70" \
 "\x7F\x7E\x1F\x2F\x1A\x20\x32\x31\x71\x70\x2F\x7E\x71\x13\x78\x00" \
 "\x37\x3F\x3A\x13\x13\x30\x3E\x1E"    // help colors

TPalette& THelp::getPalette() const
{
  static TPalette helpcolor ( cpAppColor , sizeof( cpAppColor )-1 );
  return helpcolor;
}

TDeskTop *THelp::initDeskTop( TRect r )
{
  return new TDeskTop( r );
}

void THelp::getEvent(TEvent &event)
{
  if (event.what == evKeyDown)
  {
    switch (event.keyDown.keyCode)
    {
      case kbF5:
        event.what = evCommand;
        event.message.infoPtr = 0;
        event.message.command = cmZoom;
        return;
      case kbCtrlF5:
        event.what = evCommand;
        event.message.infoPtr = 0;
        event.message.command = cmResize;
        return;
    }
  }
  TApplication::getEvent(event);
}

char *searchstring = NULL;
int maxlength = 0;
int which = 0;

class TSearchDialog : public TDialog
{
  public:
  TSearchDialog(TCollection *results) :
    TWindowInit( &TSearchDialog::initFrame ),
    TDialog (TRect (0, 0, maxlength + 4, results->getCount() + 6), "Select Result")
  {
    TRect r = getExtent();
    r.grow(-2,-2);
    r.b.y -= 2;
    options |= ofCentered;
    TListBox *resultList = new TListBox( r, 1, 0);
    resultList->newList(results);
    insert(resultList);
    r.a.x--;
    r.a.y = r.b.y + 1;
    r.b.y += 3;
    insert(new TButton(r, __("~V~iew"), cmOK, bfDefault));
    setCurrent(resultList, normalSelect);
  };
};

void setmaxlength(void *str, void *u)
{
  int l = strlen((char *) str);
  if ( l > maxlength) maxlength = l;
}

ushort getContext(TCollection *results)
{
  char *result = (char *) results->at(which);
  char *res = result + strlen(result) + 1;
  return *((ushort *) res);
}

void THelp::run()
{
  if (access(fileName,R_OK)!=0)
  {
    messageBox(__("File not found"), mfOKButton | mfError);
    endModal(cmQuit);
    return;
  }
  fpstream *helpStrm = new fpstream(fileName, CLY_IOSIn);
  TBorlandHelpFile *helpFile = new TBorlandHelpFile(*helpStrm);
  ushort start = helpFile->indexscreen;
  if (searchstring && *searchstring)
  {
    TCollection *results = helpFile->search(searchstring);
    switch (results->getCount())
    {
      case 0:
        break;
      case 1:
        start = getContext(results);
        break;
      default:
      {
        results->forEach(setmaxlength, 0);
        TDialog *SD = new TSearchDialog(results);
        if (execView(SD) == cmOK)
        {
          char* rec = (char *) malloc(SD->dataSize());
          SD->getData(rec);
          which = *((int *) (rec + 4));
          start = getContext(results);
        }
        break;
      }
    }
  }
  THelpWindow *helpWindow = new THelpWindow(helpFile, start);
  execView(validView(helpWindow));
}

static
struct CLY_option longopts[] =
{
  { "file",           0, 0, 'f' },
  { "help",           0, 0, 'h' },
  { 0, 0, 0, 0 }
};

void THelp::ParseCommandLine(int argc, char *argv[])
{
 int optc;

 while ((optc=CLY_getopt_long(argc,argv,"f:h",longopts,0))!=EOF)
   {
    switch (optc)
      {
       case 'f':
            fileName=CLY_optarg;
            break;
       case 'h':
       default:
            printf("Borland's Help file viewer\n");
            printf("Copyright (c) 1999 by Andreas F. Gorup von Besanez <besanez@compuserve.de>\n");
            printf("Copyright (c) 2002 by Jan Van Buggenhout <Chipzz@ULYSSIS.Org>\n");
            printf("Copyright (c) 2002-2003 by Salvador E. Tropea <set@ieee.org>\n");
            printf("This program is covered by the GPL license.\n");
            printf("\n\n");
            printf("%s [--file Name] Text_to_search\n",argv[0]);
            printf("--file Name    : Name of the help file\n");
            printf("--help         : This text\n");
            exit(1);
      }
   }
}

int main(int argc, char ** argv)
{
  THelp::ParseCommandLine(argc,argv);
  THelp app;
  if (argc == 2)
    searchstring = argv[1];

  app.run();
  app.shutDown();
  return 0;
}
