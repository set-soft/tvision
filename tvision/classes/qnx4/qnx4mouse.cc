#include <tv/configtv.h>

#if defined(TVOS_UNIX) && defined(TVOSf_QNX4)

#define Uses_stdlib
#define Uses_string
#define Uses_stdio
#define Uses_time
#define Uses_TEvent
#define Uses_TEventQueue
#define Uses_TScreen
#include <tv.h>

#include <tv/qnx4/screen.h>
#include <tv/qnx4/mouse.h>

#else
// Here to generate the dependencies in RHIDE
#include <tv/qnx4/screen.h>
#include <tv/qnx4/mouse.h>
#endif // TVOS_UNIX && TVOSf_QNX4
