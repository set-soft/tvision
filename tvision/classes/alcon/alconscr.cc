// -*- mode:C++; tab-width: 4 -*-
#include <tv/configtv.h>

//#define Uses_stdio // DEBUG

#define Uses_TScreen
#define Uses_TEvent
#define Uses_TGKey
#include <tv.h>

#include <tv/alcon/screen.h>
#include <tv/alcon/mouse.h>
#include <tv/alcon/key.h>
#define Uses_AlCon_conio
#include <tv/alcon/alcon.h>

#define PRINTF(FORMAT, args...)  printf("%s " FORMAT "\n", __PRETTY_FUNCTION__, ## args)

const unsigned foWmin=5, foHmin=7, foWmax=20, foHmax=32;
TScreenFont256  TScreenAlcon::font8x16={ 8,16,AlCon_ShapeFont8x16 };
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

    uchar *fontData=NULL;
    int freeFontData=1;
    TScreenFont256 *secFont=NULL;
    TScreenFont256 *useFont=NULL;
   
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

    TScreen::clearScreen=clearScreen;
    
    TScreen::getCharacters=getCharacters;
    TScreen::getCharacter=getCharacter;
    TScreen::setCharacter=setCharacter;
    TScreen::setCharacters=setCharacters;
    TScreen::setDisPaletteColors=SetDisPaletteColors;
    // Fonts stuff
    TScreen::getFontGeometry=GetFontGeometry;
    // The following can be enabled when SetFont implements a "resize"
    //TScreen::getFontGeometryRange=GetFontGeometryRange;
    TScreen::setFont_p=SetFont;
    TScreen::restoreFonts=RestoreFonts;
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
    screenBuffer = new uint16[screenWidth * screenHeight];
    
    flags0|=CanSetPalette | CanReadPalette | CanSetBFont | CanSetSBFont |
            // TODO: ¡¡Mentira!! es sólo para probar ;-), falta implementar el resize.
            CanSetVideoSize;
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
 if (wP!=fontW || hP!=fontH) return 0;
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

