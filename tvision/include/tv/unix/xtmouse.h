/* XTerm mouse handler routines header.
   Copyright (c) 2002 by Salvador E. Tropea (SET)
   Covered by the GPL license. */
#if defined(TVOS_UNIX) && !defined(TVOSf_QNXRtP) && !defined(XTERMMOUSE_INCLUDED)
#define XTERMMOUSE_INCLUDED

class TScreenXTerm;

class THWMouseXTermFull : public THWMouseUNIX
{
protected:
 THWMouseXTermFull() {};
 virtual ~THWMouseXTermFull();

 static void SuspendFull();
 static void ResumeFull();
 static void SuspendSimple();
 static void ResumeSimple();

 static void Init(int aMode);

 static int mode;
 enum
 {
  modeXTerm, // Full reporting
  modeEterm  // Just button events :-(
 };

 friend class TScreenXTerm;
};

#endif // XTERMMOUSE_INCLUDED
