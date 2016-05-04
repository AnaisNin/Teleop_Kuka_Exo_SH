#ifndef KINEMATICS_H
#define KINEMATICS_H

#include "CustomMatrix.h"
#include "Exoskeleton.h"
#include "PCA_synergy.h"
#include "Globals.h"

class Kinematics
{

 public:
	static void computeFwdKin(Exoskeleton * pExoskeleton); //user fnt - sets Exo jointPos_dh, rot tip, quat tip, euler tip, pos tip and jacobian tip
	static void compute_x_dot(Exoskeleton * pExoskeleton);//User fnt - sets x_dot=J_exo.q_dot
	static void compute_torque_ref(Exoskeleton * pExoskeleton);
	//, q_dot, velTip for each finger
	//Exoskeleton::Ftip_des_base should have been set previoulsy by 
	
 private:
	//Per finger fnts - internal, args level (no access to Exoskeleton)
	static void convert_jointPos_enc2dh(const int fingerId, const double jointPos_enc[Globals::nq_kins_pf],double jointPos_dh[Globals::nq_kins_pf]); //rad - called by computeFwdKin
    static void get_rot_base2tip_base(const int fingerId, const double jointPos_dh[Globals::nq_kins_pf],CustomMatrix * p_rotMatrix); //called by computeFwdKin
	static void get_Xp_base2tip_base(const int fingerId, const double jointPos_dh[Globals::nq_kins_pf], double res_Xp[3]); //called by computeFwdKin
	static void get_Jacobian_base2tip_base(const int fingerId, const double jointPos_dh[Globals::nq_kins_pf],CustomMatrix * p_res_Jacobian); //called by computeFwdKin
	static void project_force2torque(CustomMatrix * p_Jacobian,const double force_tip_des_base[Globals::mCart_kins_pf], double & res_torque_0); //per finger; called by compute_torque_ref
	static void compute_velTip_base(const double q_dot_dh[Globals::nq_kins_pf],CustomMatrix * p_Jacobian,double res_velTip[Globals::mCart_kins_pf]);//called by computeFwdKin
	//static void compute_q_dot_dh(const double jointPos_dh[Globals::nq_kins_pf],double res_q_dot_dh[Globals::nq_kins_pf]);
	//static void comput

};
#endif // KINEMATICS_H
