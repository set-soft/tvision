#include <tv/configtv.h>

#if defined(TVOS_UNIX) && defined(TVOSf_QNXRtP)

#define Uses_TScreen
#define Uses_TEvent
#define Uses_TDrawBuffer
#define Uses_TGKey
#define Uses_string
#define Uses_ctype
#define Uses_iostream
#include <tv.h>

#include <tv/qnxrtp/screen.h>

TScreen* TV_QNXRtPDriverCheck()
{
 TScreenQNXRtP *drv=new TScreenQNXRtP();
 if (!TScreen::initialized)
   {
    delete drv;
    return 0;
   }
 return drv;
}

#else
// Here to generate the dependencies in RHIDE
#include <tv/qnxrtp/screen.h>
#endif // TVOS_UNIX && TVOSf_QNXRtP
