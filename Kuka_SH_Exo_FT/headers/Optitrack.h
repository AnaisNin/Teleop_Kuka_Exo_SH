#ifndef OPTITRACK_H
#define OPTITRACK_H

#include "NatNetTypes.h"
#include "NatNetClient.h"
#include <stdio.h> //files

#include "OneEuroFilter.h"
#include "CPrecisionClock.h"

class Optitrack
{

public:
	static int Initialize();
	//static void resetClient();
	static struct rigidBody
	{
		int id;
		double x;
		double y;
		double z;
		double qx;
		double qy;
		double qz;
		double qw;
		//rigidBody():id(0),x(0), y(0), z(0),qx(0),qy(0),qz(0),qw(0) {} //Struct constructor
	};

	///TEST! filtering
	static double x_raw;
	static double y_raw;
	static double z_raw;
	static double qx_raw;
	static double qy_raw;
	static double qz_raw;
	static double qw_raw;

	//static cPrecisionClock* p_mClock_optitrackLoop;
	static double m_timer;

	//These should be private, make a getPosOr public. And make an array of these instances
	static rigidBody rb1;
	static rigidBody rb2;
	static rigidBody rb3;
	
	static void __cdecl DataHandler(sFrameOfMocapData* data, void* pUserData);// receives data from the server
	
	//Useless
	//static void WriteHeader(FILE* fp, sDataDescriptions* pBodyDefs);
	//static void WriteFrame(FILE* fp, sFrameOfMocapData* data);
	//static void WriteFooter(FILE* fp);
	//static void __cdecl MessageHandler(int msgType, char* msg);		// receives NatNet error mesages

	
	 static OneEuroFilter m_filter_x;
	 static OneEuroFilter m_filter_y;
	 static OneEuroFilter m_filter_z;
	 static OneEuroFilter m_filter_qx;
	 static OneEuroFilter m_filter_qy;
	 static OneEuroFilter m_filter_qz;
	 static OneEuroFilter m_filter_qw;

	  static double m_beta_filter;//=0;//to adjust,  default 1.0
	static double m_mincutoff_filter;//=1.0;//to adjust, default 1.0

	 //	Optitrack():filter_x_loc(new OneEuroFilter(0,0,0,0))   //Struct constructor with initialization list. Checked: ok. Set size of rotMatrix; quatTip() makes a zero-initialization.
		//{
		//} 
	//double freq,double mincutoff=1.0, double beta_=0.0, double dcutoff=1.0


private:
	//static int CreateClient(int iConnectionType);
	static char szMyIPAddress[128];// = "10.255.41.94";//This computer
	static char szServerIPAddress[128];// = "10.255.42.107";
	static NatNetClient * mClient;


	//static rigidBody rb2;
	//static rigidBody rb3;
	static int m_numRigidBodies;

};
#endif