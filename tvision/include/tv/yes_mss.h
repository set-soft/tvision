/*
  Copyright (c) 2001 by Salvador E. Tropea and Laurynas Biveinis
  Covered by the GPL license.
  This include disables MSS until yes_mss.h is included.
  Needed to wrap uses of operators/functions defined as macros in MSS.
*/
#ifdef REINCLUDE_MSS
#define MSS
#undef REINCLUDE_MSS
#include <mss.h>
#endif
