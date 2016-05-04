#ifndef TELEOPPOINT_H
#define TELEOPPOINT_H

#include "Optitrack.h"
#include "CustomMatrix.h"

class TeleopPoint
{
	
	public:

		struct Tracker
		{
			void set_initialPos(const double a_pos[3]);
			void set_initialPos(const Optitrack::rigidBody & a_rb);
			void set_actualPos(const Optitrack::rigidBody & a_rb);

			void set_initialRot(const Optitrack::rigidBody & a_rb);
			void set_initialRot(CustomMatrix* pa_rot);//For Kuka
			void set_actualRot(const Optitrack::rigidBody & a_rb);


			double m_actualPos[3];
			double m_initialPos[3];
			
			CustomMatrix * pm_rot_initial; //wrt base frame of associated sys (optitrack for master, kuka for slave)
			CustomMatrix * pm_rot_actual;

			//struct constructor
			Tracker():pm_rot_initial(new CustomMatrix(3,3)),pm_rot_actual(new CustomMatrix(3,3))
			{
			}
		};

		Tracker m_master;
		Tracker m_slave;

		CustomMatrix* get_pRot_m2s_ini();
		//void set_rot_m2s_ini();
		//CustomMatrix * pRot_m2s_ini;

		void compute_slave_actualPos_des();//teleop algo
		void compute_slave_deltaPos(double a_deltaPos_slave[3]);
		void compute_slave_actualRot_des();


	private:




};

#endif