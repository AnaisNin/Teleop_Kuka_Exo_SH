#ifndef BoardControlH
#define BoardControlH

//#define MAX_MOTOR_BOARDS 10
//#define MAX_BOARDS			10
#include "EXOSBoardLibrary.h"
#include "JointData.h"

class BoardControl
{

	public:
		//Communication
		static int connectUDP();
		static void setControllerGains();
		static int getBroadCastData();
		static void stopBroadCast(int BoardNumber);
		static void setBroadCastPolicy(int boardNumber, int BCastPolicy);
		static void setBroadCastRate(int boardNumber, int BCastRate);
		static void startTorqueControl(int BoardNumber, int torqueFlag);
		static void stopPositionControl();
		static void connectTCP();
		static int scanSystem();
		static void startPositionControl();

		//Encoders
		//static void loadJointDataFromDrivers();

		//static int activeBoards[MAX_BOARDS]={0,0,0,0,0,0,0,0,0,0};
		static int activeBoards[MAX_BOARDS];

		static int noActiveBoards;

		static JointData joint[1];


	private:
		
	



};
#endif;