#if !defined( __TPROGBAR_H )
#define __TPROGBAR_H

#if !defined( __MATH_H )
#include <math.h>
#endif       // __MATH_H

#define DISPLAYLEN  51      // Length (width) of calculator display

enum TCalcState { csFirst = 1, csValid, csError };

const int cmCalcButton  = 200;

class TProgressBar : public TView
{
	protected:
		char          backChar;			// background character
		char          percChar;			// percentage character
		char *        bar;				// thermometer bar
		unsigned int  maxWidth;			// maximum width of thermometer bar
		unsigned int  oldWidth;			// previous width
		unsigned int  curWidth;			// current width of percentage bar
		double        chPercent;		// percent per character
		unsigned long maxIter;			// maximum iteration
		unsigned long curIter;			// current iteration
		unsigned int  oldPercent;		// old percentage complete
		unsigned int  curPercent;		// current percentage complete

		void calcPercent ( );			// calculate new percentage

public:
	 TProgressBar(const TRect& r, unsigned long iters );
	 ~TProgressBar();
	 virtual void handleEvent(TEvent& event);
	 virtual void draw();
		virtual void term ( );			// clear the thermometer bar display

		inline unsigned long getMaxIter ( );	// get the maximum iteration
		inline unsigned long getCurIter ( );	// get the current iteration

		// change the percentage ( calls the update function )
		void setMaxIter ( unsigned long newMax );	// set the maximum iteration
		void setCurIter ( unsigned long newCur );	// set the current iteration
private:
	 TCalcState status;
	 virtual void mainProcess( void );
};

#endif      // __TPROGBAR_H
/*
class spreadIt : public TProgressBar
{
	 spreadIt(TRect& r, unsigned long iters ) :
		TProgressBar( r , iters ){};
	 virtual void mainProcess( void );
};
*/
