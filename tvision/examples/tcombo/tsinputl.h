/*************************************************************************/
/*                                                                       */
/* TSINPUTL.H                                                            */
/*                                                                       */
/* Copyright (c) 1992, Vincent J. Dentice                                */
/* All rights reserved                                                   */
/*                                                                       */
/*                                                                       */
/*   Date    Prg  Ver  Description                                       */
/* --------  ---  ---  ------------------------------------------------- */
/* 09/30/92  VJD  0.1  Initial module definition.                        */
/* 11/16/92  VJD  0.2  Added streamability to the TComboBox classes.     */
/*                     Rewrote header files to behave like original      */
/*                     Turbo Vision header files.                        */
/*                                                                       */
/*************************************************************************/

#define Uses_TCollection
#define Uses_TInputLine
#include <tv.h>


#if defined( Uses_TStaticInputLine ) && !defined( __TStaticInputLine )
#define __TStaticInputLine


class TStaticInputLine : public TInputLine {
   protected:
      TCollection *list;

      void *getNextMatch(char);

   public:
      TStaticInputLine(const TRect&, int, TCollection *aList);

      virtual void handleEvent(TEvent& event);
      virtual void newList(TCollection *aList);

   private:
      virtual const char *streamableName() const
	 { return name; }

   protected:
      TStaticInputLine(StreamableInit);
      virtual void write(opstream&);
      virtual void *read(ipstream&);

   public:
      static const char * const name;
      static TStreamable *build();
};


#endif
