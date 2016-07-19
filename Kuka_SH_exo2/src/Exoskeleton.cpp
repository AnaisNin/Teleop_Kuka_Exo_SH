#include "Exoskeleton.h"
#include "MathUtils.h"

int Exoskeleton::MaxLim_MotorEncoder[3]={0,0,0};
int Exoskeleton::MinLim_MotorEncoder[3]={0,0,0};

void Exoskeleton::set_jointPos(const HandExoskeleton ExosData)
{
	for(int it_f=0;it_f<3;it_f++) //for each finger
	{
		for(int it_j=0;it_j<6;it_j++) //for each joint
		{
			this->aFingers[it_f].jointPos_enc[it_j]=ExosData.ExosFinger[it_f].jointPosRad[it_j];
		}
	}
}

void Exoskeleton::set_torque0_ref(int a_torque0_motorRange_ref[3])
{
	for(int it=0;it<3;it++)
	{
		this->aFingers[it].torque_0_ref=a_torque0_motorRange_ref[it];
	}
}

void Exoskeleton::set_force_tip_des_base(double a_Fdes_from_syn[Globals::n_pcs])
{
	if(Globals::IDF_ACTIVE_FINGERS==1)//index finger only
	{
		for(int it=0;it<Globals::n_pcs;it++)
		{
			this->aFingers[1].force_tip_des_base[it]=a_Fdes_from_syn[it];
		}
	}
	else if(Globals::IDF_ACTIVE_FINGERS==3) //3fingers
	{
		//for(int it=0;it<Globals::mCart_kins_pf;it++)
		for(int it=0;it<Globals::numVar_pf_for_pcs;it++)
		{
			this->aFingers[0].force_tip_des_base[it]=a_Fdes_from_syn[it]; //thumb
			this->aFingers[1].force_tip_des_base[it]=a_Fdes_from_syn[it+Globals::numVar_pf_for_pcs]; //index
			this->aFingers[2].force_tip_des_base[it]=a_Fdes_from_syn[it+2*Globals::numVar_pf_for_pcs]; //middle
		}		
	}
	else
	{
		printf("ERROR - IDF_ACTIVE_FINGERS should be either 1 (index finger only) or 3 (3fingers). Exit. [Exoskeleton::set_force_tip_des_base] \n");
	}
}

void Exoskeleton::get_torque0_ref(int res_tauRef[3])
{
	for(int it=0;it<3;it++)
	{
		res_tauRef[it]=this->aFingers[it].torque_0_ref;
	}
}

int Exoskeleton::scaleTorque_to_motorRange(const double & a_torque_ini)//Clamp
{
	//int torque_min=0;
	int torqueMax_motorExo=300;//mNm - TO SET
	double gain_loc=4.0;//to make the torque fit the motor range. Tau_max exo from syn=75mNm (grasping tennis ball), max motor des=300, gain =300/75=4
	int torque_exoMotor_loc=(int)(a_torque_ini*1000.0*(-1)*gain_loc);

	if(torque_exoMotor_loc>torqueMax_motorExo)
	{
		torque_exoMotor_loc=torqueMax_motorExo;
	}
	else if(torque_exoMotor_loc<0)
	{
		torque_exoMotor_loc=0;
		printf("Negative torque, set to zero [Exo::scaleTorque_to_motorRange]\n");
	}
	
	return torque_exoMotor_loc;
}

void Exoskeleton::update_circBuffer_jointPos() //ok - jointPos_dh must be setted previously
{
	for(int k=0;k<3;k++) //for each finger
	{
		for(int iti=0;iti<Globals::nq_kins_pf;iti++) //for each joint (corresponding to each row)
		{
			//Push all the columns left
			//nrt buffer
			for(int itj=0;itj<Globals::windowSize_toComputeFittingCurve_nrt-1;itj++) //for each column until penultimate
			{
				Exoskeleton::aFingers[k].jointPos_dh_circBuff_nrt[iti][itj]=Exoskeleton::aFingers[k].jointPos_dh_circBuff_nrt[iti][itj+1];
			}
			//rt buffer
			for(int itj=0;itj<Globals::windowSize_toComputeFittingCurve_rt-1;itj++) //for each column until penultimate
			{
				Exoskeleton::aFingers[k].jointPos_dh_circBuff_rt[iti][itj]=Exoskeleton::aFingers[k].jointPos_dh_circBuff_rt[iti][itj+1];
			}

			//Store last sample in last column
			Exoskeleton::aFingers[k].jointPos_dh_circBuff_nrt[iti][Globals::windowSize_toComputeFittingCurve_nrt-1]=Exoskeleton::aFingers[k].jointPos_dh[iti];//nrt buffer
			Exoskeleton::aFingers[k].jointPos_dh_circBuff_rt[iti][Globals::windowSize_toComputeFittingCurve_rt-1]=Exoskeleton::aFingers[k].jointPos_dh[iti];//rt buffer

		}//for each row
	
	}//For each finger
}

void Exoskeleton::update_circBuffer_fittingCurves() //ok
//Moving average: build q_smooth from q to then time-derivate q_smooth. 
//q_smooth_i= [q(i-(m-1)/2)+ ... + q(i)+ ... + q(i+(m-1)/2)]/m with m=window size
//This introduces a delay of (m-1)/2 samples (we compute q_smooth at the central point of the window so delay=half window), that is time delay=(m-1)/2 * delta t (delta t = time between two samples of pos)
{
	double data_window_nrt_loc[Globals::windowSize_toComputeFittingCurve_nrt]; //sample points of same type = one window for one joint
	double fittingPoint_nrt_loc[Globals::nq_kins_pf];//fitting curve for all joints evaluated at center of window
	double data_window_rt_loc[Globals::windowSize_toComputeFittingCurve_rt]; //sample points of same type = one window
	double fittingPoint_rt_loc[Globals::nq_kins_pf];//fitting curve evaluated at one point - joint pos fitted

	for(int k=0;k<3;k++) //for each finger
	{
		for(int iti=0;iti<Globals::nq_kins_pf;iti++) //for each joint (corresponding to each row)
		{
			//Push all the columns of fittingCurve buffer left
			//nrt buffer
			for(int itj=0;itj<Globals::numSamples_toComputeFiniteDerivative-1;itj++) //for each column until penultimate
			{
				Exoskeleton::aFingers[k].jointPos_fittingCurve_circBuff_nrt[iti][itj]=Exoskeleton::aFingers[k].jointPos_fittingCurve_circBuff_nrt[iti][itj+1]; //from nrt jointPos buffer
				Exoskeleton::aFingers[k].jointPos_fittingCurve_circBuff_rt[iti][itj]=Exoskeleton::aFingers[k].jointPos_fittingCurve_circBuff_rt[iti][itj+1]; //rt buffer
			}

			//Extract window array of joint iti from circ buffer
			for(int itj=0;itj<Globals::windowSize_toComputeFittingCurve_nrt;itj++) //for all columns
			{
				data_window_nrt_loc[itj]=Exoskeleton::aFingers[k].jointPos_dh_circBuff_nrt[iti][itj];
			}
			for(int itj=0;itj<Globals::windowSize_toComputeFittingCurve_rt;itj++) //for all columns
			{
				data_window_rt_loc[itj]=Exoskeleton::aFingers[k].jointPos_dh_circBuff_rt[iti][itj];
			}
			
			//Compute fitting point from this window and fills last column of fittingCurve buffer
			MathUtils::compute_fittingCurve_movingAverage(data_window_nrt_loc,Globals::windowSize_toComputeFittingCurve_nrt,Exoskeleton::aFingers[k].jointPos_fittingCurve_circBuff_nrt[iti][Globals::numSamples_toComputeFiniteDerivative-1]);//nrt
			MathUtils::compute_fittingCurve_movingAverage(data_window_rt_loc,Globals::windowSize_toComputeFittingCurve_rt,Exoskeleton::aFingers[k].jointPos_fittingCurve_circBuff_rt[iti][Globals::numSamples_toComputeFiniteDerivative-1]);//rt

		}//for each row
	
	}//For each finger

}

void Exoskeleton::compute_q_dot_dh() //ok - Sets q_dot_dh_nrt and rt 
{
	Exoskeleton::update_circBuffer_jointPos();
	Exoskeleton::update_circBuffer_fittingCurves();

	double smooth_points_nrt_loc[Globals::numSamples_toComputeFiniteDerivative];//one window per joint
	double smooth_points_rt_loc[Globals::numSamples_toComputeFiniteDerivative];

	for(int k=0;k<3;k++)//for each finger
	{
		for(int iti=0;iti<Globals::nq_kins_pf;iti++)//For each joint
		{
			//Extract array smooth points 
			for(int itj=0;itj<Globals::numSamples_toComputeFiniteDerivative;itj++)
			{
				smooth_points_nrt_loc[itj]=Exoskeleton::aFingers[k].jointPos_fittingCurve_circBuff_nrt[iti][itj];
				smooth_points_rt_loc[itj]=Exoskeleton::aFingers[k].jointPos_fittingCurve_circBuff_rt[iti][itj];
			}

			//Compute time derivative for each joint
			MathUtils::compute_backward_finite_difference(smooth_points_nrt_loc,Globals::get_delta_t(),Exoskeleton::aFingers[k].q_dot_dh_nrt[iti]);
			MathUtils::compute_backward_finite_difference(smooth_points_rt_loc,Globals::get_delta_t(),Exoskeleton::aFingers[k].q_dot_dh_rt[iti]);
		}
	}
}

//void Exoskeleton::update_circBuffer_jointPos(Finger & a_finger)
//{
//	for(int iti=0;iti<Globals::nq_kins_pf;iti++) //for each joint (corresponding to each row)
//		{
//			//Push all the columns left
//			//nrt buffer
//			for(int itj=0;itj<Globals::windowSize_toComputeFittingCurve_nrt-1;itj++) //for each column until penultimate
//			{
//				
//				a_finger.jointPos_dh_circBuff_nrt[iti][itj]=a_finger.jointPos_dh_circBuff_nrt[iti][itj+1];
//			}
//			//rt buffer
//			for(int itj=0;itj<Globals::windowSize_toComputeFittingCurve_rt-1;itj++) //for each column until penultimate
//			{
//				a_finger.jointPos_dh_circBuff_rt[iti][itj]=a_finger.jointPos_dh_circBuff_rt[iti][itj+1];
//			}
//
//
//			//Store last sample in last column
//			a_finger.jointPos_dh_circBuff_nrt[iti][Globals::windowSize_toComputeFittingCurve_nrt-1]=a_finger.jointPos_dh[iti];//nrt buffer
//			a_finger.jointPos_dh_circBuff_rt[iti][Globals::windowSize_toComputeFittingCurve_rt-1]=a_finger.jointPos_dh[iti];//rt buffer
//
//		}//for each row
//}
