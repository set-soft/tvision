/* QNX 4 keyboard handler routines source.
    */

#include <tv/configtv.h>

#define Uses_stdio
#define Uses_unistd
#define Uses_ctype
#define Uses_stdlib
#define Uses_TEvent
#define Uses_TGKey
#define Uses_TKeys
#define Uses_TKeys_Extended
#define Uses_string
#include <tv.h>

// I delay the check to generate as much dependencies as possible
#if defined(TVOS_UNIX) && defined(TVOSf_QNX4)

#include <tv/qnx4/key.h>

#else
// Here to generate the dependencies in RHIDE
#include <tv/qnx4/key.h>
#endif // TVOS_UNIX && TVOSf_QNX4
