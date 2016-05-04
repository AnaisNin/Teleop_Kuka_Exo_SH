#ifndef JointDataH
#define JointDataH

#include "Commons.h"

#define USE_MOTOR_ENCODER 1
#define USE_JOINT_ENCODER 2

struct JointData
{
public:

	//Functions
		
	//Members
	int id;
	long pos;
	int tpos;
	float tposRAD;
	float finalTime;
	int vel;
	int tvel;
	int tor;
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
	int pidOutput;
	long pidError;
	int current;
	int trajDSP;
	long temp_Vdc;
	long tStamp;
	int fault;
	int anInput1;
	int anInput2;
	int anInput3;
	int anInput4;
	int anInput1_Adjusted;
	int	anInput1_Offset;
	int anInput2_Adjusted;
	int	anInput2_Offset;
	int absPos1;
	int absPos2;
	int quickSpeed;
	int realCurrent;
	int isActive;
	int isConnected;
	int controlType;
	long TwinActPos;
	long TwinTargPos;
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

	//Functions

	//Constructor
	//JointData(){};

	float getJointPos(int fromEncoder, int posUnit)
	{
		if(fromEncoder==USE_MOTOR_ENCODER)
			jointPosRAD=jointLimit1RAD + motorEncLine*((float)pos - (float)motorEncLimit1);
		else if(fromEncoder==USE_JOINT_ENCODER)
			jointPosRAD=jointLimit1RAD + jointEncLine*((float)absPos1 - (float)jointEncLimit1);	
			jointPosDEG=RADTODEG(jointPosRAD);

		if(posUnit==IN_RAD)
			return jointPosRAD;
		else if(posUnit==IN_DEG)
			return jointPosDEG;
		else return 0;
	}

	void setJointTargetPos(float targetPos, int posUnit)
	{
		if(posUnit==IN_DEG)
			targetPos=DEGTORAD(targetPos);
		if(motorEncLine!=0)
		{
			tpos=motorEncLimit1 + (int)((targetPos - jointLimit1RAD)/motorEncLine);

		}
		else tpos=pos;
	}




};

#endif JointDataH