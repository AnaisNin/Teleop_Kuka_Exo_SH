#include "Optitrack.h"
#include <stdio.h>
#include <string.h>
#include <windows.h> //Sleep

#include "NATUtils.h"
#include "MathUtils.h" //to remove?


//Internet
//char Optitrack::szMyIPAddress[128] = "10.255.41.108";//This computer - write proper method to get local address
//char Optitrack::szServerIPAddress[128] = "10.255.42.107";
//LAN
char Optitrack::szMyIPAddress[128] = "192.168.0.81";//This computer - write proper method to get local address
//char Optitrack::szMyIPAddress[128] = "192.168.0.83";//This computer - write proper method to get local address
char Optitrack::szServerIPAddress[128] = "192.168.0.10";

NatNetClient * Optitrack::mClient;
int Optitrack::m_numRigidBodies;
//struct Optitrack::rigidBody;
Optitrack::rigidBody Optitrack::rb1; //both rigidBody struct type and instances rbi are members of Optitrack, so mention it twice
Optitrack::rigidBody Optitrack::rb2;
Optitrack::rigidBody Optitrack::rb3;

OneEuroFilter Optitrack::m_filter_x;//=OneEuroFilter();
OneEuroFilter Optitrack::m_filter_y;
OneEuroFilter Optitrack::m_filter_z;
OneEuroFilter Optitrack::m_filter_qx;
OneEuroFilter Optitrack::m_filter_qy;
OneEuroFilter Optitrack::m_filter_qz;
OneEuroFilter Optitrack::m_filter_qw;

//cPrecisionClock* Optitrack::p_mClock_optitrackLoop;
cPrecisionClock* p_mClock_optitrackLoop;

double Optitrack::m_timer=0;

double Optitrack::x_raw;
double Optitrack::y_raw;
double Optitrack::z_raw;
double Optitrack::qx_raw;
double Optitrack::qy_raw;
double Optitrack::qz_raw;
double Optitrack::qw_raw;

double Optitrack::m_beta_filter=0.0;//=0;//to adjust,  default 1.0
double Optitrack::m_mincutoff_filter=1.0;//=1.0;//to adjust, default 1.0

//int Optitrack::Initialize()
//{
//	//mClient=new NatNetClient(ConnectionType_Unicast);
//	int iResult=Optitrack::CreateClient(ConnectionType_Unicast);
//	return iResult;
//}


int Optitrack::Initialize()
{
//	//Setting up the filter
  //double duration = 10.0 ; // seconds
  //double frequency = 120 ; // Hz
  //double mincutoff = 1.0 ; // FIXME
  //double beta = 1.0 ;      // FIXME
  //double dcutoff = 1.0 ;   // this one should be ok

  //double beta_filter_loc=0;//to adjust,  default 1.0
  //double mincutoff_filter_loc=1.0;//to adjust, default 1.0

  printf("Filter param: beta=%g minCutOff=%g[Optitrack.cpp] \n",m_beta_filter,m_mincutoff_filter);

  m_filter_x.setBeta(m_beta_filter);//To adjust
   m_filter_x.setMinCutoff(m_mincutoff_filter);//To adjust
   //filter_x_loc.setFrequency(frequency);//frequency Hz, default 120, should be ok
  //filter_x_loc.setDerivateCutoff(dcutoff);//default=1.0, should be ok
   m_filter_y.setBeta(m_beta_filter);//To adjust
   m_filter_y.setMinCutoff(m_mincutoff_filter);//To adjust

   m_filter_z.setBeta(m_beta_filter);//To adjust
   m_filter_z.setMinCutoff(m_mincutoff_filter);//To adjust

   double minCutOff_orientation_loc=0.5;
   m_filter_qx.setBeta(m_beta_filter);//To adjust
  // m_filter_qx.setMinCutoff(m_mincutoff_filter);//To adjust
   m_filter_qx.setMinCutoff(minCutOff_orientation_loc);//To adjust

   m_filter_qy.setBeta(m_beta_filter);//To adjust
   m_filter_qy.setMinCutoff(minCutOff_orientation_loc);//To adjust

   m_filter_qz.setBeta(m_beta_filter);//To adjust
   m_filter_qz.setMinCutoff(minCutOff_orientation_loc);//To adjust

   m_filter_qw.setBeta(m_beta_filter);//To adjust
   m_filter_qw.setMinCutoff(minCutOff_orientation_loc);//To adjust


	printf("Try to create NatNet client.. \n");
    // release previous server
    if(mClient)
    {
        mClient->Uninitialize();
        delete mClient;
    }

    // create NatNet client
    //mClient = new NatNetClient(ConnectionType_Unicast);
	mClient = new NatNetClient(ConnectionType_Multicast);

    // [optional] use old multicast group
    //theClient->SetMulticastAddress(szServerMulticast);

    // print version info
    unsigned char ver[4];
    mClient->NatNetVersion(ver);
    //printf("NatNet Sample Client (NatNet ver. %d.%d.%d.%d)\n", ver[0], ver[1], ver[2], ver[3]);

	
	p_mClock_optitrackLoop=new cPrecisionClock();
	p_mClock_optitrackLoop->reset();
	p_mClock_optitrackLoop->start();

    // Set callback handlers -- loop
//    mClient->SetMessageCallback(MessageHandler);
 //   mClient->SetVerbosityLevel(Verbosity_Debug);
      mClient->SetDataCallback( DataHandler, mClient );	// this function will receive data from the server

    // Init Client and connect to NatNet server
    // to use NatNet default port assigments
    
	  int retCode = mClient->Initialize(Optitrack::szMyIPAddress, Optitrack::szServerIPAddress);
    //int retCode = theClient->Initialize(szMyIPAddress, szServerMulticast);
    //int retCode = theClient->Initialize(szMyIPAddress, szServerIPAddress, MyServersCommandPort, MyServersDataPort); // to use a different port for commands and/or data:
    if (retCode != ErrorCode_OK)
    {
        printf("Unable to connect to server.  Error code: %d. Exiting", retCode);
		Sleep(5000);
        return ErrorCode_Internal;
    }
    else
    {
        // print server info
        sServerDescription ServerDescription;
        memset(&ServerDescription, 0, sizeof(ServerDescription));
        mClient->GetServerDescription(&ServerDescription);
        if(!ServerDescription.HostPresent)
        {
            printf("Unable to connect to server. Host not present. Exiting.");
            return 1;
        }
		printf("Connection to NatNet server successful :) \n");
		//printf("\t Server application info:\n");
        printf("\t Application info: %s (ver. %d.%d.%d.%d)\n", ServerDescription.szHostApp, ServerDescription.HostAppVersion[0],
            ServerDescription.HostAppVersion[1],ServerDescription.HostAppVersion[2],ServerDescription.HostAppVersion[3]);
        printf("\t NatNet Version: %d.%d.%d.%d\n", ServerDescription.NatNetVersion[0], ServerDescription.NatNetVersion[1],
            ServerDescription.NatNetVersion[2], ServerDescription.NatNetVersion[3]);
		printf("\t Client IP:%s\n", Optitrack::szMyIPAddress);
		printf("\t Server IP:%s\n", Optitrack::szServerIPAddress);
		//printf("Server IP:%s\n", szServerMulticast);

        printf("Server Name:%s\n\n", ServerDescription.szHostComputerName);
    }

	printf("NatNet Client properly initialized. \n");

	
 
    return ErrorCode_OK;

}

// DataHandler receives data from the server
void __cdecl Optitrack::DataHandler(sFrameOfMocapData* data, void* pUserData)
{

	//Optitrack::m_timer=Optitrack::p_mClock_optitrackLoop->getCPUTimeSeconds();
	Optitrack::m_timer=p_mClock_optitrackLoop->getCurrentTimeSeconds();
	//printf("Optitrack::m_timer: %g \n",Optitrack::m_timer);

	//memcpy(&optiData,data,sizeof(sFrameOfMocapData));
	//printf("DataHandler: got data \n");
	
	//printf("We are in DataHandler \n");
	//NatNetClient* pClient = (NatNetClient*) pUserData;

	////printf("Received frame %d\n", data->iFrame);
	//if(fp)
	//	_WriteFrame(fp,data);
	//int i=0;

 //   // same system latency test
 //   float fThisTick = (float)GetTickCount();
 //   float fDiff = fThisTick - data->fLatency;
 //   double dDuration = fDiff;
 //   //printf("Latency (same system) (msecs): %3.2lf\n", dDuration);


	//// Other Markers
	///*printf("Other Markers [Count=%d]\n", data->nOtherMarkers);
	//for(i=0; i < data->nOtherMarkers; i++)
	//{
	//	printf("Other Marker %d : %3.2f\t%3.2f\t%3.2f\n",
	//		i,
	//		data->OtherMarkers[i][0],
	//		data->OtherMarkers[i][1],
	//		data->OtherMarkers[i][2]);
	//}*/

	//// Rigid Bodies
	//printf("Rigid Bodies [Count=%d]\n", data->nRigidBodies);
	Optitrack::m_numRigidBodies=data->nRigidBodies;
	for(int i=0; i < data->nRigidBodies; i++)
	{

		if(i==0)
		{
			//Check proper tracking
			if( !( data->RigidBodies[i].x==0&&data->RigidBodies[i].y==0&&data->RigidBodies[i].z==0&&data->RigidBodies[i].qx==0&&data->RigidBodies[i].qy==0&&data->RigidBodies[i].qz==0&&data->RigidBodies[i].qw==0))
			{

				Optitrack::rb1.id=data->RigidBodies[i].ID;

				//Init, working, non filtered
				//if(OneEuroFilter::m_TimerTeleop<=0)//Init master, before loop -> no filtering
				//if(Optitrack::m_timer<=0)
				//{
					//Optitrack::rb1.id=data->RigidBodies[i].ID;
					//Optitrack::rb1.x=data->RigidBodies[i].x;
					//Optitrack::rb1.y=data->RigidBodies[i].y;
					//Optitrack::rb1.z=data->RigidBodies[i].z;
					//Optitrack::rb1.qx=data->RigidBodies[i].qx;
					//Optitrack::rb1.qy=data->RigidBodies[i].qy;
					//Optitrack::rb1.qz=data->RigidBodies[i].qz;
					//Optitrack::rb1.qw=data->RigidBodies[i].qw;
				//}
				//else
				//{

				////Test filtering
				//	//double x_raw=data->RigidBodies[i].x;
				//	//double y_raw=data->RigidBodies[i].y;
				//	//double z_raw=data->RigidBodies[i].z;
				//	//double qx_raw=data->RigidBodies[i].qx;
				//	//double qy_raw=data->RigidBodies[i].qy;
				//	//double qz_raw=data->RigidBodies[i].qz;
				//	//double qw_raw=data->RigidBodies[i].qw;

					Optitrack::x_raw=data->RigidBodies[i].x;
					Optitrack::y_raw=data->RigidBodies[i].y;
					Optitrack::z_raw=data->RigidBodies[i].z;
					Optitrack::qx_raw=data->RigidBodies[i].qx;
					Optitrack::qy_raw=data->RigidBodies[i].qy;
					Optitrack::qz_raw=data->RigidBodies[i].qz;
					Optitrack::qw_raw=data->RigidBodies[i].qw;

				//	//Filtered
					Optitrack::rb1.x  = m_filter_x.filter(x_raw, Optitrack::m_timer) ; //filtered values - OneEuroFilter::m_TimerTeleop
					Optitrack::rb1.y  = m_filter_y.filter(y_raw, Optitrack::m_timer) ; //filtered values
					Optitrack::rb1.z  = m_filter_z.filter(z_raw, Optitrack::m_timer) ; //filtered values
					Optitrack::rb1.qx  = m_filter_qx.filter(qx_raw, Optitrack::m_timer) ; //filtered values
					Optitrack::rb1.qy  = m_filter_qy.filter(qy_raw, Optitrack::m_timer) ; //filtered values
					Optitrack::rb1.qz  = m_filter_qz.filter(qz_raw, Optitrack::m_timer) ; //filtered values
					Optitrack::rb1.qw  = m_filter_qw.filter(qw_raw, Optitrack::m_timer) ; //filtered values
				//
				//}//else

				
//TEST

			//EulerAngles Eul_FromQuat(Quat q, int order)
			//typedef struct {float x, y, z, w;} Quat
			Quat q_loc;
			q_loc.x=rb1.qx;
			q_loc.y=rb1.qy;
			q_loc.z=rb1.qz;
			q_loc.w=rb1.qw;

			EulerAngles euler_loc;
			euler_loc=Eul_FromQuat(q_loc,EulOrdXYZr);
			//roll-yaw-pitch (same as Opti real time display)
			//printf("Euler: %f %f %f \n",NATUtils::RadiansToDegrees(euler_loc.x),NATUtils::RadiansToDegrees(euler_loc.y),NATUtils::RadiansToDegrees(euler_loc.z));

			HMatrix rot_fromEuler_loc;
			//roll_loc=euler_loc.x;
			//roll_loc=euler_loc.x;
			//roll_loc=euler_loc.x;

			Eul_ToHMatrix(euler_loc,rot_fromEuler_loc);
			//printf("%f %f %f \n %f %f %f \n %f %f %f \n\n",rot_fromEuler_loc[0][0],rot_fromEuler_loc[0][1],rot_fromEuler_loc[0][2],rot_fromEuler_loc[1][0],rot_fromEuler_loc[1][1],rot_fromEuler_loc[1][2],rot_fromEuler_loc[2][0],rot_fromEuler_loc[2][1],rot_fromEuler_loc[2][2]);
		
			double quatA_loc[4];
			quatA_loc[0]=rb1.qx;
			quatA_loc[1]=rb1.qy;
			quatA_loc[2]=rb1.qz;
			quatA_loc[3]=rb1.qw;

			CustomMatrix* p_rot_loc= MathUtils::QuatToRot(quatA_loc);
			//p_rot_loc->printf_Matrix();
			//printf("\n");

			//QuaternionToRotationMatrix(
//End test
		
			//printf("Rigid Body 1 [x y z qx qy qz qw]\t%3.2f\t%3.2f\t%3.2f\t%3.2f\t%3.2f\t%3.2f\t%3.2f\n",
			//Optitrack::rb1.x,
			//Optitrack::rb1.y,
			//Optitrack::rb1.z,
			//Optitrack::rb1.qx,
			//Optitrack::rb1.qy,
			//Optitrack::rb1.qz,
			//Optitrack::rb1.qw);

			}//proper tracking
			else
			{
				printf("[Optitrack] Wrong tracking, skip data point \n");
			}
		}
		if(i==1)
		{
			Optitrack::rb2.id=data->RigidBodies[i].ID;
			Optitrack::rb2.x=data->RigidBodies[i].x;
			Optitrack::rb2.y=data->RigidBodies[i].y;
			Optitrack::rb2.z=data->RigidBodies[i].z;
			Optitrack::rb2.qx=data->RigidBodies[i].qx;
			Optitrack::rb2.qy=data->RigidBodies[i].qy;
			Optitrack::rb2.qz=data->RigidBodies[i].qz;
			Optitrack::rb2.qw=data->RigidBodies[i].qw;

			/*printf("Rigid Body 2 [x y z qx qy qz qw]\t%3.2f\t%3.2f\t%3.2f\t%3.2f\t%3.2f\t%3.2f\t%3.2f\n",
			rb2.x,
			rb2.y,
			rb2.z,
			rb2.qx,
			rb2.qy,
			rb2.qz,
			rb2.qw);*/
		}
		if(i==2)
		{
			Optitrack::rb3.id=data->RigidBodies[i].ID;
			Optitrack::rb3.x=data->RigidBodies[i].x;
			Optitrack::rb3.y=data->RigidBodies[i].y;
			Optitrack::rb3.z=data->RigidBodies[i].z;
			Optitrack::rb3.qx=data->RigidBodies[i].qx;
			Optitrack::rb3.qy=data->RigidBodies[i].qy;
			Optitrack::rb3.qz=data->RigidBodies[i].qz;
			Optitrack::rb3.qw=data->RigidBodies[i].qw;


			/*printf("Rigid Body 3 [x y z qx qy qz qw]\t%3.2f\t%3.2f\t%3.2f\t%3.2f\t%3.2f\t%3.2f\t%3.2f\n",
			rb3.x,
			rb3.y,
			rb3.z,
			rb3.qx,
			rb3.qy,
			rb3.qz,
			rb3.qw);*/
		}

		//printf("Rigid Body [ID=%d  Error=%3.2f]\n", data->RigidBodies[i].ID, data->RigidBodies[i].MeanError);
		//printf("\tx\ty\tz\tqx\tqy\tqz\tqw\n");
		//printf("\t%3.2f\t%3.2f\t%3.2f\t%3.2f\t%3.2f\t%3.2f\t%3.2f\n",
		//	data->RigidBodies[i].x,
		//	data->RigidBodies[i].y,
		//	data->RigidBodies[i].z,
		//	data->RigidBodies[i].qx,
		//	data->RigidBodies[i].qy,
		//	data->RigidBodies[i].qz,
		//	data->RigidBodies[i].qw);

		/*printf("\tRigid body markers [Count=%d]\n", data->RigidBodies[i].nMarkers);
		for(int iMarker=0; iMarker < data->RigidBodies[i].nMarkers; iMarker++)
		{
            printf("\t\t");
            if(data->RigidBodies[i].MarkerIDs)
                printf("MarkerID:%d", data->RigidBodies[i].MarkerIDs[iMarker]);
            if(data->RigidBodies[i].MarkerSizes)
                printf("\tMarkerSize:%3.2f", data->RigidBodies[i].MarkerSizes[iMarker]);
            if(data->RigidBodies[i].Markers)
                printf("\tMarkerPos:%3.2f,%3.2f,%3.2f\n" ,
                    data->RigidBodies[i].Markers[iMarker][0],
                    data->RigidBodies[i].Markers[iMarker][1],
                    data->RigidBodies[i].Markers[iMarker][2]);
        }*/
	}

    // skeletons
    /*printf("Skeletons [Count=%d]\n", data->nSkeletons);
    for(i=0; i < data->nSkeletons; i++)
    {
        sSkeletonData skData = data->Skeletons[i];
        printf("Skeleton [ID=%d  Bone count=%d]\n", skData.skeletonID, skData.nRigidBodies);
        for(int j=0; j< skData.nRigidBodies; j++)
        {
            sRigidBodyData rbData = skData.RigidBodyData[j];
            printf("Bone %d\t%3.2f\t%3.2f\t%3.2f\t%3.2f\t%3.2f\t%3.2f\t%3.2f\n",
                    rbData.ID, rbData.x, rbData.y, rbData.z, rbData.qx, rbData.qy, rbData.qz, rbData.qw );
    
            printf("\tRigid body markers [Count=%d]\n", rbData.nMarkers);
            for(int iMarker=0; iMarker < data->RigidBodies[i].nMarkers; iMarker++)
            {
                printf("\t\t");
                if(rbData.MarkerIDs)
                    printf("MarkerID:%d", rbData.MarkerIDs[iMarker]);
                if(rbData.MarkerSizes)
                    printf("\tMarkerSize:%3.2f", rbData.MarkerSizes[iMarker]);
                if(rbData.Markers)
                    printf("\tMarkerPos:%3.2f,%3.2f,%3.2f\n" ,
                    data->RigidBodies[i].Markers[iMarker][0],
                    data->RigidBodies[i].Markers[iMarker][1],
                    data->RigidBodies[i].Markers[iMarker][2]);
            }
        }*/
    //}

}





//void Optitrack::resetClient()
//{
//	int iSuccess;
//
//	printf("\n\nre-setting Client\n\n.");
//
//	iSuccess = theClient->Uninitialize();
//	if(iSuccess != 0)
//		printf("error un-initting Client\n");
//
//	iSuccess = theClient->Initialize(szMyIPAddress, szServerIPAddress);
//	if(iSuccess != 0)
//		printf("error re-initting Client\n");
//
//}

//// MessageHandler receives NatNet error/debug messages
//void __cdecl Optitrack::MessageHandler(int msgType, char* msg)
//{
//	printf("\n%s\n", msg);
//}
//
///* File writing routines */
//void Optitrack::WriteHeader(FILE* fp, sDataDescriptions* pBodyDefs)
//{
//	int i=0;
//
//    if(!pBodyDefs->arrDataDescriptions[0].type == Descriptor_MarkerSet)
//        return;
//        
//	sMarkerSetDescription* pMS = pBodyDefs->arrDataDescriptions[0].Data.MarkerSetDescription;
//
//	fprintf(fp, "<MarkerSet>\n\n");
//	fprintf(fp, "<Name>\n%s\n</Name>\n\n", pMS->szName);
//
//	fprintf(fp, "<Markers>\n");
//	for(i=0; i < pMS->nMarkers; i++)
//	{
//		fprintf(fp, "%s\n", pMS->szMarkerNames[i]);
//	}
//	fprintf(fp, "</Markers>\n\n");
//
//	fprintf(fp, "<Data>\n");
//	fprintf(fp, "Frame#\t");
//	for(i=0; i < pMS->nMarkers; i++)
//	{
//		fprintf(fp, "M%dX\tM%dY\tM%dZ\t", i, i, i);
//	}
//	fprintf(fp,"\n");
//
//}
//
//void Optitrack::WriteFrame(FILE* fp, sFrameOfMocapData* data)
//{
//	fprintf(fp, "%d", data->iFrame);
//	for(int i =0; i < data->MocapData->nMarkers; i++)
//	{
//		fprintf(fp, "\t%.5f\t%.5f\t%.5f", data->MocapData->Markers[i][0], data->MocapData->Markers[i][1], data->MocapData->Markers[i][2]);
//	}
//	fprintf(fp, "\n");
//}
//
//void Optitrack::WriteFooter(FILE* fp)
//{
//	fprintf(fp, "</Data>\n\n");
//	fprintf(fp, "</MarkerSet>\n");
//}
