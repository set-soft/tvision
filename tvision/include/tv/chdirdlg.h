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

#if defined( Uses_TChDirDialog ) && !defined( __TChDirDialog )
#define __TChDirDialog

const ushort
    cdNormal     = 0x0000, // Option to use dialog immediately
    cdNoLoadDir  = 0x0001, // Option to init the dialog to store on a stream
    cdHelpButton = 0x0002; // Put a help button in the dialog

struct TEvent;
class TInputLine;
class TDirListBox;
class TButton;

class CLY_EXPORT TChDirDialog : public TDialog
{

public:

    friend class TDirListBox;

    TChDirDialog( ushort aOptions, ushort histId );
    virtual uint32 dataSize();
    virtual void getData( void *rec );
    virtual void handleEvent( TEvent& );
    virtual void setData( void *rec );
    virtual Boolean valid( ushort );
    virtual void shutDown();

private:

    void setUpDialog();

    TInputLine *dirInput;
    TDirListBox *dirList;
    TButton *okButton;
    TButton *chDirButton;

#if !defined( NO_STREAM )
    virtual const char *streamableName() const
	{ return name; }

protected:
    TChDirDialog( StreamableInit ) : 
		TWindowInit( &TChDirDialog::initFrame )
		, TDialog( streamableInit ) {}
    virtual void write( opstream& );
    virtual void *read( ipstream& );

public:

    static const char * const name;
    static TStreamable *build();
#endif // NO_STREAM
};

#if !defined( NO_STREAM )
inline ipstream& operator >> ( ipstream& is, TChDirDialog& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, TChDirDialog*& cl )
    { return is >> (void *&)cl; }

inline opstream& operator << ( opstream& os, TChDirDialog& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, TChDirDialog* cl )
    { return os << (TStreamable *)cl; }
#endif // NO_STREAM

#endif  // Uses_TChDirDialog

