// ***********************************************************************
//
//  LST_VIEW.H
//	used with LST_VIEW.CPP
//  List Viewer(s) that display String array(s)
//  revised November 27,1993
//  C.Porter
// ***********************************************************************


class TListViewBox : public TListViewer
{
public:
	TListViewBox( const TRect& bounds, ushort aNumCols,
					TScrollBar *hScrollBar,
					TScrollBar *vScrollBar,
					char *aList[],
					int aListSize);
	~TListViewBox();
	virtual void getText( char *dest, ccIndex item, short maxLen );
	void handleEvent (TEvent& event);
	virtual void putData(void *rec); //writes to focused line of list view
	char **strlist;
};

//*********************
// Dialog that creates 2 List Views and attaches them to scroll bar
// this example is set up for 2 ,easily modified for more
//
class TListViewDialog : public TDialog {
  public:
	TListViewDialog(const TRect &trect, char *title,char *aList[],char *aList2[],
		int aListSize, int wList);
    ~TListViewDialog() { };
    void handleEvent(TEvent &);

  private:
	TScrollBar *listScroller; // scroll bar is used by both list views
    TListViewBox *listBox;
	TListViewBox *listBox2;
	TListViewBox *listBoxPtr; // temp holds active listview ptr during modifications
	TInputLine *inputLine;
	int inputLineLen;
	TInputLine *itemNumber; // displays currently active line, not selectable
};

