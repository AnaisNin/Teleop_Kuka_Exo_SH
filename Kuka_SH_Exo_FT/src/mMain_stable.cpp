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
#include "MathUtils.h"

#include "OneEuroFilter.h"
#include "CPrecisionClock.h"
#include "CThread.h"
#include "UDP_Communication.h"
#include "devMeasurementFns.h"

//Exo
#include "EXOSBoardLibrary.h"
//#pragma comment(lib, "BoardLibrary.lib")
#pragma comment(lib, "EXOSBoardLibrary.lib")
#include "phil_board.h"
#include "Exoskeleton.h"
//#include "BoardLibrary.h"
#include "BoardControl.h"
#include "PrintFnt.h"
#include "Kinematics.h"
#include "PCA_synergy.h"

//SH
#include "SoftHandSyn.h"
#include "SoftHand.h"
#include "TorqueObserver.h"

#include "Kuka.h"

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
cPrecisionClock* Clock_a;//Kuka loop
cPrecisionClock* Clock_b;//Kuka loop
cPrecisionClock* Clock_c;//Exo_SH loop
cPrecisionClock* Clock_d;//Exo_SH loop
cPrecisionClock* Clock_Pr;

//FTsensors
double FTdata[6]={0};
//double FTdata_1[6]={0};
//double FTdata_2[6]={0};
devMeasurementFns * ATI_FTsensor;
UDP_Communication* UDP_ATI;


OneEuroFilter filter_sigma1;
OneEuroFilter filter_tauInt_SH;

//double timerTeleop;
double TimerLoop_Kuka;
double TimerLoop_Exo_SH=0.0;//delta_t one loop
double TimerCurrent_Exo_SH=0;//current time from begining of loop
bool stopProgram=false;
bool flg_teleop_Exo_SH=false;
bool flg_firstLoop=true;
bool printer_justOnce=true;
bool flg_FT_mode=false;

double Kuka_des_pos[3]={0};

bool initializationDone_KukaThread=false;
bool initializationDone_ExoSHThread=false;

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
void updateLoop_Exo_SH(void);
void Myprint(void);
void GetKeyboard(void);
void Show_options();
void gotoxy( int column, int line );
void ClearScreen();

//Exo
#define BCAST_RATE 2 // looptime (update thread)=bcast_rate*0.0005
//# define looptime (BCAST_RATE*0.0005)//s - freq main loop
# define DesLoopTime_Exo_SH (BCAST_RATE*0.0005)//s - freq main loop of Exo_SH thread
//int desTorque_exo_loc[MAX_BOARDS][MAX_MOTORS]={{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}};
int desTorque_exo_loc[MAX_BOARDS][MAX_MOTORS]={{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}};
long timeStamp_initLoop;
int counterLoop_Exo_SH=0;
HandExoskeleton ExosData;
Exoskeleton * pExo;
SoftHandSyn * pSH_Syn;
SoftHand * pSH;
//SoftHand * softHand;

double Ftip_FT[Globals::n_pcs]={0};

bool flg_runThread_Kuka=true;
bool flg_runThread_ExoSH=true;
bool flg_myPrint=false;

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
	cThread * mainThread_Kuka=new cThread();
	cThread * mainThread_Exo_SH=new cThread();

	Clock_a = new cPrecisionClock();
	Clock_b=new cPrecisionClock();
	Clock_c=new cPrecisionClock();
	Clock_d=new cPrecisionClock();
	Clock_Pr=new cPrecisionClock();

	ATI_FTsensor = new devMeasurementFns();
	UDP_ATI=new UDP_Communication();
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

		pExo=new Exoskeleton();
		pSH_Syn=new SoftHandSyn();
		pSH=new SoftHand();

		//Optitrack
		int errorCode=Optitrack::Initialize();//Starts loop updating values from Optitrack
		Sleep(5000);
		PrintFnt::ClearScreen();


		printf("Start threads \n");
		mainThread_Kuka->set(updateLoop_Kuka,CHAI_THREAD_PRIORITY_HAPTICS);
		mainThread_Exo_SH->set(updateLoop_Exo_SH,CHAI_THREAD_PRIORITY_HAPTICS);

		int printTimer=0;
		while(!stopProgram)
		{
//////			printTimer++;
//////			if(printTimer>5)
//////			{
//////				 int ch=0;
////// 
//////      //Get the keyboard input
//////            if(_kbhit())
//////            {
//////                  ch = _getch();
//////                  ClearScreen();
//////                  Show_options();
////// 
//////                  switch (ch)
//////                  {
//////
//////				  case '+':
//////				{
//////					if((pSH->rawPos+1000)<HAND_MAX)
//////					{
//////						pSH->SetDesiredPos(pSH->rawPos+1000);
//////						pSH->ApplyDesiredPosition();
//////						printf("Des pos: %i \n",pSH->GetDesiredPos());
//////					}
//////					else
//////					{
//////						printf("Max pos \n");
//////					}
//////					  
//////				 }break;
//////				case'-':
//////				{
//////					if((pSH->rawPos-1000)>HAND_MIN)
//////					{
//////						pSH->SetDesiredPos(pSH->rawPos-1000);
//////						pSH->ApplyDesiredPosition();
//////						printf("Des pos: %i \n",pSH->GetDesiredPos());
//////					}
//////					else
//////					{
//////						printf("Min pos \n");
//////					}
//////					  				
//////				 }break;
//////				case 'o':
//////					{
//////						pSH->Open();
//////					}break;
//////				case 'p':
//////				{
//////					//if(!flg_myPrint)
//////					//{
//////						flg_myPrint=true;
//////					//}
//////					//else
//////					//{
//////					//	flg_myPrint=false;
//////					//}
//////				}break;
//////case 'l':
//////				{
//////
//////						flg_myPrint=false;
//////
//////				}break;
//////				 case 'a'://Init teleop SH with exo
//////				{
//////					//Pos init
//////					Kinematics::computeFwdKin(pExo); //ok
//////					PCA_synergy::update_syn_struct(pExo);// CODE convert impObj to stiffSpring	
//////					PCA_synergy::compute_sigma1(); //Projection
//////					PCA_synergy::update_struct_from_syn(pExo);
//////
//////					printf("Start teleop SH \n");
//////					flg_teleop_Exo_SH=true;	  
//////				 }break;
//////
//////				case '4':
//////				{	//Force control should be turned off before calibrating the offsets
//////					int board_id;
//////					for(int it=0;it<3;it++)
//////					{
//////						board_id=it+1;
//////						CalibrateOffsets(board_id);
//////					}
//////					//CalibrateOffsets(board_id);
//////					printf("Calibrate offsets \n");
//////				}break;
//////				case '5':
//////				{
//////					StartForceControl();
//////					printf("Start force control \n");
//////				}break;
//////				case '6':
//////				{
//////					StopForceControl();
//////					printf("Start force control \n");
//////				}break;
//////
//////                  case '7':
//////							Optitrack::m_mincutoff_filter+=0.01;							
//////                        break;
//////                  case '1':// Pressing c biases the sensor.
//////					  {
//////							Optitrack::m_mincutoff_filter-=0.01;
//////							if(Optitrack::m_mincutoff_filter<=0)
//////							{
//////								Optitrack::m_mincutoff_filter=0;
//////							}
//////						} break;
//////					  case '8':
//////					  {
//////						  Optitrack::m_beta_filter+=0.001;
//////						} break;
//////					   case '2':// Pressing c biases the sensor.
//////					  {
//////						  Optitrack::m_beta_filter-=0.001;
//////							if(Optitrack::m_beta_filter<=0)
//////							{
//////								Optitrack::m_beta_filter=0;
//////							}
//////						} break;
//////				  case 'q':
//////					  {
//////						  StopForceControl();//Exo
//////						  flg_runThread_Kuka=false;
//////						  flg_teleop_Exo_SH=false;
//////						  flg_runThread_ExoSH=false;
//////						 stopProgram=true;
//////					  }break;
//////                  default:
//////                        break;
//////                  }
//////            }          
//////                        ch=0;
//////				if(initializationDone_KukaThread==true&&initializationDone_ExoSHThread==true&&flg_myPrint==false)//Do it once at the end of initialization
//////				{
//////					PrintFnt::ClearScreen();
//////					flg_myPrint=true;
//////					//Myprint();
//////				}
//////
//////				if(flg_myPrint)
//////				{
//////					Myprint();
//////				}
//////				else
//////				{
//////						printf("Frequency Exo_SH loop = %6.1g Hz\n",1/ TimerLoop_Exo_SH);
//////				}
//////				
//////				GetKeyboard();
//////
//////				printTimer=0;
//////			}//if printTimer>deltaNumLoops
			
			//Sleep(20);//?
		}

	//Delete pointers clocks
	delete Clock_a;
	delete Clock_b;
	delete Clock_c;
	delete Clock_d;
	delete Clock_Pr;

	//Delete pointers FT
	delete ATI_FTsensor;
	delete UDP_ATI;

	//Close exo
		for(int b=0;b<MAX_BOARDS;b++)
			BoardControl::stopBroadCast(b);     
		Sleep(500);
		CloseSockets();
	//Delete pointer exo
		delete pExo;

	//Close SH
	pSH->CloseConnection();
	//Delete pointers sh
	delete pSH;
	delete pSH_Syn;

	//Cleanup threads
	delete mainThread_Kuka; //This calls the destructor, same as mainThread_Kuka->~cThread(); //Destructor
	delete mainThread_Exo_SH; //Destructor 

	Sleep(3000);
	// Save data to file
	//saveData();

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

		//printf("TimerOpti: %g \n",Optitrack::m_timer);
		//printf("Sleeping for a while, wait for it..\n");
		Sleep(5000);
		//PrintFnt::ClearScreen();

		//To test only optitrack
		//while(1)
		//{
		//	Myprint();
		//	GetKeyboard();
		//}
		
		//Put after Kuka init when it is there
		initializationDone_KukaThread=true;//Used to trigger printerLoop
		printf("initializationDone_KukaThread: %i\n",initializationDone_KukaThread);

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

		//int printTimer=0;

		//When Kuka is connected
		//initializationDone_KukaThread=true;//Used to trigger printerLoop

		while(flg_runThread_Kuka)
		{
			TimerLoop_Kuka=Clock_b->getCurrentTimeSeconds();
			Clock_b->reset();
			Clock_b->start();

			//printTimer++;
			//if(printTimer>50)
			//{
			//	Myprint();
			//	GetKeyboard();
			//	printTimer=0;
			//}

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
						measCartPos_full[i]=friInst.getMsrCartPosition()[i];
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
							

							//Works - Send pos of optitrack directly
							//desCartPos_full[3]=p_endEffector->m_slave.m_actualPos[0];
							//desCartPos_full[7]=p_endEffector->m_slave.m_actualPos[1];
							//desCartPos_full[11]=p_endEffector->m_slave.m_actualPos[2];

							//Check max dist - BOX wrt initial pos - works
							//Kuka::m_currentPos_EE[0]=desCartPos_full[3];//Check this
							//Kuka::m_currentPos_EE[1]=desCartPos_full[7];
							//Kuka::m_currentPos_EE[2]=desCartPos_full[11];


					//Check for distance between two points - works
							Kuka::m_currentPos_EE[0]=measCartPos_full[3];//Check this
							Kuka::m_currentPos_EE[1]=measCartPos_full[7];
							Kuka::m_currentPos_EE[2]=measCartPos_full[11];

							for(int it=0;it<3;it++)
							{
								Kuka::m_desPos_EE[it]=p_endEffector->m_slave.m_actualPos[it];
							}
							MathUtils::compute_safePos(Kuka::m_currentPos_EE,Kuka::m_desPos_EE,Kuka::m_maxDist_EE,Kuka::m_refPos_EE);
							
							desCartPos_full[3]=Kuka::m_refPos_EE[0];
							desCartPos_full[7]=Kuka::m_refPos_EE[1];
							desCartPos_full[11]=Kuka::m_refPos_EE[2];

					//BOX - Limit distance between initial pos EE kuka and current pos as sphere - authorized workspace
					//TO TEST
							//MathUtils::compute_safePos(p_endEffector->m_slave.m_initialPos,Kuka::m_refPos_EE,Kuka::m_boxSize_EE,Kuka::m_refPos_workspace_EE);
							////Overwrite desCartPos_full
							//desCartPos_full[3]=Kuka::m_refPos_workspace_EE[0];
							//desCartPos_full[7]=Kuka::m_refPos_workspace_EE[1];
							//desCartPos_full[11]=Kuka::m_refPos_workspace_EE[2];
				////////////////////////////

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

///////////////////////////////////////////////////////////////
void updateLoop_Exo_SH(void) //This fnt is called only once, when thread is created. Only the content of its while() is looping (at a freq indep from thread?)
{
	//Initialization

	//Tune filters for sigma1 and interaction torque 
	//Default values: minCutOff=1.0, Beta=1.0, cf default constructor OneEuroFilter
	filter_sigma1.setBeta(1.0); //Tune
	filter_sigma1.setMinCutoff(1.0); //Tune
	filter_tauInt_SH.setBeta(1.0); //Tune
	filter_tauInt_SH.setMinCutoff(1.0); //Tune

	//Exoskeleton
		InitLibrary(MAX_BOARDS); //init the communication lib
		CloseSockets();	// reset all possible active sockets
		int res=BoardControl::connectUDP(); //Connect board
		if((res=BoardControl::scanSystem())<0)// scan for motor controllers
		{
			Sleep(3000);
			//exit(0);
		}
		BoardControl::connectTCP();// connect to motor controllers

		//Set broadcast rate and policy
		//printf("Try to set broadcast rate and policy .. \n");
		for (int b=0; b<MAX_BOARDS; b++)
		{
			if(BoardControl::joint[b].isConnected==1)
			{
				//printf("Set broadcast rate and policy of board id %i \n",b+1);
				BoardControl::setBroadCastPolicy(b,BCAST_POLICY); 
				BoardControl::setBroadCastRate(b,BCAST_RATE);    //Accelerometer Board
			}
		}
		//Sleep(5000); //50000 - Read all initialization data

		SetAnalogInputs(1, 6); //??
		//	system("cls");

		//desVel[0]=2000;
		//desVel[1]=5000;
		//r=SetDesiredVelocity(desVel);

	//PrintFnt::ClearScreen();
	// run until the defined sim time

		//SoftHand init
	pSH->Setup();

	//FTsensor init
	UDP_ATI->InitializeWinSock();

//Test SH
	//printf("Close SH..\n");
	//pSH->SetDesiredPos(HAND_MAX);
	//pSH->ApplyDesiredPosition();
	////pSH->Close();
	//printf("Wait for reaching position.. \n");
	//Sleep(5000);//
	//printf("Open SH.. \n");
	//pSH->SetDesiredPos(HAND_MIN);
	//pSH->ApplyDesiredPosition();
	////pSH->Open();
	//printf("Wait for reaching position.. \n");
	//Sleep(5000);
	//printf("Ok \n");

	//Initialize synergy stuff
	PCA_synergy::set_synData_fromFile();
	printf("SynergyMatrix properly setted from matlab file \n");
	//PCA_synergy::get_pSynMatrix()->printf_Matrix();
	//PCA_synergy::print_mean_dataCollection();

	//modifies range sigma, must be before update_syn_struct
	PCA_synergy::sigma1_range_dataCollection[0]=-0.053;//-0.022;//fully closed -0.078 [iros] ; -0.095: best fit; -0.08: touching fingers; -0.055: for contact cases
	PCA_synergy::sigma1_range_dataCollection[1]=0.06;//0.085;//0.070 [iros] fully opened
	printf("Min max sigma1 set \n");

	//Pos init
	//Kinematics::computeFwdKin(pExo); //ok
	PCA_synergy::update_syn_struct(pExo);// CODE convert impObj to stiffSpring	
	PCA_synergy::compute_sigma1(); //Projection
	PCA_synergy::update_struct_from_syn(pExo);

	StopForceControl();//on by default when starting the board - for exo

	//double TimerTeleop=0;
	//double TimerTest=0;
	Clock_c->reset();//For timer each loop
	Clock_c->start();
	Clock_d->reset();//For timer from begining of loop
	Clock_d->start();
	Clock_Pr->reset();
	Clock_Pr->start();
	//int chThread=0; //Will store _getch();

	initializationDone_ExoSHThread=true;
	printf("initializationDone_ExoSHThread: %i\n",initializationDone_ExoSHThread);


	while(flg_runThread_ExoSH)//printf("Loop time: %g \n",TimerLoop); //Slightly below 1ms - between 0.0009 and 0.001 s
	{

			if(Clock_c->getCurrentTimeSeconds()>=DesLoopTime_Exo_SH) //else, do nothing and loop again
					{
						counterLoop_Exo_SH++;
						//
						TimerLoop_Exo_SH=Clock_c->getCurrentTimeSeconds();
						Clock_c->reset();	
						Clock_c->start();
						//
						TimerCurrent_Exo_SH=Clock_d->getCurrentTimeSeconds();
						//Rq: put all the timers here, else time will depend on time required to execute the piece of loop
						//if(flg_teleop_Exo_SH){TimerTeleop=Clock_c->getCurrentTimeSeconds();}
						//if(flg_test){TimerTest=Clock_d->getCurrentTimeSeconds();}

					GetExosData(1, ExosData); //Read data from encoders
					pSH->GetSHBroadCastData();	
					if(flg_FT_mode)
					{
						if(ATI_FTsensor->ReadFTsensorData()==1)//proper reading
						{
							ATI_FTsensor->GetFTData(FTdata);
						}
					}

					//Fill pExo joint pos	
					pExo->set_jointPos(ExosData);
					Kinematics::computeFwdKin(pExo); 

		if(flg_teleop_Exo_SH==true) 
		{
			/************************************************/
			// COMMUNICATION: GET DATA FROM SOFTHAND AND EXO
			/************************************************/
			//EXO: GET ENCODER DATA -> UPDATE EXO STRUCT 
			//Cf above: GetExosData(1, ExosData); Exchange last two joints; pExo->set_jointPos(ExosData);

			//SOFTHAND: GET CONTACT STATE, IMPEDANCE -> UPDATE CLASS MEMBERS (is_contact, pos_initialContact_unitScaled, object impedance)
			//int nbytes_rcv=Server_WL::ReceivePacket_SH_updateClassMembers(); //ok 
			//printf("nbytes rcv: %i \n",nbytes_rcv);
			//printf("Tau_int: %g \n",SoftHand::get_tau_interaction());
					
			//************************************************/
			// EXO FWD KIN: COMPUTE	q
			//************************************************/
			//UPDATE KIN STRUCT FROM q_enc (q_dh accounting for offset encoders, rot mat, pos vect, jacobian and posTip)
			//Kinematics::computeFwdKin(pExo); //ok

			//If (flg_compute_S) print x_dot to file to compute S and finish
			//if(flg_compute_syn_matrix==true)
			//{
			//	pFileExoCartesianData=fopen("log/File_ExoCartesianData.txt","a"); //append mode
			//	if(pFileExoCartesianData!=NULL)
			//	{
			//		//FileUtils::printToFile_exo_x_dot(pFileExoCartesianData,pExo); TIMER
			//		//TO DO! Print to file x_tip
			//		fclose(pFileExoCartesianData);
			//	}
			//}

			//SoftHand part
			TorqueObserver::Compute_InteractionTorque_FrictionCurrent(pSH->current, pSH->GetDesiredPos(), pSH->rawPos);//Sets TorqueObserver::tau_ext_obs
			TorqueObserver::set_tau_ext_obs_filtered(filter_tauInt_SH.filter(TorqueObserver::get_tau_ext_obs(),TimerCurrent_Exo_SH));//Filtering of the observerd torque
			//cout << "interaction torque: " << TorqueObserver::get_tau_ext_obs() << endl;
			//TorqueObserver::Compute_1stContact_StiffContact(pSH->rawPos);
			//SoftHandSyn::updateClassMembers(SoftHandSyn::get_contact_state(), SoftHandSyn::get_impedance_object(), SoftHandSyn::get_pos_firstContact_unitScale(), SoftHandSyn::get_pos_unitScale(), SoftHandSyn::get_tau_interaction());
			//SoftHandSyn::updateClassMembers(SoftHandSyn::get_contact_state(), SoftHandSyn::get_impedance_object(), SoftHandSyn::get_pos_firstContact_unitScale(), SoftHandSyn::get_pos_unitScale(), SoftHandSyn::get_tau_interaction());
			pSH_Syn->set_tau_interaction((double)TorqueObserver::get_tau_ext_obs());
			pSH_Syn->set_tau_interaction_filtered((double)TorqueObserver::get_tau_ext_obs_filtered());
			

	//printf("interaction torque: %g\n", SoftHandSyn::get_tau_interaction());
	//cout << "current: " << softHand.current << endl;


			//Rest is useful iif teleop - to compute S we're done here

			//************************************************/
			/// Syn Space: compute sigma, Fexo
			//************************************************/
			//Update syn struct from exo (stack 3 fingers x, x_dot) and from softhand (sigma_initialContact, stiff_vSpring)
			PCA_synergy::update_syn_struct(pExo);// CODE convert impObj to stiffSpring	

			//Compute sigma=Sx.X, extract sigma1=sigma[0]
			PCA_synergy::compute_sigma1(); //Projection
			//ADD A FILTER HERE?
			//printf("Sigma1: %g \n",PCA_synergy::sigma1);
			//printf("sigma1 unitScale: %g \n",PCA_synergy::convert_sigmaScale2unitScale(PCA_synergy::sigma1));

			//Compute  z1 st z=[z1 0 ... 0]' with z1=tau_interaction OR z1=k.delta_sigma1 
			PCA_synergy::compute_z(); //Nm ok - Choose here to use interaction torque filtered or raw
			double z_loc[Globals::n_pcs]={0};
			PCA_synergy::get_z(z_loc);
			//printf("z1: %g \n",z_loc[0]);
			//printf("z: ");
			//for(int it=0;it<Globals::n_pcs;it++)
			//{
			//	printf("%g ",z_loc[it]);
			//}
			//printf("\n");

			//Compute F=S^(-T).z (F is 9x1)
			PCA_synergy::compute_F_syns(); //N ok
			double F_loc[Globals::n_pcs]={0};
			PCA_synergy::get_F(F_loc);
			//printf("Fz index: %g \t Fz middle: %g \n",F_loc[5],F_loc[8]);
			//printf("F: ");
			//for(int it=0;it<Globals::n_pcs;it++)
			//{
			//	printf("%g ",F_loc[it]);
			//}
			//printf("\n");

			//Update exo struct from syn (unstack 3 fingers F) as well as softhand struct (pos ref from sigma1)
			PCA_synergy::update_struct_from_syn(pExo); 

			//if( flg_FT_mode==true)
			//{
			//	//Set Ftip_FT from sensor reading
			//	pExo->set_force_tip_des_base(Ftip_FT);
			//}

			pSH_Syn->set_posRef_unitScale_filtered(filter_sigma1.filter(pSH_Syn->get_posRef_unitScaled(),TimerCurrent_Exo_SH));//Filtering sigma1
			pSH->SetDesiredNormalizedPos((float)(pSH_Syn->get_posRef_unitScale_filtered()));

			//************************************************/
			// Exo force feedback: compute tau_exo
			//************************************************/
			//Compute tau=J_exo^T.F
			Kinematics::compute_torque_ref(pExo); //ok - Rq: as kinematics includes exo, we cannot include kinematics in exo so do everything from kin
			
			if( flg_FT_mode==true)
			{
				//Set Ftip_FT from sensor reading
				pExo->aFingers[0].torque_0_ref=0;
				pExo->aFingers[1].torque_0_ref=300*sqrt(pow(FTdata[0],2)+pow(FTdata[1],2)+pow(FTdata[2],2));//norm force sensed by FTsensor * gain such that 1N gives 300Nmm
				pExo->aFingers[2].torque_0_ref=0;
				
				if(pExo->aFingers[1].torque_0_ref>300)//max torque
				{
					pExo->aFingers[1].torque_0_ref=300;
				}
				else if(pExo->aFingers[1].torque_0_ref<0)
				{
					pExo->aFingers[1].torque_0_ref=0;
					printf("Negative torque, set to zero [Exo::scaleTorque_to_motorRange]\n");
				}
			}

			int tauRef_exo_loc[3]={0,0,0};
			pExo->get_torque0_ref(tauRef_exo_loc);//mNm
			//printf("tau[0]=%i \t tau[1]=%i \t tau[2]=%i \n",tauRef_exo_loc[0],tauRef_exo_loc[1],tauRef_exo_loc[2]);
			//plop[0]=-(int)(tauRef_exo_loc[0]*1000.0);
			//tauRef_exo_loc[0]=224;
			//printf("%g \t %d\n",tauRef_exo_loc[0],tauRef_exo_loc[0]*1000.0);
			//tauRef_exo_loc[0]=5.268;
			//printf("Tau ref exo: ");
			//for(int it=0;it<3;it++)
			//{
			//	printf("%g ",tauRef_exo_loc[it]);
			//}
			//printf("\n");

			//************************************************/
			// Communication: Send data to SoftHand and Exo
			//************************************************/
			//////////Exo: send tau_ref to motors
			//////////TEST - TO REMOVE!
			//////////sinewave SH
			////////double freq_sine=0.12;
			////////double omega=2*M_PI*freq_sine;
			//////////double sineRef_unitSc_loc=0.5+0.5*sin(omega*TimerTeleop-M_PI/2);//M_PI/2 to start from open hand
			////////double sineRef_unitSc_loc=0.5+0.5*sin(omega*TimerCurrent_Exo_SH-M_PI/2);//M_PI/2 to start from open hand


			//////////desTorque_exo_loc[0][0]=-(int)(tauRef_exo_loc[0]*1000.0);//increment torque command in millinewtons - pExo->aFingers[0].torque_0_ref*1000*(-1)
			////////desTorque_exo_loc[0][0]=tauRef_exo_loc[0];//increment torque command in millinewtons - pExo->aFingers[0].torque_0_ref*1000*(-1)
			////////desTorque_exo_loc[1][0]=tauRef_exo_loc[1];//increment torque command in millinewtons
			////////desTorque_exo_loc[2][0]=tauRef_exo_loc[2];
			//////////desTorque_exo_loc[0][0]=0;
			//////////desTorque_exo_loc[0][1]=0;//To filter
			//////////desTorque_exo_loc[0][2]=0;//30;
			//////////printf("%d \n",desTorque_exo_loc[0][0]);
			//////////printf("%d \n",desTorque_exo_loc[0][1]);
			//////////desTorque_exo_loc[1][0]=tauRef_exo_loc[1];//increment torque command in millinewtons
			//////////desTorque_exo_loc[1][1]=0;//30;
			//////////desTorque_exo_loc[1][1]=0;//30;
			//////////desTorque_exo_loc[1][2]=0;//30;
			//////////desTorque_exo_loc[2][0]=tauRef_exo_loc[2];//increment torque command in millinewtons
			//////////desTorque_exo_loc[2][0]=0;//increment torque command in millinewtons
			//////////desTorque_exo_loc[2][1]=0;
			//////////desTorque_exo_loc[2][2]=0;

			//////////Sinewave Exo torque ref - overwrite
			////////double freq_sine_exo=0.5;
			////////double omega_exo=2*M_PI*freq_sine_exo;
			////////int torque_sine_loc=(int)(100+100*sin(omega_exo*TimerCurrent_Exo_SH-M_PI/2));//TimerTest
			//////////desTorque_exo_loc[0][0]=torque_sine_loc;
			//////////desTorque_exo_loc[1][0]=torque_sine_loc;
			//////////desTorque_exo_loc[2][0]=torque_sine_loc;
			//////////printf("%d \n",desTorque_exo_loc[1][0]);
			//////////printf("Tau ref middle: %d \n",desTorque_exo_loc[2][0]);

			//////////Filtering
			//////////Send data to filter as force ref motor[1] of any board
			//////////double shPosRef_toFilter_loc_D=sineRef_unitSc_loc;
			////////double shPosRef_toFilter_loc_D=SoftHandSyn::get_posRef_unitScaled();
			//////////printf("posRef to filter: %g \n",shPosRef_toFilter_loc_D);
			////////int shPosRef_toFilter_loc_I=(int)(1000000*shPosRef_toFilter_loc_D);
			////////desTorque_exo_loc[0][1]=shPosRef_toFilter_loc_I;//To filter

			////////double tauInteraction_toFilter_loc_D=SoftHandSyn::get_tau_interaction();
			//////////printf("posRef to filter: %g \n",shPosRef_toFilter_loc_D);
			////////int tauInteraction_toFilter_loc_I=(int)(1000000*tauInteraction_toFilter_loc_D);
			////////desTorque_exo_loc[1][1]=tauInteraction_toFilter_loc_I;//To filter
			////////
			//////////desTorque_exo_loc[2][1]=;//Free slot to filter

			//////////Filter tauRef_exo - just for plots, for control the ref is directly filtered in the board
			////////desTorque_exo_loc[0][2]=tauRef_exo_loc[0];
			////////desTorque_exo_loc[1][2]=tauRef_exo_loc[1];
			////////desTorque_exo_loc[2][2]=tauRef_exo_loc[2];
			////////int tauRef_exo_filtered_loc[3];
			////////tauRef_exo_filtered_loc[0]=ExosData.ExosFinger[0].M_pos[2];
			////////tauRef_exo_filtered_loc[1]=ExosData.ExosFinger[1].M_pos[2];
			////////tauRef_exo_filtered_loc[2]=ExosData.ExosFinger[2].M_pos[2];

			//Send command
			//if(flg_firstLoop)
			//{
				//for(int iti=0;iti<10;iti++)
				//{
				//	for(int itj=0;itj<3;itj++)
				//	{
				//		desTorque_exo_loc[iti][itj]=10;
				//	}
				//}
				//SetDesiredForce(desTorque_exo_loc);
			//	flg_firstLoop=false;
			//}
		
			//Remove an offset for better feeling in free space (grav comp)
			//for(int iti=0;iti<MAX_BOARDS;iti++)
			//{
			//	//for(int itj=0;itj<MAX_MOTORS;itj++)
			//	//{
			//		desTorque_exo_loc[iti][0]=desTorque_exo_loc[iti][0]-50*sin(abs(pExo->aFingers[iti].jointPos_dh[0]));
			//	//}
			//}

				for(int it=0;it<3;it++)
				{
					desTorque_exo_loc[it][0]=tauRef_exo_loc[it];
				}

			int r=SetDesiredForce(desTorque_exo_loc);

			//Softhand: send pos ref
			pSH->ApplyDesiredPosition();
			
			//int shPosRef_filtered_loc_I=ExosData.ExosFinger[0].M_pos[1];
			//double shPosRef_filtered_loc_D=(double)(shPosRef_filtered_loc_I*0.000001*0.1);//0.1 set in the firmware
			//SoftHandSyn::set_posRef_unitScale_filtered(shPosRef_filtered_loc_D);

			//int tauInteraction_filtered_loc_I=ExosData.ExosFinger[1].M_pos[1];
			//double tauInteraction_filtered_loc_D=(double)(tauInteraction_filtered_loc_I*0.000001*0.1);//0.1 set in the firmware
			//SoftHandSyn::set_tau_interaction_filtered(tauInteraction_filtered_loc_D);

			//printf("%g \t %i \t %i \t %g \n",shPosRef_toFilter_loc_D,shPosRef_toFilter_loc_I,shPosRef_filtered_loc_I,shPosRef_filtered_loc_D);
			//printf("posRef filtered: %d \t %g \n",shPosRef_filtered_loc_I,shPosRef_toFilter_loc_D);
			
			//SoftHand::set_posRef_unitScaled(sineRef_unitSc_loc);
			 //SoftHand::set_posRef_unitScale_filtered(sineRef_unitSc_loc);
			//printf("Timer: %g \t posRef: %g \n",TimerTeleop,SoftHand::get_posRef_unitScaled());

			//Server_WL::SendPacket_SoftHand_ref();
		
			//Rq: it would have been more logical to include kin in exo and do everything from exo, but now too late..
			//Rq: if flg(teleop), update S from matlab during initialization


		//////////////////////////////////////////////////////////

		//pFileKinematics=fopen("log/File_Kinematics.txt","a"); //append mode
		//if(pFileKinematics!=NULL)
		//{
		//	fprintf(pFileKinematics,"%g \t %g \t %g \t %g \t %g \t %i \t %i \t %i \t %i \t %i \t %i \t %g \t %g \t %g \t %g \t %g \t %g \t %g \t %g \t %g\n",
		//		TimerTeleop,
		//		shPosRef_toFilter_loc_D,shPosRef_filtered_loc_D,
		//		tauInteraction_toFilter_loc_D,tauInteraction_filtered_loc_D,
		//		tauRef_exo_loc[0],tauRef_exo_loc[1],tauRef_exo_loc[2],
		//		tauRef_exo_filtered_loc[0],tauRef_exo_filtered_loc[1],tauRef_exo_filtered_loc[2],
		//		F_loc[0],F_loc[1],F_loc[2],F_loc[3],F_loc[4],F_loc[5],F_loc[6],F_loc[7],F_loc[8]		
		//);
		//	//fprintf(pFileKinematics,"%ld \t %d \t %d \n",ExosData.ExosFinger[1].tStamp,shPosRef_toFilter_loc_I,shPosRef_filtered_loc_I,);
		//	
		//	//fprintf(pFileKinematics,"%g \t %d \t %d \t %d \t %d \t %d \t %d \n",SoftHand::get_tau_interaction(),tauRef_exo_loc[0],tauRef_exo_loc[1],tauRef_exo_loc[2],desTorque_exo_loc[0][0],desTorque_exo_loc[1][0],desTorque_exo_loc[2][0]);
		//	//fprintf(pFileKinematics,"%ld \t %g \t %g \t %g \t %d \t %d \t %d \t %d \t %d \t %d \t %d \t %d \t %d \n",ExosData.ExosFinger[1].tStamp,PCA_synergy::sigma1,SoftHand::get_posRef_unitScaled(),SoftHand::get_tau_interaction(),tauRef_exo_loc[0],tauRef_exo_loc[1],tauRef_exo_loc[2],desTorque_exo_loc[0][0],desTorque_exo_loc[1][0],desTorque_exo_loc[2][0],ExosData.ExosFinger[0].M_pos[1],ExosData.ExosFinger[1].M_pos[1],ExosData.ExosFinger[2].M_pos[1]);
		//	//fprintf(pFileKinematics,"%ld \t %g \t %g \t %g \t %g \t %g \t %g \t %g \t %g \t %g \n",ExosData.ExosFinger[1].tStamp,pExo->aFingers[0].posTip[0],pExo->aFingers[0].posTip[1],pExo->aFingers[0].posTip[2],pExo->aFingers[1].posTip[0],pExo->aFingers[1].posTip[1],pExo->aFingers[1].posTip[2],pExo->aFingers[2].posTip[0],pExo->aFingers[2].posTip[1],pExo->aFingers[2].posTip[2]);
		//	//fprintf(pFileKinematics,"%ld \t %g \t %g \t %g \t %g \t %g \t %g \t %g \t %g \t %g \t %g \t %g \t %g \t %g \t %g \n",ExosData.ExosFinger[1].tStamp,PCA_synergy::sigma1,SoftHand::get_tau_interaction(),F_loc[0],F_loc[1],F_loc[2],F_loc[3],F_loc[4],F_loc[5],F_loc[6],F_loc[7],F_loc[8],tauRef_exo_loc[0],tauRef_exo_loc[1],tauRef_exo_loc[2]);
		//	//fprintf(pFileKinematics,"%g \t %g \t %g \t %g \t %g \t %g \t %g \t %g  \t %g  \n",pExo->aFingers[0].posTip[0],pExo->aFingers[0].posTip[1],pExo->aFingers[0].posTip[2],pExo->aFingers[1].posTip[0],pExo->aFingers[1].posTip[1],pExo->aFingers[1].posTip[2],pExo->aFingers[2].posTip[0],pExo->aFingers[2].posTip[1],pExo->aFingers[2].posTip[2]);
		//	//FileUtils::printToFile_exo_x_dot(pFileExoCartesianData,pExo,TimerTest);
  //           //thumb xyz, index xyz, middle xyz
		//	fclose(pFileKinematics);
  //       }


		//pFileExoJointSpData=fopen("log/File_ExoJointSpData.txt","a"); //append mode
		//if(pFileExoJointSpData!=NULL)
		//{
		//	//FileUtils::printToFile_posVel_jointSp(pFileExoJointSpData,pExo,TimerTest);
		//	//fprintf(pFileExoJointSpData,"%g \t %g \t %g \t %g \t %g \t %g \t %g \n",TimerTest,pExo->aFingers[1].jointPos_dh[0],pExo->aFingers[1].jointPos_dh[1],pExo->aFingers[1].jointPos_dh[2],pExo->aFingers[1].jointPos_dh[3],pExo->aFingers[1].jointPos_dh[4],pExo->aFingers[1].jointPos_dh[5]);
		//	
		//	//Enc reading
		//	//fprintf(pFileExoJointSpData,"%ld \t %d \t %d \t %d \t %d \t %d \t %d \t %d \t %d \t %d \t %d \t %d \t %d \t %d \t %d \t %d \t %d \t %d \t %d \n",ExosData.ExosFinger[1].tStamp,
		//	//	ExosData.ExosFinger[0].jointPos[0],ExosData.ExosFinger[0].jointPos[1],ExosData.ExosFinger[0].jointPos[2],ExosData.ExosFinger[0].jointPos[3],ExosData.ExosFinger[0].jointPos[4],ExosData.ExosFinger[0].jointPos[5],
		//	//	ExosData.ExosFinger[1].jointPos[0],ExosData.ExosFinger[1].jointPos[1],ExosData.ExosFinger[1].jointPos[2],ExosData.ExosFinger[1].jointPos[3],ExosData.ExosFinger[1].jointPos[4],ExosData.ExosFinger[1].jointPos[5],
		//	//	ExosData.ExosFinger[2].jointPos[0],ExosData.ExosFinger[2].jointPos[1],ExosData.ExosFinger[2].jointPos[2],ExosData.ExosFinger[2].jointPos[3],ExosData.ExosFinger[2].jointPos[4],ExosData.ExosFinger[2].jointPos[5]);
		//	
		//	//rad
		//	fprintf(pFileExoJointSpData,"%g \t %g \t %g \t %g \t %g \t %g \t %g \t %g \t %g \t %g \t %g \t %g \t %g \t %g \t %g \t %g \t %g \t %g \t %g \n",
		//		TimerTeleop,
		//		pExo->aFingers[0].jointPos_dh[0],pExo->aFingers[0].jointPos_dh[1],pExo->aFingers[0].jointPos_dh[2],pExo->aFingers[0].jointPos_dh[3],pExo->aFingers[0].jointPos_dh[4],pExo->aFingers[0].jointPos_dh[5],
		//		pExo->aFingers[1].jointPos_dh[0],pExo->aFingers[1].jointPos_dh[1],pExo->aFingers[1].jointPos_dh[2],pExo->aFingers[1].jointPos_dh[3],pExo->aFingers[1].jointPos_dh[4],pExo->aFingers[1].jointPos_dh[5],
		//		pExo->aFingers[2].jointPos_dh[0],pExo->aFingers[2].jointPos_dh[1],pExo->aFingers[2].jointPos_dh[2],pExo->aFingers[2].jointPos_dh[3],pExo->aFingers[2].jointPos_dh[4],pExo->aFingers[2].jointPos_dh[5]
		//		);
		//	fclose(pFileExoJointSpData);
		//}

		//pFileExoCartesianData=fopen("log/File_ExoCartesianData.txt","a"); //append mode
		//if(pFileExoCartesianData!=NULL)
		//{
		//	//FileUtils::printToFile_posVel_cartSp(pFileExoCartesianData,pExo,TimerTest);
		//	//FileUtils::printToFile_exo_posQuat(pFileExoCartesianData,pExo,TimerTest);
		//	fprintf(pFileExoCartesianData,"%g \t %g \t %g \t %g \t %g \t %g \t %g \t %g \t %g \t %g \n",
		//		 TimerTeleop,
		//		 pExo->aFingers[0].posTip[0],pExo->aFingers[0].posTip[1],pExo->aFingers[0].posTip[2],
		//		 pExo->aFingers[1].posTip[0],pExo->aFingers[1].posTip[1],pExo->aFingers[1].posTip[2],
		//		 pExo->aFingers[2].posTip[0],pExo->aFingers[2].posTip[1],pExo->aFingers[2].posTip[2]
		//	);
		//	//fprintf(pFileExoCartesianData,"%g \t %g \t %g \t %g \t %g \t %g \t %g \t %g \t %g \t %g \t %g \t %g \t %g \t %g \t %g \t %g \t %g \t %g \t %g \t %g \t %g \t %g \n",TimerTest,pExo->aFingers[0].posTip[0],pExo->aFingers[0].posTip[1],pExo->aFingers[0].posTip[2],pExo->aFingers[0].quatTip[0],pExo->aFingers[0].quatTip[1],pExo->aFingers[0].quatTip[2],pExo->aFingers[0].quatTip[3],pExo->aFingers[1].posTip[0],pExo->aFingers[1].posTip[1],pExo->aFingers[1].posTip[2],pExo->aFingers[1].quatTip[0],pExo->aFingers[1].quatTip[1],pExo->aFingers[1].quatTip[2],pExo->aFingers[1].quatTip[3],pExo->aFingers[2].posTip[0],pExo->aFingers[2].posTip[1],pExo->aFingers[2].posTip[2],pExo->aFingers[2].quatTip[0],pExo->aFingers[2].quatTip[1],pExo->aFingers[2].quatTip[2],pExo->aFingers[2].quatTip[3]);//init
		//	//fprintf(pFileExoCartesianData,"%g \t %g \t %g \t %g \n",TimerTest,pExo->aFingers[1].posTip[0],pExo->aFingers[1].posTip[1],pExo->aFingers[1].posTip[2]);//init
		//	fclose(pFileExoCartesianData);
		//}

	}//flg_teleop_Exo_SH==true

	////////////////////////////////////////////////////
//printTimer++;
	//////////////////////////////////////////////////////////
			if(Clock_Pr->getCurrentTimeSeconds()>0.05)
			{
					Clock_Pr->reset();
					Clock_Pr->start();
				 int ch=0;
 
      //Get the keyboard input
            if(_kbhit())
            {
                  ch = _getch();
                  ClearScreen();
                  Show_options();
 
                  switch (ch)
                  {

				  case 'm':
				{
					/*if((pSH->rawPos+1000)<HAND_MAX)
					{*/
					int desPos_loc=pSH->rawPos+100000;
					pSH->SetDesiredPos(desPos_loc);
						//pSH->SetDesiredPos(pSH->rawPos+1000);
						pSH->ApplyDesiredPosition();
						/*printf("Des pos: %i \n",pSH->GetDesiredPos());
					}
					else
					{
						printf("Max pos \n");
					}*/
					  
				 }break;
				  case 'f':
				{
					if(flg_FT_mode==true)
					{
						printf("Switch to torque observer mode \n");
						flg_FT_mode=false;
					}
					else
					{
						printf("Switch to FT mode \n");
						flg_FT_mode=true;
					}
				 }break;
				  case 'c':
					  {
						  pSH->Close();
					  }break;
				case'n':
				{
					if((pSH->rawPos-1000)>HAND_MIN)
					{
						pSH->SetDesiredPos(pSH->rawPos-100000);
						pSH->ApplyDesiredPosition();
						printf("Des pos: %i \n",pSH->GetDesiredPos());
					}
					else
					{
						printf("Min pos \n");
					}
					  				
				 }break;
				case 'o':
					{
						pSH->Open();
					}break;
				case 'p':
				{
					//if(!flg_myPrint)
					//{
						flg_myPrint=true;
					//}
					//else
					//{
					//	flg_myPrint=false;
					//}
				}break;
case 'l':
				{

						flg_myPrint=false;

				}break;
				case 'a'://Test
				{
					//Pos init
					Kinematics::computeFwdKin(pExo); //ok
					PCA_synergy::update_syn_struct(pExo);// CODE convert impObj to stiffSpring	
					PCA_synergy::compute_sigma1(); //Projection
					PCA_synergy::update_struct_from_syn(pExo);
					printf("pSH_unitScaled: %g \n",pSH_Syn->get_posRef_unitScaled());
	  
				 }break;
				 case 'u'://Init teleop SH with exo
				{
					//Pos init
					Kinematics::computeFwdKin(pExo); //ok
					PCA_synergy::update_syn_struct(pExo);// CODE convert impObj to stiffSpring	
					PCA_synergy::compute_sigma1(); //Projection
					//Set open pos sigma1 for this user
					PCA_synergy::sigma1_range_dataCollection[1]=PCA_synergy::sigma1;//open hand
					PCA_synergy::update_struct_from_syn(pExo);

					printf("Start teleop SH \n");
					flg_teleop_Exo_SH=true;	  
				 }break;

				 case 'r'://Init teleop SH with exo
				{
					printf("Stop teleop SH \n");
					flg_teleop_Exo_SH=false;	  
				 }break;

				case '4':
				{	//Force control should be turned off before calibrating the offsets
					int board_id;
					for(int it=0;it<3;it++)
					{
						board_id=it+1;
						CalibrateOffsets(board_id);
					}
					//CalibrateOffsets(board_id);
					printf("Calibrate offsets \n");
				}break;
				case '5':
				{
						//for(int iti=0;iti<10;iti++)
						//{
						//	for(int itj=0;itj<3;itj++)
						//	{
						//		desTorque_exo_loc[iti][itj]=-50;
						//	}
						//}
						//Grav compensation (sort of..)
						for(int iti=0;iti<MAX_BOARDS;iti++)
						{
							//for(int itj=0;itj<MAX_MOTORS;itj++)
							//{
								desTorque_exo_loc[iti][0]=0;//desTorque_exo_loc[iti][0]-50*sin(abs(pExo->aFingers[iti].jointPos_dh[0]));
							//}
						}
						SetDesiredForce(desTorque_exo_loc);
					StartForceControl();
					printf("Start force control \n");
				}break;
				case '6':
				{
					StopForceControl();
					printf("Start force control \n");
				}break;

                  case '7':
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
					  case '8':
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
					   case 'b'://bias FTsensor
						 {
							ATI_FTsensor->BiasFTsensor();

						  }break;
				  case 'q':
					  {
						  StopForceControl();//Exo
						  flg_runThread_Kuka=false;
						  flg_teleop_Exo_SH=false;
						  flg_runThread_ExoSH=false;
						 stopProgram=true;
					  }break;
                  default:
                        break;
                  }
            }          
                        ch=0;
				if(initializationDone_KukaThread==true&&initializationDone_ExoSHThread==true&&flg_myPrint==false&&printer_justOnce==true)//Do it once at the end of initialization
				{
					PrintFnt::ClearScreen();
					printer_justOnce=false;
					flg_myPrint=true;
					//Myprint();
				}

				if(flg_myPrint)
				{
					Myprint();
				}
				//else
				//{
				//		printf("Frequency Exo_SH loop = %6.1g Hz\n",1/ TimerLoop_Exo_SH);
				//}
				
				//GetKeyboard();

				//printTimer=0;
			}//if printTimer>deltaNumLoops

			}//if Clock_c

			/////////////////////////////

	}//while (thread loop)



}//updateLoop_Exo_SH


/////////////////////////////////////////////////////////
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

				  case '+':
				{
					if((pSH->rawPos+1000)<HAND_MAX)
					{
						pSH->SetDesiredPos(pSH->rawPos+1000);
						pSH->ApplyDesiredPosition();
						printf("Des pos: %i \n",pSH->GetDesiredPos());
					}
					else
					{
						printf("Max pos \n");
					}
					  
				 }break;
				case'-':
				{
					if((pSH->rawPos-1000)>HAND_MIN)
					{
						pSH->SetDesiredPos(pSH->rawPos-1000);
						pSH->ApplyDesiredPosition();
						printf("Des pos: %i \n",pSH->GetDesiredPos());
					}
					else
					{
						printf("Min pos \n");
					}
					  				
				 }break;
				case 'o':
					{
						pSH->Open();
					}break;
				case 'p':
				{
					//if(!flg_myPrint)
					//{
						flg_myPrint=true;
					//}
					//else
					//{
					//	flg_myPrint=false;
					//}
				}break;
case 'l':
				{

						flg_myPrint=false;

				}break;
				 case 'a'://Init teleop SH with exo
				{
					//Pos init
					Kinematics::computeFwdKin(pExo); //ok
					PCA_synergy::update_syn_struct(pExo);// CODE convert impObj to stiffSpring	
					PCA_synergy::compute_sigma1(); //Projection
					PCA_synergy::update_struct_from_syn(pExo);

					printf("Start teleop SH \n");
					flg_teleop_Exo_SH=true;	  
				 }break;

				case '4':
				{	//Force control should be turned off before calibrating the offsets
					int board_id;
					for(int it=0;it<3;it++)
					{
						board_id=it+1;
						CalibrateOffsets(board_id);
					}
					//CalibrateOffsets(board_id);
					printf("Calibrate offsets \n");
				}break;
				case '5':
				{
					StartForceControl();
					printf("Start force control \n");
				}break;
				case '6':
				{
					StopForceControl();
					printf("Start force control \n");
				}break;

                  case '7':
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
					  case '8':
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
						  StopForceControl();//Exo
						  flg_runThread_Kuka=false;
						  flg_teleop_Exo_SH=false;
						  flg_runThread_ExoSH=false;
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

      //gotoxy(1,8);
      //if (storeData==ON)
      //      printf("\n ----------------------Storing data.        Data counter:%*d --------------\n\n",7,dataCounter);
      //else if(storeData==OFF)
      //      printf("\n ----------------------NOT Storing data.    Data counter:%*d --------------\n\n",7,dataCounter);
	  printf("Frequency Kuka loop = %0.1f Hz\n",1/TimerLoop_Kuka);
	  printf("Frequency Exo_SH loop = %0.1f Hz\n",1/ TimerLoop_Exo_SH);
 
      gotoxy(0,2);
      int p=7;
	  printf("-----------Loop Info ------------------------------------\n");
	  printf("Timer Optitrack: %10.3g \n",Optitrack::m_timer);
	  printf("Timer Exo_SH thread: %10.3g \n",TimerCurrent_Exo_SH);

	  //printf("\n Filtering Optitrack: \n");
	  //printf("m_beta filter: %10.3g \n",Optitrack::m_beta_filter);
	  //printf("m_minCutOff_filter: %10.3g \n",Optitrack::m_mincutoff_filter);
	  //printf("Optitrack raw: %10.3g \t %10.3g \t %10.3g \n",Optitrack::x_raw,Optitrack::y_raw,Optitrack::z_raw);
	  //printf("Optitrack filtered: %10.3g \t %10.3g \t %10.3g \n",Optitrack::rb1.x,Optitrack::rb1.y,Optitrack::rb1.z);

	  printf("FT data: \n");
	  printf("%10.3g \t %10.3g \t %10.3g \n",FTdata[0],FTdata[1],FTdata[2]);

	  printf("\nSynergy data: \n");
	  printf("sigma1_unitScale: %g \t sigma1: %g \n pSH_Syn pos ref: %g \t pSH_Syn pos ref filterd: %g \n",PCA_synergy::convert_sigmaScale2unitScale(PCA_synergy::sigma1),PCA_synergy::sigma1,pSH_Syn->get_posRef_unitScaled(),pSH_Syn->get_posRef_unitScale_filtered());

	 printf("\n SoftHand data: \n");
	 printf("SH pos: %*d \t  Des pos: %*d, SH current: %*d \n",7,pSH->rawPos,7,pSH->GetDesiredPos(),7,pSH->current);//7,pSH_Syn->get_posRef_unitScaled()   Pos ref SH_syn unitScaled: %10.3g,

	 printf("\n Force feedback: \n");
	 int tau_exo_loc[3]={0};
	 pExo->get_torque0_ref( tau_exo_loc);
	 printf("Interaction Torque: %f \t TauExo 0: %i \t TauExo 1: %i \t TauExo 2: %i  \n",TorqueObserver::get_tau_ext_obs(),tau_exo_loc[0],tau_exo_loc[1],tau_exo_loc[1]);

	  printf("\n Exo data: \n");
	  printf("\n\t Finger 1 \t Finger 2 \t Finger 3\n");	
	printf("\nJoint pos - reading from encoders \n");
	for(int i=0;i<6;i++)
	{
		printf("Joint%d \t %*d \t %*d \t %*d\n",i, 7,ExosData.ExosFinger[0].jointPos[i], 7,ExosData.ExosFinger[1].jointPos[i], 7,ExosData.ExosFinger[2].jointPos[i]);//init
	}

	printf("\nJoint pos - dh (deg) \n");
	for(int i=0;i<6;i++)
	{
		printf("Joint%d \t %*g \t %*g \t %*g\n",i, 7,MathUtils::rad2deg(pExo->aFingers[0].jointPos_dh[i]),7,MathUtils::rad2deg(pExo->aFingers[1].jointPos_dh[i]),7,MathUtils::rad2deg(pExo->aFingers[2].jointPos_dh[i]));
	}

	//printf("\nJoint pos - dh (rad) \n");
	//for(int i=0;i<6;i++)
	//{
	//	printf("Joint%d \t %*g \t %*g \t %*g\n",i, 7,pExo->aFingers[0].jointPos_dh[i],7,pExo->aFingers[1].jointPos_dh[i],7,pExo->aFingers[2].jointPos_dh[i]);
	//}

	//printf("\nTorque \n");
	//printf("%*hd \t %*hd \t %*hd \n", 7,ExosData.ExosFinger[0].M_tor[0],7,ExosData.ExosFinger[1].M_tor[0],7,ExosData.ExosFinger[2].M_tor[0]);


	//printf("\nJoint vel q_dot \n");
	//for(int i=0;i<6;i++)
	//{
	//	printf("Joint%d \t %*g \t %*g \t %*g\n",i, 7,pExo->aFingers[0].q_dot_dh_nrt[i], 7,pExo->aFingers[1].q_dot_dh_nrt[i], 7,pExo->aFingers[2].q_dot_dh_nrt[i]);//init
	//}
	

	printf("\nFingerTip position (m) \n");
	for(int i=0;i<3;i++)
	{
		printf("Xp[%d] \t %*g \t %*g \t %*g\n",i, 7,pExo->aFingers[0].posTip[i],7,pExo->aFingers[1].posTip[i],7,pExo->aFingers[2].posTip[i]);//init
	}

	//printf("\nFingerTip RPY (deg) \n");
	//for(int i=0;i<3;i++)
	//{
	//	printf("RPY[%d] \t %*g \t %*g \t %*g\n",i, 7,MathUtils::rad2deg(pExo->aFingers[0].RPY_Tip[i]),7,MathUtils::rad2deg(pExo->aFingers[1].RPY_Tip[i]),7,MathUtils::rad2deg(pExo->aFingers[2].RPY_Tip[i]));//init
	//}

	//printf("\nFingerTip quat \n");
	//for(int i=0;i<4;i++)
	//{
	//	printf("q[%d] \t %*g \t %*g \t %*g\n",i, 7,pExo->aFingers[0].quatTip[i],7,pExo->aFingers[1].quatTip[i],7,pExo->aFingers[2].quatTip[i]);//init
	//}

	//printf("\nFingerTip trace rot matrix \n");
	//for(int i=0;i<3;i++)
	//{
	//	double trace=pExo->aFingers[i].pRotTip->getElement(1,1)+pExo->aFingers[i].pRotTip->getElement(2,2)+pExo->aFingers[i].pRotTip->getElement(3,3);
	//	printf("%*g\n",7,trace);//init
	//}

	printf("\nIndex FingerTip rotation matrix \n");
	pExo->aFingers[1].pRotTip->printf_Matrix();

	//printf("\n Index X_dot \n");
	printf("\n X_dot index \t %*g \t %*g \t %*g\n",7,pExo->aFingers[1].velTip_nrt[0],7,pExo->aFingers[1].velTip_nrt[2],7,pExo->aFingers[1].velTip_nrt[3]);//init


	//Check signs
	//printf("\n Signs check: \n");
	//printf("r32 - r23: %g \n",pExo->aFingers[1].pRotTip->getElement(3,2)-pExo->aFingers[1].pRotTip->getElement(2,3));
	//printf("r13 - r31: %g \n",pExo->aFingers[1].pRotTip->getElement(1,3)-pExo->aFingers[1].pRotTip->getElement(3,1));
	//printf("r21 - r12: %g \n",pExo->aFingers[1].pRotTip->getElement(2,1)-pExo->aFingers[1].pRotTip->getElement(1,2));

	printf("\n");

	  
	  
	 //  printf("m_minCutOff_filter: %10.3g \n"
	 //  "Optitrack raw: %10.3g \t %10.3g \t %10.3g \n"
	 //"Optitrack filtered: %10.3g \t %10.3g \t %10.3g \n"
	 //,Optitrack::m_mincutoff_filter,
	 //Optitrack::x_raw,Optitrack::y_raw,	 Optitrack::z_raw, 
	 //Optitrack::rb1.x,Optitrack::rb1.y,Optitrack::rb1.z);

	   

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
}




