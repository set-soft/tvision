/*****************************************************************************

  Copyright (c) 2003-2004 by Grzegorz Adam Hankiewicz
  Copyright (c) 2004-2009 by Salvador E. Tropea (SET) <set@ieee.org>
  
  TScreenAlcon
  AlCon driver for Turbo Vision. It uses the conio.h emulation library on top
  of Allegro.

  Contributed by Grzegorz Adam Hankiewicz <gradha@hankiewicz.datanet.co.uk>

  This is part of Turbo Vision ditribution and covered by the same license.

*****************************************************************************/

#include <tv/configtv.h>

#define Uses_stdio // DEBUG
#define Uses_TScreen
#define Uses_TEvent
#define Uses_TGKey
#include <tv.h>

#include <tv/alcon/screen.h>
#include <tv/alcon/mouse.h>
#include <tv/alcon/key.h>
#define Uses_AlCon_conio
#include <tv/alcon/alcon.h>

#ifdef HAVE_ALLEGRO

#define PRINTF(FORMAT, args...)  printf("%s " FORMAT "\n", __PRETTY_FUNCTION__, ## args)

const unsigned foWmin=5, foHmin=7, foWmax=20, foHmax=32;
TScreenFont256  TScreenAlcon::font8x16={ 8,16,AlCon_ShapeFont8x16 };
TScreenFont256  TScreenAlcon::font10x20={ 10,20,AlCon_ShapeFont10x20 };
TScreenFont256 *TScreenAlcon::defaultFont=&font8x16;
uchar           TScreenAlcon::primaryFontChanged=0;
unsigned  TScreenAlcon::fontW;
unsigned  TScreenAlcon::fontWb;
unsigned  TScreenAlcon::fontH;
unsigned  TScreenAlcon::fontSz;

TScreen *TV_AlconDriverCheck()
{
    TScreenAlcon *drv = new TScreenAlcon();
    if (!TScreen::initialized) {
        delete drv;
        return 0;
    }
    return drv;
}


TScreenAlcon::TScreenAlcon()
{
    // TODO: That's wrong we should be shure Allegro will succeed before doing it.
    if (dCB) dCB();

    screenWidth=80; screenHeight=25;
    fontW=8; fontH=16;

    /* Code page */
    optSearch("AppCP",forcedAppCP);
    optSearch("ScrCP",forcedScrCP);
    optSearch("InpCP",forcedInpCP);
    /* User settings have more priority than detected settings */
    codePage=new TVCodePage(forcedAppCP!=-1 ? forcedAppCP : TVCodePage::ISOLatin1Linux,
                            forcedScrCP!=-1 ? forcedScrCP : TVCodePage::ISOLatin1Linux,
                            forcedInpCP!=-1 ? forcedInpCP : TVCodePage::ISOLatin1Linux);
    SetDefaultCodePages(TVCodePage::ISOLatin1Linux,TVCodePage::ISOLatin1Linux,
                        TVCodePage::ISOLatin1Linux);

    long aux;
    if (optSearch("ScreenWidth",aux))
       screenWidth=aux;
    if (optSearch("ScreenHeight",aux))
       screenHeight=aux;
    if (optSearch("FontWidth",aux))
       fontW=aux;
    if (optSearch("FontHeight",aux))
       fontH=aux;

    const char *lang = optSearch("KeyboardMapping");
    if (lang) {
        AlCon_ForcedKeyboard[0] = lang[0];
        if (lang[0])
            AlCon_ForcedKeyboard[1] = lang[1];
    }

    // Set extra cursor bits through config parameters.
    #define READ_BIT(CONFIG_NAME, BIT_NAME) \
    {                                       \
        if (optSearch(CONFIG_NAME, aux))    \
           if (aux)                         \
             AlCon_CursorBits |= BIT_NAME;  \
    }

    READ_BIT("CursorHideNormal", ALCON_CURSOR_HIDE_NORMAL);
    READ_BIT("CursorOnSquare", ALCON_CURSOR_ON_SQUARE);
    READ_BIT("CursorOnCross", ALCON_CURSOR_ON_CROSS);

    #undef READ_BIT

    uchar *fontData=NULL;
    int freeFontData=1;
    TScreenFont256 *secFont=NULL;
    TScreenFont256 *useFont=NULL;

    if (fontW==10 && fontH==20)
       defaultFont=&font10x20;
   
    if (!frCB || !(useFont=frCB(0,fontW,fontH)))
      {
       useFont=defaultFont;
       freeFontData=0;
      }

    fontW=useFont->w;
    fontH=useFont->h;
    fontWb=(useFont->w+7)/8;
    fontSz=fontWb*fontH;
    fontData=useFont->data;

    // Create screen.
    TScreenColor *pal=parseUserPalette() ? UserStartPalette : PC_BIOSPalette;
    int res=AlCon_Init(screenWidth,screenHeight,fontW,fontH,fontData,(AlCon_Color *)pal);
    if (freeFontData)
      {/* Provided by the call back */
       DeleteArray(useFont->data);
       delete useFont;
      }
    if (res)
       return;

    // Secondary font.
    aux=0;
    if (frCB && optSearch("LoadSecondaryFont",aux) && aux)
      {
       secFont=frCB(1,fontW,fontH);
       if (secFont)
         {
          AlCon_SetFont(1,secFont->data,fontW,fontH);
          DeleteArray(secFont->data);
          delete secFont;
         }
      }

    /*
        Even though we initialised AlCon with a wanted screen
        width and height, it might have had to change it in order
        to accomodate the font which was loaded.
    */
    screenWidth = AlCon_ScreenCols();
    screenHeight = AlCon_ScreenRows();
    memcpy(ActualPalette,AlCon_CurPalette,sizeof(ActualPalette));
    memcpy(OriginalPalette,AlCon_CurPalette,sizeof(OriginalPalette));

    // Find font geometry
    AlCon_GetFontGeometry(&fontW,&fontH);
    fontWb=(fontW+7)/8;
    fontSz=fontWb*fontH;
    
    TDisplayAlcon::Init();

    TScreen::Resume=Resume;
    TScreen::Suspend=Suspend;
    TScreen::clearScreen=clearScreen;
    TScreen::getCharacters=getCharacters;
    TScreen::getCharacter=getCharacter;
    TScreen::setCharacter=setCharacter;
    TScreen::setCharacters=setCharacters;
    TScreen::setDisPaletteColors=SetDisPaletteColors;
    // Fonts stuff
    TScreen::getFontGeometry=GetFontGeometry;
    TScreen::getFontGeometryRange=GetFontGeometryRange;
    TScreen::setFont_p=SetFont;
    TScreen::restoreFonts=RestoreFonts;
    TScreen::setCrtModeRes_p=SetCrtModeRes;
    initialized=1;

    TGKeyAlcon::Init();
    THWMouseAlcon::Init();

    // Tell TScreen that there is a cursor.
    setCrtData();
    startupCursor = cursorLines;
    startupMode = screenMode;

    // Create memory buffer for screen. We want a buffer (even though
    // AlCon is buffered) because otherwise TVision will use simple
    // memcpy calls to "paint" the screen. We don't want this, because
    // Allegro has to be told when to update the screen. So it's a
    // buffer on top of Allegro's buffer so that Allegro get's called.
    // Wicked. But works.
    screenBuffer=new uint16[screenWidth*screenHeight];
    
    flags0|=CanSetPalette | CanReadPalette | CanSetBFont | CanSetSBFont |
            CanSetVideoSize;

    suspended=0;
}

void TScreenAlcon::Resume()
{
 AlCon_Redraw();
 AlCon_EnableAsync();
}

void TScreenAlcon::Suspend()
{
 AlCon_DisableAsync();
}

TScreenAlcon::~TScreenAlcon()
{
    delete[] screenBuffer;
    
    // Deinitialize all.
    AlCon_Exit();
}


void TScreenAlcon::clearScreen()
{
    TDisplay::clearScreen(AlCon_ScreenCols(), AlCon_ScreenCols());
}


ushort TScreenAlcon::getCharacter(unsigned offset)
{
    ushort buffer;
    AlCon_GetScrChars(offset, &buffer, 1);
    return buffer;
}


void TScreenAlcon::getCharacters(unsigned offset, ushort *buffer, unsigned count)
{
    AlCon_GetScrChars(offset, buffer, count);
}


void TScreenAlcon::setCharacter(unsigned offset, uint32 value)
{
    AlCon_PutChar(offset, value);
}


void TScreenAlcon::setCharacters(unsigned offset, ushort *values, unsigned count)
{
    AlCon_PutBuf(offset, values, count);
}


int TScreenAlcon::GetFontGeometry(unsigned &w, unsigned &h)
{
 w=fontW;
 h=fontH;
 return 1;
}

int TScreenAlcon::GetFontGeometryRange(unsigned &wmin, unsigned &hmin,
                                       unsigned &wmax, unsigned &hmax)
{
 wmin=foWmin;
 hmin=foHmin;
 wmax=foWmax;
 hmax=foHmax;
 return 1;
}

int TScreenAlcon::SetFont(int changeP, TScreenFont256 *fontP,
                          int changeS, TScreenFont256 *fontS,
                          int fontCP, int appCP)
{
 if (!changeP && !changeS) return 1;
 // Check for restore fonts
 if (changeP && !fontP && ((!changeS && !useSecondaryFont) || (changeS && !fontS)))
    fontP=defaultFont;

 // Solve the sizes
 unsigned wP, hP, wS, hS;
 if (changeP)
   {
    if (fontP)
      {
       wP=fontP->w;
       hP=fontP->h;
      }
    else
      {
       wP=defaultFont->w;
       hP=defaultFont->h;
      }
   }
 else
   {
    wP=fontW;
    hP=fontH;
   }
 if (changeS)
   {
    if (fontS)
      {
       wS=fontS->w;
       hS=fontS->h;
      }
    else
      {// Disabled
       wS=wP;
       hS=hP;
      }
   }
 else
   {
    if (useSecondaryFont)
      {
       wS=fontW;
       hS=fontH;
      }
    else
      {// Disabled
       wS=wP;
       hS=hP;
      }
   }
 // Size missmatch
 if (wP!=wS || hP!=hS) return 0;
 // This driver currently doesn't support changing the font size on the fly
 //if (wP!=fontW || hP!=fontH) return 0;
 // Check if the size is in the range
 if (wP<foWmin || wP>foWmax || hP<foHmin || hP>foHmax)
    return 0;

 // Change the requested fonts
 if (changeP)
   {
    if (fontP && fontP->data)
      {
       AlCon_SetFont(0,fontP->data,wP,hP);
       primaryFontChanged=1;
      }
    else
      {
       AlCon_SetFont(0,defaultFont->data,wP,hP);
       primaryFontChanged=0;
      }
   }
 if (changeS)
   {
    if (fontS)
      {
       AlCon_SetFont(1,fontS->data,wP,hP);
       useSecondaryFont=1;
       AlCon_EnableSecFont();
      }
    else
      {
       useSecondaryFont=0;
       AlCon_DisableSecFont();
      }
   }
 // Change the code page
 if (changeP && fontCP!=-1)
   {
    if (appCP==-1)
       TVCodePage::SetScreenCodePage(fontCP);
    else
       TVCodePage::SetCodePage(appCP,fontCP,-1);
   }
 if (wP!=fontW || hP!=fontH)
   {
    DoResize(wP,hP);
   }
 AlCon_Redraw();

 return 1;
}

void TScreenAlcon::RestoreFonts()
{
 SetFont(1,NULL,1,NULL,TVCodePage::ISOLatin1Linux,TVCodePage::ISOLatin1Linux);
}

int TScreenAlcon::SetDisPaletteColors(int from, int number, TScreenColor *colors)
{
 int i=AlCon_SetDisPaletteColors(from,number,(AlCon_Color *)colors);
 AlCon_Redraw();
 return i;
}

/**[txh]********************************************************************

  Description:  Called to perform a screen resize. The fonts for the new
  screen size have already been loaded. The parameters indicate the size
  in pixels of each text cell. Therefore, this function adjusts the cursor,
  some internal variables, and requests Allegro to set a different size.
  
***************************************************************************/

void TScreenAlcon::DoResize(unsigned w, unsigned h)
{
 ASSERT(w>0);
 ASSERT(h>0);
 
 int ret=AlCon_Resize(screenWidth,screenHeight,w,h);
 if (ret!=0)
   {
    PRINTF("Ayeeee! A monkey with three heads! %d\n", ret);
    exit(ret);
   }
 if (w!=fontW || h!=fontH)
   {
    int cShapeFrom, cShapeTo;
    AlCon_GetCursorShape(&cShapeFrom,&cShapeTo);
    double start=cShapeFrom/(double)fontH;
    double end  =cShapeTo/(double)fontH;
    fontW=w;
    fontWb=(w+7)/8;
    fontH=h;
    fontSz=fontWb*h;
    
    /* Change the cursor shape */
    cShapeFrom=int(start*fontH+0.5);
    cShapeTo  =int(end*fontH+0.5);
    if ((unsigned)cShapeFrom>=fontH) cShapeFrom=fontH-1;
    if ((unsigned)cShapeTo>=fontH)   cShapeTo=fontH-1;
    AlCon_SetCursorShape(cShapeFrom,cShapeTo);
   }

 /* Compute cursor position and draw it */
 AlCon_GotoXY(AlCon_WhereX(),AlCon_WhereY());
}

int TScreenAlcon::SetCrtModeRes(unsigned w, unsigned h, int fW, int fH)
{
 if (fW==-1) fW=fontW;
 if (fH==-1) fH=fontH;
 if (w==screenWidth && h==screenHeight &&
     fontW==(unsigned)fW && fontH==(unsigned)fH) return 0;

 unsigned nW=fontW, nH=fontH;
 TScreenFont256 *nFont=NULL,*nsFont=NULL;
 int releaseFont=0, resetFont=0;
 // releaseSFont=0,

 // Solve the fonts, don't change them yet.
 if ((unsigned)fW!=fontW || (unsigned)fH!=fontH)
   {
    if (primaryFontChanged)
      {// The application set a font, ask for this new one
       if (frCB && (nFont=frCB(0,fW,fH)))
          releaseFont=1;
       else
         {// No replacement available, revert to our font.
          resetFont=1;
          nFont=&font8x16;
         }
      }
    else
      {
       if (fW==8 && fH==16)
          resetFont=1, nFont=&font8x16;
       else if (frCB && (nFont=frCB(0,fW,fH)))
          releaseFont=1;
       else
          resetFont=1, nFont=&font8x16;
      }
    nW=nFont->w;
    nH=nFont->h;
    if ((nW!=fontW || nH!=fontH) && useSecondaryFont)
      {
       if (frCB)
          nsFont=frCB(1,nW,nH);
//        releaseSFont=1;
      }
   }


 AlCon_DisableAsync();
 if (nFont)
   {
    AlCon_SetFont(0,nFont->data,nW,nH);
    if (resetFont)
       primaryFontChanged=0;
    if (releaseFont)
      {
       DeleteArray(nFont->data);
       delete nFont;
      }
   }
 if (useSecondaryFont)
   {
    if (nsFont)
       AlCon_SetFont(1,nsFont->data,nW,nH);
   }
 // Should I check the size?
 screenWidth=w; screenHeight=h;

 delete[] screenBuffer;
 screenBuffer=new uint16[screenWidth*screenHeight];
 memset(screenBuffer,0,screenWidth*screenHeight*sizeof(uint16));
 AlCon_EnableAsync();

 DoResize(nW,nH);

 return (nW==(unsigned)fW && nH==(unsigned)fH) ? 1 : 2;
}

#endif // HAVE_ALLEGRO
