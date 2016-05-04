#ifndef _BOARDLIBRARYSH_H_
#define _BOARDLIBRARYSH_H_

#include <winsock2.h>
#include <stdio.h>


#include "phil_boardSH.h"
#include "utils.h"

#define MAX_MOTOR_BOARDS_SH 5//30
#define MAX_BOARDS_SH 5//30
#define BASEADDR_SH 70

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

	//TCP

	//All functions return an error flag (-1 if error)

	/**
	 Any client using the library must call this function once before any other one.
	*/
	DECLDIR int InitLibrary_SH(int boardsCount);

	DECLDIR int ConnectTCP_SH(int BoardNumber);
	DECLDIR int SendTCP_SH(SOCKET SckID, CharBuff* packetToBeSent);
	DECLDIR int ReceiveTCP_SH(SOCKET SckID, CharBuff* packetToReceive);

	DECLDIR int GetBoardType_SH(int BoardNumber);
	DECLDIR float GetFirmVersion_SH(int BoardNumber); 
	DECLDIR int SetFirmVersion_SH(int BoardNumber, char* Firm);

	DECLDIR int ClearBoardFault_SH(int BoardNumber);
	DECLDIR int GetBoardFault_SH(int BoardNumber);

	DECLDIR int SetPidGains_SH(int BoardNumber, char gainSet, char * gains);
	DECLDIR int GetPidGains_SH(int BoardNumber, char gainSet, long * GainsBuf); // Returns an error flag. The buffer parameter are filled with the gains request
	DECLDIR int SetPidGainScale_SH(int BoardNumber, char gainSet, char * GainsScale);
	DECLDIR int GetPidGainScale_SH(int BoardNumber, char gainSet, long * GainsBuf); // Returns an error flag. The buffer parameter are filled with the gain scale request
	DECLDIR int SetILimGain_SH(int BoardNumber, char gainSet, long ILim);
	DECLDIR long GetILimGain_SH(int BoardNumber, char gainSet);
	
	DECLDIR int GetPidOffset_SH(int BoardNumber);
	DECLDIR long GetPidError_SH(int BoardNumber);
	DECLDIR int GetPidOutput_SH(int BoardNumber);

	DECLDIR long GetPosition_SH(int BoardNumber);
	DECLDIR int GetVelocity_SH(int BoardNumber);
	DECLDIR int GetTorque_SH(int BoardNumber);

	DECLDIR long GetDesiredPosition_SH(int BoardNumber);
	DECLDIR int GetDesiredVelocity_SH(int BoardNumber);
	DECLDIR int GetDesiredTorque_SH(int BoardNumber);

	DECLDIR int SetAccel_SH(int BoardNumber, int Accel);
	DECLDIR int GetAccel_SH(int BoardNumber);

	DECLDIR int SetMinPosition_SH(int BoardNumber, long MinPosition);
	DECLDIR long GetMinPosition_SH(int BoardNumber);
	DECLDIR int SetMaxPosition_SH(int BoardNumber, long MaxPosition);
	DECLDIR long GetMaxPosition_SH(int BoardNumber);
	DECLDIR int SetMaxVelocity_SH(int BoardNumber, int MaxVelocity);
	DECLDIR int GetMaxVelocity_SH(int BoardNumber);
	DECLDIR int SetMaxTorque_SH(int BoardNumber, int MaxTorque);
	DECLDIR int GetMaxTorque_SH(int BoardNumber);
	DECLDIR int SetMinVelocity_SH(int BoardNumber, int MinVelocity);
	DECLDIR int GetMinVelocity_SH(int BoardNumber);
	
	DECLDIR int SetCurrentLimit_SH(int BoardNumber, int CurrentLimit);
	DECLDIR int GetCurrentLimit_SH(int BoardNumber);

	DECLDIR int SetBCastRate_SH(int BoardNumber, int BCastRate);
	//DECLDIR int SetBCastRate_SH(int BoardNumber, int BCastRate, char TwinSatus)
	DECLDIR int StopBCast_SH(int BoardNumber);
	DECLDIR int GetBCastRate_SH(int BoardNumber);
	DECLDIR int SetBCastPolicy_SH(int BoardNumber, int BCastPolicy);
	DECLDIR int GetBCastPolicy_SH(int BoardNumber);
	//DECLDIR int SetExtraBCastPolicy(int BoardNumber);
	//DECLDIR int GetExtraBCastPolicy(int BoardNumber);


	DECLDIR int SetEncoderLines_SH(int BoardNumber, long EncoderLines);
	DECLDIR long GetEncoderLines_SH(int BoardNumber);
	DECLDIR int SetMotorPoles_SH(int BoardNumber, char poles);
	DECLDIR int GetMotorPoles_SH(int BoardNumber);
	DECLDIR int SetAnalogInputs_SH(int BoardNumber, char analogInputs);
	DECLDIR int DoRecalculatePosition_SH(int BoardNumber, char Calib);
	DECLDIR int GetAnalogInputs_SH(int BoardNumber);

	DECLDIR int CmdUpgrade_SH(int BoardNumber);
	DECLDIR int SaveParamsToFlash_SH(int BoardNumber);
	DECLDIR int LoadParamsFromFlash_SH(int BoardNumber);
	DECLDIR int LoadDefaultParams_SH(int BoardNumber);

	DECLDIR int ControllerRun_SH(int BoardNumber);
    DECLDIR int ControllerIdle_SH(int BoardNumber);
	DECLDIR int EnablePwmPad_SH(int BoardNumber);
	DECLDIR int DisablePwmPad_SH(int BoardNumber);
	
	DECLDIR int DoCalibrate_SH(int BoardNumber, int Direction, long InitPos);
	DECLDIR int SetCalibrationCurrent_SH(int BoardNumber, int i);
	DECLDIR int GetCalibrationCurrent_SH(int BoardNumber);

	DECLDIR int SetAbsoluteZero_SH(int BoardNumber, int AbsZero);
	DECLDIR int GetAbsoluteZero_SH(int BoardNumber);
	DECLDIR int SetTorqueAccel_SH(int BoardNumber, char tAccel);
	DECLDIR int GetTorque_SHAccel_SH(int BoardNumber);

	DECLDIR int SetMotorType_SH(int BoardNumber, char motorType);
	DECLDIR int GetMotorType_SH(int BoardNumber);

	DECLDIR void CloseSockets_SH(void);

	DECLDIR int SetMinAbsPosition_SH(int BoardNumber, int minAbsPos);
	DECLDIR int GetMinAbsPosition_SH(int BoardNumber);

	DECLDIR int SetMaxAbsPosition_SH(int BoardNumber, int maxAbsPos);
	DECLDIR int GetMaxAbsPosition_SH(int BoardNumber);
	
	// FT Sensor related commands

	DECLDIR int CalibrateOffsets_SH(int BoardNumber);
	DECLDIR int GetCalibrationOffsets_SH(int BoardNumber);

	DECLDIR int SetConversionFactors_SH(int BoardNumber, int channel, int multiplier, int offset);
	DECLDIR int GetConversionFactors_SH(int BoardNumber, char channel, int* FactorsBuf);

	DECLDIR int SetAvarageSamples_SH(int BoardNumber, int samples);
	DECLDIR int GetAvarageSamples_SH(int BoardNumber, int samples);

	DECLDIR int SetMatrixRow_SH(int BoardNumber, char row, float *matrixRow);
	DECLDIR int GetMatrixRow_SH(int BoardNumber, char row_req, char row_recv, float matrixRow[6]);

	// FT Sensor related commands


	//UDP

	DECLDIR int ConnectUDP_SH(void);
	DECLDIR int SendUDP_SH(SOCKET SckID, CharBuff* packetToBeSent); 
	DECLDIR int recvfromTimeOutUDP_SH(SOCKET socket, long sec, long usec);
	DECLDIR int ReceiveUDP_SH(SOCKET SckID, CharBuff* packetToReceive);
	DECLDIR int ReceiveUDPWithTimeOut_SH(SOCKET UDPSckID, CharBuff* packetToReceive);

	DECLDIR int GetBCastData_SH(CharBuff &packetToReceive);
	DECLDIR int WriteBCastData_SH(int BoardNumber, FILE * stream);

	DECLDIR int ScanForActiveBoards_SH(void);
	DECLDIR int GetActiveBoards_SH(int *boardIDs);

	DECLDIR int SetDesiredPosition_SH(int DesPos[MAX_BOARDS_SH]);
	//DECLDIR int SetDesiredPosition_SH(int DesPos[MAX_BOARDS_SH],CharBuff * a_packetToBeSent);
	//DECLDIR CharBuff SetDesiredPosition_SH(int DesPos[MAX_BOARDS_SH]);
	DECLDIR int SetDesiredStiffnessDamping_SH(int DesSiff[MAX_BOARDS_SH], int DesDamp[MAX_BOARDS_SH]);
	DECLDIR int SetDesiredVelocity_SH(int DesVel[MAX_BOARDS_SH]);
	DECLDIR int SetDesiredTorque_SH(int DesTor[MAX_BOARDS_SH]);
	DECLDIR int SetPidOffset_SH(int DesOffset[MAX_BOARDS_SH]);

	DECLDIR int StartPositionControl_SH(void);	// start all active boards
	DECLDIR int StartBoardPositionControl_SH(int boardNumber); // start the selected board
	DECLDIR int StopPositionControl_SH(void);  // stop all boards
	
	DECLDIR int StartVelocityControl_SH(void);
	DECLDIR int StartTorqueControl_SH(void);

	DECLDIR int StopMotorUDP_SH(void);

	DECLDIR int SetTorqueOnOff_SH(int BoardNumber, int torqueFlag);
	DECLDIR int SetMotorConfig_SH(int BoardNumber, char *mConfig);

	DECLDIR int GetTorque_SHOnOff_SH(int BoardNumber);
	DECLDIR int GetMotorConfig_SH(int BoardNumber);

	DECLDIR int SetPDlinkGains_SH(int BoardNumber, char *Gains);
	DECLDIR int GetPDlinkGains_SH(int BoardNumber, long* GainsBuf);

	
#ifdef __cplusplus
}
#endif

#endif