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

#if defined( Uses_TScreen ) && !defined( __TScreen )
#define __TScreen

// That's a wrapper to enclose the DOS fonts stuff.
// Under Linux can't be used and just returns error.
class TFont
{
public:
 TFont() {};
 virtual ~TFont() {}; // The class is destroyed
 virtual int SelectFont(int height, int width=8, int bank=0, int noForce=0, int modeRecalculate=1);
 virtual void RestoreDefaultFont(void) {};
 // This flag indicates that the next call must use the BIOS fonts or the
 // original fonts. Lamentably the last is very complicated.
 static  int UseDefaultFontsNextTime;
};

class TDisplay
{

public:

    friend class TView;

    enum videoModes
        {
        smBW80      = 0x0002,
        smCO80      = 0x0003,
        smMono      = 0x0007,
        smFont8x8   = 0x0100
        };

    static void clearScreen( uchar, uchar );

    static ushort getRows();
    static ushort getCols();

    static void setCrtMode( ushort );
    // This version sets the mode through an external program
    static void setCrtMode( char * );
    static ushort getCrtMode();
    static void GetCursor(int &x, int &y);
    static void SetCursor(int x, int y);
    static void setCursorType( ushort );
    static ushort getCursorType();
    static void SetPage(uchar);
    static uchar GetPage() { return Page; }
    static int    SelectFont(int height, int noForce=0, int modeRecalculate=1, int width=8);
    static TFont *GetFontHandler(void) { return font; }
    static void   SetFontHandler(TFont *f);
    static void   RestoreDefaultFont(void);

protected:

    TDisplay() { updateIntlChars(); };
    TDisplay( const TDisplay& ) { updateIntlChars(); };
    ~TDisplay() {};

    static TFont *font;

private:

    static void videoInt();
    static void updateIntlChars();

    static ushort * equipment;
    static uchar * crtInfo;
    static uchar * crtRows;
    static uchar Page;

};

class TScreen : public TDisplay
{

public:

    TScreen();
    ~TScreen();

    static void setVideoMode( ushort mode );
    static void setVideoMode( char *mode );
    static void clearScreen();

    static ushort startupMode;
    static ushort startupCursor;
    static ushort screenMode;
    static uchar screenWidth;
    static uchar screenHeight;
    static Boolean hiResScreen;
    static Boolean checkSnow;
    static long screenBuffer;
    static ushort cursorLines;

    static void setCrtData();
    static ushort fixCrtMode( ushort );

    static void suspend();
    static void resume();
    static void GetCursor(int &x, int &y);
    static void SetCursor(int x, int y);
    static void setCursorType( ushort );
    static ushort getCursorType();
    static ushort getRows();
    static ushort getCols();

    static ushort getCharacter(unsigned offset);
    static void getCharacter(unsigned offset,ushort *buf,unsigned count);
    static void setCharacter(unsigned offset,ushort value);
    static void setCharacter(unsigned offset,ushort *values,unsigned count);

};

#endif  // Uses_TScreen

