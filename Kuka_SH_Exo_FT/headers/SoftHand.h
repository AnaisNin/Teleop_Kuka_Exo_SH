#ifndef SOFTHAND_H
#define SOFTHAND_H

#include "BoardLibrarySH.h"
#include "phil_boardSH.h"
#pragma comment(lib, "BoardLibrarySH.lib")



#ifndef __MYFUN
#define __MYFUN

#define HAND_MIN 1000000
#define HAND_MAX 2200000 //

//#define BCAST_POLICY 1791
#define BCAST_POLICY_SH 33
#define EXTRA_BCAST_POLICY_SH 3584
#define BCAST_RATE_SH 2

#define FIRST_LBODY_BOARD 0
#define LAST_LBODY_BOARD 1

#define DEGTORAD(x)  x*6.28/360.0
#define RADTODEG(x)  x*360.0/6.28
#define RPMTODEGSEC(x)  x*360.0/(100.0*60.0)
#define DEGSECTORPM(x)  100.0*60*x/360.0
#define IN_DEG 1
#define IN_RAD 2

#define USE_MOTOR_ENCODER 1
#define USE_JOINT_ENCODER 2

#define PID_CONTROL 1
#define LQR_CONTROL 2
#define gainSet 1 // for voltage control **Yogesh**
#define ON 1
#define OFF 0
#define FORWARD 1
#define REVERSE 0
#define posMAX 1000000//1200000 - 700000
#define	posMIN 200000

typedef struct joint_data
{
	//long radPos;
	//long degPos;

	int id;
	long pos; //long
	int vel;
	int tor;
	int pidOutput;
	long pidError;
	long current;
	long temp_Vdc;
	long tStamp;

	int fault;
	int anInput1;
	int anInput2;
	int anInput3;
	int anInput4;
	int absPos1;
	int absPos2;
	int quickSpeed;
	//int realCurrent;
	long realCurrent;

	long tpos;
	long temp_tpos;
	long req_tpos;
	int TwinPos;
	short angle;
	short targAngle;
	long TwinTargPos;
	short TwinVel;
	int Xaccleration;
	int Yaccleration;
	int Zaccleration;
	int LinkVel;

	float tposRAD;
	float finalTime;

	int tvel;

	long TwinActPos;


	int ttor;
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

	int trajDSP;


	int anInput1_Adjusted;
	int	anInput1_Offset;
	int anInput2_Adjusted;
	int	anInput2_Offset;



	int isActive;
	int isConnected;
	int controlType;
	int BoardType;

	float Force;
	float Torque;
	float Force2;
	float Torque2;
	short DamperRefNormForce;
	short DamperRefTorque;
	long DamperRefDeflection;
	long DamperActDeflection;

	float getJointPos(int fromEncoder, int posUnit)
	{

		if (fromEncoder == USE_MOTOR_ENCODER)
			jointPosRAD = jointLimit1RAD + motorEncLine*((float)pos - (float)motorEncLimit1);
		else if (fromEncoder == USE_JOINT_ENCODER)
			jointPosRAD = jointLimit1RAD + jointEncLine*((float)absPos1 - (float)jointEncLimit1);
		jointPosDEG = RADTODEG(jointPosRAD);

		if (posUnit == IN_RAD)
			return jointPosRAD;
		else if (posUnit == IN_DEG)
			return jointPosDEG;
		else return 0;
	};

	void setJointTargetPos(float targetPos, int posUnit)
	{
		if (posUnit == IN_DEG)
			targetPos = DEGTORAD(targetPos);
		if (motorEncLine != 0)
		{
			tpos = motorEncLimit1 + (int)((targetPos - jointLimit1RAD) / motorEncLine);

		}
		else tpos = pos;

	};

} jointData;


class SoftHand //: public ofThread
{
	int noActiveBoards;
	int b;
	int r;
	int desVel[MAX_BOARDS_SH];
	int desPos[MAX_BOARDS_SH];
	int activeBoards[MAX_BOARDS_SH];	

	CharBuff packetToReceive;
	jointData joint[1];

	bool running;

public:
	int rawPos;
	float normalizedPos;
	long current;
	int mainOffset;

public :
	void Setup();
	void InitVars();
	//bool IsRunning();
	void SetDesiredPos(long _desPos);
	int GetDesiredPos()
	{
		return desPos[0];
	};
	int ApplyDesiredPosition();
	void SetDesiredNormalizedPos(float _desPos);
	void Open();
	void Close();
	void CloseConnection();
	void StartPosControl();
	int StopPosControl();

	//void threadedFunction();

	int GetSHBroadCastData();

	void SetOffset(int _offset)
	{
		mainOffset = _offset;
	}

private:

	int ScanSystem();
	void BoardConnectTCP();
	void SetBroadCastRate(int boardNumber, int BCastRate);
	void SetBroadCastPolicy(int boardNumber, int BCastPolicy);
	void StopBroadCast(int BoardNumber);

};

// SOME UTILS
static float Map(float value, float istart, float istop, float ostart, float ostop)
{
	return ostart + (ostop - ostart) * ((value - istart) / (istop - istart));
}

static float Clamp(float value, float min, float max)
{
	if (value < min)
		value = min;
	else if (value > max)
		value = max;

	return value;
}

#endif //MY_FUN
#endif //SOFTHAND_H