#include "TorqueObserver.h"
#include <stdlib.h> //malloc
#include "utils.h"
#include <math.h> //sqrt
//#include <unistd.h> //usleep, in ms
#include <sys/stat.h> //for IPC through mkfifo()
#include "SoftHandSyn.h" //for pos first contact
#include <stdio.h>

//Definition static non-const members
float TorqueObserver::xvLPO1[NZEROSLPO+1]={0};
float TorqueObserver::yvLPO1[NPOLESLPO+1]={0};
float TorqueObserver::xvLPO2[NZEROSLPO+1]={0};
float TorqueObserver::yvLPO2[NPOLESLPO+1]={0};
float TorqueObserver::xvLPO3[NZEROSLPO+1]={0};
float TorqueObserver::yvLPO3[NPOLESLPO+1]={0};
float TorqueObserver::xv_obs[NZEROS_obs+1]={0};
float TorqueObserver::yv_obs[NPOLES_obs+1]={0};
float TorqueObserver::tau_ext=0,TorqueObserver::tau_ext_obs=0, TorqueObserver::I_ext=0, TorqueObserver::I_ext_obs=0, TorqueObserver::I_ext_model=0,  TorqueObserver::tau_ext_model=0, TorqueObserver::tau_hat=0;
float TorqueObserver::tau_ext_obs_filtered=0;
//float TorqueObserver::I_ext_ne=0, TorqueObserver::tau_ext_ne=0,TorqueObserver::tau_ext_model_DEMO=0, TorqueObserver::I_ext_model_DEMO=0;
float TorqueObserver::I_filtered=0.0;
double TorqueObserver::pos_filtered=0.0;
int TorqueObserver::desPos[2]={0};
float TorqueObserver::currentOffset=0;
float TorqueObserver::desired_vel=0;
//double TorqueObserver::stiff_contact;

//*********************************************************************
void TorqueObserver::Update_I_filtered(const float & a_current)
{
     //Filter current
     TorqueObserver::xvLPO1[0] = TorqueObserver::xvLPO1[1];
     TorqueObserver::xvLPO1[1] = TorqueObserver::xvLPO1[2];
     TorqueObserver::xvLPO1[2] = a_current / TorqueObserver::GAINLPO();

     TorqueObserver::yvLPO1[0] = TorqueObserver::yvLPO1[1];
     TorqueObserver::yvLPO1[1] = TorqueObserver::yvLPO1[2];
     TorqueObserver::yvLPO1[2] =    (TorqueObserver::xvLPO1[0] + TorqueObserver::xvLPO1[2]) + 2 * TorqueObserver::xvLPO1[1] + (TorqueObserver::GainLPO1()  * TorqueObserver::yvLPO1[0]) + (  TorqueObserver::GainLPO2() * TorqueObserver::yvLPO1[1]);

     TorqueObserver::I_filtered = TorqueObserver::yvLPO1[2];
}
/*
void TorqueObserver::Update_Pos_filtered(const int & a_pos)
{
      //Filter pos
      TorqueObserver::xvLPO2[0] = TorqueObserver::xvLPO2[1];
      TorqueObserver::xvLPO2[1] = TorqueObserver::xvLPO2[2];
      TorqueObserver::xvLPO2[2] = ( a_pos) / TorqueObserver::GAINLPO();

      TorqueObserver::yvLPO2[0] = TorqueObserver::yvLPO2[1];
      TorqueObserver::yvLPO2[1] = TorqueObserver::yvLPO2[2];
      TorqueObserver::yvLPO2[2] = (TorqueObserver::xvLPO2[0] + TorqueObserver::xvLPO2[2]) + 2 * TorqueObserver::xvLPO2[1] + (TorqueObserver::GainLPO1()  * TorqueObserver::yvLPO2[0]) + (  TorqueObserver::GainLPO2()* TorqueObserver::yvLPO2[1]);

      TorqueObserver::pos_filtered = TorqueObserver::yvLPO2[2];
}*/

//void TorqueObserver::Update_InteractionTorque(const int & a_desPos,const int & teta_init)
void TorqueObserver::Update_InteractionTorque(const int & a_desPos,const int & a_sh_pos_actual) //sets TorqueObserver::tau_ext_model, TorqueObserver::tau_ext_obs, TorqueObserver::desired_vel and TorqueObserver::tau_hat
{
    //TO CHECK
    //unused motor_vel? int or double for motor_vel?
    //int or float for desired_vel?
    //teta_ini=homePos?

	/*printf("a_desPos: %d\n", a_desPos);
	printf("a_sh_pos_actual: %d\n\n", a_sh_pos_actual);*/

    //Update desPos
    //init
	TorqueObserver::desPos[0]=TorqueObserver::desPos[1];
    TorqueObserver::desPos[1]=a_desPos;
//TEST!!
	//TorqueObserver::desPos[0] = TorqueObserver::desPos[1];
	//TorqueObserver::desPos[1] = a_sh_pos_actual;

    //int motor_vel_loc=-TorqueObserver::gear_ratio*1000*(TorqueObserver::pos_filtered-pos_old)*TorqueObserver::vel_mult_cts();// FROM MOTOR, 1000 is 1/delta_t
    TorqueObserver::desired_vel=-TorqueObserver::gear_ratio*1000*(TorqueObserver::desPos[1]-TorqueObserver::desPos[0])*TorqueObserver::vel_mult_cts();// FROM DESIRED INPUTS

	//printf("plop %f\n", desired_vel);

    // CALCULATE HAND DISTURBANCE MODEL [tau_ext_model, used for feedforward only]
    //init
	if(TorqueObserver::desired_vel<0)//When hand is closing (no compensation during opening)
    {
        TorqueObserver::tau_ext_model=-( TorqueObserver::F_model_po() + TorqueObserver::K_model_po()*TorqueObserver::vel_mult_cts()*(SoftHandSyn::posMin -TorqueObserver::desPos[1]) + TorqueObserver::D_model_po()*TorqueObserver::desired_vel);
        //printf("\n Tau_ext_model: %f \n",TorqueObserver::tau_ext_model);
		//printf("negative\n");
    }
    else //Hand is opening, observed torque is wrong
    {
         TorqueObserver::tau_ext_model=0;		 
		 //printf("POS\n");
		// printf("plop %f\n", desired_vel);
    }
//TEST!!
	//TorqueObserver::tau_ext_model = -(TorqueObserver::F_model_po() + TorqueObserver::K_model_po()*TorqueObserver::vel_mult_cts()*(SoftHandSyn::posMin - TorqueObserver::desPos[1]) + TorqueObserver::D_model_po()*TorqueObserver::desired_vel);


    // INTERACTION TORQUE OBSERVER [tau_ext_obs]
    //Why not puting this in if(des_vel>0)?
    //TorqueObserver::tau_hat = TorqueObserver::torq_cts() * 0.001* -1* TorqueObserver::I_filtered + (2*PI)*TorqueObserver::motor_inertia()*TorqueObserver::desired_vel - TorqueObserver::tau_ext_model;
	
	//init
	TorqueObserver::tau_hat = TorqueObserver::torq_cts() * 0.001 * TorqueObserver::I_filtered + (1*2 * M_PI)*TorqueObserver::motor_inertia()*TorqueObserver::desired_vel - TorqueObserver::tau_ext_model;
	//TEST!!
	//TorqueObserver::tau_hat = TorqueObserver::torq_cts() * 0.001 * TorqueObserver::I_filtered + ( 2 * PI)*TorqueObserver::motor_inertia()*abs(TorqueObserver::desired_vel) - TorqueObserver::tau_ext_model;

	//printf("Current: %f \n", TorqueObserver::I_filtered);

	//TEST
	/*if (TorqueObserver::desired_vel <= 0)
		TorqueObserver::tau_hat = TorqueObserver::torq_cts() * 0.001 * TorqueObserver::I_filtered + (1 * 2 * PI)*TorqueObserver::motor_inertia()*TorqueObserver::desired_vel - TorqueObserver::tau_ext_model;
	else
		TorqueObserver::tau_hat = TorqueObserver::torq_cts() * 0.001 * TorqueObserver::I_filtered;// +(1 * 2 * PI)*TorqueObserver::motor_inertia()*TorqueObserver::desired_vel - TorqueObserver::tau_ext_model;
		*/

	//when des_vel negative to close hand, I_filtered is positive when closing the hand

    TorqueObserver::xv_obs[0] = TorqueObserver::xv_obs[1];
    TorqueObserver::xv_obs[1] = TorqueObserver::tau_hat / TorqueObserver::GAIN_obs();
    TorqueObserver::yv_obs[0] = TorqueObserver::yv_obs[1];
    TorqueObserver::yv_obs[1] = (TorqueObserver::xv_obs[0] + TorqueObserver::xv_obs[1])+ (  TorqueObserver::Gain1_obs() * TorqueObserver::yv_obs[0]);

    TorqueObserver::tau_ext_obs = TorqueObserver::yv_obs[1];
	TorqueObserver::tau_ext_obs = TorqueObserver::tau_ext_obs - (2 * M_PI)*TorqueObserver::motor_inertia()*TorqueObserver::desired_vel;

//TEST!!
	//TorqueObserver::tau_ext_obs = TorqueObserver::torq_cts() * 0.001 * TorqueObserver::I_filtered*2;

   // TorqueObserver::tau_ext_obs=-TorqueObserver::tau_ext_obs;//Change sign to get positive values when contact
	//tau_ext_obs: positive values when contact
	//TorqueObserver::tau_ext_obs = TorqueObserver::tau_ext_obs * 2;
	//printf("tau_ext_obs: %f\n", tau_ext_obs);
	//printf("\n");

    //me - substract the estimated torque in free space (=fnt(pos motor))
    //TorqueObserver::tau_ext_obs-=3.8*pow(10,-9)*a_sh_pos_actual;//remove torque free space - check!
    /*if(TorqueObserver::tau_ext_obs<0)
    {
        TorqueObserver::tau_ext_obs=0;
    }*/

    TorqueObserver::tau_ext_obs*=TorqueObserver::gear_ratio;
    //printf("Check substraction torque free space! [TorqueObserver::Update_InteractionTorque] \n");

}

void TorqueObserver::Update_CompensationCurrent()//Current corresponding to friction, to use for feedforward
{
    TorqueObserver::I_ext_obs=-TorqueObserver::tau_ext_obs * TorqueObserver::torq_cts_inv();//external current in Amperes
    TorqueObserver::I_ext_model=-TorqueObserver::tau_ext_model * TorqueObserver::torq_cts_inv();

    TorqueObserver::I_ext=TorqueObserver::I_ext_model;//external current in Amperes
    if (TorqueObserver::I_ext>0)
    {
        TorqueObserver::I_ext=0;
    }

    TorqueObserver::currentOffset= 1000 * TorqueObserver::I_ext;//int/float?
}

//User fnt
//void TorqueObserver::Compute_InteractionTorque_FrictionCurrent(const float & a_current,const int & a_desPos,float & res_intTorque_obs,float & res_frictCurrent)
void TorqueObserver::Compute_InteractionTorque_FrictionCurrent(const float & a_current,const int & a_desPos,const int & a_sh_pos_actual)
{
    TorqueObserver::Update_I_filtered(a_current);
    //TorqueObserver::Update_Pos_filtered(const int & a_pos);;
    TorqueObserver::Update_InteractionTorque(a_desPos,a_sh_pos_actual);
    TorqueObserver::Update_CompensationCurrent();
}


void TorqueObserver::Compute_1stContact_StiffContact(const int & a_sh_pos_actual)//Sets SoftHand pos first contact, contact state and TorqueObserver::stiff_contact
{

   // if(TorqueObserver::tau_ext_obs>=TorqueObserver::tau_int_threshContact())//contact
	if (TorqueObserver::tau_ext_obs >= 2)
    {
	
        //Update softhand members contact state and pos first contact
        if (SoftHandSyn::get_contact_state()==false)//first initial contact
        {
			SoftHandSyn::set_contact_state(true);
			SoftHandSyn::set_pos_firstContact(a_sh_pos_actual);
            printf("Initial contact pos=%i [TorqueObserver]\n", SoftHandSyn::get_pos_firstContact());
        }

        //Stiffness estimation
        if(TorqueObserver::desired_vel<=0) //Stiffness evaluation - when contact && when des_vel negative (else torque obs uncorrect)
        {
            int sh_penetration_loc=a_sh_pos_actual- SoftHandSyn::get_pos_firstContact();
            double sh_penetration_rad_loc=sh_penetration_loc*2*M_PI/TorqueObserver::sh_enc_res();
            double stiff_contact_loc=(TorqueObserver::tau_ext_obs-TorqueObserver::tau_int_threshContact())/sh_penetration_rad_loc;
			SoftHandSyn::set_stiff_contact(stiff_contact_loc);
            //Bufferize!


        }
        else //Torque obs cannot be used to estimate the stiffness
        {
            //Keep last stiffness estimated
        }
    }
    else //no contact
    {
        if(SoftHandSyn::get_contact_state()==true)//first non contact
        {
			SoftHandSyn::set_contact_state(false);
			SoftHandSyn::set_stiff_contact(0);
            //sh_deltaPos_initC_current=0;//check
            //sh_deltaPos_initC_current_rad=0;
            //SoftHandSyn::set_pos_firstContact(0);//check if needed - for printf only?
            printf("Stop contact [TorqueObserver]\n");
        }

    }

}

void TorqueObserver::set_tau_ext_obs_filtered(float a_tau_ext_obs_filtered)
{
	TorqueObserver::tau_ext_obs_filtered=a_tau_ext_obs_filtered;
}

//	//***************************loop for filtering*******************************************
//        //Filter current
//        xvLPO1[0] = xvLPO1[1]; xvLPO1[1] = xvLPO1[2];
//        xvLPO1[2] = (joint[0].pidout) / GAINLPO;
//        yvLPO1[0] = yvLPO1[1]; yvLPO1[1] = yvLPO1[2];
//        yvLPO1[2] =    (xvLPO1[0] + xvLPO1[2]) + 2 * xvLPO1[1]
//                     + (GainLPO1  * yvLPO1[0]) + (  GainLPO2* yvLPO1[1]);
//        I_filtered = yvLPO1[2];

//        //Filter pos
//		xvLPO2[0] = xvLPO2[1]; xvLPO2[1] = xvLPO2[2];
//        xvLPO2[2] = ( joint[0].pos) / GAINLPO;
//        yvLPO2[0] = yvLPO2[1]; yvLPO2[1] = yvLPO2[2];
//        yvLPO2[2] =    (xvLPO2[0] + xvLPO2[2]) + 2 * xvLPO2[1]
//                     + (GainLPO1  * yvLPO2[0]) + (  GainLPO2* yvLPO2[1]);
//        pos_filtered = yvLPO2[2];


//		if (run_once<1)
//		{
//			teta_init=my_homepo;
//			++run_once;
//		}

//		////// CALCULATE VELOCITY
//        motor_vel=-gear_ratio*1000*(pos_filtered-pos_old)*vel_mult_cts;// FROM MOTOR, 1000 is 1/delta_t
//        desired_vel=-gear_ratio*1000*(desPos[0]-des_pos_old)*vel_mult_cts;// FROM DESIRED INPUTS
//        //des_pos_old=des_pos previous loop

//		////// CALCULATE HAND DISTURBANCE MODEL
//		if(desired_vel>=0)
//		tau_ext_model =( F_model_po + K_model_po*vel_mult_cts*(teta_init-desPos[0]) + D_model_po*desired_vel);

//		////// INTERACTION TORUQE OBSERVER
//		tau_hat = torq_cts * 0.001* -1* I_filtered + (2*PI)*motor_inertia*desired_vel - tau_ext_model;

//		xv_obs[0] = xv_obs[1];
//        xv_obs[1] = tau_hat / GAIN_obs;
//        yv_obs[0] = yv_obs[1];
//        yv_obs[1] =   (xv_obs[0] + xv_obs[1])
//                     + (  Gain1_obs * yv_obs[0]);
//		tau_ext_obs = yv_obs[1];

//		tau_ext_obs=tau_ext_obs-(2*PI)*motor_inertia*desired_vel;

//		///// CALCULATE CURRENTS AND APPLY AS DISTURBANCE CURRENT
//		I_ext_obs=-tau_ext_obs * torq_cts_inv;//external current in Amperes
//		I_ext_model=-tau_ext_model * torq_cts_inv;

//		I_ext=I_ext_model;//external current in Amperes

//		if (I_ext>0)
//			I_ext=0;

//        //pid_off[0]= 1000 * I_ext; Dont use for now - current offset to feedforward to compensate for the friction, in order to have lower current error
