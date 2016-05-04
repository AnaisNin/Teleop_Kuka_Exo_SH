#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef __WIN32
#include "stdafx.h"
#endif

//Kuka
#include <iostream>
#include <cstdlib>
#include <math.h>
#include <limits.h>
//#include "friudp.h"
//#include "friremote.h"

//Optitrack
#include <stdio.h>
#include <tchar.h>
#include <conio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h> //Sleep()
#include <errno.h> 
#include "Optitrack.h"

//Kuka
#include <friComm.h>
#include <friremote.h>
#include <friudp.h>

#include "TeleopPoint.h"

#include "OneEuroFilter.h"
#include "CPrecisionClock.h"
#include "CThread.h"

//To Remove, OneEuroFilter test
void
randSeed(void) {
  srand(time(0)) ;
}
double
unifRand(void) {
  return rand() / double(RAND_MAX) ;
}
//typedef double TimeStamp ; // in seconds
//static const TimeStamp UndefinedTime = -1.0 ;
//end test filter

//Clock
cPrecisionClock* Clock_a;
cPrecisionClock* Clock_b;
//double timerTeleop;
double TimerLoop;
bool stopProgram=false;

#pragma warning( disable : 4996 )

//#include "BoardLibrary.h"
//#pragma comment(lib, "BoardLibrary.lib")
#pragma comment(lib,"Ws2_32.lib")

#ifndef M_PI 
#define M_PI 3.14159
#endif

using namespace std;

//Declaration
void updateLoop_Kuka(void);
void Myprint(void);
void GetKeyboard(void);
void Show_options();
void gotoxy( int column, int line );
void ClearScreen();

int 
#ifdef __WIN32

_tmain
#else
#ifdef _WRS_KERNEL
friSecondApp
#else
main
#endif
#endif
(int argc, char *argv[])
{
	cThread * mainThreadKuka=new cThread();

	Clock_a = new cPrecisionClock();
	Clock_b=new cPrecisionClock();
	//timerTeleop=0;
	//OneEuroFilter::m_TimerTeleop=0;

//TEST OneEuroFilter
 // randSeed() ;
 // double duration = 10.0 ; // seconds
 // double frequency = 120 ; // Hz
 // double mincutoff = 1.0 ; // FIXME
 // double beta = 1.0 ;      // FIXME
 // double dcutoff = 1.0 ;   // this one should be ok

 // std::cout << "#SRC OneEuroFilter.cc" << std::endl
	//    << "#CFG {'beta': " << beta << ", 'freq': " << frequency << ", 'dcutoff': " << dcutoff << ", 'mincutoff': " << mincutoff << "}" << std::endl
	//    << "#LOG timestamp, signal, noisy, filtered" << std::endl ;

 // OneEuroFilter f(frequency, mincutoff, beta, dcutoff) ;
 // for (TimeStamp timestamp=0.0; timestamp<duration; timestamp+=1.0/frequency) {
 //   double signal = sin(timestamp) ;
 //   double noisy = signal + (unifRand()-0.5)/5.0 ;
 //   double filtered = f.filter(noisy, timestamp) ;
 //   std::cout << timestamp << ", "
	//      << signal << ", "
	//      << noisy << ", "
	//      << filtered
	//      << std::endl ;
	//}

 // printf("Sleeping for a while.. \n");
 // Sleep(10000);


/////////////////////
		bool flg_kuka=true;

		//Optitrack
		int errorCode=Optitrack::Initialize();//Starts loop updating values from Optitrack
		Sleep(5000);

		printf("Start thread kuk \n");
		mainThreadKuka->set(updateLoop_Kuka,CHAI_THREAD_PRIORITY_HAPTICS);

		while(!stopProgram)
		{
			Sleep(20);
		}

	return EXIT_SUCCESS;


	////////////////////////////////////////
//	bool flg_kuka=true;
//
//	//Optitrack
//	//int errorCode=Optitrack::Initialize();//Starts loop updating values from Optitrack
//	//Sleep(5000);
//
//	//Teleop
//		TeleopPoint * p_endEffector=new TeleopPoint();//Point teleoperated, master=optitrack tracker 1, slave=Kuka end effector
//	
//		//Set initial pos master==optitrack tracker 1 - put this in a case?
//		//double l_optitrackPos_ini[3]={Optitrack::rb1.x,Optitrack::rb1.y,Optitrack::rb1.z};//Retrieve current pos Optitrack
//		//p_endEffector->m_master.set_initialPos( l_optitrackPos_ini);
//		printf("p_endEffector, set initial pos master: %g %g %g \n",p_endEffector->m_master.m_initialPos[0],p_endEffector->m_master.m_initialPos[1],p_endEffector->m_master.m_initialPos[2]);
//
//		//Set initial pos slave==kuka endeffector - compute
//		double l_kuka_eePos_ini[3]={0,0,0};
//		//Get Cart pos kuka ee in Kuka frame, set l_kuka_eePos_ini
//
//	//Kuka initialization
//	float measCartPos_console[FRI_CART_FRM_DIM];///*!< Number of data within a 3x4 Cartesian frame - homogenous matrix - organized row by row
//	float measCartPos_full[FRI_CART_FRM_DIM];//measured pos total=console+fri commanded
//	float desCartPos_full[FRI_CART_FRM_DIM];//measured pos total=console+fri commanded
//	float cmd_deltaPos_fri[FRI_CART_FRM_DIM];//Commanded through FRI. This is added to pos commanded by console.
//	double deltaPos=0;
//	double timeCounter=0;
//
//	//float newForceTorqueAdd[FRI_CART_VEC];
//	double KukaTimer_Counter=0;
//	friRemote friInst;
//
//
//if(flg_kuka==true)
//{
//	cout << "Opening FRI Interface" << endl;
//	FRI_QUALITY lastQuality = FRI_QUALITY_BAD;
//	FRI_CTRL lastCtrlScheme = FRI_CTRL_OTHER;
//	
//	// do one handshake before the endless loop
//	friInst.doDataExchange();
//	if(friInst.getCurrentControlScheme()!=2)
//	{
//		printf("ERROR - Set cart imp mode from Kuka console before proceeding \n");
//		cout <<"Current control scheme: " <<friInst.getCurrentControlScheme() <<endl;
//		Sleep(5000);
//	}
//
//	printf("Wait to get quality 3 to FriStart from console ..\n");
//	while( friInst.getState() != FRI_STATE_CMD) //till we get the proper quality
//	//while( friInst.getQuality() != 3) //till we get the proper quality
//	{
//			friInst.doDataExchange();
//			if ( friInst.getQuality() != lastQuality)
//			{
//				cout << "quality change detected "<< friInst.getQuality()<< " \n";
//				//cout << friInst.getMsrBuf().intf;
//				//cout << endl;
//				lastQuality=friInst.getQuality();
//			}
//	}
//	printf("Ok, kuka switched to cmd mode. Proceed.. \n");
//	//put handshake here to validate with console that we're ready.
//
//	//USEFUL? Should this be in the loop? yep.	
//	// do some handshake to KRL - Do we Have to do this?
//		// send to krl int a value
//		//friInst.setToKRLInt(0,1);
//		//if ( friInst.getQuality() >= FRI_QUALITY_OK)
//		//{
//		//	// send a second marker
//		//	friInst.setToKRLInt(0,10);
//		//}
//		//// just mirror the real value..
//		//friInst.setToKRLReal(0,friInst.getFrmKRLReal(1));
//		////////////////////////////////////////////
//
//		
//		bool firstLoop=true;
//
//		//while(1)
//		for(;;)
//		{
//			//if(firstLoop)
//			//{
//			//		//Get initial pos
//			//		for (int i = 0; i < FRI_CART_FRM_DIM; i++)
//			//		{
//			//			measCartPos_console[i] = friInst.getMsrCmdCartPosition()[i]; //Get Cart pos
//			//		}
//			//		l_kuka_eePos_ini[0]=measCartPos_console[3];
//			//		l_kuka_eePos_ini[1]=measCartPos_console[7];
//			//		l_kuka_eePos_ini[2]=measCartPos_console[11];
//			//		printf("Setting Kuka EE initial pos: %g %g %g \n",l_kuka_eePos_ini[0],l_kuka_eePos_ini[1],l_kuka_eePos_ini[2]);
//			//		p_endEffector->m_slave.set_initialPos(l_kuka_eePos_ini);
//
//			//		firstLoop=false;
//			//}
//
//			//printf("while \n");
//			//for (int i = 0; i < FRI_CART_FRM_DIM; i++)
//			//{
//			//		desCartPos_full[i] = friInst.getMsrCmdCartPosition()[i]; //Get Cart pos
//			//}
//
//			if ( lastCtrlScheme != friInst.getCurrentControlScheme())
//			{
//				cout << "switching control scheme " << lastCtrlScheme;
//				lastCtrlScheme = friInst.getCurrentControlScheme();
//				cout << " to " << lastCtrlScheme;
//			}
//
//			switch ( friInst.getCurrentControlScheme())
//			{
//				case FRI_CTRL_CART_IMP:
//				/** joint/cart positions, joint/cart stiffness, joint/cart damping
//				and additional TCP F/T can be commanded */
//				{
//					float newCartVals[FRI_CART_FRM_DIM];
//					for (int i = 0; i < FRI_CART_FRM_DIM; i++)
//					{
//						newCartVals[i] = friInst.getMsrCmdCartPosition()[i]; //Get Cart pos
//					}
//
//					if ( friInst.getState() == FRI_STATE_CMD)
//					{
//						//cout <<"CMD mode - ok" <<endl;
//						if ( friInst.isPowerOn() )
//						{
//							printf("yop \n");
//								timeCounter+=friInst.getSampleTime();
//								for (int i = 1; i <= 3; i++)
//								{
//									// perform some sort of sine wave motion
//									newCartVals[(i*4)-1]+=(float)sin( timeCounter * M_PI * 0.02) * (0.1f);
//								}
//
//						}
//					}
//					friInst.doCartesianImpedanceControl(newCartVals,NULL,NULL,NULL);
//				}break;
//				case   FRI_CTRL_POSITION:
//				{
//					friInst.doDataExchange();
//				}break;
//				case   FRI_CTRL_JNT_IMP:
//				{
//					friInst.doDataExchange();
//					//friInst.doPositionControl(newJntVals);
//				}break;
//				default:
//				{
//					//cout <<"default case for control mode " <<endl;
//				/* do nothing - just data exchange for waiting */
//					friInst.doDataExchange();
//				}break;
//			}//switch
//
//			//if(friInst.getCurrentControlScheme()==FRI_CTRL_CART_IMP)
//			//{
//			//	if ( friInst.getState() == FRI_STATE_CMD &&  friInst.isPowerOn())
//			//	{
//			//		//Update kuka pos
//			//		for (int i = 0; i < FRI_CART_FRM_DIM; i++)
//			//		{
//			//				desCartPos_full[i] = friInst.getMsrCmdCartPosition()[i]; //Get Cart pos
//			//		}
//
//			//		deltaPos+=0.0001;
//			//		desCartPos_full[3]=(float)(l_kuka_eePos_ini[0]+deltaPos);//m
//			//		printf("yop \n");
//			//		friInst.doCartesianImpedanceControl(desCartPos_full,NULL,NULL,NULL);//Call this anyway at each loop to keep kuka runnin
//			//		//friInst.doDataExchange();//Mandatory to update quality
//			//	}
//			//	else
//			//	{
//			//		printf("ERROR - not in cmd mode OR power is off \n");
//			//		 friInst.doDataExchange();//Mandatory to update quality
//			//	}
//			//	//friInst.doDataExchange();//Mandatory to update quality
//			//	//friInst.doCartesianImpedanceControl(desCartPos_full,NULL,NULL,NULL);//Call this anyway at each loop to keep kuka runnin
//			//	
//			//}
//			//else //not impedance cart mode, do nothing apart from maintaining communication
//			//{
//			//	 friInst.doDataExchange();//Mandatory to update quality
//			//}
//			
//			
//				// have some debug information every n.th. step
//			int divider = (int)( (1./friInst.getSampleTime()) *5.0);
//
//			if ( friInst.getSequenceCount() % divider == 0)
//			{
//				cout << "krl interaction \n";
//				/*cout << friInst.getMsrBuf().krl;
//				cout << "intf stat interaction \n";
//				cout << friInst.getMsrBuf().intf.stat;
//				cout << "smpl " << friInst.getSampleTime();
//
//				cout << endl;*/
//			}
//
//			// Stop request is issued from the other side
//			if ( friInst.getFrmKRLInt(0) == -1) 
//			{
//				cout << "leaving \n";
//				break;	  
//			}
//			
//			//anyway, keep udpating quality. Note that this value is updated iif friInst.doDataExchange() or friInst.doCartesianImpedanceControl(newCartVals,NULL,NULL,NULL); is called.
//			//friInst.doDataExchange();//Mandatory to update quality
//			if ( friInst.getQuality() != lastQuality)
//			{
//				cout << "quality change detected "<< friInst.getQuality()<< " \n";
//				//cout << friInst.getMsrBuf().intf;
//				//cout << endl;
//				lastQuality=friInst.getQuality();
//			}
//
//
//			//Not really useful, this is position commanded by console, always the same
//			//for (int i = 0; i < FRI_CART_FRM_DIM; i++)
//			//{
//			//	measCartPos_console[i] = friInst.getMsrCmdCartPosition()[i]; //Get Cart pos
//			//}
//
//
//			//Do nothing, keep runing
//			
//
//		
//		}//while (main loop)
//}
//
//
//
//			//This should be in a loop
//			while(0)
//			{
//				//Get pos
//				for (int i = 0; i < FRI_CART_FRM_DIM; i++)
//				{
//					//newCartVals[i] = friInst.getMsrCmdCartPosition()[i]; //Get Cart pos
//				}
//
//					if ( friInst.getState() == FRI_STATE_CMD)
//					{
//						//cout <<"CMD mode - ok" <<endl;
//						if ( friInst.isPowerOn() )
//						{
//							//cout <<"Power on - ok" <<endl;
//
//							//cout <<"Time: " <<friInst.getSampleTime() <<endl;
//							//Print Cart pos
//							//Set new Cart pos
//							//if ( friInst.getFrmKRLInt(1)  >= 1 )
//							//{
//							//	/// do force superposition...
//							//	/// if the KRL Side has set $FRI_TO_INT[2] >= 1
//							//	for (int i = 0; i < 3; i++)
//							//	newForceTorqueAdd[i]+=(float)sin( timeCounter * M_PI * 0.03) * (10.);
//							//}
//							//if ( friInst.getFrmKRLInt(1) <= 2) //Cmd sent from console, check what i deleted 
//							//{
//							//	/// do Cartesian position superposition
//							//	/// if the KRL Side has set $FRI_TO_INT[2] <= 2
//							//printf("Pos end effector: %g %g %g \n", newCartVals[3],newCartVals[7],newCartVals[11]);
//							 KukaTimer_Counter+=friInst.getSampleTime();
//								for (int i = 1; i <= 3; i++)
//								{
//									// perform some sort of sine wave motion
//									//newCartVals[(i*4)-1]+=(float)sin(   KukaTimer_Counter * M_PI * 0.02) * (0.1f);
//								}
//							//}
//						}
//						else
//						{
//							  KukaTimer_Counter=0.;
//							//cout <<"Power off" <<endl;
//						}
//					}
//					else
//					{
//						 KukaTimer_Counter=0.;
//						//cout <<"Pb - not in cmd mode" <<endl;
//					}
//					// Call to data exchange - and the like 
//					//friInst.doCartesianImpedanceControl(newCartVals,NULL,NULL,newForceTorqueAdd);
//			}//while
//
//
//	///////////////////////
//
//
//	//Make a separate thread to update pos Kuka from optitrack?
//	while(false)
//	{
//			//printf("Rigid Body 1 [x y z qx qy qz qw]\t%3.2f\t%3.2f\t%3.2f\t%3.2f\t%3.2f\t%3.2f\t%3.2f\n",Optitrack::rb1.x,Optitrack::rb1.y,Optitrack::rb1.z,Optitrack::rb1.qx,Optitrack::rb1.qy,Optitrack::rb1.qz,Optitrack::rb1.qw);
//			
//			p_endEffector->m_master.set_actualPos(Optitrack::rb1);//update actual pos master
//			p_endEffector->compute_slave_actualPos_des();//compute corresponding desired pos slave
//			//printf("Pos master: %g \t %g \t %g \n",p_endEffector->m_master.m_actualPos[0],p_endEffector->m_master.m_actualPos[1],p_endEffector->m_master.m_actualPos[2]);
//			//printf("Delta pos master: 5g 5g %g \n",p_endEffector->);
//			//printf("Pos slave: %g \t %g \t %g \n",p_endEffector->m_slave.m_actualPos[0],p_endEffector->m_slave.m_actualPos[1],p_endEffector->m_slave.m_actualPos[2]);
//			Sleep(1000);
//
//			//Set pos kuka EE as p_endEffector->m_slave.m_actualPos
//
//			//Send pos kuka
//	}
//
//	//Kuka 
//   /*int firstRun_flag = 0;
//    float tobeaddedoptix=0, tobeaddedoptiy=0, tobeaddedoptiz=0, tobeaddedkukax=0, tobeaddedkukay=0, tobeaddedkukaz=0, tobeaddedx=0, tobeaddedy=0, tobeaddedz=0;
//    float offsetoptix=0, offsetoptiy=0, offsetoptiz=0, offsetkukax=0, offsetkukay=0, offsetkukaz=0;
//    float newCartVals[FRI_CART_FRM_DIM];
//    float mydata_RigidBodies_x, mydata_RigidBodies_y, mydata_RigidBodies_z;
//
//    cout << "Opening FRI Interface" << endl;*/
// //   //{
//	//	//friRemote friInst;
//        //friRemote friInst(49948,"192.168.0.10" );
// //       FRI_QUALITY lastQuality = FRI_QUALITY_BAD;
// //       FRI_CTRL lastCtrlScheme = FRI_CTRL_OTHER;
//
// //       friInst.doDataExchange();
//
// //       for(;;)
// //       {
//
// //           friInst.setToKRLInt(0,1);
// //           if ( friInst.getQuality() >= FRI_QUALITY_OK)
// //           {
// //               friInst.setToKRLInt(0,10);
// //           }
//
// //           friInst.setToKRLReal(0,friInst.getFrmKRLReal(1));
//
// //           if ( lastCtrlScheme != friInst.getCurrentControlScheme())
// //           {
// //               cout << "switching control scheme " << lastCtrlScheme;
// //               lastCtrlScheme = friInst.getCurrentControlScheme();
// //               cout << " to " << lastCtrlScheme<<endl;
// //           }
//
// //           switch (friInst.getCurrentControlScheme())
// //           {
// //           case   FRI_CTRL_POSITION:
// //           case   FRI_CTRL_JNT_IMP:
// //           case   FRI_CTRL_CART_IMP:
//
// //               {
//	//				printf("Cart imp mode \n");
// //                   float kukax, kukay, kukaz;
//
// //                   for (int i = 0; i < FRI_CART_FRM_DIM; i++)
// //                   {
// //                       newCartVals[i] = friInst.getMsrCmdCartPosition()[i];
// //                   }
//
// //                   if ( friInst.getState() == FRI_STATE_CMD)
// //                   {
//	//					//No tracking from optitrack
// //                       //if ((mydata_RigidBodies_x == 0.0)&&(mydata_RigidBodies_y==0.0) && (mydata_RigidBodies_z == 0.0))
// //                       //break;
//
// //                       if ( friInst.isPowerOn() )
// //                       {
//	//						printf("Ready to move, waiting for pos ref \n");
// //                           /*if(firstRun_flag<1)
// //                                  {
// //                                    offsetoptix=(mydata_RigidBodies_x);
// //                                    offsetoptiy=(mydata_RigidBodies_z);
// //                                    offsetoptiz=(mydata_RigidBodies_y);
//
// //                                    offsetkukax=friInst.getMsrCmdCartPosition()[3];
// //                                    offsetkukay=friInst.getMsrCmdCartPosition()[7];
// //                                    offsetkukaz=friInst.getMsrCmdCartPosition()[11];
//
// //                                    firstRun_flag++;
// //                                  }
//
// //                            kukax=newCartVals[3]-offsetkukax;
// //                            kukay=newCartVals[7]-offsetkukay;
// //                            kukaz=newCartVals[11]-offsetkukaz;
//
// //                            tobeaddedoptix=mydata_RigidBodies_x-offsetoptix;
// //                            tobeaddedoptiy=mydata_RigidBodies_z-offsetoptiy;
// //                            tobeaddedoptiz=mydata_RigidBodies_y-offsetoptiz;
//
// //                            tobeaddedkukax=friInst.getMsrCmdCartPosition()[3]-offsetkukax;
// //                            tobeaddedkukay=friInst.getMsrCmdCartPosition()[7]-offsetkukay;
// //                            tobeaddedkukaz=friInst.getMsrCmdCartPosition()[11]-offsetkukaz;
//
// //                            tobeaddedx=(tobeaddedoptix)-tobeaddedkukax;
// //                            tobeaddedy=(tobeaddedoptiy)-tobeaddedkukay;
// //                            tobeaddedz=(tobeaddedoptiz)-tobeaddedkukaz;
//
// //                            newCartVals[3]=newCartVals[3] - tobeaddedx;
// //                            newCartVals[7]=newCartVals[7] + tobeaddedy;
// //                            newCartVals[11]=newCartVals[11] + tobeaddedz;
//	//						 */
// //                       }//if friInst.isPowerOn()
// //                   }//if FRI_STATE_CMD
//
// //                   friInst.doCartesianImpedanceControl(newCartVals,NULL,NULL,NULL);
// //               }//case RI_CTRL_CART_IMP
// //               break;
// //           default:
// //               friInst.doDataExchange();
// //           }
//
// //           if ( friInst.getFrmKRLInt(0) == -1)
// //           {
// //               cout << "leaving \n";
// //               break;
// //           }
//
// //           if ( friInst.getQuality() != lastQuality)
// //           {
// //               cout << "quality change detected "<< friInst.getQuality()<< " \n";
// //               cout << friInst.getMsrBuf().intf;
// //               cout << endl;
// //               lastQuality=friInst.getQuality();
// //           }
// //       }//for
//
//
//	//// Done - clean up.
//	//theClient->Uninitialize();
//
// //   //}
//
//    return EXIT_SUCCESS;

}//main

void updateLoop_Kuka(void)
{
	//Teleop
		TeleopPoint * p_endEffector=new TeleopPoint();//Point teleoperated, master=optitrack tracker 1, slave=Kuka end effector
	
		//Set initial pos master==optitrack tracker 1 - put this in a case?
		//double l_optitrackPos_ini[3]={Optitrack::rb1.x,Optitrack::rb1.y,Optitrack::rb1.z};//Retrieve current pos Optitrack
		p_endEffector->m_master.set_initialPos(Optitrack::rb1);
		p_endEffector->m_master.set_initialRot(Optitrack::rb1);
		printf("p_endEffector, set initial pos master: %g %g %g \n",p_endEffector->m_master.m_initialPos[0],p_endEffector->m_master.m_initialPos[1],p_endEffector->m_master.m_initialPos[2]);
		printf("p_endEffector, set initial rot master: \n");
		p_endEffector->m_master.pm_rot_initial->printf_Matrix();

		printf("TimerOpti: %g \n",Optitrack::m_timer);

		printf("Sleeping for a while, wait for it..\n");
		Sleep(2000);

		//To test only optitrack
		//while(1)
		//{
		//	Myprint();
		//	GetKeyboard();
		//}
		

		//Set initial pos slave==kuka endeffector - compute
		//Get Cart pos kuka ee in Kuka frame, set l_kuka_eePos_ini

	//Kuka initialization
		float measCartPos_console[FRI_CART_FRM_DIM]={0};///*!< Number of data within a 3x4 Cartesian frame - homogenous matrix - organized row by row
		float measCartPos_full[FRI_CART_FRM_DIM]={0};//measured pos total=console+fri commanded
		float desCartPos_full[FRI_CART_FRM_DIM]={0};//measured pos total=console+fri commanded
		float cmd_deltaPos_fri[FRI_CART_FRM_DIM]={0};//Commanded through FRI. This is added to pos commanded by console.
	double deltaPos=0;
	double timeCounter=0;
	bool firstLoop=true;

	//cout << "Opening FRI Interface for Second Sample" << endl;
	//{
		friRemote friInst;
		FRI_QUALITY lastQuality = FRI_QUALITY_BAD;
		FRI_CTRL lastCtrlScheme = FRI_CTRL_OTHER;

		//double timeCounter=0;
		friInst.doDataExchange(); // do one handshake before the endless loop

		if(friInst.getCurrentControlScheme()!=2)
		{
			printf("ERROR - Set cart imp mode from Kuka console before proceeding \n");
			cout <<"Current control scheme: " <<friInst.getCurrentControlScheme() <<endl;
			Sleep(5000);
		}

	//This makes it bug
	//printf("Wait to get quality 3 to FriStart from console ..\n");
	//while( friInst.getState() != FRI_STATE_CMD) //till we get the proper quality
	////while( friInst.getQuality() != 3) //till we get the proper quality
	//{
	//		friInst.doDataExchange();
	//		if ( friInst.getQuality() != lastQuality)
	//		{
	//			cout << "quality change detected "<< friInst.getQuality()<< " \n";
	//			//cout << friInst.getMsrBuf().intf;
	//			//cout << endl;
	//			lastQuality=friInst.getQuality();
	//		}
	//}
	//printf("Ok, kuka switched to cmd mode. Proceed.. \n");

		double deltaPos_slave[3]={0};

				/** enter main loop - wait until we enter stable command mode */
		//for(;;)
		Clock_a->reset();
		Clock_a->start();
		Clock_b->reset();
		Clock_b->start();

		int printTimer=0;

		while(1)
		{
			TimerLoop=Clock_b->getCurrentTimeSeconds();
			Clock_b->reset();
			Clock_b->start();

			printTimer++;
			if(printTimer>50)
			{
				Myprint();
				GetKeyboard();
				printTimer=0;
			}

			//OneEuroFilter::m_TimerTeleop=Clock_a->getCurrentTimeSeconds();

			//printf("Rigid Body 1 [x y z qx qy qz qw]\t%3.2f\t%3.2f\t%3.2f\t%3.2f\t%3.2f\t%3.2f\t%3.2f\n",Optitrack::rb1.x,Optitrack::rb1.y,Optitrack::rb1.z,Optitrack::rb1.qx,Optitrack::rb1.qy,Optitrack::rb1.qz,Optitrack::rb1.qw);			
			p_endEffector->m_master.set_actualPos(Optitrack::rb1);//update actual pos master
			p_endEffector->m_master.set_actualRot(Optitrack::rb1); //To check
//
//			//Set pos kuka EE as p_endEffector->m_slave.m_actualPos
//
//			//Send pos kuka
			if ( lastCtrlScheme != friInst.getCurrentControlScheme())
			{
				cout << "switching control scheme " << lastCtrlScheme;
				lastCtrlScheme = friInst.getCurrentControlScheme();
				cout << " to " << lastCtrlScheme;
			}

			// Prepare a new position command - if we are in command mode
			switch ( friInst.getCurrentControlScheme())
			{
			case FRI_CTRL_CART_IMP:
				/** joint/cart positions, joint/cart stiffness, joint/cart damping
				and additional TCP F/T can be commanded */
				{
					//cout <<"ctrl cart impedance mode" <<endl;
					//float newCartVals[FRI_CART_FRM_DIM];///*!< Number of data within a 3x4 Cartesian frame - homogenous matrix - organized row by row
					//float newForceTorqueAdd[FRI_CART_VEC];
					//float measCartOffsetVals[FRI_CART_FRM_DIM];
					//float measCartPosVals[FRI_CART_FRM_DIM];

					//Get pos
					for (int i = 0; i < FRI_CART_FRM_DIM; i++)
					{
						measCartPos_console[i] = friInst.getMsrCmdCartPosition()[i]; //Get Cart pos
						desCartPos_full[i]=measCartPos_console[i];//This needs to be defined even if we're not in the cmd loop
					}

					if(firstLoop)//set initial pos
					{
						double l_kuka_eePos_ini[3]={0,0,0};
						l_kuka_eePos_ini[0]=measCartPos_console[3];
						l_kuka_eePos_ini[1]=measCartPos_console[7];
						l_kuka_eePos_ini[2]=measCartPos_console[11];
						printf("Setting Kuka EE initial pos: %g %g %g \n",l_kuka_eePos_ini[0],l_kuka_eePos_ini[1],l_kuka_eePos_ini[2]);
						p_endEffector->m_slave.set_initialPos(l_kuka_eePos_ini);
						
						//Set initial orientation EE slave
						CustomMatrix * pl_rotKuka_ini=new CustomMatrix(3,3);
						(*pl_rotKuka_ini)(1,1)=measCartPos_console[0];
						(*pl_rotKuka_ini)(1,2)=measCartPos_console[1];
						(*pl_rotKuka_ini)(1,3)=measCartPos_console[2];

						(*pl_rotKuka_ini)(2,1)=measCartPos_console[4];
						(*pl_rotKuka_ini)(2,2)=measCartPos_console[5];
						(*pl_rotKuka_ini)(2,3)=measCartPos_console[6];

						(*pl_rotKuka_ini)(3,1)=measCartPos_console[8];
						(*pl_rotKuka_ini)(3,2)=measCartPos_console[9];
						(*pl_rotKuka_ini)(3,3)=measCartPos_console[10];
						p_endEffector->m_slave.set_initialRot(pl_rotKuka_ini);
						printf("Setting initial rot kuka: \n");
						p_endEffector->m_slave.pm_rot_initial->printf_Matrix();

						firstLoop=false;
					}

					p_endEffector->compute_slave_actualPos_des();//compute corresponding desired pos slave
					p_endEffector->compute_slave_actualRot_des();//compute corresponding desired orientation slave

					//printf("Des orientation: \n");
					//p_endEffector->m_slave.pm_rot_actual->printf_Matrix();
					
					p_endEffector->compute_slave_deltaPos(deltaPos_slave);
					//printf("Pos master: %g \t %g \t %g \n",p_endEffector->m_master.m_actualPos[0],p_endEffector->m_master.m_actualPos[1],p_endEffector->m_master.m_actualPos[2]);
					//printf("Delta pos slave: %g %g %g \n",deltaPos_slave[0],deltaPos_slave[1],deltaPos_slave[2]);
		//			//printf("Pos slave: %g \t %g \t %g \n",p_endEffector->m_slave.m_actualPos[0],p_endEffector->m_slave.m_actualPos[1],p_endEffector->m_slave.m_actualPos[2]);
		//			Sleep(1000);
					
					//for (int i = 0; i < FRI_CART_FRM_DIM; i++)
					//{
					//	newCartVals[i] = friInst.getMsrCmdCartPosition()[i]; //Get Cart pos
					//}

					if ( friInst.getState() == FRI_STATE_CMD)
					{
						//cout <<"CMD mode - ok" <<endl;
						if ( friInst.isPowerOn() )
						{
								//for (int i = 0; i < FRI_CART_FRM_DIM; i++)
								//{
								//	newCartVals[i] = friInst.getMsrCmdCartPosition()[i]; //Get Cart pos
								//	measCartOffsetVals[i]=friInst.getMsrCmdCartPosFriOffset()[i];
								//	measCartPosVals[i]=friInst.getMsrCartPosition()[i];
								//}
								//printf("Msr Cmd Cart Pos: %f %f %f \n", newCartVals[3],newCartVals[7],newCartVals[11]);
								//printf("Mes cart offset: %f %f %f \n", measCartOffsetVals[3],measCartOffsetVals[7],measCartOffsetVals[11]);
								//printf("Mes cart pos: %f %f %f \n ", measCartPosVals[3],measCartPosVals[7],measCartPosVals[11]);
								//printf("Mes cart pos: %f %f %f \n \n", friInst.getMsrCmdCartPosition()[3]+deltaPos,friInst.getMsrCmdCartPosFriOffset()[3],friInst.getMsrCartPosition()[3]);
								//printf("deltaPos: %f \n",(float)deltaPos);


								//for (int i = 0; i < FRI_CART_FRM_DIM; i++)
								//{
								//	desCartPos_full[i] = measCartPos_console[i]; //Get Cart pos
								//}

								timeCounter+=friInst.getSampleTime();
								for (int i = 1; i <= 3; i++)
								{
									// perform some sort of sine wave motion
									//newCartVals[(i*4)-1]+=(float)sin( timeCounter * M_PI * 0.02) * (0.1f);
									//measCartPos_console[(i*4)-1]+=(float)sin( timeCounter * M_PI * 0.02) * (0.1f);
									//desCartPos_full[(i*4)-1]+=(float)sin( timeCounter * M_PI * 0.02) * (0.1f);
								}
								//newCartVals[3]+=(float)sin( timeCounter * M_PI * 0.02) * (0.1f);

								//deltaPos+=0.0001;
								//cmd_deltaPos_fri[3]=deltaPos_slave[0];
								//cmd_deltaPos_fri[7]=deltaPos_slave[1];
								//cmd_deltaPos_fri[11]=deltaPos_slave[2];

								//Works
								//desCartPos_full[3]+=deltaPos_slave[0];
								//desCartPos_full[7]+=deltaPos_slave[1];
								//desCartPos_full[11]+=deltaPos_slave[2];

								//cmd_deltaPos_fri[3]+=0.0001;
								//cmd_deltaPos_fri[7]=0;
								//cmd_deltaPos_fri[11]=0;

							//Works
							desCartPos_full[3]=p_endEffector->m_slave.m_actualPos[0];
							desCartPos_full[7]=p_endEffector->m_slave.m_actualPos[1];
							desCartPos_full[11]=p_endEffector->m_slave.m_actualPos[2];
							//Orientation
							/*desCartPos_full[0]=(*p_endEffector->m_slave.pm_rot_initial)(1,1);
							desCartPos_full[1]=(*p_endEffector->m_slave.pm_rot_initial)(1,2);
							desCartPos_full[2]=(*p_endEffector->m_slave.pm_rot_initial)(1,3);
							
							desCartPos_full[4]=(*p_endEffector->m_slave.pm_rot_initial)(2,1);
							desCartPos_full[5]=(*p_endEffector->m_slave.pm_rot_initial)(2,2);
							desCartPos_full[6]=(*p_endEffector->m_slave.pm_rot_initial)(2,3);
							
							desCartPos_full[8]=(*p_endEffector->m_slave.pm_rot_initial)(3,1);
							desCartPos_full[9]=(*p_endEffector->m_slave.pm_rot_initial)(3,2);
							desCartPos_full[10]=(*p_endEffector->m_slave.pm_rot_initial)(3,3);*/
							
							//Annnnd Works!!!
							desCartPos_full[0]=(*p_endEffector->m_slave.pm_rot_actual)(1,1);
							desCartPos_full[1]=(*p_endEffector->m_slave.pm_rot_actual)(1,2);
							desCartPos_full[2]=(*p_endEffector->m_slave.pm_rot_actual)(1,3);
							
							desCartPos_full[4]=(*p_endEffector->m_slave.pm_rot_actual)(2,1);
							desCartPos_full[5]=(*p_endEffector->m_slave.pm_rot_actual)(2,2);
							desCartPos_full[6]=(*p_endEffector->m_slave.pm_rot_actual)(2,3);
							
							desCartPos_full[8]=(*p_endEffector->m_slave.pm_rot_actual)(3,1);
							desCartPos_full[9]=(*p_endEffector->m_slave.pm_rot_actual)(3,2);
							desCartPos_full[10]=(*p_endEffector->m_slave.pm_rot_actual)(3,3);

								//for (int i = 1; i <= 3; i++)
								//{
								//	 desCartPos_full[i]=measCartPos_console[i]+cmd_deltaPos_fri[i];
								//}
								//measCartPos_console[3]=(float)( friInst.getMsrCmdCartPosition()[3]+deltaPos);//m
								//newCartVals[3]=(float)( friInst.getMsrCmdCartPosition()[3]+deltaPos);//m
								//printf("Des pos end effector: %f %f %f \n \n", newCartVals[3],newCartVals[7],newCartVals[11]);
							
						}//power
					}//cmd
					//friInst.doCartesianImpedanceControl(newCartVals,NULL,NULL,NULL);//needed!! Otherwise quality does not get updated, so never switch to cmd mode
					//friInst.doCartesianImpedanceControl(measCartPos_console,NULL,NULL,NULL);//needed!! Otherwise quality does not get updated, so never switch to cmd mode
					friInst.doCartesianImpedanceControl(desCartPos_full,NULL,NULL,NULL);//needed!! Otherwise quality does not get updated, so never switch to cmd mode
				}break;
			default:
				{				
				friInst.doDataExchange(); /* do nothing - just data exchange for waiting */
				}break;
			}

			// Stop request is issued from the other side
			if ( friInst.getFrmKRLInt(0) == -1) 
			{
				cout << "leaving \n";
				break;	  
			}

			//Important! update quality
			if ( friInst.getQuality() != lastQuality)
			{
				cout << "quality change detected "<< friInst.getQuality()<< " \n";
				//cout << friInst.getMsrBuf().intf;
				//cout << endl;
				lastQuality=friInst.getQuality();
			}
		}




}//updateLoop_Kuka

/////////////////////
//Custom

/******************************************/
//The ClearScreen() function clears the screen as recomended in http://www.cplusplus.com/forum/articles/10515/
void ClearScreen()
  {
  HANDLE                     hStdOut;
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  DWORD                      count;
  DWORD                      cellCount;
  COORD                      homeCoords = { 0, 0 };
 
  hStdOut = GetStdHandle( STD_OUTPUT_HANDLE );
  if (hStdOut == INVALID_HANDLE_VALUE) return;
 
  /* Get the number of cells in the current buffer */
  if (!GetConsoleScreenBufferInfo( hStdOut, &csbi )) return;
  cellCount = csbi.dwSize.X *csbi.dwSize.Y;
 
  /* Fill the entire buffer with spaces */
  if (!FillConsoleOutputCharacter(
    hStdOut,
    (TCHAR) ' ',
    cellCount,
    homeCoords,
    &count
    )) return;
 
  /* Fill the entire buffer with the current colors and attributes */
  if (!FillConsoleOutputAttribute(
    hStdOut,
    csbi.wAttributes,
    cellCount,
    homeCoords,
    &count
    )) return;
 
  /* Move the cursor home */
  SetConsoleCursorPosition( hStdOut, homeCoords );
  }
/******************************************/
//The gotoxy() function brings the cursor to the requested location for printing
void gotoxy( int column, int line )
  {
  COORD coord;
  coord.X = column;
  coord.Y = line;
  SetConsoleCursorPosition(
    GetStdHandle( STD_OUTPUT_HANDLE ),
    coord
    );
  }
 
///******************************************/
////The saveData() function saves the recorded sensor data in a time stamped file.
//void saveData()
//{        
//    FILE *stream;
//    char FileName[100];
//    time_t rawtime;
//    struct tm * timeinfo;
//    char dateBuffer [80];
//
//    // Create a string that contains the current date and time
//    time (&rawtime);
//    timeinfo = localtime (&rawtime);
//    strftime (dateBuffer,80,"%d_%m_%Y_%H_%M",timeinfo);
//
//    //create a filename which contains the current date and time
//    strcpy(FileName,"IIT_FT17_Data");
//    strcat(FileName, dateBuffer);
//    strcat(FileName, ".txt");
//   
//    //open the file
//    if( (stream = fopen( FileName, "w+" )) == NULL )
//          printf( "File ' %s ' was not opened\n", FileName);
//    else
//    {    
//          //Put the recorded data
//          for(int d=0;d<dataCounter;d++)
//          {
//                fprintf(stream,"%.3f %.3f %.3f %.3f %.3f %.3f %.3f %.3f %.3f %.3f %.3f %.3f %.3f\n",fdata[d][0],
//                fdata[d][1],fdata[d][2],fdata[d][3],fdata[d][4],fdata[d][5],fdata[d][6],fdata[d][7],fdata[d][8],fdata[d][9],fdata[d][10],
//                fdata[d][11],fdata[d][12]);
//          }
//         
//          fclose(stream);
//          gotoxy(0,32); printf("\n %d sampled data stored in file: %s .", dataCounter, FileName);
//    }
//}
 
/******************************************/
//The Show_options() function prints the available keyboard activated options of this program
void Show_options()
{
      ClearScreen();
      gotoxy(5,0);
	  printf("Keyboard: + or -");
      /*printf ("IIT Force-Torque sensor software Example\n");     
      printf ("------------- Keyboard Options ------------------\n\n");      
      printf ("c - Bias sensor\n");
      printf ("q - Exit\n"); */
}
//Keyboard Interface and screen printing loop runs at a lower speed
//Get the keyboard input
void GetKeyboard(void)
{
      int ch=0;
 
      //Get the keyboard input
            if(_kbhit())
            {
                  ch = _getch();
                  ClearScreen();
                  Show_options();
 
                  switch (ch)
                  {
                  case '4':// pressing q closes comunications and exits the program
                              //simulationRunning = false;
							Optitrack::m_mincutoff_filter+=0.01;							
                        break;
                  case '1':// Pressing c biases the sensor.
					  {
							Optitrack::m_mincutoff_filter-=0.01;
							if(Optitrack::m_mincutoff_filter<=0)
							{
								Optitrack::m_mincutoff_filter=0;
							}
						} break;
					  case '5':// Pressing c biases the sensor.
					  {
						  Optitrack::m_beta_filter+=0.001;
						} break;
					   case '2':// Pressing c biases the sensor.
					  {
						  Optitrack::m_beta_filter-=0.001;
							if(Optitrack::m_beta_filter<=0)
							{
								Optitrack::m_beta_filter=0;
							}
						} break;
				  case 'q':
					  {
							stopProgram=true;
					  }break;
                  default:
                        break;
                  }
            }          
                        ch=0;
}
/******************************************/
//The Myprint() function prints the sensor data at a slower rate that the acquisition thread
void Myprint(void)
{

      gotoxy(1,8);
      //if (storeData==ON)
      //      printf("\n ----------------------Storing data.        Data counter:%*d --------------\n\n",7,dataCounter);
      //else if(storeData==OFF)
      //      printf("\n ----------------------NOT Storing data.    Data counter:%*d --------------\n\n",7,dataCounter);
	  printf("Frequency = %0.1f Hz\n",1/TimerLoop);
 
      gotoxy(0,15);
      int p=7;

	  //printf("m_minCutOff_filter: %10.3g \n",Optitrack::m_mincutoff_filter);
	  //printf("Optitrack raw: %10.3g \t %10.3g \t %10.3g \n",Optitrack::x_raw,Optitrack::y_raw,Optitrack::z_raw);
	  //printf("Optitrack filtered: %10.3g \t %10.3g \t %10.3g \n",Optitrack::rb1.x,Optitrack::rb1.y,Optitrack::rb1.z);
	  printf("Timer Optitrack: %10.3g \n",Optitrack::m_timer);
	  printf("m_beta filter: %10.3g \n",Optitrack::m_beta_filter);
	   printf("m_minCutOff_filter: %10.3g \n"
	   "Optitrack raw: %10.3g \t %10.3g \t %10.3g \n"
	 "Optitrack filtered: %10.3g \t %10.3g \t %10.3g \n"
	 ,Optitrack::m_mincutoff_filter,
	 Optitrack::x_raw,Optitrack::y_raw,	 Optitrack::z_raw, 
	 Optitrack::rb1.x,Optitrack::rb1.y,Optitrack::rb1.z);

      /*printf("Time stamp:%ld\n\n"
            "Strain gauge readings:\nCH1=%*d, CH2=%*d, CH3=%*d, CH4=%*d, CH5=%*d, CH6=%*d\n\n"
            "Strain gauge RAW readings:\nch1=%*d, ch2=%*d, ch3=%*d, ch4=%*d, ch5=%*d, ch6=%*d\n\n",
            MyFTSensor.tStamp,
            p,MyFTSensor.ChRaw_Offs[0],p,MyFTSensor.ChRaw_Offs[1],p,MyFTSensor.ChRaw_Offs[2],p,MyFTSensor.ChRaw_Offs[3],p,MyFTSensor.ChRaw_Offs[4],p,MyFTSensor.ChRaw_Offs[5],           
            p,MyFTSensor.ChRaw[0],p,MyFTSensor.ChRaw[1],p,MyFTSensor.ChRaw[2],p,MyFTSensor.ChRaw[3],p,MyFTSensor.ChRaw[4],p,MyFTSensor.ChRaw[5]);
           
      printf("Force-Torque readings in SI Units:\n fx=%10.3f, fy=%10.3f, fz=%10.3f,\n tx=%10.3f, ty=%10.3f, tz=%10.3f\n\n",
            MyFTSensor.ft[0], MyFTSensor.ft[1],MyFTSensor.ft[2],MyFTSensor.ft[3],MyFTSensor.ft[4],MyFTSensor.ft[5]);
           
      printf("Filtered Force-Torque readings in SI Units:\n : \n fx=%10.3f, fy=%10.3f, fz=%10.3f,\n tx=%10.3f, ty=%10.3f, tz=%10.3f",
            MyFTSensor.filt_ft[0], MyFTSensor.filt_ft[1],MyFTSensor.filt_ft[2],MyFTSensor.filt_ft[3],MyFTSensor.filt_ft[4],MyFTSensor.filt_ft[5]);
 */
	  printf("CustomPrint \n");
}




