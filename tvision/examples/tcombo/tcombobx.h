/*************************************************************************/
/*                                                                       */
/* TCOMBOBX.H                                                            */
/*                                                                       */
/* Copyright (c) 1992, Vincent J. Dentice                                */
/* All rights reserved                                                   */
/*                                                                       */
/* The TComboBox class is an extension to Borland International's Turbo  */
/* Vision Applications Framework for DOS.  It provides a class that      */
/* acts like a Combo Box in other graphical environments like Microsoft  */
/* Windows and IBM OS/2.                                                 */
/*                                                                       */
/* It is designed to be with a TDialog class and a TCollection Class.    */
/*                                                                       */
/*                                                                       */
/*   Date    Prg  Ver  Description                                       */
/* --------  ---  ---  ------------------------------------------------- */
/* 09/30/92  VJD  0.1  Initial module definition.                        */
/* 11/16/92  VJD  0.2  Added streamability to the TComboBox classes.     */
/*                     Rewrote header files to behave like original      */
/*                     Turbo Vision header files.                        */
/* 11/17/92  VJD  0.3  Added getData and setData functions to TComboBox, */
/*                     and also added the missing dataSize function to   */
/*                     TComboViewer.                                     */
/*                                                                       */
/*************************************************************************/

#if defined( Uses_TComboBox )
#define Uses_TCollection
#define Uses_TInputLine
#define Uses_TView
#endif

#if defined( Uses_TComboWindow )
#define Uses_TComboViewer
#define Uses_TWindow
#endif

#if defined( Uses_TComboViewer )
#define Uses_TCollection
#define Uses_TEvent
#define Uses_TListViewer
#endif

#if defined( Uses_TCollection ) && !defined( __TCollection )
#define INCLUDE_TV_H
#endif

#if defined( Uses_TEvent ) && !defined( __TEvent)
#define INCLUDE_TV_H
#endif

#if defined( Uses_TInputLine ) && !defined( __TInputLine )
#define INCLUDE_TV_H
#endif

#if defined( Uses_TListViewer ) && !defined( __TListViewer )
#define INCLUDE_TV_H
#endif

#if defined( Uses_TView ) && !defined( __TView )
#define INCLUDE_TV_H
#endif

#if defined( Uses_TWindow ) && !defined( __TWindow )
#define INCLUDE_TV_H
#endif

#if defined( INCLUDE_TV_H )
#include <tv.h>
#endif


#if defined( Uses_TComboBox ) && !defined( __TComboBox )
#define __TComboBox

class TComboBox : public TView {
   protected:
      static char *icon;
      TInputLine  *link;
      TCollection *list;

   public:
      TComboBox(const TRect&, TInputLine *, TCollection *);

      virtual unsigned dataSize();
      virtual void draw();
      virtual void getData(void *);
      TPalette& getPalette() const;
      virtual void handleEvent(TEvent&);
      virtual void newList(TCollection *);
      virtual void setData(void *);
      void shutDown();

   private:
      virtual const char *streamableName() const
	 { return name; }

   protected:
      TComboBox(StreamableInit);
      virtual void write(opstream&);
      virtual void *read(ipstream&);

   public:
      static const char * const name;
      static TStreamable *build();
};

inline ipstream& operator >> ( ipstream& is, TComboBox& cl )
     { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, TComboBox*& cl )
     { return is >> (void *&)cl; }

inline opstream& operator << ( opstream& os, TComboBox& cl )
     { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, TComboBox* cl )
     { return os << (TStreamable *)cl; }

#endif



#if defined( Uses_TComboViewer ) && !defined( __TComboViewer )
#define __TComboViewer

class TComboViewer : public TListViewer {
   public:
      TCollection *list;

      TComboViewer(const TRect&, TCollection*, TScrollBar*);

      virtual unsigned dataSize();
      TPalette& getPalette() const;
      virtual void getData(void *);
      virtual void getText(char *, ccIndex, short);
      virtual void handleEvent(TEvent&);
      virtual void newList(TCollection *);
      virtual void setData(void *);
      void shutDown();

   private:
      virtual const char *streamableName() const
	 { return name; }

   protected:
      TComboViewer(StreamableInit);
      virtual void write(opstream&);
      virtual void *read(ipstream&);

   public:
      static const char * const name;
      static TStreamable *build();
};

inline ipstream& operator >> ( ipstream& is, TComboViewer& cl )
     { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, TComboViewer*& cl )
     { return is >> (void *&)cl; }

inline opstream& operator << ( opstream& os, TComboViewer& cl )
     { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, TComboViewer* cl )
     { return os << (TStreamable *)cl; }

#endif


#if defined( Uses_TComboWindow ) && !defined( __TComboWindow )
#define __TComboWindow

class TComboWindow : public TWindow {
   protected:
      TComboViewer *viewer;

   public:
      TComboWindow(const TRect&, TCollection*);

      TPalette& getPalette() const;
      void getSelection(char *);
      virtual void handleEvent(TEvent&);
      void setSelection(const char *);

   private:
      virtual const char *streamableName() const
	 { return name; }

   protected:
      TComboWindow(StreamableInit);
      virtual void write(opstream&);
      virtual void *read(ipstream&);

   public:
      static const char * const name;
      static TStreamable *build();
};

inline ipstream& operator >> ( ipstream& is, TComboWindow& cl )
     { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, TComboWindow*& cl )
     { return is >> (void *&)cl; }

inline opstream& operator << ( opstream& os, TComboWindow& cl )
     { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, TComboWindow* cl )
     { return os << (TStreamable *)cl; }

#endif
