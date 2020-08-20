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

#define Uses_TScreen
#define Uses_TRadioButtons
#define Uses_TMenuBox
#define Uses_TFrame
#define Uses_TIndicator
#define Uses_THistory
#define Uses_TColorSelector
#define Uses_TMonoSelector
#define Uses_TColorDialog
#define Uses_TInputLine
#define Uses_TStatusLine
#define Uses_TCheckBoxes
#define Uses_TScrollBar
#define Uses_TButton
#define Uses_TDirListBox
#define Uses_TFileEditor
#define Uses_TFileInfoPane
#define Uses_TDeskTop
#define Uses_TListViewer
#include <tv.h>

// All the strings are arrays ASCIIZ
// All duplicated to restore the original

uchar TView::specialChars[] =
{
    175, 174, 26, 27, ' ', ' ', 0
};
uchar TView::ospecialChars[] =
{
    175, 174, 26, 27, ' ', ' ', 0
};

uchar TView::noMoireUnFill=' ';  // Just space
uchar TView::noMoireFill=0xDB;   // Full block
uchar TView::onoMoireFill=0xDB;  // Full block

char TRadioButtons::button[] = " ( ) ";
char TRadioButtons::obutton[] = " ( ) ";
char TRadioButtons::check = 7; // 
char TRadioButtons::ocheck = 7;

//" ÚÄ¿  ÀÄÙ  ³ ³  ÃÄ´ " 8
char TMenuBox::frameChars[] = " \332\304\277  \300\304\331  \263 \263  \303\304\264 ";
char TMenuBox::oframeChars[] = " \332\304\277  \300\304\331  \263 \263  \303\304\264 ";
char TMenuBox::rightArrow = 16; // 
char TMenuBox::orightArrow = 16; // 

const char TFrame::initFrame[19] =
  "\x06\x0A\x0C\x05\x00\x05\x03\x0A\x09\x16\x1A\x1C\x15\x00\x15\x13\x1A\x19";

char TFrame::frameChars[33] =
    "   À ³ÚÃ ÙÄÁ¿´ÂÅ   È ºÉÇ ¼ÍÏ»¶Ñ "; // for UnitedStates code page
char TFrame::oframeChars[33] =
    "   À ³ÚÃ ÙÄÁ¿´ÂÅ   È ºÉÇ ¼ÍÏ»¶Ñ "; // for UnitedStates code page

// þ  
char TFrame::closeIcon[] = "[~\xFE~]";
char TFrame::ocloseIcon[] = "[~\xFE~]";
char TFrame::zoomIcon[] = "[~\x18~]";
char TFrame::ozoomIcon[] = "[~\x18~]";
char TFrame::unZoomIcon[] = "[~\x12~]";
char TFrame::ounZoomIcon[] = "[~\x12~]";
// Note: Eddie proposed 0xF (WHITE SUN WITH RAYS) but this reduces to * and
// is the same as 0xFE (BLACK SQUARE) (also reduced to *).
char TFrame::animIcon[] = "[~+~]";
char TFrame::oanimIcon[] = "[~+~]";
char TFrame::dragIcon[] = "~ÄÙ~";
char TFrame::odragIcon[] = "~ÄÙ~";

char TIndicator::dragFrame = '\xCD';  // Í
char TIndicator::odragFrame = '\xCD';  // Í
char TIndicator::normalFrame = '\xC4'; // Ä
char TIndicator::onormalFrame = '\xC4'; // Ä
char TIndicator::modifiedStar = 15; // 
char TIndicator::omodifiedStar = 15; // 

char THistory::icon[] = "\xDE~\x19~\xDD"; // ÞÝ
char THistory::oicon[] = "\xDE~\x19~\xDD"; // ÞÝ

char TColorSelector::icon = '\xDB'; // Û
char TColorSelector::oicon = '\xDB'; // Û
char TColorSelector::mark = 8;
char TColorSelector::omark = 8;

char TMonoSelector::button[] = " ( ) ";
char TMonoSelector::obutton[] = " ( ) ";

char TInputLineBase::rightArrow = '\x10';
char TInputLineBase::orightArrow = '\x10';
char TInputLineBase::leftArrow = '\x11';
char TInputLineBase::oleftArrow = '\x11';

char TStatusLine::hintSeparator[] = "\xB3 "; // ³
char TStatusLine::ohintSeparator[] = "\xB3 "; // ³

char TCheckBoxes::button[] = " [ ] ";
char TCheckBoxes::obutton[] = " [ ] ";

TScrollChars TScrollBar::vChars  = {30, 31, char(177), char(254), char(178)}; // ±þ²
TScrollChars TScrollBar::ovChars = {30, 31, char(177), char(254), char(178)}; // ±þ²
TScrollChars TScrollBar::hChars  = {17, 16, char(177), char(254), char(178)}; // ±þ²
TScrollChars TScrollBar::ohChars = {17, 16, char(177), char(254), char(178)}; // ±þ²

char TButton::shadows[] = "\xDC\xDB\xDF"; // ÜÛß
char TButton::oshadows[] = "\xDC\xDB\xDF"; // ÜÛß
char TButton::markers[] = "[]";
char TButton::omarkers[] = "[]";

char TDirListBox::pathDir[]   = "ÀÄÂ";
char TDirListBox::opathDir[]   = "ÀÄÂ";
char TDirListBox::firstDir[]  =   "ÀÂÄ";
char TDirListBox::ofirstDir[]  =   "ÀÂÄ";
char TDirListBox::middleDir[] =   " ÃÄ";
char TDirListBox::omiddleDir[] =   " ÃÄ";
char TDirListBox::lastDir[]   =   " ÀÄ";
char TDirListBox::olastDir[]   =   " ÀÄ";
char TDirListBox::graphics[]  = "ÀÃÄ";
char TDirListBox::ographics[]  = "ÀÃÄ";

const char * TFileInfoPane::pmText = "p";
const char * TFileInfoPane::amText = "a";

char TDeskTop::defaultBkgrnd = '\xB0';
char TDeskTop::odefaultBkgrnd = '\xB0';

uchar TListViewer::columnSeparator = 179; // ³
uchar TListViewer::ocolumnSeparator = 179; // ³

