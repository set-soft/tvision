#include <tv/configtv.h>

#if defined(TVOS_UNIX) && defined(TVOSf_QNX4)

#include <tv/qnx4/screen.h>
#include <tv/qnx4/mouse.h>


#else
// Here to generate the dependencies in RHIDE
#include <tv/qnx4/screen.h>
#include <tv/qnx4/mouse.h>
#endif // TVOS_UNIX && TVOSf_QNX4
