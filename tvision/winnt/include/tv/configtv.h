/* Specially created for BC++ and MSVC */

#if defined(_MSVC) || defined(__MSC_VER) || defined(_MSC_VER)
 /* Microsoft C compiler */
 #define TVComp_MSC
#else
 /* Borland C++ compiler */
 #define TVOSf_NT
 #define TVComp_BCPP
#endif

#define TVOS_Win32
#define TVCompf_
#define TVCPU_x86

