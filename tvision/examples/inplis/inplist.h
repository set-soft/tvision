/*

  Created by L. Borobia 92-11-30

  File: INPLIST.H

  A little example, how to execute an inputline in a
  focused item from a listbox.

*/

#define Uses_TKeys
#define Uses_TEvent
#define Uses_TRect
#define Uses_TDialog
#define Uses_TInputLine
#define Uses_TScrollBar
#define Uses_TListBox
#define Uses_TCollection
#define Uses_TDialog

#include <tv.h>

#define MAXLABELLEN 40
#define MAXVALUELEN 20

typedef struct
{
  char label[MAXLABELLEN];
  char value[MAXVALUELEN];
} ListBoxItem;

class TLineCollection : public TCollection
{
  public:
    TLineCollection(short lim, short delta) : TCollection(lim, delta) {}
    virtual void  freeItem(void *p) { delete (ListBoxItem *)p; }
  private:
    virtual void *readItem( ipstream& ) { return 0; }
    virtual void writeItem( void *, opstream& ) {}
};

class TExecInputLine : TInputLine
{
  public:
   TExecInputLine(const TRect&, int);
   virtual ushort execute();
   virtual void set(void *rec);
   virtual void get(void *rec);
};

class TInputBox :  public TListBox
{
  public:
    TInputBox( const TRect&, ushort, TScrollBar *);
    virtual void getText( char *, int, short );
    virtual void handleEvent(TEvent& );
    virtual ushort inputData();
};

class TInputDialog : public TDialog
{
 public:
    TInputDialog(const TRect&, const char *, TCollection *);
 private:
    TScrollBar *vScrollBar;
    TInputBox  *inputBox;
};

