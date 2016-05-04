#ifndef _BOARDLIBRARY_H_
#define _BOARDLIBRARY_H_

#include <winsock2.h>
#include <stdio.h>


#include "phil_board.h"
#include "utils.h"

//////#define MAX_MOTOR_BOARDS 15
//////#define MAX_BOARDS 30
//////#define BASEADDR 70
#define MAX_MOTOR_BOARDS 10
#define MAX_BOARDS			10//34
#define MAX_MOTORS			3

//#define BASEADDR			40//40-Exoskeleton (It has been overriden to 50 for compatibility of this dll with the instrumented object)


#define BASEADDR			50 ////Depends on the board, check with IAR. For spare board, 70 - for exo board, 50.
//#define BASEADDR			70//70-MotorController

#define PCKT_SIZE			128
#define FNGR_JOINTS			6// 6 finger joints (1 Opt. encoder 5 Magnetic)
#define BCAST_POLICY		135//7 for time stamp//1792
//#define EXTRA_BCAST_POLICY	64//4032
#define EXTRA_BCAST_POLICY	192//4032
#define ACC_EXTRA_BCAST_POLICY 15872//3584


#define DLL_EXPORT
#define THUMB_ID	2
#define INDEX_ID	3
#define MIDDLE_ID	4

#define DLL_EXPORT
#if defined DLL_EXPORT
#define DECLDIR __declspec(dllexport)
#else
#define DECLDIR __declspec(dllimport)
#endif

//extern "C": tells the compiler that it is okay to use this in C or C++.
#ifdef __cplusplus
extern "C" {
#endif
	 
//********************EXOS STRUCTURES**************************************************
#define NumMot 3 //the number of motors driven by the board. This number is 3 and affects the data size in the packet.
typedef struct Exos_Finger{
	int id;	 
	 
		 unsigned char PacketRaw[PCKT_SIZE]; // raw data exactly as it appears in the packet
		 int data[PCKT_SIZE];// Data vector after rearranging bytes

		 int jointPos[6];// 1 optical and 5 magnetic - units encoder#
		 int jointPosOffs[6];
		 double jointPosRad[6];// io - encoder reading converted in rad
		 //double jointPos_dh[6];// io - rad, following dh convention. Filled by client application.

		 //double Xp_tip_base[3]; //cartesian coordinates


		 int M_pos[NumMot];
		 //int M_vel[NumMot];
		 short M_vel[NumMot];
		// short M_tor[NumMot];
		 //int M_vel[NumMot];
		 int M_tor[NumMot];

		 int pidOutput[NumMot];
		 long pidError[NumMot];
		 long realCurrent[NumMot];
	 	 long temp_Vdc;
		 long tStamp;
		 int fault;
		 int anInput1;
		 int anInput2;
		 int anInput3;
		 int anInput4;
		 int quickSpeed[NumMot];
		 long TargPos[NumMot];
		 long TempTargPos[NumMot];
		 long ReqTargPos[NumMot];
		 short slaveTorque[NumMot];

	 //int tpos[NumMot];

	 float tposRAD;
	 float finalTime;

	 //int tvel;
	 //int tor;
	 //int ttor;

	 short tvel;	 
	 int anInput1_Adjusted;
	 int anInput1_Offset;
	 int anInput2_Adjusted;
	 int anInput2_Offset;

	 short ttor;
	 int motorEncLimit1;
	 int motorEncLimit2;
	 int jointEncLimit1;
	 int jointEncLimit2;
	 float jointLimit1DEG;
	 float jointLimit2DEG;
	 float jointLimit1RAD;
	 float jointLimit2RAD;
	 float motorEncLine;
	 float jointEncLine;
	 float jointPosRAD;
	 float jointPosDEG;
	 int minvel;
	 int maxvel;
	 int maxCur;
	 int pgain;
	 int igain;
	 int dgain;
	 int ilimit;
	 int gainscale;
	 int offset;
	 
	 int current;
	 int trajDSP;
	 int absPos1;
	 int absPos2;
	 int isActive;
	 int isConnected;
	 int controlType;
	 long ActPos[NumMot];

	 short TwinVel;
	 short DamperRefNormForce;
	 short DamperRefTorque;
	 long DamperRefDeflection;
	 long DamperActDeflection;	
	 int Xaccleration;
	 int Yaccleration;
	 int Zaccleration;
	 float Force;
	 float Torque;
	 float Force2;
	 float Torque2;
}Exos_Finger;

//Exos_Finger ExosFinger[3];

typedef struct HandExoskeleton {
 Exos_Finger ExosFinger[3];
 
 int Xaccleration;
 int Yaccleration;
 int Zaccleration;
 long Accel[4];// only3 are used but we need 6

 float t_stamp;
 int isConnected;
} HandExoskeleton ;

//**************END OF EXOS STRUCTURES**************************************************

	//TCP

	//All functions return an error flag (-1 if error)

	/**
	 Any client using the library must call this function once before any other one.
	*/
	int DECLDIR InitLibrary(int boardsCount);

	DECLDIR int ConnectTCP(int BoardNumber);
	DECLDIR int SendTCP(SOCKET SckID, CharBuff* packetToBeSent);
	DECLDIR int ReceiveTCP(SOCKET SckID, CharBuff* packetToReceive);

	DECLDIR int GetBoardType(int BoardNumber);
	DECLDIR float GetFirmVersion(int BoardNumber); 
	DECLDIR int SetFirmVersion(int BoardNumber, char* Firm);

	DECLDIR int ClearBoardFault(int BoardNumber);
	DECLDIR int GetBoardFault(int BoardNumber);

	DECLDIR int SetPidGains(int BoardNumber, char gainSet, char * gains);
	DECLDIR int GetPidGains(int BoardNumber, char gainSet, long * GainsBuf); // Returns an error flag. The buffer parameter are filled with the gains request
	DECLDIR int SetPidGainScale(int BoardNumber, char gainSet, char * GainsScale);
	DECLDIR int GetPidGainScale(int BoardNumber, char gainSet, long * GainsBuf); // Returns an error flag. The buffer parameter are filled with the gain scale request
	DECLDIR int SetILimGain(int BoardNumber, char gainSet, long ILim);
	DECLDIR long GetILimGain(int BoardNumber, char gainSet);
	
	DECLDIR int GetPidOffset(int BoardNumber);
	DECLDIR long GetPidError(int BoardNumber);
	DECLDIR int GetPidOutput(int BoardNumber);

	DECLDIR long GetPosition(int BoardNumber);
	DECLDIR int GetVelocity(int BoardNumber);
	DECLDIR int GetTorque(int BoardNumber);

	DECLDIR long GetDesiredPosition(int BoardNumber);
	DECLDIR int GetDesiredVelocity(int BoardNumber);
	DECLDIR int GetDesiredTorque(int BoardNumber);

	DECLDIR int SetAccel(int BoardNumber, int Accel);
	DECLDIR int GetAccel(int BoardNumber);

	DECLDIR int SetMinPosition(int BoardNumber, long MinPosition);
	DECLDIR long GetMinPosition(int BoardNumber);
	DECLDIR int SetMaxPosition(int BoardNumber, long MaxPosition);
	DECLDIR long GetMaxPosition(int BoardNumber);
	DECLDIR int SetMaxVelocity(int BoardNumber, int MaxVelocity);
	DECLDIR int GetMaxVelocity(int BoardNumber);
	DECLDIR int SetMaxTorque(int BoardNumber, int MaxTorque);
	DECLDIR int GetMaxTorque(int BoardNumber);
	DECLDIR int SetMinVelocity(int BoardNumber, int MinVelocity);
	DECLDIR int GetMinVelocity(int BoardNumber);
	
	DECLDIR int SetCurrentLimit(int BoardNumber, int CurrentLimit);
	DECLDIR int GetCurrentLimit(int BoardNumber);

	DECLDIR int SetBCastRate(int BoardNumber, int BCastRate);
	//DECLDIR int SetBCastRate(int BoardNumber, int BCastRate, char TwinSatus)
	DECLDIR int StopBCast(int BoardNumber);
	DECLDIR int GetBCastRate(int BoardNumber);
	DECLDIR int SetBCastPolicy(int BoardNumber, int BCastPolicy);
	DECLDIR int GetBCastPolicy(int BoardNumber);
	//DECLDIR int SetExtraBCastPolicy(int BoardNumber);
	//DECLDIR int GetExtraBCastPolicy(int BoardNumber);


	DECLDIR int SetEncoderLines(int BoardNumber, long EncoderLines);
	DECLDIR long GetEncoderLines(int BoardNumber);
	DECLDIR int SetMotorPoles(int BoardNumber, char poles);
	DECLDIR int GetMotorPoles(int BoardNumber);
	DECLDIR int SetAnalogInputs(int BoardNumber, char analogInputs);
	DECLDIR int DoRecalculatePosition(int BoardNumber, char Calib);
	DECLDIR int GetAnalogInputs(int BoardNumber);

	DECLDIR int CmdUpgrade(int BoardNumber);
	DECLDIR int SaveParamsToFlash(int BoardNumber);
	DECLDIR int LoadParamsFromFlash(int BoardNumber);
	DECLDIR int LoadDefaultParams(int BoardNumber);

	DECLDIR int ControllerRun(int BoardNumber);
    DECLDIR int ControllerIdle(int BoardNumber);
	DECLDIR int EnablePwmPad(int BoardNumber);
	DECLDIR int DisablePwmPad(int BoardNumber);
	
	DECLDIR int DoCalibrate(int BoardNumber, int Direction, long InitPos);
	DECLDIR int SetCalibrationCurrent(int BoardNumber, int i);
	DECLDIR int GetCalibrationCurrent(int BoardNumber);

	DECLDIR int SetAbsoluteZero(int BoardNumber, int AbsZero);
	DECLDIR int GetAbsoluteZero(int BoardNumber);
	DECLDIR int SetTorqueAccel(int BoardNumber, char tAccel);
	DECLDIR int GetTorqueAccel(int BoardNumber);

	DECLDIR int SetMotorType(int BoardNumber, char motorType);
	DECLDIR int GetMotorType(int BoardNumber);

	DECLDIR void CloseSockets(void);

	DECLDIR int SetMinAbsPosition(int BoardNumber, int minAbsPos);
	DECLDIR int GetMinAbsPosition(int BoardNumber);

	DECLDIR int SetMaxAbsPosition(int BoardNumber, int maxAbsPos);
	DECLDIR int GetMaxAbsPosition(int BoardNumber);
	
	// FT Sensor related commands

	DECLDIR int CalibrateOffsets(int BoardNumber);
	DECLDIR int GetCalibrationOffsets(int BoardNumber);

	DECLDIR int SetConversionFactors(int BoardNumber, int channel, int multiplier, int offset);
	DECLDIR int GetConversionFactors(int BoardNumber, char channel, int* FactorsBuf);

	DECLDIR int SetAvarageSamples(int BoardNumber, int samples);
	DECLDIR int GetAvarageSamples(int BoardNumber, int samples);

	DECLDIR int SetMatrixRow(int BoardNumber, char row, float *matrixRow);
	DECLDIR int GetMatrixRow(int BoardNumber, char row_req, char row_recv, float matrixRow[6]);

	// FT Sensor related commands


	//UDP

	DECLDIR int ConnectUDP(void);
	DECLDIR int SendUDP(SOCKET SckID, CharBuff* packetToBeSent); 
	DECLDIR int recvfromTimeOutUDP(SOCKET socket, long sec, long usec);
	DECLDIR int ReceiveUDP(SOCKET SckID, CharBuff* packetToReceive);
	DECLDIR int ReceiveUDPWithTimeOut(SOCKET UDPSckID, CharBuff* packetToReceive);

	DECLDIR int GetBCastData(CharBuff &packetToReceive);
	DECLDIR int GetExosDataMatlab(int BoardID, int ExosData[][FNGR_JOINTS]);
	DECLDIR int GetExosData(int BoardID, HandExoskeleton &MyExosData);
	DECLDIR int* GetExosFingerData(int theFinger);
	DECLDIR int** GetAllExosFingerData();
	DECLDIR int GetAccelerometer(int BoardID, int AccelData[4]);
	DECLDIR int WriteBCastData(int BoardNumber, FILE * stream);

	DECLDIR int ScanForActiveBoards(void);
	DECLDIR int GetActiveBoards(int *boardIDs);

	//DECLDIR int SetDesiredPosition(int DesPos[MAX_BOARDS]);
	DECLDIR CharBuff SetDesiredPosition_m(int DesPos[MAX_BOARDS]);//ADDED - customized
	DECLDIR int SetDesiredStiffnessDamping(int DesSiff[MAX_BOARDS], int DesDamp[MAX_BOARDS]);
	DECLDIR int SetDesiredVelocity(int DesVel[MAX_BOARDS]);
	DECLDIR int SetDesiredTorque(int DesTor[MAX_BOARDS]);
	DECLDIR int SetPidOffset(int DesOffset[MAX_BOARDS]);

	DECLDIR int StartPositionControl(void);	// start all active boards
	DECLDIR int StartBoardPositionControl(int boardNumber); // start the selected board
	DECLDIR int StopPositionControl(void);  // stop all boards
	
	DECLDIR int StartVelocityControl(void);
	DECLDIR int StartTorqueControl(void);

	DECLDIR int StopMotorUDP(void);

	DECLDIR int SetTorqueOnOff(int BoardNumber, int torqueFlag);
	DECLDIR int SetMotorConfig(int BoardNumber, char *mConfig);

	DECLDIR int GetTorqueOnOff(int BoardNumber);
	DECLDIR int GetMotorConfig(int BoardNumber);

	DECLDIR int SetPDlinkGains(int BoardNumber, char *Gains);
	DECLDIR int GetPDlinkGains(int BoardNumber, long* GainsBuf);

	//NEW
	//DECLDIR int SetDesiredForce(int DesForce[][MAX_MOTORS]);
	DECLDIR int SetDesiredForce(int DesForce[MAX_BOARDS][MAX_MOTORS]);
	DECLDIR int SetDesiredPosition(int DesPos[][MAX_MOTORS]);//CHECK
	DECLDIR int StartForceControl(void);
	DECLDIR int StopForceControl(void);  // stop all boards
	DECLDIR int TakeJointOffsets(int BoardNumber);
	
#ifdef __cplusplus
}
#endif

#endif