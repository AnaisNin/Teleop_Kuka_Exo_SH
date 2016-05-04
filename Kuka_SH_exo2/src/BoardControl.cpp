#include "BoardControl.h"
#include "EXOSBoardLibrary.h"

#define FIRST_LBODY_BOARD 0
#define LAST_LBODY_BOARD 2

//#define BASE_ADDRESS 70
//#define BASE_ADDRESS 20

//Initialization class static members
int BoardControl::activeBoards[MAX_BOARDS]={0};
int BoardControl::noActiveBoards=0;
JointData BoardControl::joint[1]={1,0,0,0,0,0,200,0,0,10000,10000,0,0,60,-60,0,0,0,0,0,0,0,3333,0,1600,16,1000,32000,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,PID_CONTROL,};

/******************************************/
void BoardControl::setBroadCastRate(int boardNumber, int BCastRate)
{
	int r=SetBCastRate(activeBoards[boardNumber], BCastRate);  // 1=0.5msec 
	//r=SetBCastRate(BoardControl::activeBoards[boardNumber], 2*BCastRate, TwinSatus);  // 1=0.5msec 
	if(r!=-1)
		printf("Board %d :Broadcast request sent succed, Rate:%dHz",activeBoards[boardNumber],(int)(1000.0/(float)BCastRate));
	else
		printf("Board %d :Broadcast request sent failed!, Rate:%dHz",activeBoards[boardNumber],(int)(1000.0/(float)BCastRate));
}

/******************************************/
void BoardControl::setBroadCastPolicy(int boardNumber, int BCastPolicy)
{
	int r=SetBCastPolicy(activeBoards[boardNumber], BCastPolicy);
	if(r!=-1)
		printf("Board %d :Set Broad Cast Policy succeded\n",activeBoards[boardNumber]);
	else
		printf("Board %d :Get Broad Cast Policy failed! Error:%d\n",activeBoards[boardNumber],r);
}

/******************************************/
int BoardControl::connectUDP()
/******************************************/
{
	//printf("Connect UDP.. \n");
	int r=ConnectUDP();
	if(r<0)
		printf("\t FAILED to create UDP socket, Error:%d [BoardControl::connectUDP()]\n",r);	
	else
		printf("\t UDP socket created, SocketID:%d [BoardControl::connectUDP()]\n",r);
	
	//printf("..done. \n");
	printf("\n");
	return r;
}

/******************************************/
void BoardControl::connectTCP()
/******************************************/
{
	printf("Creating TCP Connection....\n");
	for(int b=FIRST_LBODY_BOARD;b<MAX_BOARDS;b++)
	{
		if(activeBoards[b]>0){
			int r=0;
			if((r=ConnectTCP(activeBoards[b]))>0){
				//printf("\t BoardID:%d ->TCP Connection established[IP:169.254.89.%d, SocketID:%d ]\n", activeBoards[b],BASE_ADDRESS+BoardControl::activeBoards[b],r);
				//printf("\t BoardID:%d ->TCP Connection established[IP:xxx.xxx.xxx.%d, SocketID:%d ]\n", activeBoards[b],BASE_ADDRESS+BoardControl::activeBoards[b],r);
				printf("\t BoardID:%d ->TCP Connection established ; socket id=%d \n", activeBoards[b],r);
				BoardControl::joint[b].isConnected=1;
			}
			else{
				//printf("\t BoardID:%d ->TCP Connection failed[IP:169.254.89.%d, SocketID:%d ]\n", activeBoards[b],BASE_ADDRESS+BoardControl::activeBoards[b],r);
				//printf("\t BoardID:%d ->TCP Connection failed[IP:xxx.xxx.xxx.%d, SocketID:%d ]\n", activeBoards[b],BASE_ADDRESS+BoardControl::activeBoards[b],r);
				printf("\t BoardID:%d ->TCP Connection FAILED ; socket id=%d \n", activeBoards[b],r);
				BoardControl::joint[b].isConnected=0;

				//printf("Board id %i rejected \n",b);
				Sleep(5000);

				//TO UNCOMMENT
				//printf("Program should exit now \n");
				exit(0);
			}
		}
	}
	printf(".. done. \n");
	printf("\n");
}

/******************************************/
int BoardControl::scanSystem()
/******************************************/
{
	printf("Scan boards.. \n");
	int r=0;
	if((r=ScanForActiveBoards())>0){
		BoardControl::noActiveBoards=r;
		GetActiveBoards(BoardControl::activeBoards);
	}
	else if( r==-1)
		printf("\t Scanning broadcast request: Sent failed!\n");
	else if(r==-2)
		printf("\t Broadcast packet read failed!\n");
	else if(r==-3)
		printf("\t Scan completed: No active boards found!\n");
	else{
		printf("\t Scan completed: %d boards found \n", r);
		//for(int b=FIRST_LBODY_BOARD;b<LAST_LBODY_BOARD;b++)
		for(int b=FIRST_LBODY_BOARD;b<MAX_BOARDS;b++)			
		{
			//printf("b: %i \n",b);
			if(BoardControl::activeBoards[b]>0)
				//printf("\t BoardID:%d found at [IP:169.254.89.%d]\n", BoardControl::activeBoards[b],BASE_ADDRESS+BoardControl::activeBoards[b]);
				//printf("\t BoardID:%d found at [IP:xxx.xxx.xxx.%d]\n", BoardControl::activeBoards[b],BASE_ADDRESS+BoardControl::activeBoards[b]);
				printf("\t BoardID:%d found \n", BoardControl::activeBoards[b]);
			//else
			//	printf("This board in not active \n");
		}
	}
	printf("..done. \n");
	printf("\n");
	return r;
}



void BoardControl::stopBroadCast(int BoardNumber)
{
	int r=StopBCast(BoardControl::activeBoards[BoardNumber]);
	if(r!=-1)
		printf("Board %d :Stop Broadcast request sent succed\n",BoardControl::activeBoards[BoardNumber]);
	else
		printf("Board %d :Stop Broadcast request sent failed!\n",BoardControl::activeBoards[BoardNumber]);
}

void BoardControl::startTorqueControl(int BoardNumber, int torqueFlag)
{
	int B;
	int t;
	B=BoardNumber;
	t=torqueFlag;


	int r=SetTorqueOnOff(B,t);

	if(r!=-1)
		printf("Board %d :Start Position Control succeded, %d\n",activeBoards[B],r);
	else
		printf("Board %d :Start Position Control failed! Error:%d\n",activeBoards[B],r);

	printf("\n");
}

/******************************************/
void BoardControl::startPositionControl()
/******************************************/
{
	for(int b=FIRST_LBODY_BOARD;b<LAST_LBODY_BOARD;b++)
		if(BoardControl::activeBoards[b]>0){
			int r=StartBoardPositionControl(BoardControl::activeBoards[b]);
			if(r!=-1)
				printf("Board %d :Start Position Control succeded, %d\n",BoardControl::activeBoards[b],r);
			else
				printf("Board %d :Start Position Control failed! Error:%d\n",BoardControl::activeBoards[b],r);
		}
		printf("\n");
}


void BoardControl::stopPositionControl()
{
	int r=StopPositionControl();
	if(r!=-1)
		printf("All Active Boards:Stop Position Control succeded\n");
	else
		printf("All Active Boards:Stop Position Control failed! Error:%d\n",r);
	printf("\n");
}


//void BoardControl::loadJointDataFromDrivers()
//{
//	long temp;
//	long r[3];
//
//	for(int b=FIRST_LBODY_BOARD;b<LAST_LBODY_BOARD;b++){
//		if(joint[b].isConnected){
//			temp=GetPosition(activeBoards[b]);
//			joint[b].pos=temp;
//			GetVelocity(b);
//			joint[b].vel=temp;
//			GetTorque(b);
//			joint[b].tor=temp;
//			temp=GetMinPosition(activeBoards[b]);
//			joint[b].motorEncLimit1=temp;
//			temp=GetMaxPosition(activeBoards[b]);
//			joint[b].motorEncLimit2=temp;
//			temp=GetMinVelocity(activeBoards[b]);
//			joint[b].minvel=temp;
//			temp=GetMaxVelocity(activeBoards[b]);
//			joint[b].maxvel=temp;
//			temp=GetCurrentLimit(activeBoards[b]);
//			joint[b].current=temp;
//			temp=GetPidGains(activeBoards[b],1,r);
//			joint[b].pgain=r[0];
//			joint[b].igain=r[1];
//			joint[b].dgain=r[2];
//			temp=GetPidGainScale(activeBoards[b],1,r);
//			joint[b].gainscale=r[0];
//			temp=GetILimGain(activeBoards[b],1);
//			joint[b].ilimit=temp;
//			temp=GetPidOffset(activeBoards[b]);
//			joint[b].offset=temp;
//
//			printf("b%d,p:%d,minp:%d,maxp:%d,mc:%d,(p:%d,i:%d,d:%d, il:%d,o:%d)\n",
//				BoardControl::activeBoards[b],joint[b].pos,joint[b].motorEncLimit1,joint[b].motorEncLimit2,joint[b].current,
//				joint[b].pgain,joint[b].igain,joint[b].dgain,joint[b].ilimit,joint[b].offset);
//
//		}
//	}
//	printf("\n");
//}


//void BoardControl::setControllerGains()
//{
	//posPID[0]=0;
	//posPID[1]=0;
	//posPID[2]=0;

	//torPID[0]=0;
	//torPID[1]=0;
	//torPID[2]=0;

	//// sprintf_s is changing gains (long datatypes) to char types for SetPidGains function variable char * gains. 
	//sprintf_s(pidGains, sizeof(pidGains)/sizeof(char), "%d,%d,%d", gains[0],gains[1],gains[2]); 
	//temp=SetPidGains(2,1,pidGains);

	//GetPidGains(1,1,gains);
	//gotoxy(1,20); printf("p=%d, i=%d, d=%d \n",gains[0],gains[1],gains[2]);
//}

//int BoardControl::getBroadCastData()
//{

	//int bytesReceived=0;
	//int boardNo;
	//int pktRead=0;
	//int PcCnt=0;

	//for(int pkt=0;pkt<noActiveBoards+1;pkt++)
	//	if((bytesReceived=GetBCastData(packetToReceive))>0)
	//	{
	//		if(packetToReceive.content[2] == (char)0xbb)   // this is a boradcast packet
	//		{
	//			PcCnt=3;
	//			boardNo=packetToReceive.content[3];			

	//			if (boardNo-1==0)// This reads the damper motor board which has a modified protocol
	//			{
	//				if (BCAST_POLICY&0x1){//check if bit 0 of the BCAST_POLICY is asserted
	//					joint[boardNo-1].pos=(unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+2]<<8)& 0xff00) 
	//						+((packetToReceive.content[PcCnt+3]<<16)& 0xff0000)+((packetToReceive.content[PcCnt+4]<<24)& 0xff000000);
	//					PcCnt=PcCnt+4;
	//				}
	//				if (BCAST_POLICY&0x2){//check if bit 1 of the BCAST_POLICY is asserted 
	//					joint[boardNo-1].DamperRefNormForce = (unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+2]<<8)& 0xff00);
	//					PcCnt=PcCnt+2;
	//				}
	//				if (BCAST_POLICY&0x4){//check if bit 2 of the BCAST_POLICY is asserted
	//					joint[boardNo-1].DamperRefTorque = (unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+1]<<8)& 0xff00);
	//					PcCnt=PcCnt+2;
	//				}
	//				if (BCAST_POLICY&0x8){//check if bit 3 of the BCAST_POLICY is asserted
	//					joint[boardNo-1].pidOutput = (unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+1]<<8)& 0xff00);
	//					PcCnt=PcCnt+2;
	//				}
	//				if (BCAST_POLICY&0x10){//check if bit 4 of the BCAST_POLICY is asserted
	//					joint[boardNo-1].DamperRefDeflection =(unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+2]<<8)& 0xff00) 
	//						+((packetToReceive.content[PcCnt+3]<<16)& 0xff0000)+((packetToReceive.content[PcCnt+4]<<24)& 0xff000000);
	//					PcCnt=PcCnt+4;
	//				}
	//				if (BCAST_POLICY&0x20){//check if bit 5 of the BCAST_POLICY is asserted
	//					joint[boardNo-1].DamperActDeflection =(unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+2]<<8)& 0xff00) 
	//						+((packetToReceive.content[PcCnt+3]<<16)& 0xff0000)+((packetToReceive.content[PcCnt+4]<<24)& 0xff000000);
	//					PcCnt=PcCnt+4;
	//				}
	//				if (BCAST_POLICY&0x40){//check if bit 6 of the BCAST_POLICY is asserted
	//					joint[boardNo-1].temp_Vdc=(unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+2]<<8)& 0xff00) 
	//						+((packetToReceive.content[PcCnt+3]<<16)& 0xff0000)+((packetToReceive.content[PcCnt+4]<<24)& 0xff000000);
	//					PcCnt=PcCnt+4;
	//				}
	//				if (BCAST_POLICY&0x80){//check if bit 7 of the BCAST_POLICY is asserted
	//					joint[boardNo-1].tStamp =(unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+2]<<8)& 0xff00) 
	//						+((packetToReceive.content[PcCnt+3]<<16)& 0xff0000)+((packetToReceive.content[PcCnt+4]<<24)& 0xff000000);
	//					PcCnt=PcCnt+4;
	//				}
	//				if (BCAST_POLICY&0x100){//check if bit 8 of the BCAST_POLICY is asserted
	//					joint[boardNo-1].fault = (unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+1]<<8)& 0xff00);
	//					PcCnt=PcCnt+2;
	//				}
	//				//Construct analogue input selection
	//				int AB=2*((BCAST_POLICY&0x400)>>10)+((BCAST_POLICY&0x200)>>9);
	//				switch (AB)
	//				{
	//				case 0:
	//					break;
	//				case 1:
	//					break;
	//				case 2:
	//					break;
	//				case 3:
	//					{
	//						joint[boardNo-1].anInput1= ((unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+2]<<8)& 0xff00));
	//						PcCnt=PcCnt+2;
	//						joint[boardNo-1].anInput2= ((unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+2]<<8)& 0xff00));
	//						PcCnt=PcCnt+2;

	//						// anInput3 and anInput4 receive force and torque in decaN and mNm 
	//						//for fitting  positive and negative forces. Force up to 327.67N with a resolution of 10mN. Torque up to 32.767Nm with a resolution of 1mNm.
	//						if(((packetToReceive.content[PcCnt+2])& 0x80)>0) joint[boardNo-1].anInput3= ((unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+2]<<8)& 0xff00))-0xffff;
	//						else joint[boardNo-1].anInput3= ((unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+2]<<8)& 0xff00));
	//						joint[boardNo-1].Force=joint[boardNo-1].anInput3/10.0;// convert to Newtons from decaNewtons
	//						PcCnt=PcCnt+2;
	//						// decaN and mNm for fitting  positive and negative forces up to 327.67N with a resolution of 10mN.
	//						if(((packetToReceive.content[PcCnt+2])& 0x80)>0) joint[boardNo-1].anInput4= ((unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+2]<<8)& 0xff00))-0xffff;
	//						else joint[boardNo-1].anInput4= ((unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+2]<<8)& 0xff00));
	//						joint[boardNo-1].Torque=joint[boardNo-1].anInput4/1000.0;// convert to Nm from mNm
	//						PcCnt=PcCnt+2;
	//					}
	//					break;
	//				}			
	//				if (EXTRA_BCAST_POLICY&0x1){//check if bit 0 of the EXTRA_BCAST_POLICY is asserted
	//					PcCnt=PcCnt+4;
	//				}
	//				if (EXTRA_BCAST_POLICY&0x2){//check if bit 1 of the EXTRA_BCAST_POLICY is asserted 						
	//					PcCnt=PcCnt+4;
	//				}
	//				if (EXTRA_BCAST_POLICY&0x4){//check if bit 2 of the EXTRA_BCAST_POLICY is asserted
	//					PcCnt=PcCnt+4;
	//				}
	//				if (EXTRA_BCAST_POLICY&0x8){//check if bit 3 of the EXTRA_BCAST_POLICY is asserted
	//					PcCnt=PcCnt+4;
	//				}
	//				if (EXTRA_BCAST_POLICY&0x10){//check if bit 4 of the EXTRA_BCAST_POLICY is asserted
	//					PcCnt=PcCnt+2;
	//				}
	//				if (EXTRA_BCAST_POLICY&0x20){//check if bit 5 of the EXTRA_BCAST_POLICY is asserted
	//					PcCnt=PcCnt+2;
	//				}
	//				if (EXTRA_BCAST_POLICY&0x40){//check if bit 6 of the EXTRA_BCAST_POLICY is asserted
	//					joint[boardNo-1].TwinActPos= (unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+2]<<8)& 0xff00) 
	//						+((packetToReceive.content[PcCnt+3]<<16)& 0xff0000)+((packetToReceive.content[PcCnt+4]<<24)& 0xff000000);
	//					PcCnt=PcCnt+4;
	//				}
	//				if (EXTRA_BCAST_POLICY&0x80){//check if bit 7 of the EXTRA_BCAST_POLICY is asserted
	//					joint[boardNo-1].TwinTargPos= (unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+2]<<8)& 0xff00) 
	//						+((packetToReceive.content[PcCnt+3]<<16)& 0xff0000)+((packetToReceive.content[PcCnt+4]<<24)& 0xff000000);
	//					PcCnt=PcCnt+4;
	//				}
	//				if (EXTRA_BCAST_POLICY&0x100){//check if bit 8 of the EXTRA_BCAST_POLICY is asserted
	//					joint[boardNo-1].TwinVel= (unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+2]<<8)& 0xff00);
	//					PcCnt=PcCnt+2;
	//				}	
	//				if (EXTRA_BCAST_POLICY&0x200){//check if bit 9 of the EXTRA_BCAST_POLICY is asserted
	//					if(((packetToReceive.content[PcCnt+2])& 0x80)>0) joint[boardNo-1].Xaccleration = ((unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+2]<<8)& 0xff00))-0xffff;
	//					else joint[boardNo-1].Xaccleration = ((unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+2]<<8)& 0xff00));
	//					PcCnt=PcCnt+2;
	//				}	
	//				if (EXTRA_BCAST_POLICY&0x400){//check if bit 10 of the EXTRA_BCAST_POLICY is asserted
	//					if(((packetToReceive.content[PcCnt+2])& 0x80)>0) joint[boardNo-1].Yaccleration = ((unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+2]<<8)& 0xff00))-0xffff;
	//					else joint[boardNo-1].Yaccleration = ((unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+2]<<8)& 0xff00));
	//					PcCnt=PcCnt+2;
	//				}	
	//				if (EXTRA_BCAST_POLICY&0x800){//check if bit 11 of the EXTRA_BCAST_POLICY is asserted
	//					if(((packetToReceive.content[PcCnt+2])& 0x80)>0) joint[boardNo-1].Zaccleration = ((unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+2]<<8)& 0xff00))-0xffff;
	//					else joint[boardNo-1].Zaccleration = ((unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+2]<<8)& 0xff00));
	//					PcCnt=PcCnt+2;
	//				}					
	//			}
	//			else      //This reads the main motor board
	//			{
	//				if (BCAST_POLICY&0x1){//check if bit 0 of the BCAST_POLICY is asserted
	//					joint[boardNo-1].pos=(unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+2]<<8)& 0xff00) 
	//						+((packetToReceive.content[PcCnt+3]<<16)& 0xff0000)+((packetToReceive.content[PcCnt+4]<<24)& 0xff000000);
	//					PcCnt=PcCnt+4;
	//				}
	//				if (BCAST_POLICY&0x2){//check if bit 1 of the BCAST_POLICY is asserted
	//					if(((packetToReceive.content[PcCnt+2])& 0x80)>0) joint[boardNo-1].vel= ((unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+2]<<8)& 0xff00))-0xffff;
	//					else joint[boardNo-1].vel= ((unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+2]<<8)& 0xff00));
	//					PcCnt=PcCnt+2;
	//				}
	//				if (BCAST_POLICY&0x4){//check if bit 2 of the BCAST_POLICY is asserted
	//					joint[boardNo-1].tor= packetToReceive.content[PcCnt+1]+(packetToReceive.content[PcCnt+2]*256);
	//					PcCnt=PcCnt+2;
	//				}				
	//				if (BCAST_POLICY&0x8){//check if bit 3 of the BCAST_POLICY is asserted
	//					joint[boardNo-1].pidOutput = (unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+1]<<8)& 0xff00);
	//					PcCnt=PcCnt+2;
	//				}
	//				if (BCAST_POLICY&0x10){//check if bit 4 of the BCAST_POLICY is asserted
	//					joint[boardNo-1].pidError=(unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+2]<<8)& 0xff00) 
	//						+((packetToReceive.content[PcCnt+3]<<16)& 0xff0000)+((packetToReceive.content[PcCnt+4]<<24)& 0xff000000);
	//					PcCnt=PcCnt+4;
	//				}
	//				if (BCAST_POLICY&0x20){//check if bit 5 of the BCAST_POLICY is asserted
	//					if (((packetToReceive.content[PcCnt+4])& 0x80)>0) 
	//						joint[boardNo-1].realCurrent=(unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+2]<<8)& 0xff00) 
	//						+((packetToReceive.content[PcCnt+3]<<16)& 0xff0000)+((packetToReceive.content[PcCnt+4]<<24)& 0xff000000)-0xffff;
	//					else
	//						joint[boardNo-1].realCurrent=(unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+2]<<8)& 0xff00) 
	//						+((packetToReceive.content[PcCnt+3]<<16)& 0xff0000)+((packetToReceive.content[PcCnt+4]<<24)& 0xff000000);
	//					PcCnt=PcCnt+4;
	//				}
	//				if (BCAST_POLICY&0x40){//check if bit 6 of the BCAST_POLICY is asserted
	//					joint[boardNo-1].temp_Vdc=(unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+2]<<8)& 0xff00) 
	//						+((packetToReceive.content[PcCnt+3]<<16)& 0xff0000)+((packetToReceive.content[PcCnt+4]<<24)& 0xff000000);
	//					PcCnt=PcCnt+4;
	//				}
	//				if (BCAST_POLICY&0x80){//check if bit 7 of the BCAST_POLICY is asserted
	//					joint[boardNo-1].tStamp =(unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+2]<<8)& 0xff00) 
	//						+((packetToReceive.content[PcCnt+3]<<16)& 0xff0000)+((packetToReceive.content[PcCnt+4]<<24)& 0xff000000);
	//					PcCnt=PcCnt+4;
	//				}
	//				if (BCAST_POLICY&0x100){//check if bit 8 of the BCAST_POLICY is asserted
	//					joint[boardNo-1].fault = (unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+1]<<8)& 0xff00);
	//					PcCnt=PcCnt+2;
	//				}
	//				//Construct analogue input selection
	//				int AB=2*((BCAST_POLICY&0x400)>>10)+((BCAST_POLICY&0x200)>>9);
	//				switch (AB)
	//				{
	//				case 0:
	//					break;
	//				case 1:
	//					break;
	//				case 2:
	//					break;
	//				case 3:
	//					{
	//						joint[boardNo-1].anInput1= ((unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+2]<<8)& 0xff00));
	//						PcCnt=PcCnt+2;
	//						joint[boardNo-1].anInput2= ((unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+2]<<8)& 0xff00));
	//						PcCnt=PcCnt+2;

	//						// anInput3 and anInput4 receive force and torque in decaN and mNm 
	//						//for fitting  positive and negative forces. Force up to 327.67N with a resolution of 10mN. Torque up to 32.767Nm with a resolution of 1mNm.
	//						if(((packetToReceive.content[PcCnt+2])& 0x80)>0) joint[boardNo-1].anInput3= ((unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+2]<<8)& 0xff00))-0xffff;
	//						else joint[boardNo-1].anInput3= ((unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+2]<<8)& 0xff00));
	//						joint[boardNo-1].Force=joint[boardNo-1].anInput3/10.0;// convert to Newtons from decaNewtons
	//						PcCnt=PcCnt+2;
	//						// decaN and mNm for fitting  positive and negative forces up to 327.67N with a resolution of 10mN.
	//						if(((packetToReceive.content[PcCnt+2])& 0x80)>0) joint[boardNo-1].anInput4= ((unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+2]<<8)& 0xff00))-0xffff;
	//						else joint[boardNo-1].anInput4= ((unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+2]<<8)& 0xff00));
	//						joint[boardNo-1].Torque=joint[boardNo-1].anInput4/1000.0;// convert to Nm from mNm
	//						PcCnt=PcCnt+2;
	//					}
	//					break;
	//				}
	//				if (EXTRA_BCAST_POLICY&0x1){//check if bit 0 of the EXTRA_BCAST_POLICY is asserted
	//					PcCnt=PcCnt+4;
	//				}
	//				if (EXTRA_BCAST_POLICY&0x2){//check if bit 1 of the EXTRA_BCAST_POLICY is asserted 						
	//					PcCnt=PcCnt+4;
	//				}
	//				if (EXTRA_BCAST_POLICY&0x4){//check if bit 2 of the EXTRA_BCAST_POLICY is asserted
	//					PcCnt=PcCnt+4;
	//				}
	//				if (EXTRA_BCAST_POLICY&0x8){//check if bit 3 of the EXTRA_BCAST_POLICY is asserted
	//					PcCnt=PcCnt+4;
	//				}
	//				if (EXTRA_BCAST_POLICY&0x10){//check if bit 4 of the EXTRA_BCAST_POLICY is asserted
	//					PcCnt=PcCnt+2;
	//				}
	//				if (EXTRA_BCAST_POLICY&0x20){//check if bit 5 of the EXTRA_BCAST_POLICY is asserted
	//					PcCnt=PcCnt+2;
	//				}
	//				if (EXTRA_BCAST_POLICY&0x40){//check if bit 6 of the EXTRA_BCAST_POLICY is asserted
	//					joint[boardNo-1].TwinActPos= (unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+2]<<8)& 0xff00) 
	//						+((packetToReceive.content[PcCnt+3]<<16)& 0xff0000)+((packetToReceive.content[PcCnt+4]<<24)& 0xff000000);
	//					PcCnt=PcCnt+4;
	//				}
	//				if (EXTRA_BCAST_POLICY&0x80){//check if bit 7 of the EXTRA_BCAST_POLICY is asserted
	//					joint[boardNo-1].TwinTargPos= (unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+2]<<8)& 0xff00) 
	//						+((packetToReceive.content[PcCnt+3]<<16)& 0xff0000)+((packetToReceive.content[PcCnt+4]<<24)& 0xff000000);
	//					PcCnt=PcCnt+4;
	//				}
	//				if (EXTRA_BCAST_POLICY&0x100){//check if bit 8 of the EXTRA_BCAST_POLICY is asserted
	//					joint[boardNo-1].TwinVel= (unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+2]<<8)& 0xff00);
	//					PcCnt=PcCnt+2;
	//				}	
	//				if (EXTRA_BCAST_POLICY&0x200){//check if bit 9 of the EXTRA_BCAST_POLICY is asserted
	//					if(((packetToReceive.content[PcCnt+2])& 0x80)>0) joint[boardNo-1].Xaccleration = ((unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+2]<<8)& 0xff00))-0xffff;
	//					else joint[boardNo-1].Xaccleration = ((unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+2]<<8)& 0xff00));
	//					PcCnt=PcCnt+2;
	//				}	
	//				if (EXTRA_BCAST_POLICY&0x400){//check if bit 10 of the EXTRA_BCAST_POLICY is asserted
	//					if(((packetToReceive.content[PcCnt+2])& 0x80)>0) joint[boardNo-1].Yaccleration = ((unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+2]<<8)& 0xff00))-0xffff;
	//					else joint[boardNo-1].Yaccleration = ((unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+2]<<8)& 0xff00));
	//					PcCnt=PcCnt+2;
	//				}	
	//				if (EXTRA_BCAST_POLICY&0x800){//check if bit 11 of the EXTRA_BCAST_POLICY is asserted
	//					if(((packetToReceive.content[PcCnt+2])& 0x80)>0) joint[boardNo-1].Zaccleration = ((unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+2]<<8)& 0xff00))-0xffff;
	//					else joint[boardNo-1].Zaccleration = ((unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+2]<<8)& 0xff00));
	//					PcCnt=PcCnt+2;
	//				}
	//			}

	//		}
	//		bytesReceived=0;
	//	}	
	//	if((desTor& 0x8000)>0) desTor=desTor -0xffff;

	//	//	//---------------Calculation of force from the Calibration matrix------------------------------------------------------
	//	//	
	//	//	//The next two lines use the calibration matris to generate the forcein N and the torque in Nm from the raw data in counts
	//	//	//works only when the incoming values are in counts.  the firmware is now changed for anInput3 and anInput4 to send
	//	//	// decaN and mNm for fitting  positive and negative forces up to 327.67N with a resolution of 10mN.
	//	joint[0].anInput1_Adjusted=joint[0].anInput1-joint[0].anInput1_Offset;
	//	joint[0].anInput2_Adjusted=joint[0].anInput2-joint[0].anInput2_Offset;

	//	joint[0].Force2= joint[0].anInput1_Adjusted*Cmatrix[0][0] + joint[0].anInput2_Adjusted*Cmatrix[1][0];
	//	joint[0].Torque2= joint[0].anInput1_Adjusted*Cmatrix[0][1] + joint[0].anInput2_Adjusted*Cmatrix[1][1];
	//	//	
	//	//	//---------------END of Calculation of force from the Calibration matrix-------------------------------------------------
	//	return 0;
//}