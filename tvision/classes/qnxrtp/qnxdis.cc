#include <tv/configtv.h>

#if defined(TVOS_UNIX) && defined(TVOSf_QNXRtP)

#define Uses_stdio
#define Uses_stdlib
#define Uses_unistd
#define Uses_TDisplay
#define Uses_TScreen
#define Uses_string
#include <tv.h>

#include <tv/qnxrtp/screen.h>

#else
// Here to generate the dependencies in RHIDE
#include <tv/qnxrtp/screen.h>
#endif // TVOS_UNIX && TVOSf_QNXRtP
