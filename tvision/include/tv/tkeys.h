/*
  Abstract keys definition by SET.
  Here I define just the keys defined in tkeys.h and not the 2048
  combinations I detect.
*/

#if !defined( __TKEYS_H )
#define __TKEYS_H

#ifdef Uses_TKeys_Extended
// That's a request for ALL the codes
#include <tv/tkeyext.h>
#else

const ushort

// Normal keys
    kbSpace     = 0x0034,

//  Control keys

    kbCtrlA=0x0101,kbCtrlB=0x0102,kbCtrlC=0x0103,kbCtrlD=0x0104,
    kbCtrlE=0x0105,kbCtrlF=0x0106,kbCtrlG=0x0107,kbCtrlH=0x0108,
    kbCtrlI=0x0109,kbCtrlJ=0x010a,kbCtrlK=0x010b,kbCtrlL=0x010c,
    kbCtrlM=0x010d,kbCtrlN=0x010e,kbCtrlO=0x010f,kbCtrlP=0x0110,
    kbCtrlQ=0x0111,kbCtrlR=0x0112,kbCtrlS=0x0113,kbCtrlT=0x0114,
    kbCtrlU=0x0115,kbCtrlV=0x0116,kbCtrlW=0x0117,kbCtrlX=0x0118,
    kbCtrlY=0x0119,kbCtrlZ=0x011a,

// Extended key codes

    kbEsc       = 0x001f,   kbAltSpace  = 0x0234,   kbCtrlIns   = 0x014d,
    kbShiftIns  = 0x00cd,   kbCtrlDel   = 0x014e,   kbShiftDel  = 0x00ce,
    kbCtrlShiftIns = 0x01cd, kbCtrlShiftDel = 0x01ce,
    kbBack      = 0x002a,   kbCtrlBack  = 0x012a,   kbShiftTab  = 0x00ab,
    kbTab       = 0x002b,   kbAltA=0x0201,          kbAltB=0x0202,
    kbAltC=0x0203,          kbAltD=0x0204,          kbAltE=0x0205,
    kbAltF=0x0206,          kbAltG=0x0207,          kbAltH=0x0208,
    kbAltI=0x0209,          kbAltJ=0x020a,          kbAltK=0x020b,
    kbAltL=0x020c,          kbAltM=0x020d,          kbAltN=0x020e,
    kbAltO=0x020f,          kbAltP=0x0210,          kbAltQ=0x0211,
    kbAltR=0x0212,          kbAltS=0x0213,          kbAltT=0x0214,
    kbAltU=0x0215,          kbAltV=0x0216,          kbAltW=0x0217,
    kbAltX=0x0218,          kbAltY=0x0219,          kbAltZ=0x021a,
    kbCtrlEnter=0x012c,     kbEnter=0x002c,         kbF1=0x0039,
    kbF2=0x003a,            kbF3=0x003b,            kbF4=0x003c,
    kbF5=0x003d,            kbF6=0x003e,            kbF7=0x003f,
    kbF8=0x0040,            kbF9=0x0041,            kbF10=0x0042,
    kbF11=0x0043,           kbF12=0x0044,
    kbHome=0x0045,          kbUp=0x0046,            kbPgUp=0x0047,
    kbLeft=0x0048,          kbRight=0x0049,         kbEnd=0x004a,
    kbDown=0x004b,          kbPgDn=0x004c,          kbIns=0x004d,
    kbDel=0x004e,
    // These are normal + & - because I think isn't good make any
    // difference
    kbGrayMinus = 0x0035,   kbGrayPlus  = 0x0036,
    kbShiftF1=0x00b9,       kbShiftF2=0x00ba,       kbShiftF3=0x00bb,
    kbShiftF4=0x00bc,       kbShiftF5=0x00bd,       kbShiftF6=0x00be,
    kbShiftF7=0x00bf,       kbShiftF8=0x00c0,       kbShiftF9=0x00c1,
    kbShiftF10=0x00c2,      kbShiftF11=0x00c3,      kbShiftF12=0x00c4,
    kbCtrlF1=0x0139,        kbCtrlF2=0x013a,        kbCtrlF3=0x013b,
    kbCtrlF4=0x013c,        kbCtrlF5=0x013d,        kbCtrlF6=0x013e,
    kbCtrlF7=0x013f,        kbCtrlF8=0x0140,        kbCtrlF9=0x0141,
    kbCtrlF10=0x0142,       kbCtrlF11=0x0143,       kbCtrlF12=0x0144,
    kbAltF1=0x0239,         kbAltF2=0x023a,         kbAltF3=0x023b,
    kbAltF4=0x023c,         kbAltF5=0x023d,         kbAltF6=0x023e,
    kbAltF7=0x023f,         kbAltF8=0x0240,         kbAltF9=0x0241,
    kbAltF10=0x0242,        kbAltF11=0x0243,        kbAltF12=0x0244,
    kbCtrlPrtSc = 0x0137,   kbCtrlLeft=0x0148,      kbCtrlRight=0x0149,
    kbCtrlEnd=0x014a,       kbCtrlPgDn=0x014c,      kbCtrlHome  = 0x0145,
    kbAlt1=0x0221,          kbAlt2=0x0222,          kbAlt3=0x0223,
    kbAlt4=0x0224,          kbAlt5=0x0225,          kbAlt6=0x0226,
    kbAlt7=0x0227,          kbAlt8=0x0228,          kbAlt9=0x0229,
    kbAlt0      = 0x0220,   kbAltMinus  = 0x0235,   kbAltEqual  = 0x0238,
    kbCtrlPgUp  = 0x0147,   kbNoKey     = 0x0000,
    kbAltBack=0x022a;
#endif

//  Keyboard state and shift masks
const ushort
    kbRightShift  = 0x0001,
    kbLeftShift   = 0x0002,
    kbShift       = kbLeftShift | kbRightShift,
    kbLeftCtrl    = 0x0004,
    kbRightCtrl   = 0x0004,
    kbCtrlShift   = kbLeftCtrl | kbRightCtrl,
    kbLeftAlt     = 0x0008,
    kbRightAlt    = 0x0008,
    kbAltShift    = kbLeftAlt | kbRightAlt,
    kbScrollState = 0x0010,
    kbNumState    = 0x0020,
    kbCapsState   = 0x0040,
    kbInsState    = 0x0080;

    
#endif
