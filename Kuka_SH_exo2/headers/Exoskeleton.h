#ifndef EXOSKELETON_H
#define EXOSKELETON_H

//io
#include "CustomMatrix.h"
#include "EXOSBoardLibrary.h"
#include "Globals.h"

class Exoskeleton
{
	public:
		//Rq: this shouldn't be public..

	//cartesian, in base frame
	struct Finger{
		double jointPos_enc[Globals::nq_kins_pf];//rad
		double jointPos_dh[Globals::nq_kins_pf];//rad

		//Store data in circ buffer for smoothing curve and time derivative computation
		double jointPos_dh_circBuff_rt[Globals::nq_kins_pf][Globals::windowSize_toComputeFittingCurve_rt];//Each column is a time sample, each row is a variable (here qi)
		double jointPos_dh_circBuff_nrt[Globals::nq_kins_pf][Globals::windowSize_toComputeFittingCurve_nrt];
		//From here we have time delay
		double jointPos_fittingCurve_circBuff_rt[Globals::nq_kins_pf][Globals::numSamples_toComputeFiniteDerivative];//each row corresponds to a joint, each column to a sample - 2 samples for backward finite difference, 3 samples for central difference
		double jointPos_fittingCurve_circBuff_nrt[Globals::nq_kins_pf][Globals::numSamples_toComputeFiniteDerivative];//2 samples for backward finite difference, 3 samples for central difference
		double q_dot_dh_nrt[Globals::nq_kins_pf];//rad/s - time derivative of the fitting curve - non real time: smoother but larger time delay
		double q_dot_dh_rt[Globals::nq_kins_pf];//rad/s - time derivative of the fitting curve - real time: less smooth but smaller time delay
		//Rq: first valid value of q_dot is num_samples_to_compute_q_smooth + (num_q_smooth to compute derivative-1)=(m+1)th sample if 2 points used to differentiale, (m+2)th sample if 3 points used to differentiate


		double posTip[3];

		CustomMatrix* pRotTip;
		double RPY_Tip[3];
		double quatTip[4];
		double quatTip_raw[4]; //Without picking up closest neighbour - for comparison only

		CustomMatrix * pJacobianTip; //Jacobian from base to tip, expressed in base frame - 
		double force_tip_des_base[Globals::numVar_pf_for_pcs]; //RENAME! //Desired force to apply at tip, = force + torque to apply to EE - computed from syn - expressed in base frame (ref from teleop sys, without considering capabilities of exo)
		int torque_0_ref; //Torque to apply to joint 0, ref from Ftip_des_base projected on torque space and scaled to motor range

		double velTip_nrt[Globals::mCart_kins_pf]; //vx vy vz wx wy wz - nrt=smoother but larger time delay
		double velTip_rt[Globals::mCart_kins_pf]; //vx vy vz wx wy wz - rt = less smooth but smaller time delay

		//struct constructor
		Finger():pRotTip(new CustomMatrix(3,3)),pJacobianTip(new CustomMatrix(Globals::mCart_kins_pf,Globals::nq_kins_pf)),quatTip(),quatTip_raw()   //Struct constructor with initialization list. Checked: ok. Set size of rotMatrix; quatTip() makes a zero-initialization.
		{
		} 

		//Struct function
		//void set_force_tip_des_base(double a_Fdes[Globals::mCart_kins_pf])
		//{
		//	for (int it=0;it<Globals::mCart_kins_pf;it++)
		//	{	
		//		force_tip_des_base[it]=a_Fdes[it];
		//	}
		//}		
	};

	Finger aFingers[3];//thumb,index,middle


	//User fnt
	void compute_q_dot_dh();//ok
	void set_jointPos(const HandExoskeleton ExosData);//Fnt to fill jointPos_enc for each finger
	void set_force_tip_des_base(double a_Fdes_from_syn[Globals::n_pcs]); //Rename this fnt!!
	void set_torque0_ref(int a_torque0_motorRange_ref[3]);
	void get_torque0_ref(int res_tauRef[3]);
	static int scaleTorque_to_motorRange(const double & a_torque_ini);//TO CODE!

	//old
	static int MaxLim_MotorEncoder[3];
	static int MinLim_MotorEncoder[3];

private:
	void update_circBuffer_jointPos();//TO CHECK - called by compute_q_dot_dh()
	void update_circBuffer_fittingCurves();//TO CHECK - called by compute_q_dot_dh()
};

#endif EXOSKELETON_H