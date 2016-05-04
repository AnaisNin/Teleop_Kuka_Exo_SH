#ifndef TORQUEOBSERVER_H
#define TORQUEOBSERVER_H

//#include "Globals.h"
#define _USE_MATH_DEFINES //This+math.h to get M_PI
#include <math.h>

class TorqueObserver
{
public:
    //User fnt
    //static void Compute_InteractionTorque_FrictionCurrent(const float & a_current,const int & a_desPos,float & res_intTorque_obs,float & res_frictCurrent);
  //  static void Compute_InteractionTorque_FrictionCurrent(const float & a_current,const int & a_desPos,const int & a_sh_pos_actual);
    static void Compute_1stContact_StiffContact(const int & a_sh_pos_actual);

     //Accessors for class members
     static float get_tau_ext_obs(){return tau_ext_obs;}//interaction torque
	 static float get_tau_ext_obs_filtered(){return tau_ext_obs_filtered;}
     static float get_I_filtered(){return I_filtered;}
     static double get_pos_filtered(){return pos_filtered;}
     static float get_currentOffset(){return currentOffset;}
     static float get_desired_vel(){return desired_vel;}
	 static void set_tau_ext_obs_filtered(float a_tau_ext_obs_filtered);
	
     //static double get_stiff_contact(){return stiff_contact;}
     //static float get_tau_hat(){return tau_hat;}

     //Constants
     static float tau_int_threshContact(){return 0;}//0.0065 to set! torque defining contact
     static int sh_enc_res(){return 628000;}//to check
     //0 since we already substract the torque free space in update_interactionTorque

	 static void Compute_InteractionTorque_FrictionCurrent(const float & a_current, const int & a_desPos, const int & a_sh_pos_actual);

	
private:

    //Core fnt
    static void Update_I_filtered(const float & a_current);
    static void Update_Pos_filtered(const int & a_pos);
	static void Update_InteractionTorque(const int & a_desPos, const int & a_sh_pos_actual);
	static void Update_CompensationCurrent();

	

     //Members
    static float I_filtered;//float or int?
    static double pos_filtered;//unused - double or int?
    static float currentOffset;//float or int??
    static int desPos[2];//desPos[0]=prev, desPos[1]=current des pos
    static float desired_vel;

    //Constants
    //filter coefs
    static const int NZEROSLPO=2;
    static const int NPOLESLPO=2;
    static double GAINLPO(){return 2.555570536e+04;}
    static double GainLPO1(){return -0.9823854506;}
    static double GainLPO2(){return 1.9822289298;}
    static float xvLPO1[NZEROSLPO+1];
    static float yvLPO1[NPOLESLPO+1];
    static float xvLPO2[NZEROSLPO+1];
    static float yvLPO2[NPOLESLPO+1];
    static float xvLPO3[NZEROSLPO+1];
    static float yvLPO3[NPOLESLPO+1];
    static const int NZEROS_obs=1;
    static const int NPOLES_obs=1;
    static double GAIN_obs(){return 3.193088390e+02;}
    static double Gain1_obs(){return 0.9937364715;}
    static float xv_obs[NZEROS_obs+1];
    static float yv_obs[NPOLES_obs+1];

    //K_model_po=coulomb friction, K_model_po=friction coeff, both estimated previously (hand-specific, matlab)
    static float D_model_po(){return 0.0;}//0.00001 positive -- -0.00000109
    static float K_model_po(){return 0.0003;}//No feedback in free space but oscillations: 0.0007 0.006 - 0.001 -- 0.0002 -- 0.0000554
    static float F_model_po(){return 0.0;}//0.0013 -- 0.0008 -- 0.0

//    static float D_model_po(){return 0.0;}//positive
//    static float K_model_po(){return 0.0;}//0.001 -- 0.0002
//    static float F_model_po(){return 0.0;}//0.0013 -- 0.0008


//    static float D_model_ne(){return 0.0;}//negative
//    static float K_model_ne(){return 0.0;}
//    static float F_model_ne(){return 0.0;}

    //Accessors
    //Diff I_ext and I_Ext_model?
    static float tau_ext_obs;//interaction torque
	static float tau_ext_obs_filtered;
    static float tau_ext, I_ext, I_ext_obs, I_ext_model, tau_ext_model, tau_hat;
    //tau_ext_model_DEMO, I_ext_model_DEMO, tau_ext_ne, I_ext_ne

     //Motor specifications
     static const int gear_ratio=83;
     static double torq_cts(){return 0.0184;}
     static double motor_inertia(){return 0.000000555;}
     static float inv_enc(){return (float)1/628000;}
     static float gear_rat_inv(){return (float) 1/gear_ratio;}
     static float torq_cts_inv(){return (float) 1/torq_cts();}
	 static float vel_mult_cts(){return (float) (2*M_PI)/(628000);}


};
#endif // TORQUEOBSERVER_H
