#ifndef MATHUTILS_H
#define MATHUTILS_H

#include "CustomMatrix.h"
#include <stdio.h>
#define _USE_MATH_DEFINES //This+math.h to get M_PI
#include <math.h>
#include "Globals.h"

class MathUtils
{
public:

	static double deg2rad(double x_deg);
	static double rad2deg(double x_rad);
	static void RotToRPY(CustomMatrix * pRot, double res_RPY[3]);
	static void RPYtoRot(double RPY[3],CustomMatrix* res_pRot);

	//Quat methods checked, ok
	static void RotToQuat(CustomMatrix * pRot, double res_quat[4]); //From Shoemake
	static void getQuatNorm(double quat[4],double & res_norm);
	//static void QuatToRot(double quat[4],CustomMatrix * res_pRot);
	static CustomMatrix* QuatToRot(double a_quat[4]);
	static void Quat_Multiplication(double quat_a[4], double quat_b[4], double quat_res[4]);
	static double Quat_euclidianDistance(double quat_a[4], double quat_b[4]);
	static void RotToQuat_closestNeighbour(CustomMatrix * pRot, double neighbour_quat[4], double res_quat[4]);
	static int convert_double2int(const double x); //ok

	//Compute fitting curve and then compute its time derivative
	static int compute_fittingCurve_movingAverage(double * raw_points_window,const int & a_windowSize,double & res_smoothValue_centerWindow); //ok - windowSize should be uneven! num points in raw_points_window - returns -1 if issue, 1 if ok
	//window-moving smoothing technique==window moving fitting curve approximation
	//static void compute_fittingCurve_Savitzky_Golay();
	static void compute_backward_finite_difference(const double data_points[2],const double & delta_t,double & res_valueDerivative);//ok - delta_t=time sample between the two data points

	static void compute_safePos(const double a_A[3],const double a_B[3],const double & a_maxDist,double res_C[3]);//Compute coordinates of point C which is on the line from A to B at a distance of maxDist from A (for Optitrack)

};

#endif;