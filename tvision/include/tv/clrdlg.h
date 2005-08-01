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

#if defined( Uses_TColorDialog ) && !defined( __TColorDialog )
#define __TColorDialog

class TColorGroup;
struct TEvent;
class TColorDisplay;
class TColorGroupList;
class TLabel;
class TColorSelector;
class TMonoSelector;
class TPalette;

class CLY_EXPORT TColorDialog : public TDialog
{

public:

    TColorDialog( TPalette *aPalette, TColorGroup *aGroups );
    ~TColorDialog();
    virtual uint32 dataSize();
    virtual void getData( void *rec );
    virtual void handleEvent( TEvent& event );
    virtual void setData( void *rec);

    TPalette *pal;

protected:

    TColorDisplay *display;
    TColorGroupList *groups;
    TLabel *forLabel;
    TColorSelector *forSel;
    TLabel *bakLabel;
    TColorSelector *bakSel;
    TLabel *monoLabel;
    TMonoSelector *monoSel;
#if !defined( NO_STREAM )
private:

    
    virtual const char *streamableName() const
        { return name; }

protected:

    TColorDialog( StreamableInit );
    virtual void write( opstream& );
    virtual void *read( ipstream& );

public:

    static const char * const name;
    static TStreamable *build();
#endif
};

#if !defined( NO_STREAM )
inline ipstream& operator >> ( ipstream& is, TColorDialog& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, TColorDialog*& cl )
    { return is >> (void *&)cl; }

inline opstream& operator << ( opstream& os, TColorDialog& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, TColorDialog* cl )
    { return os << (TStreamable *)cl; }
#endif // NO_STREAM

#endif  // TColorDialog

