#include <tv/configtv.h>

#if defined(TVOS_UNIX) && defined(TVOSf_QNXRtP)

#define Uses_stdio
#define Uses_unistd
#define Uses_ctype
#define Uses_stdlib
#define Uses_TEvent
#define Uses_TGKey
#define Uses_FullSingleKeySymbols
#define Uses_string
#include <tv.h>
#include <tv/qnxrtp/key.h>


#else
// Here to generate the dependencies in RHIDE
#include <tv/qnxrtp/key.h>
#endif // TVOS_UNIX && TVOSf_QNXRtP
