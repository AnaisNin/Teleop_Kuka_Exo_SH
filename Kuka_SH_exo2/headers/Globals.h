#ifndef GLOBALS_H
#define GLOBALS_H

class Globals 
//Put here var that are used in multiple classes, to avoid a loop of include (include B in A and A in B because they need the const declared in the other class)
{
public:

	static const int IDF_ACTIVE_FINGERS=3; //1: index finger only - 3: all fingers - Used to perform pca analysis

	//In kinematic space (kins) - (exo-related) - per finger (pf)
	static const int nq_kins_pf=6; //num joints per finger
	static const int mCart_kins_pf=6; // 6 - TO SET - num var describing ee pos (3) and orientation (4)
	static const int numVar_pf_for_pcs=3;
	static const int numFingers=3;

	//In syn space - all fingers (af)
	//static const int n_pcs=mCart_kins_pf*IDF_ACTIVE_FINGERS; //Temp, considering only 1 finger - Replace by n_syns_af
	static const int n_pcs=9; //Temp, considering only 1 finger - Replace by n_syns_af
	//Dimension principal components space = dim of the eigen vectors (matrix to copy is n_pcs x n_pcs)
	//static const int n_syns_af=mCart_kins_pf*numFingers; //Dimension synergy space=dimension principal component space
	//For 1 finger, n_pcs=(3pos + 4quat)=7
	//For 3 fingers, n_pcs=(3+4)*3=21

	//Filtering and time-derivation
	static const int windowSize_toComputeFittingCurve_rt=5; //Must be uneven - Number of samples from the raw data that are considered to compute a piecewise fitting curve 
	static const int windowSize_toComputeFittingCurve_nrt=29; //Must be uneven - Number of samples from the raw data that are considered to compute a piecewise fitting curve 
	static const int numSamples_toComputeFiniteDerivative=2; //2 for backward finite derivative, 3 for central finite derivative
	//Rq: time delay introduced by the smooting is (windowSize-1)/2 x time sampling, that is for windowSize=5samples, 2*1ms delay
	//When used for Real Time computation, the delay should be reduced so take small window size; for non real time take large window and make slow motions
	
	//Cannot declare const double, just const int. In c++11 constexpr allows to declare const double. Else use a fnt. 
	//static constexpr double delta_t=0.001;//1ms between two data points
	static double get_delta_t(){return 0.001;}//1ms between two data points

};                                                                                                                       

#endif