#include <tv/configtv.h>

#ifdef TVCompf_djgpp
#include <dpmi.h>
#include <unistd.h>

void CLY_YieldProcessor(int micros)
{
 __dpmi_yield();
 if (micros>0)
    usleep(micros);
}
#endif

#if defined(TVOSf_QNX4)
   #include <time.h>
   
   static void usleep(unsigned long sleeptime)
   {
      struct timespec wait;
      
      wait.tv_sec=0;
      wait.tv_nsec=sleeptime;
      
      do {
         if (nanosleep(&wait, &wait)==0)
         {
            break;
         }
      } while(1);
   }
#endif // TVOSf_QNX4

#ifdef TVOS_UNIX
#include <unistd.h>
 // See if this system have the POSIX function
 #if 0 //def _POSIX_PRIORITY_SCHEDULING
 #include <sched.h>

 void CLY_YieldProcessor(int micros)
 {
  sched_yield();
  if (micros>0)
     usleep(micros);
 }
 #else
 // No POSIX, just sleep
 void CLY_YieldProcessor(int micros)
 {
  if (micros<0)
     micros=10000; // 10 ms
  usleep(micros);
 }
 #endif
#endif

#ifdef TVComp_BCPP //TVOSf_NT
void CLY_YieldProcessor(int micros)
{
 extern void __tvWin32Yield(int micros);
 if (micros<0)
    micros=27472; // 1000000 / (18.2 * 2)
 __tvWin32Yield(micros);
}
#endif

#if defined(TVOS_Win32) && !defined(TVComp_BCPP) //!defined(TVOSf_NT)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

void CLY_YieldProcessor(int micros)
{
 Sleep(micros/1000);
}
#endif
