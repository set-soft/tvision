/* Specially created for BC++ and MSVC */

#if defined(_MSVC) || defined(__MSC_VER) || defined(_MSC_VER)
 /* Microsoft C compiler */
 #define TVComp_MSC
 #if _MSC_VER <= 1000
   /* MSVC 4.0 and previous versions lacks too much functionality used by this
      driver. So we just disable it. */
   #define TV_Disable_WinGr_Driver
 #endif
#else
 /* Borland C++ compiler */
 #define TVOSf_NT
 #define TVComp_BCPP
#endif

#define TVOS_Win32
#define TVCompf_
#define TVCPU_x86

