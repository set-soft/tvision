/* Specially created for BC++ and MSVC */

#if defined(_MSVC) || defined(__MSC_VER) || defined(_MSC_VER)
 /* Microsoft C compiler */
 #define TVComp_MSC
 /* MSVC 4.0 and previous versions lacks too much functionality used by this
    driver. So we just disable it. (v 1000) */
 /* MSVC 6.0 also failed to compile it. So we are just disabling it until
    somebody fixes the problems. (v 1200) */
 #define TV_Disable_WinGr_Driver
#elif defined(__WATCOMC__)
 #define TVOSf_NT
 #define TVComp_Watcom
#else
 /* Borland C++ compiler */
 #define TVOSf_NT
 #define TVComp_BCPP
#endif

#define TVOS_Win32
#define TVCompf_
#define TVCPU_x86

