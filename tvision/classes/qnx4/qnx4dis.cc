/* QNX 4 screen routines source.
    */

#include <tv/configtv.h>

#define Uses_stdio
#define Uses_stdlib
#define Uses_unistd
#define Uses_TDisplay
#define Uses_TScreen
#define Uses_string

#include <tv.h>

// I delay the check to generate as much dependencies as possible
#if defined(TVOS_UNIX) && defined(TVOSf_QNX4)

#include <tv/qnx4/screen.h>

#else
// Here to generate the dependencies in RHIDE
#include <tv/qnx4/screen.h>
#endif // TVOS_UNIX && TVOSf_QNX4
