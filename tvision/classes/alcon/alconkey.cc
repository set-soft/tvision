// -*- mode:C++; tab-width: 4 -*-
#include <tv/configtv.h>

#define Uses_TScreen
#define Uses_TEvent
#define Uses_TGKey
#include <tv.h>

#include <tv/alcon/key.h>
#define Uses_AlCon_conio
#include <tv/alcon/alcon.h>


void TGKeyAlcon::Init()
{
    TGKey::kbhit = KbHit;
    TGKey::clear = Clear;
    TGKey::gkey = GKey;
//  TGKey::getShiftState=GetShiftState;
//  TGKey::fillTEvent = FillTEvent;
}

int TGKeyAlcon::KbHit()
{
    return AlCon_KbHit();
}

void TGKeyAlcon::Clear()
{
    AlCon_ClearKeyBuf();
}

ushort TGKeyAlcon::GKey()
{
    unsigned aSymbol;
    uchar aScan;
    unsigned aFlags;
    
    return AlCon_GetKey(&aSymbol, &aScan, &aFlags);
}

