/* QNX 4 screen routines source.
    */

#include <tv/configtv.h>

#define Uses_TScreen
#define Uses_TEvent
#define Uses_TDrawBuffer
#define Uses_TGKey
#define Uses_TVCodePage
#define Uses_string
#define Uses_ctype
#define Uses_stdio
#define Uses_unistd
#define Uses_fcntl
#define Uses_stdlib
#define Uses_sys_stat

#include <tv.h>

// I delay the check to generate as much dependencies as possible
#if defined(TVOS_UNIX) && defined(TVOSf_QNX4)

#include <tv/qnx4/screen.h>
#include <tv/qnx4/key.h>

#else
// Here to generate the dependencies in RHIDE
#include <tv/qnx4/screen.h>
#include <tv/qnx4/key.h>
#endif // TVOS_UNIX && TVOSf_QNX4
