#ifndef LOWPASSFILTER
#define LOWPASSFILTER


#include <iostream>
#include <stdexcept>
#include <cmath>
#include <ctime>


class LowPassFilter {
    
  double y, a, s ;
  bool initialized ;

  void setAlpha(double alpha) {
    if (alpha<=0.0 || alpha>1.0) 
      //throw std::range_error("alpha should be in (0.0., 1.0]") ;
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

#endif