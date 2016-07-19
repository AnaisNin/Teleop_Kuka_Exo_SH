#include "TeleopPoint.h"
#include "MathUtils.h"

#include "NATUtils.h"

//CustomMatrix * pRot_m2s_ini=new CustomMatrix(3,3);

CustomMatrix* TeleopPoint::get_pRot_m2s_ini()
{
	//printf("TO SET - TeleopPoint::set_rot_m2s_ini() \n");

	CustomMatrix * pRot=new CustomMatrix(3,3);

	//Identity
	//(*pRot)(1,1)=1;
	//(*pRot)(2,1)=0;
	//(*pRot)(3,1)=0;

	//(*pRot)(1,2)=0;
	//(*pRot)(2,2)=1;
	//(*pRot)(3,2)=0;

	//(*pRot)(1,3)=0;
	//(*pRot)(2,3)=0;
	//(*pRot)(3,3)=1;

//Mine (z normal to screen, x leftwards)
	//(*pRot)(1,1)=1;
	//(*pRot)(2,1)=0;
	//(*pRot)(3,1)=0;

	//(*pRot)(1,2)=0;
	//(*pRot)(2,2)=0;
	//(*pRot)(3,2)=-1;

	//(*pRot)(1,3)=0;
	//(*pRot)(2,3)=1;
	//(*pRot)(3,3)=0;

//Dario
	//(*pRot)(1,1)=-1;
	//(*pRot)(2,1)=0;
	//(*pRot)(3,1)=0;

	//(*pRot)(1,2)=0;
	//(*pRot)(2,2)=0;
	//(*pRot)(3,2)=1;

	//(*pRot)(1,3)=0;
	//(*pRot)(2,3)=1;
	//(*pRot)(3,3)=0;

//Experiment
	(*pRot)(1,1)=0;
	(*pRot)(2,1)=0;
	(*pRot)(3,1)=-1;

	(*pRot)(1,2)=-1;
	(*pRot)(2,2)=0;
	(*pRot)(3,2)=0;

	(*pRot)(1,3)=0;
	(*pRot)(2,3)=1;
	(*pRot)(3,3)=0;

	//Experiment
	//(*pRot)(1,1)=-1;
	//(*pRot)(2,1)=0;
	//(*pRot)(3,1)=0;

	//(*pRot)(1,2)=0;
	//(*pRot)(2,2)=0;
	//(*pRot)(3,2)=1;

	//(*pRot)(1,3)=0;
	//(*pRot)(2,3)=1;
	//(*pRot)(3,3)=0;


	//printf("[get_pRot_m2s_ini] \n");
	//pRot->printf_Matrix();

	return pRot;
}

//void TeleopPoint::set_rot_m2s_ini()
//{
//	printf("TO SET - TeleopPoint::set_rot_m2s_ini() \n");
//
//	(*(this->pRot_m2s_ini))(0,0)=1;
//	(*(this->pRot_m2s_ini))(1,0)=0;
//	(*(this->pRot_m2s_ini))(2,0)=0;
//
//	(*(this->pRot_m2s_ini))(0,1)=0;
//	(*(this->pRot_m2s_ini))(1,1)=1;
//	(*(this->pRot_m2s_ini))(2,1)=0;
//
//	(*(this->pRot_m2s_ini))(0,2)=0;
//	(*(this->pRot_m2s_ini))(1,2)=0;
//	(*(this->pRot_m2s_ini))(2,2)=1;
//}

void TeleopPoint::Tracker::set_initialPos(const double a_pos[3])
{
	for(int it=0;it<3;it++)
	{
		this->m_initialPos[it]=a_pos[it];
	}
	
}

void TeleopPoint::Tracker::set_initialPos(const Optitrack::rigidBody & a_rb)
{
		this->m_initialPos[0]=a_rb.x;
		this->m_initialPos[1]=a_rb.y;
		this->m_initialPos[2]=a_rb.z;	
}

void TeleopPoint::Tracker::set_actualPos(const Optitrack::rigidBody & a_rb)
{
	this->m_actualPos[0]=a_rb.x;
	this->m_actualPos[1]=a_rb.y;
	this->m_actualPos[2]=a_rb.z;
}

void TeleopPoint::compute_slave_actualPos_des()
{
	//Des pos=pos ini + delta pos
	//With delta pos= master pos actual - master pos ini

	//Compute delta pos master 
	double l_deltaPosM_m[3];//Delta pos ini-actual master expressed in master frame
	for(int it=0;it<3;it++)
	{
		l_deltaPosM_m[it]=this->m_master.m_actualPos[it]-this->m_master.m_initialPos[it];	
	}
	//printf("Delta pos master: %g %g %g \t [TeleopPoint::compute_slave_actualPos_des] \n",l_deltaPosM_m[0],l_deltaPosM_m[1],l_deltaPosM_m[2]);

	//Change expression frame from master to slave
	CustomMatrix * l_pRot_m2s_ini_transpose=(this->get_pRot_m2s_ini())->transpose();
	//printf("Rot m2s: \n");
	//this->get_pRot_m2s_ini()->printf_Matrix();
	//l_pRot_m2s_ini_transpose->printf_Matrix();
	
	double* l_pDeltaPosM_s=CustomMatrix::MatrixProduct(l_pRot_m2s_ini_transpose,l_deltaPosM_m);//3x1
	//printf("Delta pos slave: %g %g %g \t [TeleopPoint::compute_slave_actualPos_des] \n",l_pDeltaPosM_s[0],l_pDeltaPosM_s[1],l_pDeltaPosM_s[2]);

	//Compute des actual pos slave = pos ini + delta pos ini-actual
	for(int it=0;it<3;it++)
	{
		this->m_slave.m_actualPos[it]=this->m_slave.m_initialPos[it]+l_pDeltaPosM_s[it];
	}

	//Note: Kuka is taking only delta pos, not final pos. So no need to compute last step=initial+delta.

}

void TeleopPoint::compute_slave_deltaPos(double a_deltaPos_slave[3])
{	
	//Des pos=pos ini + delta pos
	//With delta pos= master pos actual - master pos ini

	//Compute delta pos master 
	double l_deltaPosM_m[3];//Delta pos ini-actual master expressed in master frame
	for(int it=0;it<3;it++)
	{
		l_deltaPosM_m[it]=this->m_master.m_actualPos[it]-this->m_master.m_initialPos[it];	
	}
	//printf("Delta pos master: %g %g %g \t [TeleopPoint::compute_slave_actualPos_des] \n",l_deltaPosM_m[0],l_deltaPosM_m[1],l_deltaPosM_m[2]);

	//Change expression frame from master to slave
	CustomMatrix * l_pRot_m2s_ini_transpose=(this->get_pRot_m2s_ini())->transpose();
	//printf("Rot m2s: \n");
	//this->get_pRot_m2s_ini()->printf_Matrix();
	//l_pRot_m2s_ini_transpose->printf_Matrix();
	
	double* l_pDeltaPosM_s=CustomMatrix::MatrixProduct(l_pRot_m2s_ini_transpose,l_deltaPosM_m);//3x1
	for(int it=0;it<3;it++)
	{
		a_deltaPos_slave[it]=l_pDeltaPosM_s[it];
	}
	//printf("Delta pos slave: %g %g %g \t [TeleopPoint::compute_slave_actualPos_des] \n",l_pDeltaPosM_s[0],l_pDeltaPosM_s[1],l_pDeltaPosM_s[2]);
}

void TeleopPoint::Tracker::set_initialRot(const Optitrack::rigidBody & a_rb)
{

////////
//UNCOMMENT!!
	//double l_quat[4]={0};
	//l_quat[0]=a_rb.qx;
	//l_quat[1]=a_rb.qy;
	//l_quat[2]=a_rb.qz;
	//l_quat[3]=a_rb.qw;

	////Convert quat to rot
	//CustomMatrix* pl_rot=MathUtils::QuatToRot(l_quat);
	//
	////Set class member
	//this->pm_rot_initial=pl_rot;

	////TEST!
	//EulerAngles Eul_FromQuat(Quat q, int order)
			//typedef struct {float x, y, z, w;} Quat
			Quat q_loc;
			q_loc.x=a_rb.qx;
			q_loc.y=a_rb.qy;
			q_loc.z=a_rb.qz;
			q_loc.w=a_rb.qw;

			EulerAngles euler_loc;
			euler_loc=Eul_FromQuat(q_loc,EulOrdXYZr);
			//roll-yaw-pitch (same as Opti real time display)
			//printf("Euler: %f %f %f \n",NATUtils::RadiansToDegrees(euler_loc.x),NATUtils::RadiansToDegrees(euler_loc.y),NATUtils::RadiansToDegrees(euler_loc.z));

			HMatrix rot_fromEuler_loc;
			Eul_ToHMatrix(euler_loc,rot_fromEuler_loc);
			//printf("%f %f %f \n %f %f %f \n %f %f %f \n\n",rot_fromEuler_loc[0][0],rot_fromEuler_loc[0][1],rot_fromEuler_loc[0][2],rot_fromEuler_loc[1][0],rot_fromEuler_loc[1][1],rot_fromEuler_loc[1][2],rot_fromEuler_loc[2][0],rot_fromEuler_loc[2][1],rot_fromEuler_loc[2][2]);
			
			//Fill class struct
			for(int iti=0;iti<3;iti++)
			{
				for(int itj=0;itj<3;itj++)
				{
					(*this->pm_rot_initial)(iti+1,itj+1)=rot_fromEuler_loc[iti][itj];
				}
			}


}

void TeleopPoint::Tracker::set_actualRot(const Optitrack::rigidBody & a_rb)
{

////////////////////
	//UNCOMMENT!
 //   double l_quat[4]={0};
	//l_quat[0]=a_rb.qx;
	//l_quat[1]=a_rb.qy;
	//l_quat[2]=a_rb.qz;
	//l_quat[3]=a_rb.qw;

	////Convert quat to rot
	//CustomMatrix* pl_rot=MathUtils::QuatToRot(l_quat);
	//
	////Set class member
	//this->pm_rot_actual=pl_rot;

////TEST!
	//EulerAngles Eul_FromQuat(Quat q, int order)
			//typedef struct {float x, y, z, w;} Quat
			Quat q_loc;
			q_loc.x=a_rb.qx;
			q_loc.y=a_rb.qy;
			q_loc.z=a_rb.qz;
			q_loc.w=a_rb.qw;

			EulerAngles euler_loc;
			euler_loc=Eul_FromQuat(q_loc,EulOrdXYZr);
			//roll-yaw-pitch (same as Opti real time display)
			//printf("Euler: %f %f %f \n",NATUtils::RadiansToDegrees(euler_loc.x),NATUtils::RadiansToDegrees(euler_loc.y),NATUtils::RadiansToDegrees(euler_loc.z));

			HMatrix rot_fromEuler_loc;
			Eul_ToHMatrix(euler_loc,rot_fromEuler_loc);
			//printf("%f %f %f \n %f %f %f \n %f %f %f \n\n",rot_fromEuler_loc[0][0],rot_fromEuler_loc[0][1],rot_fromEuler_loc[0][2],rot_fromEuler_loc[1][0],rot_fromEuler_loc[1][1],rot_fromEuler_loc[1][2],rot_fromEuler_loc[2][0],rot_fromEuler_loc[2][1],rot_fromEuler_loc[2][2]);
			
			//Fill class struct
			for(int iti=0;iti<3;iti++)
			{
				for(int itj=0;itj<3;itj++)
				{
					(*this->pm_rot_actual)(iti+1,itj+1)=rot_fromEuler_loc[iti][itj];
				}
			}
}


//check!
void TeleopPoint::compute_slave_actualRot_des()
{
	//Rotation matrix master initial-actual, expressed in master frame (optitrack)
	CustomMatrix * pRot_m_ini2actual_m=CustomMatrix::MatrixProduct(this->m_master.pm_rot_actual,this->m_master.pm_rot_initial->getInverse());

	//Transformation master initial-actual, expressed in slave frame (kuka)
	CustomMatrix * pRot_interm=CustomMatrix::MatrixProduct(pRot_m_ini2actual_m,TeleopPoint::get_pRot_m2s_ini());
	CustomMatrix * pRot_m_ini2actual_s=CustomMatrix::MatrixProduct(TeleopPoint::get_pRot_m2s_ini()->getInverse(),pRot_interm);

	//Transformation slave initial-actual=transformation master initial-actual
	//Transformation slave kuka to actual=transformation slave initial-actual * transformation slave kuka to initial
	this->m_slave.pm_rot_actual=CustomMatrix::MatrixProduct(pRot_m_ini2actual_s,this->m_slave.pm_rot_initial);
}

void TeleopPoint::Tracker::set_initialRot(CustomMatrix* pa_rot)
{
	this->pm_rot_initial=pa_rot;
}
