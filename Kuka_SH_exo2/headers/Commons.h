#ifndef COMMONS_H
#define COMMONS_H

#define PI 3.141592653589793
#define DEGTORAD(x)  x*6.28/360.0
#define RADTODEG(x)  x*360.0/6.28
#define RPMTODEGSEC(x)  x*360.0/(100.0*60.0)
#define DEGSECTORPM(x)  100.0*60*x/360.0

#define IN_DEG 1
#define IN_RAD 2
#define ON 1
#define OFF 0

#define PID_CONTROL 1
#define LQR_CONTROL 2


#endif COMMONS_H