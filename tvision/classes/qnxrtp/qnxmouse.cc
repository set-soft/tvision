#include <tv/configtv.h>

#if defined(TVOS_UNIX) && defined(TVOSf_QNXRtP)

#define Uses_stdlib
#define Uses_string
#define Uses_stdio
#define Uses_time
#define Uses_TEvent
#define Uses_TEventQueue
#define Uses_TScreen
#include <tv.h>
#include <termios.h>
#include <tv/qnxrtp/screen.h>
#include <tv/qnxrtp/mouse.h>


#else
// Here to generate the dependencies in RHIDE
#include <tv/qnxrtp/screen.h>
#include <tv/qnxrtp/mouse.h>
#endif // TVOS_UNIX && TVOSf_QNXRtP
