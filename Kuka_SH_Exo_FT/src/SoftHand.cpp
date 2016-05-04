//#include "stdafx.h"
#include "SoftHand.h"


void SoftHand::Setup()
{
	printf("Setup SH.. \n");
	int res;
	SoftHand::InitVars();
	
	CloseSockets_SH();
	r=InitLibrary_SH(MAX_BOARDS_SH);

	res = ConnectUDP_SH();

	if ((res = SoftHand::ScanSystem())<0)
	{
		Sleep(2000);
		//exit(0);
	}

	SoftHand::BoardConnectTCP(); // was connectTCP

	SoftHand::SetBroadCastRate(0, BCAST_RATE_SH);    //Main Motor Board
	SoftHand::SetBroadCastPolicy(0, BCAST_POLICY_SH);
	

	//int mtype = SetMotorType(1, 0x03);

	long gains[3];
	char* Cgains;
	char pidGains[20];

	gains[0]=4000;
	gains[1]=0;
	gains[2]=0;
	sprintf_s(pidGains, sizeof(pidGains)/sizeof(char), "%d,%d,%d", gains[0],gains[1],gains[2]);

	printf("done: %d: \n", SetPidGains_SH(1, 1, pidGains));

	GetPidGains_SH(1, 1, gains);
	printf("SH Pos Gains: p=%d, i=%d, d=%d \n", gains[0], gains[1], gains[2]);
	
	Sleep(250);

	SetAnalogInputs_SH(1, 6);

	Sleep(500);
	for(int it=0;it<MAX_BOARDS_SH;it++)
	{
		SoftHand::desVel[it]=5000;//8000 - max: about 10000
	}
	
	int retVal=SetDesiredVelocity_SH(SoftHand::desVel);


	SoftHand::StartPosControl();

	Sleep(500);

	SoftHand::GetSHBroadCastData();
	mainOffset = rawPos;

	SoftHand::SetDesiredPos(rawPos);

	printf("SoftHand mainOffset is  %d\n", mainOffset);

	//startThread();
}

void SoftHand::InitVars()
{
	noActiveBoards = 0;
	b = 0;
	r = 0;
	normalizedPos = -1;
	mainOffset = 0;

	for (int i = 0; i < MAX_BOARDS_SH; i++)
	{
		activeBoards[i] = 0;
		desPos[i] = 0;
		desVel[i] = 0;
	}

	//running = false;
}

//bool SoftHand::IsRunning()
//{
//	return running;
//}

void SoftHand::CloseConnection()
{
	//if(running)
	//	waitForThread(true);

	// open the hand before stopping position control and closing connection
	SoftHand::Open();

	SoftHand::StopPosControl();

	for (int b = FIRST_LBODY_BOARD; b<LAST_LBODY_BOARD; b++)
		SoftHand::StopBroadCast(b);
	Sleep(500);
	CloseSockets_SH();
	Sleep(500);
}

void SoftHand::SetDesiredPos(long _desPos)
{
	//lock();
	desPos[0] = Clamp(_desPos, max(mainOffset, HAND_MIN), HAND_MAX);
	//printf("SH setting des pos %i\n",desPos[0]);
	//unlock();
}

// should we put some control on the speed here???
// maybe just checking what the desired value is, and if too big just put a for and slowly move there...
int SoftHand::ApplyDesiredPosition()
{
	int ret;
	//lock();
	//printf("Applying des pos SH: %i  [SoftHand.cpp]\n",desPos[0]);
		SetDesiredPosition_SH(desPos);
	//unlock();

	//return ret;
	return 0;
}

void SoftHand::SetDesiredNormalizedPos(float _desPos)
{
	if (mainOffset != 0)
	{
		float des = Map(Clamp(_desPos, 0.0f, 1.0f), 0.0f, 1.0f, mainOffset, HAND_MAX);
		SoftHand::SetDesiredPos(des);
	}
}

void SoftHand::Open()
{
	//if (!running)
	//{
		int diff = rawPos - max(mainOffset, HAND_MIN);
		int startVal = rawPos;
		int incr = diff*0.0005;


		if (diff < 1000)
			return;

		for (int i = 0; i < diff; i += incr)
		{
			SoftHand::GetSHBroadCastData();
			SoftHand::SetDesiredPos(startVal - i);
			SoftHand::ApplyDesiredPosition();
		}
	//}
}

void SoftHand::Close()
{
	//if (!running)
	//{
		int diff = HAND_MAX - rawPos;
		int startVal = rawPos;
		int incr = diff*0.0005;

		//printf("SH: raw pos: %i, diff: %i \n",rawPos,diff);

		for (int i = 0; i < diff; i += incr)
		{
			SoftHand::GetSHBroadCastData();
			SoftHand::SetDesiredPos(startVal + i);
			SoftHand::ApplyDesiredPosition();
		}
	//}
}

int SoftHand::ScanSystem()
/******************************************/
{
	int r = 0;
	if ((r = ScanForActiveBoards_SH())>0)
	{
		noActiveBoards = r;
		GetActiveBoards_SH(activeBoards);
	}
	if (r == -1)
		printf("Scanning broadcast request: Sent failed!\n");
	else if (r == -2)
		printf("Broadcast packet read failed!\n");
	else if (r == -3)
		printf("Scan completed: No active boards found!\n");
	else
	{
		printf("Scan completed: (%d) Boards found !\n", r);
		for (b = FIRST_LBODY_BOARD; b<LAST_LBODY_BOARD; b++)
			if (activeBoards[b]>0)
				printf("BoardID:%d found at [IP:169.254.89.%d]\n", activeBoards[b], 70 + activeBoards[b]);
	}
	printf("\n");
	return r;
}

void SoftHand::BoardConnectTCP()
/******************************************/
{
	printf("Creating TCP Connection....\n");
	for (b = FIRST_LBODY_BOARD; b<MAX_BOARDS_SH; b++)
		if (activeBoards[b]>0)
		{
			if ((r = ConnectTCP_SH(activeBoards[b]))>0)
			{
				printf("BoardID:%d ->TCP Connection established[IP:169.254.89.%d, SocketID:%d ]\n", activeBoards[b], 70 + activeBoards[b], r);
				joint[b].isConnected = 1;
			}
			else
			{
				printf("BoardID:%d ->TCP Connection failed[IP:169.254.89.%d, SocketID:%d ]\n", activeBoards[b], 70 + activeBoards[b], r);
				joint[b].isConnected = 0;
				Sleep(5000);
				exit(0);
			}
		}
	printf("\n");
}

void SoftHand::SetBroadCastRate(int boardNumber, int BCastRate)
/******************************************/
{
	r = SetBCastRate_SH(activeBoards[boardNumber], BCastRate);  // 1=0.5msec 
															 //r=SetBCastRate(activeBoards[boardNumber], 2*BCastRate, TwinSatus);  // 1=0.5msec 
	if (r != -1)
		printf("Board %d :Broadcast request sent succed, Rate:%dHz\n", activeBoards[boardNumber], (int)(1000.0 / (float)BCastRate));
	else
		printf("Board %d :Broadcast request sent failed!, Rate:%dHz\n", activeBoards[boardNumber], (int)(1000.0 / (float)BCastRate));
}


/******************************************/
void SoftHand::SetBroadCastPolicy(int boardNumber, int BCastPolicy)
/******************************************/
{
	r = SetBCastPolicy_SH(activeBoards[boardNumber], BCastPolicy);
	if (r != -1)
		printf("Board %d :Set Broad Cast Policy succeded\n", activeBoards[boardNumber]);
	else
		printf("Board %d :Get Broad Cast Policy failed! Error:%d\n", activeBoards[boardNumber], r);
}

void SoftHand::StopBroadCast(int BoardNumber)
/******************************************/
{
	r = StopBCast_SH(activeBoards[BoardNumber]);
	if (r != -1)
		printf("Board %d :Stop Broadcast request sent succed\n", activeBoards[BoardNumber]);
	else
		printf("Board %d :Stop Broadcast request sent failed!\n", activeBoards[BoardNumber]);
}

void SoftHand::StartPosControl()
/******************************************/
{	
	for (b = FIRST_LBODY_BOARD; b<LAST_LBODY_BOARD; b++)
		if (activeBoards[b]>0)
		{
			r = StartBoardPositionControl_SH(activeBoards[b]);
			if (r != -1)
				printf("Board %d :Start Position Control succeded, %d\n", activeBoards[b], r);
			else
				printf("Board %d :Start Position Control failed! Error:%d\n", activeBoards[b], r);
		}
	printf("\n");
}

int SoftHand::StopPosControl()
{
	return StopPositionControl_SH();
}

//void SoftHand::threadedFunction()
//{
//	running = true;
//	printf("Starting SH thread \n");
//	//stcout << "starting SoftHand thread" << endl;
//
//	while (isThreadRunning())
//	{	
//		GetSHBroadCastData();	
//		ApplyDesiredPosition();
//	}
//
//	running = false;
//	cout << "SoftHand thread stopping" << endl;
//}

int SoftHand::GetSHBroadCastData()
{
	int bytesReceived = 0;
	int boardNo;
	int pktRead = 0;
	int PcCnt = 0;

	for (int pkt = 0; pkt<noActiveBoards + 1; pkt++)
		if ((bytesReceived = GetBCastData_SH(packetToReceive))>0)
		{
			if (packetToReceive.content[2] == (char)0xbb)   // this is a boradcast packet
			{
				PcCnt = 3;
				boardNo = packetToReceive.content[3];

				//printf("SH broadcasting..\n");

				////////////////////////This reads the main motor board
				{
					if (BCAST_POLICY_SH & 0x1)
					{//check if bit 0 of the BCAST_POLICY_SH is asserted
						joint[boardNo - 1].pos = (unsigned char)packetToReceive.content[PcCnt + 1] + ((packetToReceive.content[PcCnt + 2] << 8) & 0xff00)
							+ ((packetToReceive.content[PcCnt + 3] << 16) & 0xff0000) + ((packetToReceive.content[PcCnt + 4] << 24) & 0xff000000);
						PcCnt = PcCnt + 4;
					}
					if (BCAST_POLICY_SH & 0x2)
					{//check if bit 1 of the BCAST_POLICY_SH is asserted
						if (((packetToReceive.content[PcCnt + 2]) & 0x80)>0) joint[boardNo - 1].vel = ((unsigned char)packetToReceive.content[PcCnt + 1] + ((packetToReceive.content[PcCnt + 2] << 8) & 0xff00)) - 0xffff;
						else joint[boardNo - 1].vel = ((unsigned char)packetToReceive.content[PcCnt + 1] + ((packetToReceive.content[PcCnt + 2] << 8) & 0xff00));
						PcCnt = PcCnt + 2;
					}
					if (BCAST_POLICY_SH & 0x4)
					{//check if bit 2 of the BCAST_POLICY_SH is asserted
						joint[boardNo - 1].tor = packetToReceive.content[PcCnt + 1] + (packetToReceive.content[PcCnt + 2] * 256);
						PcCnt = PcCnt + 2;
					}
					if (BCAST_POLICY_SH & 0x8)
					{//check if bit 3 of the BCAST_POLICY_SH is asserted
						joint[boardNo - 1].pidOutput = (unsigned char)packetToReceive.content[PcCnt + 1] + ((packetToReceive.content[PcCnt + 1] << 8) & 0xff00);
						PcCnt = PcCnt + 2;
					}
					if (BCAST_POLICY_SH & 0x10)
					{//check if bit 4 of the BCAST_POLICY_SH is asserted
						joint[boardNo - 1].pidError = (unsigned char)packetToReceive.content[PcCnt + 1] + ((packetToReceive.content[PcCnt + 2] << 8) & 0xff00)
							+ ((packetToReceive.content[PcCnt + 3] << 16) & 0xff0000) + ((packetToReceive.content[PcCnt + 4] << 24) & 0xff000000);
						PcCnt = PcCnt + 4;
					}
					if (BCAST_POLICY_SH & 0x20)
					{//check if bit 5 of the BCAST_POLICY_SH is asserted
						joint[boardNo - 1].current = (unsigned char)packetToReceive.content[PcCnt + 1] + ((packetToReceive.content[PcCnt + 2] << 8) & 0xff00)
							+ ((packetToReceive.content[PcCnt + 3] << 16) & 0xff0000) + ((packetToReceive.content[PcCnt + 4] << 24) & 0xff000000);
						PcCnt = PcCnt + 4;
						//	if (((packetToReceive.content[PcCnt+4])& 0x80)>0)
						//joint[boardNo-1].realCurrent=(unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+2]<<8)& 0xff00) 
						//	+((packetToReceive.content[PcCnt+3]<<16)& 0xff0000)+((packetToReceive.content[PcCnt+4]<<24)& 0xff000000)-0xffff;
						//else
						//	joint[boardNo-1].realCurrent=(unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+2]<<8)& 0xff00) 
						//	+((packetToReceive.content[PcCnt+3]<<16)& 0xff0000)+((packetToReceive.content[PcCnt+4]<<24)& 0xff000000);
						//PcCnt=PcCnt+4;
					}
					if (BCAST_POLICY_SH & 0x40)
					{//check if bit 6 of the BCAST_POLICY_SH is asserted
						joint[boardNo - 1].temp_Vdc = (unsigned char)packetToReceive.content[PcCnt + 1] + ((packetToReceive.content[PcCnt + 2] << 8) & 0xff00)
							+ ((packetToReceive.content[PcCnt + 3] << 16) & 0xff0000) + ((packetToReceive.content[PcCnt + 4] << 24) & 0xff000000);
						PcCnt = PcCnt + 4;
					}
					if (BCAST_POLICY_SH & 0x80)
					{//check if bit 7 of the BCAST_POLICY_SH is asserted
						joint[boardNo - 1].tStamp = (unsigned char)packetToReceive.content[PcCnt + 1] + ((packetToReceive.content[PcCnt + 2] << 8) & 0xff00)
							+ ((packetToReceive.content[PcCnt + 3] << 16) & 0xff0000) + ((packetToReceive.content[PcCnt + 4] << 24) & 0xff000000);
						PcCnt = PcCnt + 4;
					}
					if (BCAST_POLICY_SH & 0x100)
					{//check if bit 8 of the BCAST_POLICY_SH is asserted
						joint[boardNo - 1].fault = (unsigned char)packetToReceive.content[PcCnt + 1] + ((packetToReceive.content[PcCnt + 1] << 8) & 0xff00);
						PcCnt = PcCnt + 2;
					}
					//Construct analogue input selection
					int AB = 2 * ((BCAST_POLICY_SH & 0x400) >> 10) + ((BCAST_POLICY_SH & 0x200) >> 9);
					switch (AB)
					{
						case 0:
							break;
						case 1:
							break;
						case 2:
							break;
						case 3:
						{
							joint[boardNo - 1].anInput1 = ((unsigned char)packetToReceive.content[PcCnt + 1] + ((packetToReceive.content[PcCnt + 2] << 8) & 0xff00));
							PcCnt = PcCnt + 2;
							joint[boardNo - 1].anInput2 = ((unsigned char)packetToReceive.content[PcCnt + 1] + ((packetToReceive.content[PcCnt + 2] << 8) & 0xff00));
							PcCnt = PcCnt + 2;

							// anInput3 and anInput4 receive force and torque in decaN and mNm 
							//for fitting  positive and negative forces. Force up to 327.67N with a resolution of 10mN. Torque up to 32.767Nm with a resolution of 1mNm.
							if (((packetToReceive.content[PcCnt + 2]) & 0x80)>0) joint[boardNo - 1].anInput3 = ((unsigned char)packetToReceive.content[PcCnt + 1] + ((packetToReceive.content[PcCnt + 2] << 8) & 0xff00)) - 0xffff;
							else joint[boardNo - 1].anInput3 = ((unsigned char)packetToReceive.content[PcCnt + 1] + ((packetToReceive.content[PcCnt + 2] << 8) & 0xff00));
							joint[boardNo - 1].Force = joint[boardNo - 1].anInput3 / 10.0;// convert to Newtons from decaNewtons
							PcCnt = PcCnt + 2;
							// decaN and mNm for fitting  positive and negative forces up to 327.67N with a resolution of 10mN.
							if (((packetToReceive.content[PcCnt + 2]) & 0x80)>0) joint[boardNo - 1].anInput4 = ((unsigned char)packetToReceive.content[PcCnt + 1] + ((packetToReceive.content[PcCnt + 2] << 8) & 0xff00)) - 0xffff;
							else joint[boardNo - 1].anInput4 = ((unsigned char)packetToReceive.content[PcCnt + 1] + ((packetToReceive.content[PcCnt + 2] << 8) & 0xff00));
							joint[boardNo - 1].Torque = joint[boardNo - 1].anInput4 / 1000.0;// convert to Nm from mNm
							PcCnt = PcCnt + 2;
						}
						break;
					}
					if (EXTRA_BCAST_POLICY_SH & 0x1)
					{//check if bit 0 of the EXTRA_BCAST_POLICY_SH is asserted
						joint[boardNo - 1].tpos = (unsigned char)packetToReceive.content[PcCnt + 1] + ((packetToReceive.content[PcCnt + 2] << 8) & 0xff00)
							+ ((packetToReceive.content[PcCnt + 3] << 16) & 0xff0000) + ((packetToReceive.content[PcCnt + 4] << 24) & 0xff000000);
						PcCnt = PcCnt + 4;
					}
					if (EXTRA_BCAST_POLICY_SH & 0x2)
					{//check if bit 1 of the EXTRA_BCAST_POLICY_SH is asserted 						
						joint[boardNo - 1].temp_tpos = (unsigned char)packetToReceive.content[PcCnt + 1] + ((packetToReceive.content[PcCnt + 2] << 8) & 0xff00)
							+ ((packetToReceive.content[PcCnt + 3] << 16) & 0xff0000) + ((packetToReceive.content[PcCnt + 4] << 24) & 0xff000000);
						PcCnt = PcCnt + 4;
					}
					if (EXTRA_BCAST_POLICY_SH & 0x4)
					{//check if bit 2 of the EXTRA_BCAST_POLICY_SH is asserted
						joint[boardNo - 1].req_tpos = (unsigned char)packetToReceive.content[PcCnt + 1] + ((packetToReceive.content[PcCnt + 2] << 8) & 0xff00)
							+ ((packetToReceive.content[PcCnt + 3] << 16) & 0xff0000) + ((packetToReceive.content[PcCnt + 4] << 24) & 0xff000000);
						PcCnt = PcCnt + 4;
					}
					if (EXTRA_BCAST_POLICY_SH & 0x8)
					{//check if bit 3 of the EXTRA_BCAST_POLICY_SH is asserted
						joint[boardNo - 1].TwinPos = (unsigned char)packetToReceive.content[PcCnt + 1] + ((packetToReceive.content[PcCnt + 2] << 8) & 0xff00)
							+ ((packetToReceive.content[PcCnt + 3] << 16) & 0xff0000) + ((packetToReceive.content[PcCnt + 4] << 24) & 0xff000000);
						PcCnt = PcCnt + 4;
					}
					if (EXTRA_BCAST_POLICY_SH & 0x10)
					{//check if bit 4 of the EXTRA_BCAST_POLICY_SH is asserted
						joint[boardNo - 1].angle = (unsigned char)packetToReceive.content[PcCnt + 1] + ((packetToReceive.content[PcCnt + 2] << 8) & 0xff00);
						PcCnt = PcCnt + 2;
					}
					if (EXTRA_BCAST_POLICY_SH & 0x20)
					{//check if bit 5 of the EXTRA_BCAST_POLICY_SH is asserted
						joint[boardNo - 1].targAngle = (unsigned char)packetToReceive.content[PcCnt + 1] + ((packetToReceive.content[PcCnt + 2] << 8) & 0xff00);
						PcCnt = PcCnt + 2;
					}
					if (EXTRA_BCAST_POLICY_SH & 0x40)
					{//check if bit 6 of the EXTRA_BCAST_POLICY_SH is asserted
						joint[boardNo - 1].TwinActPos = (unsigned char)packetToReceive.content[PcCnt + 1] + ((packetToReceive.content[PcCnt + 2] << 8) & 0xff00)
							+ ((packetToReceive.content[PcCnt + 3] << 16) & 0xff0000) + ((packetToReceive.content[PcCnt + 4] << 24) & 0xff000000);
						PcCnt = PcCnt + 4;
					}
					if (EXTRA_BCAST_POLICY_SH & 0x80)
					{//check if bit 7 of the EXTRA_BCAST_POLICY_SH is asserted
						joint[boardNo - 1].TwinTargPos = (unsigned char)packetToReceive.content[PcCnt + 1] + ((packetToReceive.content[PcCnt + 2] << 8) & 0xff00)
							+ ((packetToReceive.content[PcCnt + 3] << 16) & 0xff0000) + ((packetToReceive.content[PcCnt + 4] << 24) & 0xff000000);
						PcCnt = PcCnt + 4;
					}
					if (EXTRA_BCAST_POLICY_SH & 0x100)
					{//check if bit 8 of the EXTRA_BCAST_POLICY_SH is asserted
						joint[boardNo - 1].TwinVel = (unsigned char)packetToReceive.content[PcCnt + 1] + ((packetToReceive.content[PcCnt + 2] << 8) & 0xff00);
						PcCnt = PcCnt + 2;
					}
					if (EXTRA_BCAST_POLICY_SH & 0x200)
					{//check if bit 9 of the EXTRA_BCAST_POLICY_SH is asserted
						if (((packetToReceive.content[PcCnt + 2]) & 0x80)>0) joint[boardNo - 1].Xaccleration = ((unsigned char)packetToReceive.content[PcCnt + 1] + ((packetToReceive.content[PcCnt + 2] << 8) & 0xff00)) - 0xffff;
						else joint[boardNo - 1].Xaccleration = ((unsigned char)packetToReceive.content[PcCnt + 1] + ((packetToReceive.content[PcCnt + 2] << 8) & 0xff00));
						PcCnt = PcCnt + 2;
					}
					if (EXTRA_BCAST_POLICY_SH & 0x400)
					{//check if bit 10 of the EXTRA_BCAST_POLICY_SH is asserted
						if (((packetToReceive.content[PcCnt + 2]) & 0x80)>0) joint[boardNo - 1].Yaccleration = ((unsigned char)packetToReceive.content[PcCnt + 1] + ((packetToReceive.content[PcCnt + 2] << 8) & 0xff00)) - 0xffff;
						else joint[boardNo - 1].Yaccleration = ((unsigned char)packetToReceive.content[PcCnt + 1] + ((packetToReceive.content[PcCnt + 2] << 8) & 0xff00));
						PcCnt = PcCnt + 2;
					}
					if (EXTRA_BCAST_POLICY_SH & 0x800)
					{//check if bit 11 of the EXTRA_BCAST_POLICY_SH is asserted
						if (((packetToReceive.content[PcCnt + 2]) & 0x80)>0) joint[boardNo - 1].Zaccleration = ((unsigned char)packetToReceive.content[PcCnt + 1] + ((packetToReceive.content[PcCnt + 2] << 8) & 0xff00)) - 0xffff;
						else joint[boardNo - 1].Zaccleration = ((unsigned char)packetToReceive.content[PcCnt + 1] + ((packetToReceive.content[PcCnt + 2] << 8) & 0xff00));
						PcCnt = PcCnt + 2;
					}
				/*	if (EXTRA_BCAST_POLICY_SH & 0x1000)
					{//check if bit 11 of the EXTRA_BCAST_POLICY_SH is asserted
						joint[boardNo - 1].LinkVel = (unsigned char)packetToReceive.content[PcCnt + 1] + ((packetToReceive.content[PcCnt + 2] << 8) & 0xff00);
						PcCnt = PcCnt + 2;
					}*/
				}

			}
			bytesReceived = 0;
		}
	
		//lock();

			current = joint[0].current; // nobody is changing current variable value, just reading it...but let's keep this inside the lock for now!
			rawPos = joint[0].pos;

			if (mainOffset != 0)
				normalizedPos = Map(float(rawPos), float(mainOffset), HAND_MAX, 0.0f, 1.0f);
			else
				normalizedPos = -1;

		//unlock();

	return 0;
}