// #if defined( Uses_TProgressBar ) && !defined( __TProgressBar )
// #define __TProgressBar

/* Include the commented directives above if you want to implement this
   in your TV.LIB. Don't forget the #endif at the end of this file! To
   utilize "#define Uses_TProgressBar" put the follow directives in TV.H, 
   after #if defined( Uses_TButton ) would be a good place.

#if defined( Uses_TProgressBar )
#define Uses_TView
#define __INC_DIALOGS_H
#endif

   After uncommenting the top two directives of this header file and the 
   #endif at the end of this header file copy the contents of this file to 
   the DIALOGS.H file in TVISION\INCLUDE. Put it before the last set of 
   directives or to be safe somewhere in the middle between "#define Uses_*" 
   blocks. Don't forget to make a backup of DIALOGS.H if you make mistakes.

   In MAKEFILE. in \TVISION\SOURCE edit the OBJS = statement to include
   TPROGBAR.OBJ or compile it alone and just add it to the TV.LIB via
   TLIB (easier).

   To comiple individually and use try:
	 bcc -c -P -O1 -ml -I<include dirs> -n<obj dirs> tprogbar.cpp
	    + add "-Y -Vs -B" for overlay version of TV.
	 tlib tv.lib /0 +TPROGBAR

   For example:
         bcc -c -P -O1 -kl -I\BC\INCLUDE;\BC\TVISION\INCLUDE tprogbar.cpp
         tlib \BC\TVISION\LIB\TV.LIB /0 +tprogbar

   After this you can remove TPROGBAR.CPP from your project list and it will
be included in the link with TV.LIB.

*/

class TRect;

class TProgressBar : public TView
{
public:
   // default the background char to 178, but you can pass any char you want
   TProgressBar(const TRect& r, unsigned long iters, char abackChar='²');
   ~TProgressBar();
   virtual void draw();
   virtual TPalette& getPalette() const;
   virtual void update(unsigned long aProgress);

   inline unsigned long getTotal();	// get the maximum iteration
   inline unsigned long getProgress();	// get the current iteration

   // change the percentage ( calls the update function )
   void setTotal(unsigned long newTotal); // set the maximum iteration
   void setProgress(unsigned long newProgress); // set the current iteration

protected:
   char          backChar;   // background character
   unsigned long total;      // total iterations to complete 100 %
   unsigned long progress;   // current iteration value
   char *        bar;	     // thermometer bar
   unsigned int  dispLen;    // length of bar
   unsigned int  curPercent; // current percentage
   unsigned int  curWidth;
   unsigned int  numOffset;  // offset in the string to display the percentage
   double        charValue;

private:
   virtual const char* streamableName() const
      {return name;}
   void calcPercent();     // calculate new percentage

protected:
   TProgressBar( StreamableInit) : TView( streamableInit ) {};
   virtual void write(opstream&);
   virtual void *read(ipstream&);

public:
   static const char * const name;
   static TStreamable *build();
};

inline ipstream& operator >> ( ipstream& is, TProgressBar& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, TProgressBar*& cl )
    { return is >> (void *&)cl; }

inline opstream& operator << ( opstream& os, TProgressBar& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, TProgressBar* cl )
    { return os << (TStreamable *)cl; }

//#endif      // defined( Uses_TProgressBar ) && !defined( __TProgressBar )
