#ifndef ONEEUROFILTER
#define ONEEUROFILTER

#include <iostream>
#include <stdexcept>
#include <cmath>
#include <ctime>

#define _USE_MATH_DEFINES //This+math.h to get M_PI
#include <math.h>

//#include "LowPassFilter.h"

//http://cristal.univ-lille.fr/~casiez/1euro/
//http://www.lifl.fr/~casiez/publications/CHI2012-casiez.pdf
//The 1e filter (“one Euro filter”) is a simple algorithm to filter noisy signals for high precision and responsiveness. 
//It uses a first order low-pass filter with an adaptive cutoff frequency: at low speeds, a low cutoff stabilizes the signal by reducing  jitter, but as speed increases, the cutoff is increased to reduce lag
//It uses a low-pass filter, but the cutoff frequency changes according to speed: at low speeds, a low cutoff reduces jitter at the expense of lag, but at high speeds, the cutoff is increased to reduce lag rather than jitter. T

typedef double TimeStamp ; // in seconds
static const TimeStamp UndefinedTime = -1.0 ;

	
class LowPassFilter {
    
  double y, a, s ;
  bool initialized ;

  void setAlpha(double alpha) {
    if (alpha<=0.0 || alpha>1.0) 
      throw std::range_error("alpha should be in (0.0., 1.0]") ;
    a = alpha ;
  }

public:

  LowPassFilter(double alpha, double initval=0.0) {
    y = s = initval ;
    setAlpha(alpha) ;
    initialized = false ;
  }

  double filter(double value) {
    double result ;
    if (initialized)
      result = a*value + (1.0-a)*s ;
    else {
      result = value ;
      initialized = true ;
    }
    y = value ;
    s = result ;
    return result ;
  }

  double filterWithAlpha(double value, double alpha) {
    setAlpha(alpha) ;
    return filter(value) ;
  }

  bool hasLastRawValue(void) {
    return initialized ;
  }

  double lastRawValue(void) {
    return y ;
  }

} ;


class OneEuroFilter{

public: 

	//Check these 2
	//static double m_TimerTeleop;


  double freq;
  double mincutoff;
  double beta_;
  double dcutoff;
  LowPassFilter *x;
  LowPassFilter *dx;
  TimeStamp lasttime;

  double alpha(double cutoff) {
    double te = 1.0 / freq ;
    double tau = 1.0 / (2*M_PI*cutoff) ;
    return 1.0 / (1.0 + tau/te) ;
  }

  void setFrequency(double f) {
    if (f<=0) throw std::range_error("freq should be >0") ;
    freq = f ;
  }

  void setMinCutoff(double mc) {
    if (mc<=0) throw std::range_error("mincutoff should be >0") ;
    mincutoff = mc ;
  }

  void setBeta(double b) {
    beta_ = b ;
  }

  void setDerivateCutoff(double dc) {
    if (dc<=0) throw std::range_error("dcutoff should be >0") ;
    dcutoff = dc ;
  }

public:

	//Me, added: default constructor, to be able to create instance in Optitrack.cpp
	OneEuroFilter()
	{
		setFrequency(120) ;
		setMinCutoff(1.0) ;
		setBeta(1.0) ;
		setDerivateCutoff(1.0) ;
		x = new LowPassFilter(alpha(mincutoff)) ;
		dx = new LowPassFilter(alpha(dcutoff)) ;
		lasttime = UndefinedTime ;
	}

  OneEuroFilter(double freq,
		double mincutoff=1.0, double beta_=0.0, double dcutoff=1.0) {
    setFrequency(freq) ;
    setMinCutoff(mincutoff) ;
    setBeta(beta_) ;
    setDerivateCutoff(dcutoff) ;
    x = new LowPassFilter(alpha(mincutoff)) ;
    dx = new LowPassFilter(alpha(dcutoff)) ;
    lasttime = UndefinedTime ;
  }

  double filter(double value, TimeStamp timestamp=UndefinedTime) {
    // update the sampling frequency based on timestamps
    if (lasttime!=UndefinedTime && timestamp!=UndefinedTime)
      freq = 1.0 / (timestamp-lasttime) ;
    lasttime = timestamp ;
    // estimate the current variation per second 
    double dvalue = x->hasLastRawValue() ? (value - x->lastRawValue())*freq : 0.0 ; // FIXME: 0.0 or value?
	//printf("OneEuroFilter dvalue: %g \n",dvalue);
	//printf("OneEuroFilter dcutoff: %g \n",dcutoff);
	//printf("OneEuroFilter alpha(dcutoff): %g \n",alpha(dcutoff));
	//printf("OneEuroFilter alpha(dcutoff): %g \n",filterWithAlpha(dvalue, alpha(dcutoff));
    double edvalue = dx->filterWithAlpha(dvalue, alpha(dcutoff)) ;
	//printf("OneEuroFilter edvalue: %g \n",edvalue);
    // use it to update the cutoff frequency
    double cutoff = mincutoff + beta_*fabs(edvalue) ;
    // filter the given value
    return x->filterWithAlpha(value, alpha(cutoff)) ;
  }

  ~OneEuroFilter(void) {
    delete x ;
    delete dx ;
  }



};


#endif