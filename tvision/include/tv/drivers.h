typedef TScreen *(*drvChecker)();

extern TScreen *TV_LinuxDriverCheck();
extern TScreen *TV_DOSDriverCheck();
extern TScreen *TV_XDriverCheck();
extern TScreen *TV_Win32DriverCheck();
