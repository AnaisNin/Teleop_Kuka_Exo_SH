//Added in "project/properties/configuration properties/linker/input/additional dependencies": WS2_32.lib

#pragma comment(lib, "ws2_32.lib")
#include "../include/EXOSBoardLibrary.h"
#include <iostream>

#define _USE_MATH_DEFINES //This+math.h to get M_PI
#include <math.h>
//Modify in ConnectTCP and ConnectUDP the ip address to connect with and the port number (Port)


//Comunication
//#include "../include/stdafx.h"

static const unsigned int Port = 2000; //Init=23
int activeBoardIDs[MAX_BOARDS];
int numberOfActiveBoards;
static int bcast_policy=1;
int BroadCastPolicy=1;

int** tempData;

static const int resRelEnc=30000;//joint 0 - increment of 30000 represents 2pi - can count several turns.
static const int resAbsEnc=4096; //joints 1..5 - Reading from 0 to 4096 which corresponds to 2pi. Can count only one turn, then goes back to 0




	 //WSADATA              wsaData;
	 SOCKET               boardSckID[MAX_BOARDS], SckID, UDPSckID;
	 
	 SOCKADDR_IN          ReceiverAddr;/*, SrcInfo*/;

	 HandExoskeleton Exos;


//*******************************

	 //NEW

//**************************************************************************
//DECLDIR int SetDesiredForce(int DesForce[][MAX_MOTORS]) //TODO: to send the message if there is more than 1 board
DECLDIR int SetDesiredForce(int DesForce[MAX_BOARDS][MAX_MOTORS]) //TODO: to send the message if there is more than 1 board
{
	int BytesSent, BoardNum, i;
	char payload[PACKET_MAX_SIZE];
	CharBuff packetToBeSent;

	//Getting active board
	//BoardNum = GetActiveBoards();

//	BoardNum = MAX_BOARDS;

	//Sends request
	//for (i = 0; i<BoardNum/2; i++)
	//{					
	//	if(activeBoardIDs[i]<=0)
	//	{
	//		//cout << "activeBoardIDs[" << i << "] = " << activeBoardIDs[i] << endl;
	//		payload[4*i] = 0x00;
	//		payload[4*i+1] = 0x00;
	//		payload[4*i+2] = 0x00;
	//		payload[4*i+3] = 0x00;
	//	}	
	//	else
	//	{	
	//		/*payload[12*(i-1)]   = ( DesForce[i-1][0]);
	//		payload[12*(i-1)+1] = ((DesForce[i-1][0]));
	//		payload[12*(i-1)+2] = ((DesForce[i-1][0]));
	//		payload[12*(i-1)+3] = ((DesForce[i-1][0]));

	//		payload[12*(i-1)+4] = ( DesForce[i-1][1]);
	//		payload[12*(i-1)+5] = ((DesForce[i-1][1]));
	//		payload[12*(i-1)+6] = ((DesForce[i-1][1]));
	//		payload[12*(i-1)+7] = ((DesForce[i-1][1]));

	//		payload[12*(i-1)+8] = (DesForce[i-1][2]);
	//		payload[12*(i-1)+9] = ((DesForce[i-1][2]));
	//		payload[12*(i-1)+10] = ((DesForce[i-1][2]));
	//		payload[12*(i-1)+11] = ((DesForce[i-1][2]));*/

	//		payload[12*(i-1)]   = ( DesForce[i-1][0] & 0x000000ff);
	//		payload[12*(i-1)+1] = ((DesForce[i-1][0]>>8) & 0x000000ff);
	//		payload[12*(i-1)+2] = ((DesForce[i-1][0]>>16) & 0x000000ff);
	//		payload[12*(i-1)+3] = ((DesForce[i-1][0]>>24) & 0x000000ff);

	//		payload[12*(i-1)+4] = ( DesForce[i-1][1] & 0x000000ff);
	//		payload[12*(i-1)+5] = ((DesForce[i-1][1]>>8) & 0x000000ff);
	//		payload[12*(i-1)+6] = ((DesForce[i-1][1]>>16) & 0x000000ff);
	//		payload[12*(i-1)+7] = ((DesForce[i-1][1]>>24) & 0x000000ff);

	//		payload[12*(i-1)+8] = (DesForce[i-1][2] & 0x000000ff);
	//		payload[12*(i-1)+9] = ((DesForce[i-1][2]>>8) & 0x000000ff);
	//		payload[12*(i-1)+10] = ((DesForce[i-1][2]>>16) & 0x000000ff);
	//		payload[12*(i-1)+11] = ((DesForce[i-1][2]>>24) & 0x000000ff);
	//	}
	//}
	//		
	
	//TEST 
	//board_id=it+1;//Assumes id boards=1,2,3
	//i=board_id*36-33-3;//Cf hardware board 3 motors - ui_ethernet.c   else if (pucData[2] == SET_DESIRED_POSITION){..}
	//We need startbyte_packet = board_id*36-33 starting from byte 0 of the packet (cf IAR). That means startbyte_payload=startbyte_packet-3 (3 bytes for header packet)
			
	//Stable, board_id 3 only 
		//int board_id=3;
		//i=(board_id-1)*12;
		//	
		//	//motor 1 considered board
		//	payload[i]=DesForce[2][0] & 0x000000ff;
		//	payload[i+1]=((DesForce[2][0]>>8) & 0x000000ff);
		//	payload[i+2]=((DesForce[2][0]>>16) & 0x000000ff);
		//	payload[i+3]=((DesForce[2][0]>>24) & 0x000000ff);
		//	//motor 2 same board
		//	payload[i+4]=DesForce[2][1] & 0x000000ff;
		//	payload[i+5]=((DesForce[2][1]>>8) & 0x000000ff);
		//	payload[i+6]=((DesForce[2][1]>>16) & 0x000000ff);
		//	payload[i+7]=((DesForce[2][1]>>24) & 0x000000ff);
		//	//motor 3 same board
		//	payload[i+8]=DesForce[2][2] & 0x000000ff;
		//	payload[i+9]=((DesForce[2][2]>>8) & 0x000000ff);
		//	payload[i+10]=((DesForce[2][2]>>16) & 0x000000ff);
		//	payload[i+11]=((DesForce[2][2]>>24) & 0x000000ff);
		//	//payload[i+11]=0x00;

	for(int bid=1;bid<MAX_BOARDS;bid++)
	{
		i=(bid-1)*12;//start byte of payload - 12bytes per board since for each board 3 motors=3int=3*4bytes

		//motor 1 considered board
		payload[i]=DesForce[bid-1][0] & 0x000000ff;
		payload[i+1]=((DesForce[bid-1][0]>>8) & 0x000000ff);
		payload[i+2]=((DesForce[bid-1][0]>>16) & 0x000000ff);
		payload[i+3]=((DesForce[bid-1][0]>>24) & 0x000000ff);
		//motor 2 same board
		payload[i+4]=DesForce[bid-1][1] & 0x000000ff;
		payload[i+5]=((DesForce[bid-1][1]>>8) & 0x000000ff);
		payload[i+6]=((DesForce[bid-1][1]>>16) & 0x000000ff);
		payload[i+7]=((DesForce[bid-1][1]>>24) & 0x000000ff);
		//motor 3 same board
		payload[i+8]=DesForce[bid-1][2] & 0x000000ff;
		payload[i+9]=((DesForce[bid-1][2]>>8) & 0x000000ff);
		payload[i+10]=((DesForce[bid-1][2]>>16) & 0x000000ff);
		payload[i+11]=((DesForce[bid-1][2]>>24) & 0x000000ff);
		//payload[i+11]=0x00;

	}

			
			

	buildUDPCommand(&packetToBeSent, SET_DESIRED_FORCE , payload);

	BytesSent = SendUDP(UDPSckID, &packetToBeSent);

	//printf("SetDesiredForce [ExosBoardLibrary]\n");

	//cout << "el valor de UDPSckID en SetDesiredForce es: " << UDPSckID << endl;
	//cout << "paquete a enviar = " << &packetToBeSent << endl;
	if (BytesSent == -1)
		return -1;
	
	return 0;
}

//******************************************************************************

DECLDIR int StartForceControl(void) //TODO: to send the message if there is more than 1 board
{
	int BytesSent, BoardNum, i;
	char payload[PACKET_MAX_SIZE];
	CharBuff packetToBeSent;

	//Getting active board
//	BoardNum = GetActiveBoards();
	BoardNum = MAX_MOTOR_BOARDS;

	//Sends request
	for (i = 0; i<BoardNum; i++)
		if(activeBoardIDs[i]<=0)
			payload[i] = 0x00;
		else
			payload[i] = 0x03;
	
	buildUDPCommand(&packetToBeSent, FORCE_MOVE, payload);
	BytesSent = SendUDP(UDPSckID, &packetToBeSent);

	//cout << "StartForceControl() is running " << endl;

	if (BytesSent == -1)
		return -1;
	
	return 0;
}

//*****************************************************************
DECLDIR int StopForceControl(void) //TODO: to send the message if there is more than 1 board
{
	int BytesSent, BoardNum, i;
	char payload[PACKET_MAX_SIZE];
	CharBuff packetToBeSent;

	//Getting active board
//	BoardNum = GetActiveBoards();
	BoardNum = MAX_MOTOR_BOARDS;

	//Sends request
	for (i = 0; i<BoardNum; i++)
			payload[i] = 0x01;
	
	buildUDPCommand(&packetToBeSent, POSITION_MOVE , payload);
	BytesSent = SendUDP(UDPSckID, &packetToBeSent);
	if (BytesSent == -1)
		return -1;
	
	return 0;
}

//*****************************************************************
//Nadia 
DECLDIR int SetDesiredPosition(int DesPos[][MAX_MOTORS]) //TODO: to send the message if there is more than 1 board
{
	int BytesSent, BoardNum, i;
	char payload[PACKET_MAX_SIZE];
	CharBuff packetToBeSent;

	//Getting active board
	//BoardNum = GetActiveBoards();

	BoardNum = MAX_BOARDS;

	//Sends request
	for (i = 0; i<BoardNum/2; i++)
	{					
		if(activeBoardIDs[i]<=0)
		{
			//cout << "activeBoardIDs[" << i << "] = " << activeBoardIDs[i] << endl;
			payload[4*i] = 0x00;
			payload[4*i+1] = 0x00;
			payload[4*i+2] = 0x00;
			payload[4*i+3] = 0x00;
		}	
		else
		{				
			payload[12*(i-1)]   = ( DesPos[i-1][0] & 0x000000ff);
			payload[12*(i-1)+1] = ((DesPos[i-1][0]>>8) & 0x000000ff);
			payload[12*(i-1)+2] = ((DesPos[i-1][0]>>16) & 0x000000ff);
			payload[12*(i-1)+3] = ((DesPos[i-1][0]>>24) & 0x000000ff);

			payload[12*(i-1)+4] = ( DesPos[i-1][1] & 0x000000ff);
			payload[12*(i-1)+5] = ((DesPos[i-1][1]>>8) & 0x000000ff);
			payload[12*(i-1)+6] = ((DesPos[i-1][1]>>16) & 0x000000ff);
			payload[12*(i-1)+7] = ((DesPos[i-1][1]>>24) & 0x000000ff);

			payload[12*(i-1)+8] = (DesPos[i-1][2] & 0x000000ff);
			payload[12*(i-1)+9] = ((DesPos[i-1][2]>>8) & 0x000000ff);
			payload[12*(i-1)+10] = ((DesPos[i-1][2]>>16) & 0x000000ff);
			payload[12*(i-1)+11] = ((DesPos[i-1][2]>>24) & 0x000000ff);					
			
		}					
	}

	buildUDPCommand(&packetToBeSent, SET_DESIRED_POSITION , payload);

	BytesSent = SendUDP(UDPSckID, &packetToBeSent);

	//cout << "el valor de UDPSckID en SetDesiredPosition es: " << UDPSckID << endl;
	//cout << "paquete a enviar = " << &packetToBeSent << endl;
	if (BytesSent == -1)
		return -1;
	
	return 0;
}

//**************************************************

//	DECLDIR int SetDesiredPosition(int DesPos[MAX_BOARDS]) //TODO: to send the message if there is more than 1 board
//	{
//		int BytesSent, BoardNum, i;
//		char payload[PACKET_MAX_SIZE];
//		CharBuff packetToBeSent;
//
//		//Getting active board
//		//BoardNum = GetActiveBoards();
//
//		BoardNum = MAX_BOARDS;
//
//		//Sends request
//		for (i = 0; i<BoardNum/2; i++)
//		{
//			if(activeBoardIDs[i]<=0)
//			{
//				payload[4*i] = 0x00;
//				payload[4*i+1] = 0x00;
//				payload[4*i+2] = 0x00;
//				payload[4*i+3] = 0x00;
//			}	
//			else
//			{
//				//payload[4*i] =	 (DesPos[i] & 0x000000ff);
//				//payload[4*i+1] = ((DesPos[i]>>8) & 0x000000ff);
//				//payload[4*i+2] = ((DesPos[i]>>16) & 0x000000ff);
//				//payload[4*i+3] = ((DesPos[i]>>24) & 0x000000ff);
////DesPos[0]=10;
////DesPos[1]=11;
////DesPos[2]=22;
////DesPos[3]=33;
//				payload[8*i] =	 (DesPos[2*i] & 0x000000ff);
//				payload[8*i+1] = ((DesPos[2*i]>>8) & 0x000000ff);
//				payload[8*i+2] = ((DesPos[2*i]>>16) & 0x000000ff);
//				payload[8*i+3] = ((DesPos[2*i]>>24) & 0x000000ff);
//
//				payload[8*i+4] = (DesPos[2*i+1] & 0x000000ff);
//				payload[8*i+5] = ((DesPos[2*i+1]>>8) & 0x000000ff);
//				payload[8*i+6] = ((DesPos[2*i+1]>>16) & 0x000000ff);
//				payload[8*i+7] = ((DesPos[2*i+1]>>24) & 0x000000ff);
//				
//
//			/*	payload[4*i]   = DesPos[i]%256;
//				payload[4*i+1] = DesPos[i]/256;
//				payload[4*i+2] = DesPos[i]/65536;
//				payload[4*i+3] = DesPos[i]/16777216; */
//				
//			}
//			
//				
//		}
//
//		printf("Set Desired Position: %*ld\n",7,DesPos[0]);
//		printf("Set Desired Position: %*ld\n",7,DesPos[1]);
//		printf("Set Desired Position: %*ld\n",7,DesPos[2]);
//	/*	for (i = 0; i < (4*BoardNum-4); i++)
//			payload[i] = 0x00;
//		
//		payload[4*BoardNum-4] = DesPos%256;
//		payload[4*BoardNum-3] = DesPos/256;
//		payload[4*BoardNum-2] = DesPos/65536;
//		payload[4*BoardNum-1] = DesPos/16777216;  */
//
//		buildUDPCommand(&packetToBeSent, SET_DESIRED_POSITION , payload);
//	
//		BytesSent = SendUDP(UDPSckID, &packetToBeSent);
//		if (BytesSent == -1)
//			return -1;
//		
//		return 0;
//	}


//****************************************************
	 DECLDIR int InitLibrary(int boardsCount)
	 {
		 tempData = new int*[3];
		 tempData[0] = new int[6];
		 tempData[1] = new int[6];
		 tempData[2] = new int[6];

		 // Initialize Winsock version 2.2
		//WSADATA wsaData;
		//WSAStartup(MAKEWORD(2,2), &wsaData);

		 // Initialize phil_board.c
		init(boardsCount);
		return 0;
	 }

//******************************************************************************
    DECLDIR int ConnectTCP(int BoardNumber)
	{
		
		char ip_addr_str[14];
		int  RetCode;
		SOCKADDR_IN ServerAddr;


		// Initialize Winsock version 2.2
		WSADATA wsaData;
		WSAStartup(MAKEWORD(2,2), &wsaData);
		
		//sprintf(ip_addr_str, "169.254.89.%d", BASEADDR+BoardNumber); //Initial
		//printf("[EXOSBoardLibrary.cpp] New lib \n");
		sprintf(ip_addr_str, "172.16.1.%d", BASEADDR+BoardNumber);
		printf("Try to set up TCP connection with 172.16.1.%d [BoardLibrary.cpp::ConnectTCP] \n", BASEADDR+BoardNumber);		
		
		// Create a new socket to make a client connection.
		// AF_INET = 2, The Internet Protocol version 4 (IPv4) address family, TCP protocol

		boardSckID[BoardNumber] = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		if(boardSckID[BoardNumber] == INVALID_SOCKET)
		{
			printf("Client: socket() failed! Error code: %ld\n", WSAGetLastError());
			WSACleanup(); // Do the clean up
			return -1;
		}
	
		 // IPv4
		 ServerAddr.sin_family = AF_INET;

		 // Port no.
		 ServerAddr.sin_port = htons(Port);

		 // The IP address
		 ServerAddr.sin_addr.s_addr = inet_addr(ip_addr_str);

		 // Make a connection to the server with socket SckID.

		 RetCode = connect(boardSckID[BoardNumber], (SOCKADDR *) &ServerAddr, sizeof(ServerAddr));

		 if(RetCode != 0)
		 {
			  closesocket(boardSckID[BoardNumber]);
			  WSACleanup();
			  return -1;
		 }

		 return boardSckID[BoardNumber];

	}

//******************************************************************************


	DECLDIR int SendTCP(SOCKET SckID, CharBuff* packetToBeSent)
	{
		int BytesSent;

		BytesSent = send(SckID, packetToBeSent->content, packetToBeSent->size, 0);
		if(BytesSent == SOCKET_ERROR)
			return -1;
		else
			return BytesSent;
	}

	
//******************************************************************************

	DECLDIR int ReceiveTCP(SOCKET SckID, CharBuff* packetToReceive)
	{
		int BytesReceived;

		BytesReceived = recv(SckID, packetToReceive->content, sizeof(packetToReceive->content), 0);
		packetToReceive->size = BytesReceived;
		return BytesReceived;
	}

	
//******************************************************************************

	DECLDIR int GetBoardType(int BoardNumber)
	{
		int BytesSent, BytesReceived, BoardType;
		CharBuff packetToBeSent, packetToReceive;
		SckID=boardSckID[BoardNumber];
		
		//Sends request
		buildTCPCommand(&packetToBeSent, GET_BOARD_TYPE, NULL);
		BytesSent = SendTCP(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;

		//Receives answer
		BytesReceived = ReceiveTCP(SckID, &packetToReceive);
		if (BytesReceived == -1)
			return -1;
		
		//TODO use descriptor information to retrieve the payload, not constants:
		BoardType = (unsigned char)(packetToReceive.content[3]);

		return BoardType;
	}

	
//******************************************************************************

	DECLDIR float GetFirmVersion(int BoardNumber)
	{
		int BytesSent, BytesReceived, Firm0, Firm1;
		char Firm[3];
		float fFirm;
		CharBuff packetToBeSent, packetToReceive;
		SckID=boardSckID[BoardNumber];

		//Sends request		
		buildTCPCommand(&packetToBeSent, GET_FIRMWARE_VERSION, NULL);
		BytesSent = SendTCP(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;

		//Receives answer
		BytesReceived = ReceiveTCP(SckID, &packetToReceive);
		if (BytesReceived == -1)
			return -1;

		Firm0 = (unsigned char)(packetToReceive.content[3]);
		Firm1 = ((unsigned char) (packetToReceive.content[4]));

		//Put in the form Firm0.Firm1, e.g. Version "0.1"
		
		sprintf(Firm, "%d.%d", Firm0, Firm1);
		
		fFirm = (float)atof(Firm);
		
		return fFirm;
	}
	

//******************************************************************************

	DECLDIR int SetFirmVersion(int BoardNumber, char * firm) //TODO: board doesn't update the firmaware version.
	{
		int BytesSent;
		char payload[2];
		CharBuff packetToBeSent;
		SckID=boardSckID[BoardNumber];

		sscanf_s(firm, "%d.%d", &payload[0], &payload[1]);

		//Sends request
		buildTCPCommand(&packetToBeSent, SET_FIRMWARE_VERSION, payload);
		BytesSent = SendTCP(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		else
			return 0;
	}

//******************************************************************************

	DECLDIR int ClearBoardFault(int BoardNumber)
	{
		int BytesSent;
		CharBuff packetToBeSent;
		SckID=boardSckID[BoardNumber];

		//Sends request		
		buildTCPCommand(&packetToBeSent, CLEAR_BOARD_FAULT, NULL);
		BytesSent = SendTCP(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		else
			return 0;

	}

//******************************************************************************

	DECLDIR int GetBoardFault(int BoardNumber)
	{
		int BytesSent, BytesReceived, offset, Faults;
		CharBuff packetToBeSent, packetToReceive;
		SckID=boardSckID[BoardNumber];

		//Sends request
		buildTCPCommand(&packetToBeSent, GET_BOARD_FAULT, NULL);
		BytesSent = SendTCP(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;

		//Receives answer
		BytesReceived = ReceiveTCP(SckID, &packetToReceive);
		if (BytesReceived == -1)
			return -1;

		offset = getPayloadOffset(getDescriptor_TCPRep(REPLY_BOARD_FAULT)); //payload initial position
		bytesToInt(&packetToReceive.content[offset], 2, &Faults); //converts the payload bytes to an integer

		return Faults;
	}

//******************************************************************************

	DECLDIR int SetPidGains(int BoardNumber, char gainSet, char * Gains)
    {
		int BytesSent;
		long P, I, D;
		char payload[13];
		CharBuff packetToBeSent;
		SckID=boardSckID[BoardNumber];
		

		sscanf_s(Gains, "%d,%d,%d", &P, &I, &D); //Get the gains from the input array

		//Sends request
		payload[0] = gainSet;
		longToBytes(P, payload+1, 4);
		longToBytes(I, payload+5, 4);
		longToBytes(D, payload+9, 4);

		buildTCPCommand(&packetToBeSent, SET_PID_GAINS, payload);
		BytesSent = SendTCP(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		else
			return 0;
	}

//******************************************************************************

	DECLDIR int GetPidGains(int BoardNumber, char gainSet, long* GainsBuf)
    {
		int BytesSent, BytesReceived, offset;
		long p, i, d;
		CharBuff packetToBeSent, packetToReceive;
		SckID=boardSckID[BoardNumber];

		//Sends request
		buildTCPCommand(&packetToBeSent, GET_PID_GAINS, &gainSet);
		BytesSent = SendTCP(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;

		//Receives answer
		BytesReceived = ReceiveTCP(SckID, &packetToReceive);
		if (BytesReceived == -1)
			return -1;

		offset = getPayloadOffset(getDescriptor_TCPRep(REPLY_PID_GAINS)) + 1; //payload initial position
		bytesToLong(&packetToReceive.content[offset], 4, &p); //p gain
		bytesToLong(&packetToReceive.content[offset+4], 4, &i); //i gain
		bytesToLong(&packetToReceive.content[offset+8], 4, &d); //d gain
			
		GainsBuf[0] = p;
		GainsBuf[1] = i;
		GainsBuf[2] = d;

		return 0;
	}

//******************************************************************************

	DECLDIR int SetPidGainScale(int BoardNumber, char gainSet, char * GainsScale)
    {
		int BytesSent;
		long P, I, D;
		char payload[13]; //TODO: can we avoid the constant?
		CharBuff packetToBeSent;
		SckID=boardSckID[BoardNumber];
		
		sscanf_s(GainsScale, "%d,%d,%d", &P, &I, &D);

		//Sends request
		payload[0] = gainSet;
		longToBytes(P, payload+1, 4);
		longToBytes(I, payload+5, 4);
		longToBytes(D, payload+9, 4);

		buildTCPCommand(&packetToBeSent, SET_PID_GAIN_SCALE, payload);
		BytesSent = SendTCP(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		else
			return 0;
	}

//******************************************************************************

	DECLDIR int GetPidGainScale(int BoardNumber, char gainSet, long* GainsBuf)
    {
		int BytesSent, BytesReceived, offset;
		long p, i, d;
		CharBuff packetToBeSent, packetToReceive;
		SckID=boardSckID[BoardNumber];

		//Sends request
		buildTCPCommand(&packetToBeSent, GET_PID_GAIN_SCALE, &gainSet);
		BytesSent = SendTCP(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;

		//Receives answer
		BytesReceived = ReceiveTCP(SckID, &packetToReceive);
		if (BytesReceived == -1)
			return -1;
			
		offset = getPayloadOffset(getDescriptor_TCPRep(REPLY_PID_GAIN_SCALE)) + 1; //payload initial position
		bytesToLong(&packetToReceive.content[offset], 4, &p); //p gain scale
		bytesToLong(&packetToReceive.content[offset+4], 4, &i); //i gain scale
		bytesToLong(&packetToReceive.content[offset+8], 4, &d); //d gain scale
			
		GainsBuf[0] = p;
		GainsBuf[1] = i;
		GainsBuf[2] = d;

		return 0;
	}

//******************************************************************************

	DECLDIR int SetILimGain(int BoardNumber, char gainSet, long ILim)
    {
		int BytesSent;
		char payload[5];
		CharBuff packetToBeSent;
		SckID=boardSckID[BoardNumber];

		//Sends request
		payload[0] = gainSet;
		longToBytes(ILim, payload+1, sizeof(payload)-1);

		buildTCPCommand(&packetToBeSent, SET_ILIM_GAIN, payload);
		BytesSent = SendTCP(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		else
			return 0;
	}

//******************************************************************************

	DECLDIR long GetILimGain(int BoardNumber, char gainSet)
    {
		int BytesSent, BytesReceived, offset;
		long limit;
		CharBuff packetToBeSent, packetToReceive;
		SckID=boardSckID[BoardNumber];


		//Sends request
		buildTCPCommand(&packetToBeSent, GET_ILIM_GAIN, &gainSet);
		BytesSent = SendTCP(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;

		//Receives answer
		BytesReceived = ReceiveTCP(SckID, &packetToReceive);
		if (BytesReceived == -1)
			return -1;
			
		//Limit
		offset = getPayloadOffset(getDescriptor_TCPRep(REPLY_ILIM_GAIN)) + 1; //Add one to the payload offset to skip the first byte, which is the gain set:
		bytesToLong(&packetToReceive.content[offset], 4, &limit);

		return limit;
	}

//******************************************************************************

	DECLDIR long GetPidError(int BoardNumber)
    {
		int BytesSent, BytesReceived, offset;
		long e;
		CharBuff packetToBeSent, packetToReceive;
		SckID=boardSckID[BoardNumber];

		//Sends request
		buildTCPCommand(&packetToBeSent, GET_PID_ERROR, NULL);
		BytesSent = SendTCP(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		
		//Receives answer
		BytesReceived = ReceiveTCP(SckID, &packetToReceive);
		if (BytesReceived == -1)
			return -1;

		//PID error
		offset = getPayloadOffset(getDescriptor_TCPRep(REPLY_PID_ERROR));
		bytesToLong(&packetToReceive.content[offset], 4, &e);

		return e ; //TODO: what means this number?
	}

//******************************************************************************

	DECLDIR int GetPidOutput(int BoardNumber)
    {
		int BytesSent, BytesReceived, offset, u;
		CharBuff packetToBeSent, packetToReceive;
		SckID=boardSckID[BoardNumber];

		//Sends request
		buildTCPCommand(&packetToBeSent, GET_PID_OUTPUT, NULL);
		BytesSent = SendTCP(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;

		//Receives answer
		BytesReceived = ReceiveTCP(SckID, &packetToReceive);
		if (BytesReceived == -1)
			return -1;

		//PID output
		offset = getPayloadOffset(getDescriptor_TCPRep(REPLY_PID_OUTPUT));
		bytesToInt(&packetToReceive.content[offset], 2, &u);

		return u; //is it the duty cycle?
	}

//******************************************************************************

	DECLDIR int GetPidOffset(int BoardNumber)
    {
		int BytesSent, BytesReceived, offset, off;
		CharBuff packetToBeSent, packetToReceive;
		SckID=boardSckID[BoardNumber];

		//Sends request
		buildTCPCommand(&packetToBeSent, GET_PID_OFFSET, NULL);
		BytesSent = SendTCP(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;

		//Receives answer
		BytesReceived = ReceiveTCP(SckID, &packetToReceive);
		if (BytesReceived == -1)
			return -1;

		//PID offset
		offset = getPayloadOffset(getDescriptor_TCPRep(REPLY_PID_OFFSET));
		bytesToInt(&packetToReceive.content[offset], 2, &off);

		return off;
	}

//******************************************************************************

	DECLDIR long GetPosition(int BoardNumber)
    {
		int BytesSent, BytesReceived, offset;
		long pos;
		CharBuff packetToBeSent, packetToReceive;
		SckID=boardSckID[BoardNumber];

		//Sends request
		buildTCPCommand(&packetToBeSent, GET_ENCODER_POSITION, NULL);
		BytesSent = SendTCP(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;

		//Receives answer
		BytesReceived = ReceiveTCP(SckID, &packetToReceive);
		if (BytesReceived == -1)
			return -1;

		offset = getPayloadOffset(getDescriptor_TCPRep(REPLY_ENCODER_POSITION));
		bytesToLong(&packetToReceive.content[offset], 4, &pos);

		return pos;
	}

//******************************************************************************

	DECLDIR int GetVelocity(int BoardNumber)
    {
		int BytesSent, BytesReceived, offset, v;
		CharBuff packetToBeSent, packetToReceive;
		SckID=boardSckID[BoardNumber];

		//Sends request
		buildTCPCommand(&packetToBeSent, GET_ENCODER_VELOCITY , NULL);
		BytesSent = SendTCP(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		
		//Receives answer
		BytesReceived = ReceiveTCP(SckID, &packetToReceive);
		if (BytesReceived == -1)
			return -1;

		offset = getPayloadOffset(getDescriptor_TCPRep(REPLY_ENCODER_VELOCITY));
		bytesToInt(&packetToReceive.content[offset], 2, &v);

		return v;
	}

//******************************************************************************

	DECLDIR int GetTorque(int BoardNumber)	
    {
		int BytesSent, BytesReceived, offset, torque;
		CharBuff packetToBeSent, packetToReceive;
		SckID=boardSckID[BoardNumber];

		//Sends request
		buildTCPCommand(&packetToBeSent, GET_MEASURED_TORQUE , NULL);
		BytesSent = SendTCP(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		
		//Receives answer
		BytesReceived = ReceiveTCP(SckID, &packetToReceive);
		if (BytesReceived == -1)
			return -1;

		offset = getPayloadOffset(getDescriptor_TCPRep(REPLY_MEASURED_TORQUE));
		bytesToInt(&packetToReceive.content[offset], 2, &torque);

		return torque;
	}

//******************************************************************************

	DECLDIR long GetDesiredPosition(int BoardNumber)
    {
		int BytesSent, BytesReceived, offset;
		long dp;
		CharBuff packetToBeSent, packetToReceive;
		SckID=boardSckID[BoardNumber];

		//Sends request
		buildTCPCommand(&packetToBeSent, GET_DESIRED_POSITION, NULL);
		BytesSent = SendTCP(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;

		//Receives answer
		BytesReceived = ReceiveTCP(SckID, &packetToReceive);
		if (BytesReceived == -1)
			return -1;

		offset = getPayloadOffset(getDescriptor_TCPRep(REPLY_DESIRED_POSITION));
		bytesToLong(&packetToReceive.content[offset], 4, &dp);

		return dp;
	}

//******************************************************************************

	DECLDIR int GetDesiredVelocity(int BoardNumber)
    {
		int BytesSent, BytesReceived, offset, dv;
		CharBuff packetToBeSent, packetToReceive;
		SckID=boardSckID[BoardNumber];

		//Sends request
		buildTCPCommand(&packetToBeSent, GET_DESIRED_VELOCITY, NULL);
		BytesSent = SendTCP(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;

		//Receives answer
		BytesReceived = ReceiveTCP(SckID, &packetToReceive);
		if (BytesReceived == -1)
			return -1;

		offset = getPayloadOffset(getDescriptor_TCPRep(REPLY_DESIRED_VELOCITY));
		bytesToInt(&packetToReceive.content[offset], 2, &dv);

		return dv;
	}

//******************************************************************************

	DECLDIR int GetDesiredTorque(int BoardNumber)
    {
		int BytesSent, BytesReceived, offset, dt;
		CharBuff packetToBeSent, packetToReceive;
		SckID=boardSckID[BoardNumber];

		//Sends request
		buildTCPCommand(&packetToBeSent, GET_DESIRED_TORQUE, NULL);
		BytesSent = SendTCP(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;

		//Receives answer
		BytesReceived = ReceiveTCP(SckID, &packetToReceive);
		if (BytesReceived == -1)
			return -1;

		offset = getPayloadOffset(getDescriptor_TCPRep(REPLY_DESIRED_TORQUE));
		bytesToInt(&packetToReceive.content[offset], 2, &dt);

		return dt;
	}

//******************************************************************************

	DECLDIR int SetAccel(int BoardNumber, int Accel)
    {
		int BytesSent;
		char payload[2];
		CharBuff packetToBeSent;
		SckID=boardSckID[BoardNumber];

		//Sends request
		intToBytes(Accel, payload, sizeof(payload));

		buildTCPCommand(&packetToBeSent, SET_ACCEL, payload);
		BytesSent = SendTCP(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		else
			return 0;
	}

//******************************************************************************

	DECLDIR int GetAccel(int BoardNumber)
    {
		int BytesSent, BytesReceived, offset, a;
		CharBuff packetToBeSent, packetToReceive;
		SckID=boardSckID[BoardNumber];

		//Sends request
		buildTCPCommand(&packetToBeSent, GET_ACCEL, NULL);
		BytesSent = SendTCP(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;

		//Receives answer
		BytesReceived = ReceiveTCP(SckID, &packetToReceive);
		if (BytesReceived == -1)
			return -1;

		offset = getPayloadOffset(getDescriptor_TCPRep(REPLY_ACCEL));
		bytesToInt(&packetToReceive.content[offset], 2, &a);

		return a;
	}

//******************************************************************************

	DECLDIR int SetMinPosition(int BoardNumber, long MinPosition)
    {
		int BytesSent;
		char payload[4];
		CharBuff packetToBeSent;
		SckID=boardSckID[BoardNumber];

		//Sends request
		longToBytes(MinPosition, payload, sizeof(payload));

		buildTCPCommand(&packetToBeSent, SET_MIN_POSITION, payload);
		BytesSent = SendTCP(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		else
			return 0;
	}

//******************************************************************************

	DECLDIR long GetMinPosition(int BoardNumber)
    {
		int BytesSent, BytesReceived, offset;
		long minPos;
		CharBuff packetToBeSent, packetToReceive;
		SckID=boardSckID[BoardNumber];

		//Sends request
		buildTCPCommand(&packetToBeSent, GET_MIN_POSITION, NULL);
		BytesSent = SendTCP(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;

		//Receives answer
		BytesReceived = ReceiveTCP(SckID, &packetToReceive);
		if (BytesReceived == -1)
			return -1;

		offset = getPayloadOffset(getDescriptor_TCPRep(REPLY_MIN_POSITION));
		bytesToLong(&packetToReceive.content[offset], 4, &minPos);

		return minPos;
	}

//******************************************************************************

	DECLDIR int SetMaxPosition(int BoardNumber, long MaxPosition)
    {
		int BytesSent;
		char payload[4];
		CharBuff packetToBeSent;
		SckID=boardSckID[BoardNumber];

		//Sends request
		longToBytes(MaxPosition, payload, sizeof(payload));

		buildTCPCommand(&packetToBeSent, SET_MAX_POSITION, payload);
		BytesSent = SendTCP(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		else
			return 0;
	}

//******************************************************************************

	DECLDIR long GetMaxPosition(int BoardNumber)
    {
		int BytesSent, BytesReceived, offset;
		long maxPos;
		CharBuff packetToBeSent, packetToReceive;
		SckID=boardSckID[BoardNumber];

		//Sends request
		buildTCPCommand(&packetToBeSent, GET_MAX_POSITION, NULL);
		BytesSent = SendTCP(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;

		//Receives answer
		BytesReceived = ReceiveTCP(SckID, &packetToReceive);
		if (BytesReceived == -1)
			return -1;

		offset = getPayloadOffset(getDescriptor_TCPRep(REPLY_MAX_POSITION));
		bytesToLong(&packetToReceive.content[offset], 4, &maxPos);

		return maxPos;
	}

//******************************************************************************

	DECLDIR int SetMaxVelocity(int BoardNumber, int MaxVelocity)
    {
		int BytesSent;
		char payload[2];
		CharBuff packetToBeSent;
		SckID=boardSckID[BoardNumber];

		//Sends request
		intToBytes(MaxVelocity, payload, sizeof(payload));

		buildTCPCommand(&packetToBeSent, SET_MAX_VELOCITY, payload);
		BytesSent = SendTCP(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		else
			return 0;
	}

//******************************************************************************

	DECLDIR int GetMaxVelocity(int BoardNumber)
    {
		int BytesSent, BytesReceived, offset, maxVel;
		CharBuff packetToBeSent, packetToReceive;
		SckID=boardSckID[BoardNumber];

		//Sends request
		buildTCPCommand(&packetToBeSent, GET_MAX_VELOCITY, NULL);
		BytesSent = SendTCP(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;

		//Receives answer
		BytesReceived = ReceiveTCP(SckID, &packetToReceive);
		if (BytesReceived == -1)
			return -1;

		offset = getPayloadOffset(getDescriptor_TCPRep(REPLY_MAX_VELOCITY));
		bytesToInt(&packetToReceive.content[offset], 2, &maxVel);

		return maxVel;
	}

//******************************************************************************

	DECLDIR int SetMaxTorque(int BoardNumber, int MaxTorque)
    {
		int BytesSent;
		char payload[2];
		CharBuff packetToBeSent;
		SckID=boardSckID[BoardNumber];

		//Sends request
		intToBytes(MaxTorque, payload, sizeof(payload));

		buildTCPCommand(&packetToBeSent, SET_MAX_TORQUE, payload);
		BytesSent = SendTCP(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		else
			return 0;
	}

//******************************************************************************

	DECLDIR int GetMaxTorque(int BoardNumber)
    {
		int BytesSent, BytesReceived, offset, maxT;
		CharBuff packetToBeSent, packetToReceive;
		SckID=boardSckID[BoardNumber];

		//Sends request	
		buildTCPCommand(&packetToBeSent, GET_MAX_TORQUE, NULL);
		BytesSent = SendTCP(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;

		//Receives answer
		BytesReceived = ReceiveTCP(SckID, &packetToReceive);
		if (BytesReceived == -1)
			return -1;

		offset = getPayloadOffset(getDescriptor_TCPRep(REPLY_MAX_TORQUE));
		bytesToInt(&packetToReceive.content[offset], 2, &maxT);

		return maxT;
	}

//******************************************************************************

	DECLDIR int SetMinVelocity(int BoardNumber, int MinVelocity)
    {
		int BytesSent;
		char payload[2];
		CharBuff packetToBeSent;
		SckID=boardSckID[BoardNumber];

		//Sends request
		intToBytes(MinVelocity, payload, sizeof(payload));

		buildTCPCommand(&packetToBeSent, SET_MIN_VELOCITY, payload);
		BytesSent = SendTCP(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		else
			return 0;
	}

//******************************************************************************

	DECLDIR int GetMinVelocity(int BoardNumber)
    {
		int BytesSent, BytesReceived, offset, minVel;
		CharBuff packetToBeSent, packetToReceive;
		SckID=boardSckID[BoardNumber];

		//Sends request
		buildTCPCommand(&packetToBeSent, GET_MIN_VELOCITY, NULL);
		BytesSent = SendTCP(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;

		//Receives answer
		BytesReceived = ReceiveTCP(SckID, &packetToReceive);
		if (BytesReceived == -1)
			return -1;

		offset = getPayloadOffset(getDescriptor_TCPRep(REPLY_MIN_VELOCITY));
		bytesToInt(&packetToReceive.content[offset], 2, &minVel);

		return minVel;
	}

//******************************************************************************

	DECLDIR int SetCurrentLimit(int BoardNumber, int CurrentLimit)
    {
		int BytesSent;
		char payload[2];
		CharBuff packetToBeSent;
		SckID=boardSckID[BoardNumber];

		//Sends request
		intToBytes(CurrentLimit, payload, sizeof(payload));

		buildTCPCommand(&packetToBeSent, SET_CURRENT_LIMIT, payload);
		BytesSent = SendTCP(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		else
			return 0;
	}

//******************************************************************************

	DECLDIR int GetCurrentLimit(int BoardNumber)
    {
		int BytesSent, BytesReceived, offset, iLim;
		CharBuff packetToBeSent, packetToReceive;
		SckID=boardSckID[BoardNumber];

		//Sends request
		buildTCPCommand(&packetToBeSent, GET_CURRENT_LIMIT, NULL);
		BytesSent = SendTCP(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;

		//Receives answer
		BytesReceived = ReceiveTCP(SckID, &packetToReceive);
		if (BytesReceived == -1)
			return -1;

		offset = getPayloadOffset(getDescriptor_TCPRep(REPLY_CURRENT_LIMIT));
		bytesToInt(&packetToReceive.content[offset], 2, &iLim);

		return iLim;
	}

//******************************************************************************

//DECLDIR int SetBCastRate(int BoardNumber, int BCastRate, char TwinSatus)
DECLDIR int SetBCastRate(int BoardNumber, int BCastRate)
		/*First payload byte: how often the board broadcast the packets (in miliseconds)

		Second payload byte: only the last 2 bits care. The LSB one able or not the board to send the packets (Default: 0).
							   The other one able or not the board to send a "reply" command when it receives a "set" command (Default: 0).*/
    {
		int BytesSent;
		char payload[2];
		CharBuff packetToBeSent;
		SckID=boardSckID[BoardNumber];

		//Sends request
		payload[0] = BCastRate;
		payload[1] = 1;//the "1" means when someone set the broadcast rate the board will be automatically set to send packets.
		//payload[1] = TwinSatus;//Set the transmission status(0=notransmissio, 1=Transmit to PC, 8=Transmit To twin, 9=Transmit to both)
		buildTCPCommand(&packetToBeSent, SET_BCAST_RATE, payload);
		BytesSent = SendTCP(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		else
			return 0;
	}

DECLDIR int StopBCast(int BoardNumber)
{
		int BytesSent;
		char payload[2];
		CharBuff packetToBeSent;
		SckID=boardSckID[BoardNumber];

		//Sends request
		payload[0] = 10;   // this can be anything since the boards will stop  broadcasting
		payload[1] = 0;  //"0" means stop the broadcast 
		buildTCPCommand(&packetToBeSent, SET_BCAST_RATE, payload);
		BytesSent = SendTCP(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		else
			return 0;
}

//******************************************************************************

	DECLDIR int GetBCastRate(int BoardNumber)
    {
		int BytesSent, BytesReceived, offset, bc;
		CharBuff packetToBeSent, packetToReceive;
		SckID=boardSckID[BoardNumber];

		//Sends request
		buildTCPCommand(&packetToBeSent, GET_BCAST_RATE, NULL);
		BytesSent = SendTCP(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;

		//Receives answer
		BytesReceived = ReceiveTCP(SckID, &packetToReceive);
		if (BytesReceived == -1)
			return -1;

		offset = getPayloadOffset(getDescriptor_TCPRep(REPLY_BCAST_RATE));
		bytesToInt(&packetToReceive.content[offset], 1, &bc);

		return bc;
	}

//******************************************************************************

	DECLDIR int SetBCastPolicy(int BoardNumber, int BCastPolicy)
    {
		int BytesSent;
		char payload[2];
		CharBuff packetToBeSent;
		SckID=boardSckID[BoardNumber];
		BroadCastPolicy=BCastPolicy;

		//printf("Set BCastPolicy: %*d,BCastPolicy\n");

		//Sends request
		intToBytes(BCastPolicy, payload, sizeof(payload));

		buildTCPCommand(&packetToBeSent, SET_BCAST_POLICY, payload);
		BytesSent = SendTCP(SckID, &packetToBeSent);


		int extraBCastPolicy=3584;
		//Sends request
		intToBytes(extraBCastPolicy, payload, sizeof(payload));

		buildTCPCommand(&packetToBeSent, SET_EXTRA_BCAST_POLICY, payload);
		BytesSent = SendTCP(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		else
			return 0;
		
	}

////******************************************************************************
//
//		DECLDIR int SetExtraBCastPolicy(int BoardNumber)
//    {
//		int BytesSent;
//		char payload[2];
//		CharBuff packetToBeSent;
//		SckID=boardSckID[BoardNumber];
//	
//		int extraBCastPolicy=3584;
//		//Sends request
//		intToBytes(extraBCastPolicy, payload, sizeof(payload));
//
//		buildTCPCommand(&packetToBeSent, SET_EXTRA_BCAST_POLICY, payload);
//		BytesSent = SendTCP(SckID, &packetToBeSent);
//		if (BytesSent == -1)
//			return -1;
//		else
//			return 0;
//	}

//******************************************************************************

	DECLDIR int GetBCastPolicy(int BoardNumber)
    {
		int BytesSent, BytesReceived, offset, bc;
		CharBuff packetToBeSent, packetToReceive;
		SckID=boardSckID[BoardNumber];

		//Sends request		
		buildTCPCommand(&packetToBeSent, GET_BCAST_POLICY, NULL);
		BytesSent = SendTCP(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;

		//Receives answer
		BytesReceived = ReceiveTCP(SckID, &packetToReceive);
		if (BytesReceived == -1)
			return -1;

		offset = getPayloadOffset(getDescriptor_TCPRep(REPLY_BCAST_POLICY));
		bytesToInt(&packetToReceive.content[offset], 2, &bc);

		return bc;
	}

//******************************************************************************

	DECLDIR int SetEncoderLines(int BoardNumber, long EncoderLines)
    {
		int BytesSent;
		char payload[4];
		CharBuff packetToBeSent;
		SckID=boardSckID[BoardNumber];

		//Sends request
		longToBytes(EncoderLines, payload, sizeof(payload));

		buildTCPCommand(&packetToBeSent, SET_ENCODER_LINES, payload);
		BytesSent = SendTCP(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		else
			return 0;
	}

//******************************************************************************

	DECLDIR long GetEncoderLines(int BoardNumber)
    {
		int BytesSent, BytesReceived, offset;
		long el;
		CharBuff packetToBeSent, packetToReceive;
		SckID=boardSckID[BoardNumber];

		//Sends request
		buildTCPCommand(&packetToBeSent, GET_ENCODER_LINES, NULL);
		BytesSent = SendTCP(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;

		//Receives answer
		BytesReceived = ReceiveTCP(SckID, &packetToReceive);
		if (BytesReceived == -1)
			return -1;

		offset = getPayloadOffset(getDescriptor_TCPRep(REPLY_ENCODER_LINES));
		bytesToLong(&packetToReceive.content[offset], 4, &el);

		return el;
	}

//******************************************************************************

	DECLDIR int SetMotorPoles(int BoardNumber, char poles)
    {
		int BytesSent;
		CharBuff packetToBeSent;
		SckID=boardSckID[BoardNumber];

		//Sends request
		buildTCPCommand(&packetToBeSent, SET_MOTOR_POLES, &poles);
		BytesSent = SendTCP(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		else
			return 0;
	}

//******************************************************************************

	DECLDIR int GetMotorPoles(int BoardNumber)
    {
		int BytesSent, BytesReceived, offset, mp;
		CharBuff packetToBeSent, packetToReceive;
		SckID=boardSckID[BoardNumber];

		//Sends request
		buildTCPCommand(&packetToBeSent, GET_MOTOR_POLES, NULL);
		BytesSent = SendTCP(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;

		//Receives answer
		BytesReceived = ReceiveTCP(SckID, &packetToReceive);
		if (BytesReceived == -1)
			return -1;

		offset = getPayloadOffset(getDescriptor_TCPRep(REPLY_MOTOR_POLES));
		bytesToInt(&packetToReceive.content[offset], 1, &mp);

		return mp;
	}

//******************************************************************************

	DECLDIR int SetAnalogInputs(int BoardNumber, char anInputs)
    {
		int BytesSent;
		CharBuff packetToBeSent;
		SckID=boardSckID[BoardNumber];

		//Sends request
		buildTCPCommand(&packetToBeSent, SET_ANALOG_INPUTS, &anInputs);
		BytesSent = SendTCP(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		else
			return 0;
	}
//******************************************************************************
	DECLDIR int DoRecalculatePosition(int BoardNumber, char Calib)
    {
		int BytesSent;
		CharBuff packetToBeSent;
		SckID=boardSckID[BoardNumber];

		//Sends request
		buildTCPCommand(&packetToBeSent, DO_RECALCULATE_POSITION, &Calib);
		BytesSent = SendTCP(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		else
			return 0;
	}

//******************************************************************************

	DECLDIR int GetAnalogInputs(int BoardNumber)
    {
		int BytesSent, BytesReceived, offset, ai;
		CharBuff packetToBeSent, packetToReceive;
		SckID=boardSckID[BoardNumber];

		//Sends request		
		buildTCPCommand(&packetToBeSent, GET_ANALOG_INPUTS, NULL);
		BytesSent = SendTCP(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;

		//Receives answer
		BytesReceived = ReceiveTCP(SckID, &packetToReceive);
		if (BytesReceived == -1)
			return -1;

		offset = getPayloadOffset(getDescriptor_TCPRep(REPLY_ANALOG_INPUTS));
		bytesToInt(&packetToReceive.content[offset], 1, &ai);

		return ai;
	}

//******************************************************************************

	DECLDIR int CmdUpgrade(int BoardNumber)
    {
		int BytesSent;
		CharBuff packetToBeSent;
		SckID=boardSckID[BoardNumber];

		//Sends request		
		buildTCPCommand(&packetToBeSent, CMD_UPGRADE, NULL);
		BytesSent = SendTCP(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		else
			return 0;
	}

//******************************************************************************

	DECLDIR int SaveParamsToFlash(int BoardNumber)
    {
		int BytesSent;
		CharBuff packetToBeSent;
		SckID=boardSckID[BoardNumber];

		//Sends request		
		buildTCPCommand(&packetToBeSent, SAVE_PARAMS_TO_FLASH, NULL);
		BytesSent = SendTCP(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		else
			return 0;
	}

//******************************************************************************

	DECLDIR int LoadParamsFromFlash(int BoardNumber) //TODO: does it receive any answer?
    {
		int BytesSent;
		CharBuff packetToBeSent;
		SckID=boardSckID[BoardNumber];

		//Sends request		
		buildTCPCommand(&packetToBeSent, LOAD_PARAMS_FROM_FLASH, NULL);
		BytesSent = SendTCP(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		else
			return 0;
	}

//******************************************************************************

	DECLDIR int LoadDefaultParams(int BoardNumber)
    {
		int BytesSent;
		CharBuff packetToBeSent;
		SckID=boardSckID[BoardNumber];

		//Sends request		
		buildTCPCommand(&packetToBeSent, LOAD_DEFAULT_PARAMS, NULL);
		BytesSent = SendTCP(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		else
			return 0;
	}

//******************************************************************************

    DECLDIR int ControllerRun(int BoardNumber)
    {
		int BytesSent;
		CharBuff packetToBeSent;
		SckID=boardSckID[BoardNumber];

		//Sends request		
		buildTCPCommand(&packetToBeSent, CONTROLLER_RUN, NULL);
		BytesSent = SendTCP(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		else
			return 0;
	}

//******************************************************************************

    DECLDIR int ControllerIdle(int BoardNumber)
	{
		int BytesSent;
		CharBuff packetToBeSent;
		SckID=boardSckID[BoardNumber];

		//Sends request
		buildTCPCommand(&packetToBeSent, CONTROLLER_IDLE, NULL);
		BytesSent = SendTCP(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		else
			return 0;
	}

//******************************************************************************

	DECLDIR int EnablePwmPad(int BoardNumber)
	{
		int BytesSent;
		CharBuff packetToBeSent;
		SckID=boardSckID[BoardNumber];

		//Sends request
		buildTCPCommand(&packetToBeSent, ENABLE_PWM_PAD, NULL);
		BytesSent = SendTCP(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		else
			return 0;
	}

//******************************************************************************

	DECLDIR int DisablePwmPad(int BoardNumber)
	{
		int BytesSent;
		CharBuff packetToBeSent;
		SckID=boardSckID[BoardNumber];

		//Sends request
		buildTCPCommand(&packetToBeSent, DISABLE_PWM_PAD, NULL);
		BytesSent = SendTCP(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		else
			return 0;
	}

//******************************************************************************

	DECLDIR int DoCalibrate(int BoardNumber, int direction, long initPos)
	{
		int BytesSent;
		char payload[5];
		CharBuff packetToBeSent;
		SckID=boardSckID[BoardNumber];

		//Sends request
		payload[0] = direction;
		longToBytes(initPos, payload+1, sizeof(payload)-1);

		buildTCPCommand(&packetToBeSent, DO_CALIBRATE, payload);
		BytesSent = SendTCP(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		else
			return 0;
	}

//******************************************************************************

	DECLDIR int SetCalibrationCurrent(int BoardNumber, int i)
    {
		int BytesSent;
		char payload[2];
		CharBuff packetToBeSent;
		SckID=boardSckID[BoardNumber];

		//Sends request
		intToBytes(i, payload, sizeof(payload));

		buildTCPCommand(&packetToBeSent, SET_CALIBRATION_CURRENT, payload);
		BytesSent = SendTCP(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		else
			return 0;
	}

//******************************************************************************

	DECLDIR int GetCalibrationCurrent(int BoardNumber)
	{
		int BytesSent, BytesReceived, offset, i;
		CharBuff packetToBeSent, packetToReceive;
		SckID=boardSckID[BoardNumber];

		//Sends request
		buildTCPCommand(&packetToBeSent, GET_CALIBRATION_CURRENT, NULL);
		BytesSent = SendTCP(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
	
		//Receives answer
		BytesReceived = ReceiveTCP(SckID, &packetToReceive);
		if (BytesReceived == -1)
			return -1;

		offset = getPayloadOffset(getDescriptor_TCPRep(REPLY_CALIBRATION_CURRENT));
		bytesToInt(&packetToReceive.content[offset], 2, &i);

		return i;
	}

//******************************************************************************

	DECLDIR int SetAbsoluteZero(int BoardNumber, int absZero)
	{
		int BytesSent;
		char payload[2];
		CharBuff packetToBeSent;
		SckID=boardSckID[BoardNumber];

		//Sends request
		intToBytes(absZero, payload, sizeof(payload));

		buildTCPCommand(&packetToBeSent, SET_ABSOLUTE_ZERO, payload);
		BytesSent = SendTCP(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		else
			return 0;
	}


//******************************************************************************

	DECLDIR int GetAbsoluteZero(int BoardNumber)
    {
		int BytesSent, BytesReceived, offset, az;
		CharBuff packetToBeSent, packetToReceive;
		SckID=boardSckID[BoardNumber];

		//Sends request
		buildTCPCommand(&packetToBeSent, GET_ABSOLUTE_ZERO, NULL);
		BytesSent = SendTCP(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;

		//Receives answer
		BytesReceived = ReceiveTCP(SckID, &packetToReceive);
		if (BytesReceived == -1)
			return -1;

		offset = getPayloadOffset(getDescriptor_TCPRep(REPLY_ABSOLUTE_ZERO));
		bytesToInt(&packetToReceive.content[offset], 2, &az);

		return az;
	}

//******************************************************************************

	DECLDIR int SetTorqueAccel(int BoardNumber, char tAccel)
	{
		int BytesSent;
		CharBuff packetToBeSent;
		SckID=boardSckID[BoardNumber];

		//Sends request
		buildTCPCommand(&packetToBeSent, SET_TORQUE_ACCEL, &tAccel);
		BytesSent = SendTCP(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		else
			return 0;
	}


//******************************************************************************

	DECLDIR int GetTorqueAccel(int BoardNumber)
    {
		int BytesSent, BytesReceived, offset, ta;
		CharBuff packetToBeSent, packetToReceive;
		SckID=boardSckID[BoardNumber];

		//Sends request
		buildTCPCommand(&packetToBeSent, GET_TORQUE_ACCEL, NULL);
		BytesSent = SendTCP(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;

		//Receives answer
		BytesReceived = ReceiveTCP(SckID, &packetToReceive);
		if (BytesReceived == -1)
			return -1;

		offset = getPayloadOffset(getDescriptor_TCPRep(REPLY_TORQUE_ACCEL));
		bytesToInt(&packetToReceive.content[offset], 2, &ta);

		return ta;
	}

//******************************************************************************

	//MotorType: 1 for Brushed DC Motor and 0 for Brushless DC Motor
	DECLDIR int SetMotorType(int BoardNumber, char motorType)
	{
		int BytesSent;
		CharBuff packetToBeSent;
		SckID=boardSckID[BoardNumber];

		//Sends request
		buildTCPCommand(&packetToBeSent, SET_MOTOR_TYPE , &motorType);
		BytesSent = SendTCP(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		else
			return 0;

	}

//******************************************************************************

	DECLDIR int GetMotorType(int BoardNumber)
	{
		int BytesSent, BytesReceived, offset, mt;
		CharBuff packetToBeSent, packetToReceive;
		SckID=boardSckID[BoardNumber];

		//MotorType: 1 for Brushed DC Motor and 0 for Brushless DC Motor

		//Sends request
		buildTCPCommand(&packetToBeSent, GET_MOTOR_TYPE , NULL);
		BytesSent = SendTCP(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;

		//Receives answer
		BytesReceived = ReceiveTCP(SckID, &packetToReceive);
		if (BytesReceived == -1)
			return -1;

		offset = getPayloadOffset(getDescriptor_TCPRep(REPLY_MOTOR_TYPE));
		bytesToInt(&packetToReceive.content[offset], 1, &mt);

		return mt;

	}

//******************************************************************************

	DECLDIR void CloseSockets(void)
	{
		int b=0;
		for(b=0;b<MAX_BOARDS;b++)
			closesocket(boardSckID[b]);
		closesocket(UDPSckID);
	}


DECLDIR int SetMinAbsPosition(int BoardNumber, int minAbsPos)
	{
		int BytesSent;
		char payload[2];
		CharBuff packetToBeSent;
		SckID=boardSckID[BoardNumber];

		//Sends request
		intToBytes(minAbsPos, payload, sizeof(payload));

		buildTCPCommand(&packetToBeSent, SET_MIN_ABS_POSITION, payload);
		BytesSent = SendTCP(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		else
			return 0;
	}


//******************************************************************************

	DECLDIR int GetMinAbsPosition(int BoardNumber)
    {
		int BytesSent, BytesReceived, offset, minAbsPos;
		CharBuff packetToBeSent, packetToReceive;
		SckID=boardSckID[BoardNumber];

		//Sends request
		buildTCPCommand(&packetToBeSent, GET_MIN_ABS_POSITION, NULL);
		BytesSent = SendTCP(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;

		//Receives answer
		BytesReceived = ReceiveTCP(SckID, &packetToReceive);
		if (BytesReceived == -1)
			return -1;

		offset = getPayloadOffset(getDescriptor_TCPRep(REPLY_MIN_ABS_POSITION));
		bytesToInt(&packetToReceive.content[offset], 2, &minAbsPos);

		return minAbsPos;
	}

	DECLDIR int SetMaxAbsPosition(int BoardNumber, int maxAbsPos)
	{
		int BytesSent;
		char payload[2];
		CharBuff packetToBeSent;
		SckID=boardSckID[BoardNumber];

		//Sends request
		intToBytes(maxAbsPos, payload, sizeof(payload));

		buildTCPCommand(&packetToBeSent, SET_MAX_ABS_POSITION, payload);
		BytesSent = SendTCP(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		else
			return 0;
	}


//******************************************************************************

	DECLDIR int GetMaxAbsPosition(int BoardNumber)
    {
		int BytesSent, BytesReceived, offset, maxAbsPos;
		CharBuff packetToBeSent, packetToReceive;
		SckID=boardSckID[BoardNumber];

		//Sends request
		buildTCPCommand(&packetToBeSent, GET_MAX_ABS_POSITION, NULL);
		BytesSent = SendTCP(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;

		//Receives answer
		BytesReceived = ReceiveTCP(SckID, &packetToReceive);
		if (BytesReceived == -1)
			return -1;

		offset = getPayloadOffset(getDescriptor_TCPRep(REPLY_MAX_ABS_POSITION));
		bytesToInt(&packetToReceive.content[offset], 2, &maxAbsPos);

		return maxAbsPos;
	}




//******************************************************************************
// -------------------------------- UDP ---------------------------------------
//******************************************************************************

	//DECLDIR int ConnectUDP(void)
	//{
	//	int iOptVal, iOptLen, iOptVal2;
	//	char ip_addr_str[20] = "255.255.255.255"; //Broadcast address
	//	//char ip_addr_str[20] = "169.254.89.74"; //Board address

	//	//init(4);
	//	
	//	// Initialize Winsock version 2.2
	//	WSADATA wsaData;
	//	
	//	if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0)
	//	{
	//		WSACleanup();
	//		return -4;
	//	}

	//	// Create a new socket to receive datagrams on.

	//	UDPSckID = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	//	if (UDPSckID == INVALID_SOCKET) //Error at socket()
	//	{
	//		  WSACleanup();
	//		  return -1;
	//	}

	//	iOptVal = 1;
	//	iOptLen = sizeof(int);


	//	if (setsockopt(UDPSckID, SOL_SOCKET, SO_BROADCAST, (char*)&iOptVal, iOptLen) == SOCKET_ERROR) //Allow Broadcast Failed


	//	//	return -2;
	//	          //  iOptVal = 1;
 //           //iOptVal2=200;//set receive buffer to make communication faster
 //           //iOptLen = sizeof(int);
 //           
 //           //original  if (setsockopt(UDPSckID, SOL_SOCKET, SO_BROADCAST, (char*)&iOptVal, iOptLen) == SOCKET_ERROR) //Allow Broadcast Failed
 //           //if (setsockopt(UDPSckID, SOL_SOCKET, SO_BROADCAST, (char*)&iOptVal, iOptLen) == 
 //           //      SOCKET_ERROR||setsockopt(UDPSckID, SOL_SOCKET, SO_RCVBUF, (char*)&iOptVal2, iOptLen) == SOCKET_ERROR) //Allow Broadcast Failed
 //     
 //                 return -2;


	//	ReceiverAddr.sin_family = AF_INET;
	//    ReceiverAddr.sin_port = htons(Port);
	// 	ReceiverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	//    // Associate the address information with the socket using bind.
	//    // At this point you can receive datagrams on your bound socket.

	//	
	//	if (bind(UDPSckID, (SOCKADDR *)&ReceiverAddr, sizeof(ReceiverAddr)) == SOCKET_ERROR) //Error at bind()
	//    {
	//		closesocket(UDPSckID);
	//		WSACleanup();
	//		return -3;
	//	}

	//	ReceiverAddr.sin_addr.s_addr = inet_addr(ip_addr_str);

	//	return UDPSckID;
	//}




	//-------------------------------------------------------------------------------

	DECLDIR int ConnectUDP(void)
	{
		printf("Create UDP socket bounded on address {INADDR_ANY ; port 2000} [EXOSBoardLibrary::ConnectUDP]\n");
		int iOptVal, iOptLen, iOptVal2;
		char ip_addr_str[20] = "255.255.255.255"; //Broadcast address
		//char ip_addr_str[20] = "169.254.89.51"; //Board address

		//init(4);

		// Initialize Winsock version 2.2
		WSADATA wsaData;

		if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0)
		{
			WSACleanup();
			return -4;
		}

		// Create a new socket to receive datagrams on.

		UDPSckID = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

		if (UDPSckID == INVALID_SOCKET) //Error at socket()
		{
			WSACleanup();
			return -1;
		}

		//Set socket options
		iOptVal = 1;
		iOptVal2=100;//set receive buffer to make communication faster
		iOptLen = sizeof(int);

		if (setsockopt(UDPSckID, SOL_SOCKET, SO_BROADCAST, (char*)&iOptVal, iOptLen) == SOCKET_ERROR) //Allow Broadcast Failed
		if (setsockopt(UDPSckID, SOL_SOCKET, SO_BROADCAST, (char*)&iOptVal, iOptLen) == 
			SOCKET_ERROR||setsockopt(UDPSckID, SOL_SOCKET, SO_RCVBUF, (char*)&iOptVal2, iOptLen) == SOCKET_ERROR) //Allow Broadcast Failed

			return -2;


		//io - local= addr binded to socket, != from addr used to send udp
		SOCKADDR_IN LocalAddr;
		memset((void*)&LocalAddr,0,sizeof(LocalAddr));
		LocalAddr.sin_family=AF_INET;
		//LocalAddr.sin_port = htons(2000);//2000
		LocalAddr.sin_port = htons(Port);//initial
	 	//LocalAddr.sin_addr.s_addr = inet_addr("172.16.1.10");//cf linux
		LocalAddr.sin_addr.s_addr = htonl(INADDR_ANY);//initial
		//LocalAddr.sin_addr.s_addr = inet_addr("172.16.1.50"); //I specified this since I am working with more than one network interface

	    // Associate the address information with the socket using bind.
	    // At this point you can receive datagrams on your bound socket.

		//if (bind(UDPSckID, (SOCKADDR *)&ReceiverAddr, sizeof(ReceiverAddr)) == SOCKET_ERROR) //initial
			if (bind(UDPSckID, (SOCKADDR *)&LocalAddr, sizeof(LocalAddr)) == SOCKET_ERROR) //io
	    {
			closesocket(UDPSckID);
			WSACleanup();
			return -3;
		}

		//ReceiverAddr used in SendUDP
		ReceiverAddr.sin_family = AF_INET;
	    ReceiverAddr.sin_port = htons(Port);
		ReceiverAddr.sin_addr.s_addr = inet_addr(ip_addr_str); //initial
		//ReceiverAddr.sin_addr.s_addr = INADDR_BROADCAST;//cf linux


		//init
		//ReceiverAddr.sin_family = AF_INET;
		//ReceiverAddr.sin_port = htons(Port);
		//ReceiverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

		//// Associate the address information with the socket using bind.
		//// At this point you can receive datagrams on your bound socket.

		//if (bind(UDPSckID, (SOCKADDR *)&ReceiverAddr, sizeof(ReceiverAddr)) == SOCKET_ERROR) //Error at bind()
		//{
		//	closesocket(UDPSckID);
		//	WSACleanup();
		//	return -3;
		//}

		//ReceiverAddr.sin_addr.s_addr = inet_addr(ip_addr_str);

		return UDPSckID;
	}
	//-------------------------------------------------------------------------------



//******************************************************************************

	DECLDIR int SendUDP(SOCKET UDPSckID, CharBuff* packetToBeSent)
	{
		int BytesSent, len;
		SOCKADDR_IN SrcInfo;
		
		//Send packet
		BytesSent = sendto(UDPSckID, packetToBeSent->content, packetToBeSent->size, 0, (SOCKADDR *)&ReceiverAddr, sizeof(ReceiverAddr));

		// Some info on the receiver side...

		// Allocate the required resources
		memset(&SrcInfo, 0, sizeof(SrcInfo));
		len = sizeof(SrcInfo);
		getsockname(UDPSckID, (SOCKADDR *)&SrcInfo, &len);

		//Some info on the sender side
		getpeername(UDPSckID, (SOCKADDR *)&ReceiverAddr, (int *)sizeof(ReceiverAddr));

		return BytesSent;
	}

//******************************************************************************

	DECLDIR int recvfromTimeOutUDP(SOCKET socket, long sec, long usec)

	{
		// Setup timeval variable
		struct timeval timeout;
		struct fd_set fds;

		timeout.tv_sec = sec;
		timeout.tv_usec = usec;

		// Setup fd_set structure
		FD_ZERO(&fds);
		FD_SET(socket, &fds);

		// Return value:
		// -1: error occurred
		// 0: timed out
		// > 0: data ready to be read
		return select(0, &fds, 0, 0, &timeout);
	}

//******************************************************************************

	DECLDIR int ReceiveUDP(SOCKET UDPSckID, CharBuff* packetToReceive)
	{
		SOCKADDR_IN SenderAddr;
		int BytesReceived, SelectTiming, SenderAddrSize;
		
		SelectTiming = recvfromTimeOutUDP(UDPSckID, 1, 100);
		
		BytesReceived=0;
	//	SelectTiming=2;

     	SenderAddrSize = sizeof(SenderAddr);

			 switch (SelectTiming)
			 {
				 case 0:
					 return -2;
					 break;
				 case -1:
					 return -3;
					 break;
				 default:
					 BytesReceived = recvfrom(UDPSckID, packetToReceive->content, sizeof(packetToReceive->content),  0, (SOCKADDR *)&SenderAddr, &SenderAddrSize);
					 packetToReceive->size = BytesReceived;
			 }

		return BytesReceived;
	}

//******************************************************************************

//******************************************************************************

	DECLDIR int ReceiveUDPWithTimeOut(SOCKET UDPSckID, CharBuff* packetToReceive)
	{
		SOCKADDR_IN SenderAddr;
		int BytesReceived, SelectTiming, SenderAddrSize;
		
		SelectTiming = recvfromTimeOutUDP(UDPSckID, 2, 1000);
		
		

     	SenderAddrSize = sizeof(SenderAddr);

			 switch (SelectTiming)
			 {
				 case 0:
					 return -2;
					 break;
				 case -1:
					 return -3;
					 break;
				 default:
					 BytesReceived = recvfrom(UDPSckID, packetToReceive->content, sizeof(packetToReceive->content),  0, (SOCKADDR *)&SenderAddr, &SenderAddrSize);
					 packetToReceive->size = BytesReceived;
			 }

		return BytesReceived;
	}

//******************************************************************************




	DECLDIR int WriteBCastData(FILE * stream)
	{
		int BytesReceived, i;
		CharBuff packetToReceive;
		
		
		//TODO: I don't know why, but we can receive broadcast data just if we send some data first.
		//I chose to send the command "get active boards" (called in the interface)
		
/*		int activeBoards = GetActiveBoards();
		if (activeBoards == -1)
			return -1;
		BytesReceived = ReceiveUDP(UDPSckID, &packetToReceive); */

		BytesReceived = ReceiveUDP(UDPSckID, &packetToReceive);

		if (BytesReceived < 0)
			return BytesReceived;
		else
		{
			for (i=0;i<BytesReceived;i++) //initial position = 4: discards the header and takes just the interesting data
			{
				int x = (unsigned char) packetToReceive.content[i];
				fprintf(stream,"%d, ", x );
			}
			fprintf(stream,"\n");
		}

		return 0;
		
	}


 DECLDIR int GetBCastData(CharBuff &packetToReceive)
	{
		int BytesReceived;
		
		//TODO: I don't know why, but we can receive broadcast data just if we send some data first.
		//I chose to send the command "get active boards" (called in the interface)
		
	/*	int activeBoards = GetActiveBoards();
		if (activeBoards == -1)
			return -1;
		BytesReceived = ReceiveUDP(UDPSckID, &packetToReceive); */

		BytesReceived = ReceiveUDP(UDPSckID, &packetToReceive);
		return BytesReceived;
		
	}

//*********************************************************************
DECLDIR int GetData(void)
{
	//printf("[ExosBoardLibrary::GetData] \n");
	int bytesReceived=0;
	int boardNo;
	int pktRead=0;
	int sensNum=0;
	CharBuff packetToReceive;

	union float_convert
	{
		unsigned char cval[4];
		float fval;
	}fnum_co;

	float data_array[6];
	//bcast_policy=GetBCastPolicy(BoardID);
	
	//////int bytesReceived=0;
	//////int boardNo;
	//////int pktRead=0;
	//////int PcCnt=0;

	//////for(int pkt=0;pkt<noActiveBoards+1;pkt++)
	//////	if((bytesReceived=GetBCastData(packetToReceive))>0)
	//////	{
	//////		if(packetToReceive.content[2] == (char)0xbb)   // this is a boradcast packet
	//////		{


	for(int pkt=0;pkt<numberOfActiveBoards;pkt++)
		if((bytesReceived=GetBCastData(packetToReceive))>0)
		{

			//------------------------   parse Hand Exoskeleton board data protocol-------------------------
			//----------------------------------------------------------------------------------------------
			//----------------------------------------------------------------------------------------------	
			if(packetToReceive.content[0] == (char)0xfd && packetToReceive.content[2] == (char)0xbd)   // this is a Hand Exoskeleton broadcast packet
			{

				//boardNo=packetToReceive.content[3]-1;//init
				boardNo=packetToReceive.content[3];
				//printf("[GetData] packetToReceive.content[3]: %i \n",packetToReceive.content[3]);

				//------------------------   Parse Broadcast Data  ---------------------------------------------
				//----------------------------------------------------------------------------------------------
				//----------------------------------------------------------------------------------------------

				int PcCnt=3;
				if (BroadCastPolicy&0x1){//check if bit 0 of the BCAST_POLICY is asserted
					for (int i=0; i<3; i++ )
					{
						Exos.ExosFinger[boardNo-1].M_pos[i]=(unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+2]<<8)& 0xff00) 
							+((packetToReceive.content[PcCnt+3]<<16)& 0xff0000)+((packetToReceive.content[PcCnt+4]<<24)& 0xff000000);
						PcCnt=PcCnt+4;
						//Exos.ExosFinger[boardNo-1].M_pos[i]=i+1;

					}
				}
				if (BroadCastPolicy&0x2){//check if bit 1 of the BCAST_POLICY is asserted
					for (int i=0; i<3; i++ )
					{
						Exos.ExosFinger[boardNo-1].M_vel[i]= ((unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+2]<<8)& 0xff00));
						/*
						if(((packetToReceive.content[PcCnt+2])& 0x80)>0) Exos.ExosFinger[boardNo-1].M_vel[i]= ((unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+2]<<8)& 0xff00))-0xffff;
						else Exos.ExosFinger[boardNo-1].M_vel[i]= ((unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+2]<<8)& 0xff00));*/

						PcCnt=PcCnt+2;
					}
				}
				if (BroadCastPolicy&0x4){//check if bit 2 of the BCAST_POLICY is asserted
					for (int i=0; i<3; i++ )
					{
						Exos.ExosFinger[boardNo-1].M_tor[i]=(unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+2]<<8)& 0xff00);								
						/*if(((packetToReceive.content[PcCnt+2])& 0x80)>0) Exos.ExosFinger[boardNo-1].M_tor[i]= ((unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+1]<<8)& 0xff00))-0xffff;
						else Exos.ExosFinger[boardNo-1].M_tor[i]= ((unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+1]<<8)& 0xff00));*/
						//if (i==2)
					//	printf("\t %*d \t %*d \t %*d\n",7,Exos.ExosFinger[boardNo-1].M_tor[0],7,Exos.ExosFinger[boardNo-1].M_tor[1],7,Exos.ExosFinger[boardNo-1].M_tor[2]);
							//printf("\t %d \t %d \t %d \n",Exos.ExosFinger[boardNo-1].M_tor[0],Exos.ExosFinger[boardNo-1].M_tor[1],Exos.ExosFinger[boardNo-1].M_tor[2]);
						
						PcCnt=PcCnt+2;
					}
				}			
		
				if (BroadCastPolicy&0x8){//check if bit 3 of the BCAST_POLICY is asserted
					for (int i=0; i<3; i++ )
					{
						Exos.ExosFinger[boardNo-1].pidOutput[i] = (unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+1]<<8)& 0xff00);
						PcCnt=PcCnt+2;
					}
				}
				if (BroadCastPolicy&0x10){//check if bit 4 of the BCAST_POLICY is asserted
					for (int i=0; i<3; i++ )
					{
						Exos.ExosFinger[boardNo-1].pidError[i]=(unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+2]<<8)& 0xff00) 
							+((packetToReceive.content[PcCnt+3]<<16)& 0xff0000)+((packetToReceive.content[PcCnt+4]<<24)& 0xff000000);
						PcCnt=PcCnt+4;
					}
				}
				if (BroadCastPolicy&0x20){//check if bit 5 of the BCAST_POLICY is asserted
					for (int i=0; i<3; i++ )
					{
						if (((packetToReceive.content[PcCnt+4])& 0x80)>0) 
							Exos.ExosFinger[boardNo-1].realCurrent[i]=(unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+2]<<8)& 0xff00) 
							+((packetToReceive.content[PcCnt+3]<<16)& 0xff0000)+((packetToReceive.content[PcCnt+4]<<24)& 0xff000000)-0xffff;
						else
							Exos.ExosFinger[boardNo-1].realCurrent[i]=(unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+2]<<8)& 0xff00) 
							+((packetToReceive.content[PcCnt+3]<<16)& 0xff0000)+((packetToReceive.content[PcCnt+4]<<24)& 0xff000000);
						PcCnt=PcCnt+4;
					}
				}
				if (BroadCastPolicy&0x40){//check if bit 6 of the BCAST_POLICY is asserted
					Exos.ExosFinger[boardNo-1].temp_Vdc=(unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+2]<<8)& 0xff00) 
						+((packetToReceive.content[PcCnt+3]<<16)& 0xff0000)+((packetToReceive.content[PcCnt+4]<<24)& 0xff000000);
					PcCnt=PcCnt+4;
				}
				if (BroadCastPolicy&0x80){//check if bit 7 of the BCAST_POLICY is asserted
					Exos.ExosFinger[boardNo-1].tStamp =(unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+2]<<8)& 0xff00) 
						+((packetToReceive.content[PcCnt+3]<<16)& 0xff0000)+((packetToReceive.content[PcCnt+4]<<24)& 0xff000000);
					PcCnt=PcCnt+4;
				}
				if (BroadCastPolicy&0x100){//check if bit 8 of the BCAST_POLICY is asserted
					Exos.ExosFinger[boardNo-1].fault = (unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+1]<<8)& 0xff00);
					PcCnt=PcCnt+2;
				}
				//Construct analogue input selection
				int AB=2*((BroadCastPolicy&0x400)>>10)+((BroadCastPolicy&0x200)>>9);
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
						Exos.ExosFinger[boardNo-1].anInput1= ((unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+2]<<8)& 0xff00));
						PcCnt=PcCnt+2;
						Exos.ExosFinger[boardNo-1].anInput2= ((unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+2]<<8)& 0xff00));
						PcCnt=PcCnt+2;

						// anInput3 and anInput4 receive force and torque in decaN and mNm 
						//for fitting  positive and negative forces. Force up to 327.67N with a resolution of 10mN. Torque up to 32.767Nm with a resolution of 1mNm.
						if(((packetToReceive.content[PcCnt+2])& 0x80)>0) Exos.ExosFinger[boardNo-1].anInput3= ((unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+2]<<8)& 0xff00))-0xffff;
						else Exos.ExosFinger[boardNo-1].anInput3= ((unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+2]<<8)& 0xff00));
						Exos.ExosFinger[boardNo-1].Force=Exos.ExosFinger[boardNo-1].anInput3/10.0;// convert to Newtons from decaNewtons
						PcCnt=PcCnt+2;
						// decaN and mNm for fitting  positive and negative forces up to 327.67N with a resolution of 10mN.
						if(((packetToReceive.content[PcCnt+2])& 0x80)>0) Exos.ExosFinger[boardNo-1].anInput4= ((unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+2]<<8)& 0xff00))-0xffff;
						else Exos.ExosFinger[boardNo-1].anInput4= ((unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+2]<<8)& 0xff00));
						Exos.ExosFinger[boardNo-1].Torque=Exos.ExosFinger[boardNo-1].anInput4/1000.0;// convert to Nm from mNm
						PcCnt=PcCnt+2;
					}
					break;
				}
				if (BroadCastPolicy&0x800){//check if bit 11 of the BCAST_POLICY is asserted
					for (int i=0; i<3; i++ )
					{
						if(((packetToReceive.content[PcCnt+2])& 0x80)>0) Exos.ExosFinger[boardNo-1].quickSpeed[i]= ((unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+2]<<8)& 0xff00))-0xffff;
						else Exos.ExosFinger[boardNo-1].quickSpeed[i]= ((unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+2]<<8)& 0xff00));
						PcCnt=PcCnt+2;
					}
				}
				if (BroadCastPolicy&0x1000){//check if bit 12 of the BCAST_POLICY is asserted
					for (int i=0; i<3; i++ )
					{
						PcCnt=PcCnt+2;//// motor_state placeholder
					}
				}
				if (BroadCastPolicy&0x2000){//check if bit 13 of the BCAST_POLICY is asserted
					for (int i=0; i<3; i++ )
					{
						PcCnt=PcCnt+2;// real_current placeholder
					}
				}
				if (BroadCastPolicy&0x4000){//check if bit 14 of the BCAST_POLICY is asserted
					for (int i=0; i<3; i++ )
					{
						PcCnt=PcCnt+2;// relative_position placeholder
					}
				}
				if (BroadCastPolicy&0x8000){//check if bit 15 of the BCAST_POLICY is asserted
					for (int i=0; i<3; i++ )
					{
						PcCnt=PcCnt+2;// target_angle placeholder
					}
				}
				//
				//------------------------   Parse Extra Broadcast Data  ---------------------------------------

				if (EXTRA_BCAST_POLICY&0x1){//check if bit 0 of the EXTRA_BCAST_POLICY is asserted
					for (int i=0; i<3; i++ )
					{
						Exos.ExosFinger[boardNo-1].TargPos[i]= (unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+2]<<8)& 0xff00) 
							+((packetToReceive.content[PcCnt+3]<<16)& 0xff0000)+((packetToReceive.content[PcCnt+4]<<24)& 0xff000000);
						PcCnt=PcCnt+4;
					}
				}
				if (EXTRA_BCAST_POLICY&0x2){//check if bit 1 of the EXTRA_BCAST_POLICY is asserted 						

					for (int i=0; i<3; i++ )
					{
						Exos.ExosFinger[boardNo-1].TempTargPos[i]= (unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+2]<<8)& 0xff00) 
							+((packetToReceive.content[PcCnt+3]<<16)& 0xff0000)+((packetToReceive.content[PcCnt+4]<<24)& 0xff000000);
						PcCnt=PcCnt+4;
					}
				}
				if (EXTRA_BCAST_POLICY&0x4){//check if bit 2 of the EXTRA_BCAST_POLICY is asserted
					for (int i=0; i<3; i++ )
					{
						Exos.ExosFinger[boardNo-1].ReqTargPos[i]= (unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+2]<<8)& 0xff00) 
							+((packetToReceive.content[PcCnt+3]<<16)& 0xff0000)+((packetToReceive.content[PcCnt+4]<<24)& 0xff000000);
						PcCnt=PcCnt+4;
					}
				}
				if (EXTRA_BCAST_POLICY&0x8){//check if bit 3 of the EXTRA_BCAST_POLICY is asserted
					PcCnt=PcCnt+4;// twin_position placeholder
				}
				if (EXTRA_BCAST_POLICY&0x10){//check if bit 4 of the EXTRA_BCAST_POLICY is asserted
					PcCnt=PcCnt+2;//// angle placeholder
				}
				if (EXTRA_BCAST_POLICY&0x20){//check if bit 5 of the EXTRA_BCAST_POLICY is asserted
					PcCnt=PcCnt+2;// target_angle placeholder
				}
				if (EXTRA_BCAST_POLICY&0x40){//check if bit 6 of the EXTRA_BCAST_POLICY is asserted
					Exos.ExosFinger[boardNo-1].jointPos[0]=Exos.ExosFinger[boardNo-1].M_pos[0];// insert the motor position as the first joint position
					//printf("\t %d \t %d  \t %d\n",Exos.ExosFinger[boardNo-1].jointPos[0],Exos.ExosFinger[boardNo-1].jointPos[1],Exos.ExosFinger[boardNo-1].jointPos[2]);

					//Exos.ExosFinger[boardNo-1].M_pos[0]=1;
					for (int i=0; i<6; i++ )
					{
						Exos.ExosFinger[boardNo-1].jointPos[i+1]= (unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+2]<<8)& 0xff00) 
							+((packetToReceive.content[PcCnt+3]<<16)& 0xff0000)+((packetToReceive.content[PcCnt+4]<<24)& 0xff000000);
						//Exos.ExosFinger[boardNo-1].jointPos[i+1]=i+1;
						PcCnt=PcCnt+4;
					}
				}
				if (EXTRA_BCAST_POLICY&0x80){//check if bit 7 of the EXTRA_BCAST_POLICY is asserted
					for (int i=0; i<6; i++ )
					{
						Exos.ExosFinger[boardNo-1].slaveTorque[i]= (unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+2]<<8)& 0xff00) 
							+((packetToReceive.content[PcCnt+3]<<16)& 0xff0000)+((packetToReceive.content[PcCnt+4]<<24)& 0xff000000);
						PcCnt=PcCnt+4;
						}
				}
				
////When linux stops loop for some reason boardNo is set to unvalid num(-858993460) for one loop and then goes back to valid range. This avoid the program to break because tries to unvalid access
		if(boardNo==1||boardNo==2||boardNo==3)
		{
			//Encoder zero - high resolution - sent in mRad by board
			//Exos.ExosFinger[boardNo-1].jointPosRad[0]=(double)Exos.ExosFinger[boardNo-1].jointPos[0]*2*M_PI/resRelEnc;//When board is sending enc reading
			Exos.ExosFinger[boardNo-1].jointPosRad[0]=(double)(Exos.ExosFinger[boardNo-1].jointPos[0])*0.0001;//when board is sending for joint 0 pos in 1e-4 Rad
			//Rest of encoders, lower resolution - sent in enc reading by board
			for(int it=1;it<6;it++)
			{
				//printf("ExosBoardLibrary: plop \n");
				//Sleep(1);
				Exos.ExosFinger[boardNo-1].jointPosRad[it]=Exos.ExosFinger[boardNo-1].jointPos[it]*2*M_PI/resAbsEnc;//board is sending joints 1..6 pos in enc units
			}
			//printf("Pos rad properly set [ExosBoardLibrary::GetData]\n");
		}
		else
		{
			printf("Unvalid boardNo: %i - skip [ExosBoardLibrary::GetData]\n",boardNo);
		}


			}
			//------------------------   parse Accelerometer board data protocol-------------------------
			//----------------------------------------------------------------------------------------------
			//----------------------------------------------------------------------------------------------	
			if(packetToReceive.content[2] == (char)0xbb)   // this is a Motor Controller boradcast packet
			{
				boardNo=packetToReceive.content[3];
				//------------------------   Parse Broadcast Data  ---------------------------------------------
				//----------------------------------------------------------------------------------------------
				//----------------------------------------------------------------------------------------------
				//for (int i=0; i<bytesReceived; i++)// 
				//{
				//	Exos.ExosFinger[boardNo-1].PacketRaw[i]=(unsigned char)packetToReceive.content[i];
				//}

				int PcCnt=3;
				if (BroadCastPolicy&0x1){//check if bit 0 of the BCAST_POLICY is asserted
					for (int i=0; i<1; i++ )
					{
						Exos.ExosFinger[boardNo-1].M_pos[i]=(unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+2]<<8)& 0xff00) 
							+((packetToReceive.content[PcCnt+3]<<16)& 0xff0000)+((packetToReceive.content[PcCnt+4]<<24)& 0xff000000);
						PcCnt=PcCnt+4;
						//Exos.ExosFinger[boardNo-1].M_pos[i]=i+1;
					}
				}
				if (BroadCastPolicy&0x2){//check if bit 1 of the BCAST_POLICY is asserted
					for (int i=0; i<1; i++ )
					{
						if(((packetToReceive.content[PcCnt+2])& 0x80)>0) Exos.ExosFinger[boardNo-1].M_vel[i]= ((unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+2]<<8)& 0xff00))-0xffff;
						else Exos.ExosFinger[boardNo-1].M_vel[i]= ((unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+2]<<8)& 0xff00));
						PcCnt=PcCnt+2;
					}
				}
				if (BroadCastPolicy&0x4){//check if bit 2 of the BCAST_POLICY is asserted
					for (int i=0; i<1; i++ )
					{
						Exos.ExosFinger[boardNo-1].M_tor[i]= (unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+1]<<8)& 0xff00);
						PcCnt=PcCnt+2;
					}
				}				
				if (BroadCastPolicy&0x8){//check if bit 3 of the BCAST_POLICY is asserted
					for (int i=0; i<1; i++ )
					{
						Exos.ExosFinger[boardNo-1].pidOutput[i] = (unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+1]<<8)& 0xff00);
						PcCnt=PcCnt+2;
					}
				}
				if (BroadCastPolicy&0x10){//check if bit 4 of the BCAST_POLICY is asserted
					for (int i=0; i<1; i++ )
					{
						Exos.ExosFinger[boardNo-1].pidError[i]=(unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+2]<<8)& 0xff00) 
							+((packetToReceive.content[PcCnt+3]<<16)& 0xff0000)+((packetToReceive.content[PcCnt+4]<<24)& 0xff000000);
						PcCnt=PcCnt+4;
					}
				}
				if (BroadCastPolicy&0x20){//check if bit 5 of the BCAST_POLICY is asserted
					for (int i=0; i<1; i++ )
					{
						if (((packetToReceive.content[PcCnt+4])& 0x80)>0) 
							Exos.ExosFinger[boardNo-1].realCurrent[i]=(unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+2]<<8)& 0xff00) 
							+((packetToReceive.content[PcCnt+3]<<16)& 0xff0000)+((packetToReceive.content[PcCnt+4]<<24)& 0xff000000)-0xffff;
						else
							Exos.ExosFinger[boardNo-1].realCurrent[i]=(unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+2]<<8)& 0xff00) 
							+((packetToReceive.content[PcCnt+3]<<16)& 0xff0000)+((packetToReceive.content[PcCnt+4]<<24)& 0xff000000);
						PcCnt=PcCnt+4;
					}
				}
				if (BroadCastPolicy&0x40){//check if bit 6 of the BCAST_POLICY is asserted
					Exos.ExosFinger[boardNo-1].temp_Vdc=(unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+2]<<8)& 0xff00) 
						+((packetToReceive.content[PcCnt+3]<<16)& 0xff0000)+((packetToReceive.content[PcCnt+4]<<24)& 0xff000000);
					PcCnt=PcCnt+4;
				}
				if (BroadCastPolicy&0x80){//check if bit 7 of the BCAST_POLICY is asserted
					Exos.ExosFinger[boardNo-1].tStamp =(unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+2]<<8)& 0xff00) 
						+((packetToReceive.content[PcCnt+3]<<16)& 0xff0000)+((packetToReceive.content[PcCnt+4]<<24)& 0xff000000);
					PcCnt=PcCnt+4;
				}
				if (BroadCastPolicy&0x100){//check if bit 8 of the BCAST_POLICY is asserted
					Exos.ExosFinger[boardNo-1].fault = (unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+1]<<8)& 0xff00);
					PcCnt=PcCnt+2;
				}
				//Construct analogue input selection
				int AB=2*((BroadCastPolicy&0x400)>>10)+((BroadCastPolicy&0x200)>>9);
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
						Exos.ExosFinger[boardNo-1].anInput1= ((unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+2]<<8)& 0xff00));
						PcCnt=PcCnt+2;
						Exos.ExosFinger[boardNo-1].anInput2= ((unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+2]<<8)& 0xff00));
						PcCnt=PcCnt+2;

						// anInput3 and anInput4 receive force and torque in decaN and mNm 
						//for fitting  positive and negative forces. Force up to 327.67N with a resolution of 10mN. Torque up to 32.767Nm with a resolution of 1mNm.
						if(((packetToReceive.content[PcCnt+2])& 0x80)>0) Exos.ExosFinger[boardNo-1].anInput3= ((unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+2]<<8)& 0xff00))-0xffff;
						else Exos.ExosFinger[boardNo-1].anInput3= ((unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+2]<<8)& 0xff00));
						Exos.ExosFinger[boardNo-1].Force=Exos.ExosFinger[boardNo-1].anInput3/10.0;// convert to Newtons from decaNewtons
						PcCnt=PcCnt+2;
						// decaN and mNm for fitting  positive and negative forces up to 327.67N with a resolution of 10mN.
						if(((packetToReceive.content[PcCnt+2])& 0x80)>0) Exos.ExosFinger[boardNo-1].anInput4= ((unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+2]<<8)& 0xff00))-0xffff;
						else Exos.ExosFinger[boardNo-1].anInput4= ((unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+2]<<8)& 0xff00));
						Exos.ExosFinger[boardNo-1].Torque=Exos.ExosFinger[boardNo-1].anInput4/1000.0;// convert to Nm from mNm
						PcCnt=PcCnt+2;
					}
					break;
				}//switch (AB)
				if (BroadCastPolicy&0x800){//check if bit 11 of the BCAST_POLICY is asserted
		/*			for (int i=0; i<3; i++ )
					{
						if(((packetToReceive.content[PcCnt+2])& 0x80)>0) Exos.ExosFinger[boardNo-1].quickSpeed[i]= ((unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+2]<<8)& 0xff00))-0xffff;
						else Exos.ExosFinger[boardNo-1].quickSpeed[i]= ((unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+2]<<8)& 0xff00));
						PcCnt=PcCnt+2;
					}*/
					PcCnt=PcCnt+2;
				}
				if (BroadCastPolicy&0x1000){//check if bit 12 of the BCAST_POLICY is asserted
					//for (int i=0; i<3; i++ )
					//{
					//	PcCnt=PcCnt+2;//// motor_state placeholder
					//}
					PcCnt=PcCnt+2;
				}
				if (BroadCastPolicy&0x2000){//check if bit 13 of the BCAST_POLICY is asserted
					//for (int i=0; i<3; i++ )
					//{
					//	PcCnt=PcCnt+2;// real_current placeholder
					//}
					PcCnt=PcCnt+2;
				}
				if (BroadCastPolicy&0x4000){//check if bit 14 of the BCAST_POLICY is asserted
					//for (int i=0; i<3; i++ )
					//{
					//	PcCnt=PcCnt+2;// relative_position placeholder
					//}
					PcCnt=PcCnt+2;
				}
				if (BroadCastPolicy&0x8000){//check if bit 15 of the BCAST_POLICY is asserted
					//for (int i=0; i<3; i++ )
					//{
					//	PcCnt=PcCnt+2;// target_angle placeholder
					//}
					PcCnt=PcCnt+2;
				}
				//
				//------------------------   Parse Extra Broadcast Data  ---------------------------------------
				//----------------------------------------------------------------------------------------------
				//----------------------------------------------------------------------------------------------

				if (ACC_EXTRA_BCAST_POLICY&0x1){//check if bit 0 of the ACC_EXTRA_BCAST_POLICY is asserted
					/*for (int i=0; i<3; i++ )
					{
						Exos.ExosFinger[boardNo-1].TargPos[i]= (unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+2]<<8)& 0xff00) 
							+((packetToReceive.content[PcCnt+3]<<16)& 0xff0000)+((packetToReceive.content[PcCnt+4]<<24)& 0xff000000);
						PcCnt=PcCnt+4;
					}*/
					PcCnt=PcCnt+4;
				}
				if (ACC_EXTRA_BCAST_POLICY&0x2){//check if bit 1 of the ACC_EXTRA_BCAST_POLICY is asserted 						

					//for (int i=0; i<3; i++ )
					//{
					//	Exos.ExosFinger[boardNo-1].TempTargPos[i]= (unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+2]<<8)& 0xff00) 
					//		+((packetToReceive.content[PcCnt+3]<<16)& 0xff0000)+((packetToReceive.content[PcCnt+4]<<24)& 0xff000000);
					//	PcCnt=PcCnt+4;
					//}
					PcCnt=PcCnt+4;
				}
				if (ACC_EXTRA_BCAST_POLICY&0x4){//check if bit 2 of the ACC_EXTRA_BCAST_POLICY is asserted
					//for (int i=0; i<3; i++ )
					//{
					//	Exos.ExosFinger[boardNo-1].ReqTargPos[i]= (unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+2]<<8)& 0xff00) 
					//		+((packetToReceive.content[PcCnt+3]<<16)& 0xff0000)+((packetToReceive.content[PcCnt+4]<<24)& 0xff000000);
					//	PcCnt=PcCnt+4;
					//}
					PcCnt=PcCnt+4;
				}
				if (ACC_EXTRA_BCAST_POLICY&0x8){//check if bit 3 of the ACC_EXTRA_BCAST_POLICY is asserted
					PcCnt=PcCnt+4;// twin_position placeholder
				}
				if (ACC_EXTRA_BCAST_POLICY&0x10){//check if bit 4 of the ACC_EXTRA_BCAST_POLICY is asserted
					PcCnt=PcCnt+2;//// angle placeholder
				}
				if (ACC_EXTRA_BCAST_POLICY&0x20){//check if bit 5 of the ACC_EXTRA_BCAST_POLICY is asserted
					PcCnt=PcCnt+2;// target_angle placeholder
				}
				if (ACC_EXTRA_BCAST_POLICY&0x40){//check if bit 6 of the ACC_EXTRA_BCAST_POLICY is asserted
					Exos.ExosFinger[boardNo-1].jointPos[0]=Exos.ExosFinger[boardNo-1].M_pos[0];// insert the motor position as the first joint position
					//for (int i=0; i<6; i++ )
					//{
					//	Exos.ExosFinger[boardNo-1].jointPos[i+1]= (unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+2]<<8)& 0xff00) 
					//		+((packetToReceive.content[PcCnt+3]<<16)& 0xff0000)+((packetToReceive.content[PcCnt+4]<<24)& 0xff000000);
					//	//Exos.ExosFinger[boardNo-1].jointPos[i+1]=i+1;
					//	PcCnt=PcCnt+4;
					//}
					PcCnt=PcCnt+4;
				}
				if (ACC_EXTRA_BCAST_POLICY&0x80){//check if bit 7 of the ACC_EXTRA_BCAST_POLICY is asserted
				/*	for (int i=0; i<6; i++ )
					{
						Exos.ExosFinger[boardNo-1].slaveTorque[3]= (unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+2]<<8)& 0xff00) 
							+((packetToReceive.content[PcCnt+3]<<16)& 0xff0000)+((packetToReceive.content[PcCnt+4]<<24)& 0xff000000);
						PcCnt=PcCnt+4;
					}*/
					PcCnt=PcCnt+4;
				}

				if (ACC_EXTRA_BCAST_POLICY&0x100){//check if bit 8 of the EXTRA_BCAST_POLICY is asserted
	
					PcCnt=PcCnt+2;
				}	
				if (ACC_EXTRA_BCAST_POLICY&0x200){//check if bit 9 of the EXTRA_BCAST_POLICY is asserted
					if(((packetToReceive.content[PcCnt+2])& 0x80)>0) Exos.Accel[0] = ((unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+2]<<8)& 0xff00))-0xffff;
					else Exos.Accel[0] = ((unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+2]<<8)& 0xff00));
					PcCnt=PcCnt+2;
				}	
				if (ACC_EXTRA_BCAST_POLICY&0x400){//check if bit 10 of the EXTRA_BCAST_POLICY is asserted
					if(((packetToReceive.content[PcCnt+2])& 0x80)>0) Exos.Accel[1] = ((unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+2]<<8)& 0xff00))-0xffff;
					else Exos.Accel[1] = ((unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+2]<<8)& 0xff00));
					PcCnt=PcCnt+2;
				}	
				if (ACC_EXTRA_BCAST_POLICY&0x800){//check if bit 11 of the EXTRA_BCAST_POLICY is asserted
					if(((packetToReceive.content[PcCnt+2])& 0x80)>0) Exos.Accel[2] = ((unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+2]<<8)& 0xff00))-0xffff;
					else Exos.Accel[2]= ((unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+2]<<8)& 0xff00));
					PcCnt=PcCnt+2;
				}
				if (ACC_EXTRA_BCAST_POLICY&0x1000){//check if bit 11 of the EXTRA_BCAST_POLICY is asserted
					PcCnt=PcCnt+4;
				}
				if (ACC_EXTRA_BCAST_POLICY&0x2000){//check if bit 11 of the EXTRA_BCAST_POLICY is asserted
					Exos.Accel[3]=(unsigned char)packetToReceive.content[PcCnt+1]+((packetToReceive.content[PcCnt+2]<<8)& 0xff00) 
						+((packetToReceive.content[PcCnt+3]<<16)& 0xff0000)+((packetToReceive.content[PcCnt+4]<<24)& 0xff000000);
					PcCnt=PcCnt+4;
				}





			}//if packetToReceive.content[2] == (char)0xbb
		}

		//io - set jointPosRad
		//first encoder, high resolution

		//DEBUG
		//Breaks because unvalid board no at some point
		//double plop;
		//plop=(double)Exos.ExosFinger[0].jointPos[0];//ok
		//plop=(double)Exos.ExosFinger[1].jointPos[0];//ok
		//plop=(double)Exos.ExosFinger[2].jointPos[0];//ok
		//plop=(double)Exos.ExosFinger[boardNo-1].jointPos[0];//makes break
		////printf("boardNo: %i [exolib]\n",boardNo);
		//if(boardNo==1|boardNo==2|boardNo==3)
		//{
		//	plop=(double)Exos.ExosFinger[boardNo-1].jointPos[0];//makes break
		//}
		//else
		//{
		//	printf("Unvalid boardNo: %i - skip \n",boardNo);
		//}


		

		//INITIAL
		//Exos.ExosFinger[boardNo-1].jointPosRad[0]=(double)Exos.ExosFinger[boardNo-1].jointPos[0]*2*M_PI/resRelEnc;//When board is sending enc reading
		//printf("boardNo: %i - jointPos[0]: %i [ExoLib.cpp]\n",boardNo,Exos.ExosFinger[boardNo-1].jointPos[0]);
		//printf("[EXOSBoardLibrary::GetData] joint 0 pos %g \n",Exos.ExosFinger[boardNo-1].jointPos[0]*2*M_PI/resRelEnc);
		//Exos.ExosFinger[boardNo-1].jointPosRad[0]=(double)Exos.ExosFinger[boardNo-1].jointPos[0]*0.001;//when board is sending for joint 0 pos in mRad
		
		//rest of encoders, lower resolution
		//for(int it=1;it<6;it++)
		//{
		//	Exos.ExosFinger[boardNo-1].jointPosRad[it]=Exos.ExosFinger[boardNo-1].jointPos[it]*2*M_PI/resAbsEnc;//board is sending joints 1..6 pos in enc units
		//}


		//**********************
		//UNCOMMENT!!
		//****************
	


		////if(packetToReceive.content[2] == (char)0xbc && bcast_policy== 2)   // this is a boradcast packet
		////{
		////	boardNo=packetToReceive.content[3];				
		////}
		////if(packetToReceive.content[2] == (char)0xbc && bcast_policy == 64)   // this is a boradcast packet
		////{			
		////	bytesReceived=0;
		////}			 


		////if(bcast_policy == 2 || bcast_policy == 64){}

		return numberOfActiveBoards;	
}
 //******************************************************************************
 DECLDIR int GetExosDataMatlab(int BoardID, int ExosData[][FNGR_JOINTS])
 {
	 int boardNo;
	 GetData();
	 boardNo=BoardID;
	 for (int i=0; i<6; i++)
	 {
		 //FingerData[i]=Exos.ExosFinger[boardNo-1].jointPos[i];
		 printf("DLL responds to GetFingerPositions");
		 ExosData[0][i]=Exos.ExosFinger[THUMB_ID-1].jointPos[i];
		 ExosData[1][i]=Exos.ExosFinger[INDEX_ID-1].jointPos[i];
		 ExosData[2][i]=Exos.ExosFinger[MIDDLE_ID-1].jointPos[i];

	 }
	 for (int i=0; i<4; i++)
	 {
		 ExosData[3][i]=(int)Exos.Accel[i];	
	 }

	 return numberOfActiveBoards;	
 }
	DECLDIR int TakeJointOffsets(int BoardNumber)
	{
	int boardNo;
	 GetData();
	 boardNo=BoardNumber;

	for(int i=0;i<6;i++) 	
					{
							Exos.ExosFinger[0].jointPosOffs[i]=Exos.ExosFinger[0].jointPos[i];
							Exos.ExosFinger[1].jointPosOffs[i]=Exos.ExosFinger[1].jointPos[i];
							Exos.ExosFinger[2].jointPosOffs[i]=Exos.ExosFinger[2].jointPos[i];
					}
	 return numberOfActiveBoards;	
	}
 //******************************************************************************
 DECLDIR int GetExosData(int BoardID, HandExoskeleton &MyExosData)
 {
	 //printf("[ExosBoardLibrary::GetExosData]\n");
	 int boardNo;
	 GetData();
	 boardNo=BoardID;
	 MyExosData=Exos;
     //printf("\t %*hd \t %*hd \t %*hd\n", 7,Exos.ExosFinger[0].M_tor[0], 7,Exos.ExosFinger[1].M_tor[0], 7,Exos.ExosFinger[2].M_tor[0]);
	// printf("\t %*d \t %*d \t %*d\n",7,Exos.ExosFinger[0].slaveTorque[0],7,Exos.ExosFinger[0].M_tor[0],7,Exos.ExosFinger[2].slaveTorque[0]);

	 // printf("\t %d \t %d \t %d \n", Exos.ExosFinger[0].M_vel[0], Exos.ExosFinger[1].M_vel[0], Exos.ExosFinger[2].M_vel[0]);
	//printf("\t %d \t %d \t %d \n", Exos.ExosFinger[0].M_tor[0], Exos.ExosFinger[1].M_tor[0], Exos.ExosFinger[2].M_tor[0]);
	 ////for (int i=0; i<6; i++)
	 ////{
	 ////	
	 ////	//FingerData[i]=Exos.ExosFinger[boardNo-1].jointPos[i];
	 ////	printf("DLL responds to GetFingerPositions");
	 ////	ExosData[0][i]=Exos.ExosFinger[THUMB_ID-1].jointPos[i];
	 ////	ExosData[1][i]=Exos.ExosFinger[INDEX_ID-1].jointPos[i];
	 ////	ExosData[2][i]=Exos.ExosFinger[MIDDLE_ID-1].jointPos[i];

	 ////}
	 ////for (int i=0; i<4; i++)
	 ////{
	 ////	ExosData[3][i]=(int)Exos.Accel[i];	
	 ////}
//Debug print
//	 for(int i=0;i<6;i++)
//		 printf("Joint%d \t %*d \t %*d \t %*d\n",i, 7,MyExosData.ExosFinger[0].jointPos[i], 7,MyExosData.ExosFinger[1].jointPos[i], 7,MyExosData.ExosFinger[2].jointPos[i]);

	 return numberOfActiveBoards;	
 }

 

 DECLDIR int* GetExosFingerData(int theFinger)
 {
	GetData();
	return Exos.ExosFinger[theFinger].jointPos;
 }

 DECLDIR int** GetAllExosFingerData()
 {
	
	GetData();

	tempData[0] = Exos.ExosFinger[0].jointPos;
	tempData[1] = Exos.ExosFinger[1].jointPos;
	tempData[2] = Exos.ExosFinger[2].jointPos;

	return tempData;
 }


 //******************************************************************************
 DECLDIR int GetAccelerometer(int BoardID, int AccelData[4])
 {
	 int boardNo;
	 GetData();
	 boardNo=BoardID;
	 for (int i=0; i<4; i++)
	 {
		 AccelData[i]=Exos.Accel[i];
		 printf("DLL responds to GetFingerPositions");
	 }

	 return numberOfActiveBoards;	
 }

		
//******************************************************************************

	DECLDIR int GetActiveBoards(int *boardIDs) //TODO: for more than 1 board
	{
		int i=0;

		if(numberOfActiveBoards>0)
			for(i=0;i<MAX_BOARDS;i++)
				boardIDs[i] = activeBoardIDs[i];

		return numberOfActiveBoards;
	}



//******************************************************************************
DECLDIR int ScanForActiveBoards(void)
{

		int BytesSent, BytesReceived, BoardNum;
		CharBuff packetToBeSent, packetToReceive;
		int i;
		


		//Sends request
		buildUDPCommand(&packetToBeSent, GET_ACTIVE_BOARDS , NULL);
		BytesSent = SendUDP(UDPSckID, &packetToBeSent);


		if (BytesSent == -1)
			return -1;

	
	//	for(i=0;i<60000;i++);
		//Receives answer
		BytesReceived = ReceiveUDPWithTimeOut(UDPSckID, &packetToReceive); //Drops the first answer if it is equal to the sent one (due broadcast)
		if (BytesReceived == -1)
			return -2;

		numberOfActiveBoards=0;
		for(i=0;i<MAX_BOARDS;i++)
			activeBoardIDs[i]=0;

		do{
			BytesReceived=0;
			BytesReceived = ReceiveUDPWithTimeOut(UDPSckID, &packetToReceive);
			if(BytesReceived>0)
				if(packetToReceive.content[2] == (char)0x82)   // if it is a board response packet
				  {
					BoardNum = (unsigned char)(packetToReceive.content[4]);
					activeBoardIDs[BoardNum-1]=BoardNum; // save board id
					printf("Active board found - ip='xxx.xxx.xxx.%i' - id=%i [ExosBoardLibrary.cpp::ScanForActiveBoards]\n",BASEADDR+BoardNum,BoardNum);
					numberOfActiveBoards++;
				  }
		}while(BytesReceived>0);

	//	activeBoardIDs[7]=8;
	//	numberOfActiveBoards++;
		if(numberOfActiveBoards>0)
			return numberOfActiveBoards;
		else	
		{
			printf("New Print No boards");
			return -3;
		}
}


//******************************************************************************
DECLDIR CharBuff SetDesiredPosition_m(int DesPos[MAX_BOARDS])
	{
		//printf("DesPos: %i %i %i %i %i %i %i %i \n",DesPos[0],DesPos[1],DesPos[2],DesPos[3],DesPos[4],DesPos[5],DesPos[6],DesPos[7],DesPos[8]);
		int BytesSent, BoardNum, i;
		char payload[PACKET_MAX_SIZE];
		CharBuff packetToBeSent;

		//Getting active board
		//BoardNum = GetActiveBoards();

		BoardNum = MAX_BOARDS;
	for (i = 0; i<BoardNum; i++)
	{
				payload[4*i] =	 (DesPos[i] & 0x000000ff);
				payload[4*i+1] = ((DesPos[i]>>8) & 0x000000ff);
				payload[4*i+2] = ((DesPos[i]>>16) & 0x000000ff);
				payload[4*i+3] = ((DesPos[i]>>24) & 0x000000ff);
	}

		////Sends request
		//for (i = 0; i<BoardNum; i++)
		//{
		//	if(activeBoardIDs[i]<=0)
		//	{
		//		printf("here we are a \n");
		//		payload[4*i] = 0x00;
		//		payload[4*i+1] = 0x00;
		//		payload[4*i+2] = 0x00;
		//		payload[4*i+3] = 0x00;
		//	}	
		//	else
		//	{
		//		printf("here we are b \n");
		//		payload[4*i] =	 (DesPos[i] & 0x000000ff);
		//		payload[4*i+1] = ((DesPos[i]>>8) & 0x000000ff);
		//		payload[4*i+2] = ((DesPos[i]>>16) & 0x000000ff);
		//		payload[4*i+3] = ((DesPos[i]>>24) & 0x000000ff);

		//	/*	payload[4*i]   = DesPos[i]%256;
		//		payload[4*i+1] = DesPos[i]/256;
		//		payload[4*i+2] = DesPos[i]/65536;
		//		payload[4*i+3] = DesPos[i]/16777216; */

		//	}
		//}

	/*	for (i = 0; i < (4*BoardNum-4); i++)
			payload[i] = 0x00;
		
		payload[4*BoardNum-4] = DesPos%256;
		payload[4*BoardNum-3] = DesPos/256;
		payload[4*BoardNum-2] = DesPos/65536;
		payload[4*BoardNum-1] = DesPos/16777216;  */
		
		//printf("payload - bytes: %#x %#x %#x %#x %#x %#x %#x %#x\n",payload[0],payload[1],payload[2],payload[3],payload[4],payload[5],payload[6],payload[7]);
		buildUDPCommand(&packetToBeSent, SET_DESIRED_POSITION , payload);
		//printf("packetToBeSent - bytes: %#x %#x %#x %#x %#x %#x %#x %#x \n",packetToBeSent.content[0],packetToBeSent.content[1],packetToBeSent.content[2],packetToBeSent.content[3],packetToBeSent.content[4],packetToBeSent.content[5],packetToBeSent.content[6],packetToBeSent.content[6],packetToBeSent.content[7]);
		
		//printf("[BoardLibrary.cpp] Sizeof payload: %i \n",sizeof(payload));
		//printf("[BoardLibrary.cpp] Data to pack: DesPos[0]=%d \n",DesPos[0]);
		//printf("[BoardLibrary.cpp] Packet content: %d %u %u \n",DesPos[0],packetToBeSent.content[3],packetToBeSent.content[7]);
		//printf("[BoardLibrary.cpp] Packet content- Id packet: %d, 1st int of data:%d, 2nd int of data:%u \n",packetToBeSent.content[0],packetToBeSent.content[3],packetToBeSent.content[7]);
		//printf("[BoardLibrary.cpp] Packet header (first byte): %#1x, 2nd byte: %#1x, 3rd byte: %#1x \n",packetToBeSent.content[0],packetToBeSent.content[1],packetToBeSent.content[2]);
		
		if(packetToBeSent.content[0]==0xffffffff)
		{	
			//printf("First byte==0xffffffff \n");
		}
		else
		{	
			printf("First byte != 0xffffffff \n");
		}
		
		if(packetToBeSent.content[2]==0x5b)
		{	
			//printf("3rd byte==0x5b \n");
		}
		else
		{	
			printf("3rd byte != 0x5b \n");
		}
//////////////////////////////
//int startByte_consideredInteger=3;
int startByte_consideredInteger=3;

    //Unpack first int=num rigid body
    int firstInt=(unsigned char)packetToBeSent.content[startByte_consideredInteger]+((packetToBeSent.content[startByte_consideredInteger+1]<<8)&0xff00)+((packetToBeSent.content[startByte_consideredInteger+2]<<16) & 0xff0000)+((packetToBeSent.content[startByte_consideredInteger+3]<<24)&0xff000000);

    int numRigidBody=firstInt;
    //int nInt_sent=numRigidBody*8+1; //id,xyzqxqyqzqw*n+firstInt=numRigidBody
	int nInt_sent=3*8+1; //id,xyzqxqyqzqw*n+firstInt=numRigidBody
    //int packet_data_int[nInt_sent];
	int packet_data_int[25];

    ////Reorder the bytes of each integer (MSB -- LSB) and take care of sign
    for(int it=0;it<nInt_sent;it++)
    {
        packet_data_int[it]=(unsigned char)packetToBeSent.content[startByte_consideredInteger]+((packetToBeSent.content[startByte_consideredInteger+1]<<8)&0xff00)+((packetToBeSent.content[startByte_consideredInteger+2]<<16) & 0xff0000)+((packetToBeSent.content[startByte_consideredInteger+3]<<24)&0xff000000);
        //printf("data_int[%i]: %i \n",it,packet_data_int[it]);
        startByte_consideredInteger+=4;
    }

//////////////////
		//a_packetToBeSent=&packetToBeSent;
		
		return packetToBeSent;
		/*
		BytesSent = SendUDP(UDPSckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		
		return 0;
		*/
	}


DECLDIR int SetDesiredStiffnessDamping(int DesSiff[MAX_BOARDS], int DesDamp[MAX_BOARDS]) //TODO: to send the message if there is more than 1 board
	{
		int BytesSent, BoardNum, i;
		char payload[PACKET_MAX_SIZE];
		CharBuff packetToBeSent;

		//Getting active board
		//BoardNum = GetActiveBoards();

		BoardNum = MAX_BOARDS;

		//Sends request
		for (i = 0; i<BoardNum; i++)
		{
			if(activeBoardIDs[i]<=0)
			{
				payload[6*i] = 0x00;
				payload[6*i+1] = 0x00;
				payload[6*i+2] = 0x00;
				payload[6*i+3] = 0x00;
				payload[6*i+4] = 0x00;
				payload[6*i+5] = 0x00;
			}	
			else
			{
		//		payload[4*i] =	 (DesPos[i] & 0x000000ff);
		//		payload[4*i+1] = ((DesPos[i]>>8) & 0x000000ff);
		//		payload[4*i+2] = ((DesPos[i]>>16) & 0x000000ff);
		//		payload[4*i+3] = ((DesPos[i]>>24) & 0x000000ff);

				payload[6*i]   = DesSiff[i]%256;
				payload[6*i+1] = DesSiff[i]/256;
				payload[6*i+2] = DesSiff[i]/65536;
				payload[6*i+3] = DesDamp[i]%256;
				payload[6*i+4] = DesDamp[i]/256;
				payload[6*i+5] = DesDamp[i]/65536;

			}
		}

	/*	for (i = 0; i < (4*BoardNum-4); i++)
			payload[i] = 0x00;
		
		payload[4*BoardNum-4] = DesPos%256;
		payload[4*BoardNum-3] = DesPos/256;
		payload[4*BoardNum-2] = DesPos/65536;
		payload[4*BoardNum-1] = DesPos/16777216;  */

		buildUDPCommand(&packetToBeSent, SET_DESIRED_STIFFDAMP , payload);
	
		BytesSent = SendUDP(UDPSckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		
		return 0;
	}
//******************************************************************************

	DECLDIR int SetDesiredVelocity(int DesVel[MAX_BOARDS]) //TODO: to send the message if there is more than 1 board
	{
		int BytesSent, BoardNum, i;
		char payload[PACKET_MAX_SIZE];
		CharBuff packetToBeSent;

		//Getting active board
		//BoardNum = GetActiveBoards();

		BoardNum = MAX_BOARDS;

		//Sends request
		for (i = 0; i<BoardNum; i++)
		{
			if(activeBoardIDs[i]<=0)
			{
				payload[2*i] = 0x00;
				payload[2*i+1] = 0x00;
			}	
			else
			{
				payload[2*i] = (DesVel[i] & 0x00ff);
				payload[2*i+1] = ((DesVel[i]>>8) & 0x00ff);
			}
		}

/*		for (i = 0; i < (2*BoardNum-2); i++)
			payload[i] = 0x00;
		
		payload[2*BoardNum-2] = DesVel%256;
		payload[2*BoardNum-1] = DesVel/256;   */

		buildUDPCommand(&packetToBeSent, SET_DESIRED_VELOCITY , payload);
	
		BytesSent = SendUDP(UDPSckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		
		return 0;
		
	}

	int SetDesiredTorque(int DesTor[MAX_BOARDS])
	{
		int BytesSent, BoardNum, i;
		char payload[PACKET_MAX_SIZE];
		CharBuff packetToBeSent;

		//Getting active board
		//BoardNum = GetActiveBoards();

		BoardNum = MAX_BOARDS;

		//Sends request
		for (i = 0; i<BoardNum; i++)
		{
			if(activeBoardIDs[i]<=0)
			{
				payload[2*i] = 0x00;
				payload[2*i+1] = 0x00;
			}	
			else
			{
				payload[2*i] = (DesTor[i] & 0x00ff);
				payload[2*i+1] = ((DesTor[i]>>8) & 0x00ff);
			}
		}

		buildUDPCommand(&packetToBeSent, SET_DESIRED_TORQUE , payload);
	
		BytesSent = SendUDP(UDPSckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		
		return 0;
	}

//******************************************************************************
DECLDIR int SetPidOffset(int DesOffset[MAX_BOARDS])
{

		int BytesSent, BoardNum, i;
		char payload[PACKET_MAX_SIZE];
		CharBuff packetToBeSent;

		//Getting active board
		//BoardNum = GetActiveBoards();

		BoardNum = MAX_BOARDS;

		//Sends request
		for (i = 0; i<BoardNum; i++)
		{
			if(activeBoardIDs[i]<=0)
			{
				payload[2*i] = 0x00;
				payload[2*i+1] = 0x00;
			}	
			else
			{
				payload[2*i] = (DesOffset[i] & 0x00ff);
				payload[2*i+1] = ((DesOffset[i]>>8) & 0x00ff);
			}
		}

/*		for (i = 0; i < (2*BoardNum-2); i++)
			payload[i] = 0x00;
		
		payload[2*BoardNum-2] = DesVel%256;
		payload[2*BoardNum-1] = DesVel/256;   */

		buildUDPCommand(&packetToBeSent, SET_PID_OFFSET , payload);
	
		BytesSent = SendUDP(UDPSckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		
		return 0;
		
	}


//******************************************************************************

	DECLDIR int StartPositionControl(void) //TODO: to send the message if there is more than 1 board
	{
		int BytesSent, BoardNum, i;
		char payload[PACKET_MAX_SIZE];
		CharBuff packetToBeSent;

		//Getting active board
	//	BoardNum = GetActiveBoards();
		BoardNum = MAX_MOTOR_BOARDS;

		//Sends request
		for (i = 0; i<BoardNum; i++)
			if(activeBoardIDs[i]<=0)
				payload[i] = 0x00;
			else
				payload[i] = 0x03;
		
		buildUDPCommand(&packetToBeSent, POSITION_MOVE , payload);
		BytesSent = SendUDP(UDPSckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		
		return 0;
	}


//******************************************************************************

DECLDIR int StopPositionControl(void) //TODO: to send the message if there is more than 1 board
	{
		int BytesSent, BoardNum, i;
		char payload[PACKET_MAX_SIZE];
		CharBuff packetToBeSent;

		//Getting active board
	//	BoardNum = GetActiveBoards();
		BoardNum = MAX_MOTOR_BOARDS;

		//Sends request
		for (i = 0; i<BoardNum; i++)
				payload[i] = 0x01;
		
		buildUDPCommand(&packetToBeSent, POSITION_MOVE , payload);
		BytesSent = SendUDP(UDPSckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		
		return 0;
	}


//******************************************************************************

DECLDIR int StartBoardPositionControl(int boardNumber)
	{
		int BytesSent, BoardNum, i;
		char payload[PACKET_MAX_SIZE];
		CharBuff packetToBeSent;

		//Getting active board
	//	BoardNum = GetActiveBoards();
		BoardNum = MAX_MOTOR_BOARDS;
		//Sends request
		for (i = 0; i<BoardNum; i++)     //stop all boards
			payload[i] = 0x00;   
		payload[boardNumber-1] = 0x03;   // start the selected board
		buildUDPCommand(&packetToBeSent, POSITION_MOVE , payload);
		BytesSent = SendUDP(UDPSckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		
		return 0;
	}



//******************************************************************************

	DECLDIR int StartVelocityControl(void) //TODO: to send the message if there is more than 1 board
	{
		int BytesSent, BoardNum, i;
		char payload[PACKET_MAX_SIZE];
		CharBuff packetToBeSent;

		//Getting active board
	//	BoardNum = GetActiveBoards();
		BoardNum = MAX_BOARDS;

		//Sends request
		for (i = 0; i<BoardNum; i++)
			if(activeBoardIDs[i]<=0)
				payload[i] = 0x00;
			else
				payload[i] = 0x03;
		
		buildUDPCommand(&packetToBeSent, VELOCITY_MOVE , payload);
		BytesSent = SendUDP(UDPSckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		
		return 0;
	}

//******************************************************************************

	DECLDIR int StopMotorUDP(void) //TODO: to send the message if there is more than 1 board
	{
		int BytesSent, BoardNum, i;
		char payload[PACKET_MAX_SIZE];
		CharBuff packetToBeSent;

		//Getting active board
//		BoardNum = GetActiveBoards();
		BoardNum = MAX_BOARDS;

		//Sends request
		for (i = 0; i < (BoardNum-1); i++)
			payload[i] = 0x00;
		
		payload[BoardNum - 1] = 0x01;
		
		buildUDPCommand(&packetToBeSent, POSITION_MOVE , payload);
		BytesSent = SendUDP(UDPSckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		
		return 0;
	}

//******************************************************************************
	
//}

	//	NEW FUNCTIONS FOR IMPEDANCE CONTROL	//

DECLDIR int SetTorqueOnOff(int BoardNumber, int torqueFlag)
    {
		int BytesSent;
		char payload[1];
		CharBuff packetToBeSent;
		SckID=boardSckID[BoardNumber];

		//Sends request
		intToBytes(torqueFlag, payload, sizeof(payload));

		buildTCPCommand(&packetToBeSent, SET_TORQUE_ONOFF, payload);
		BytesSent = SendTCP(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		else
			return 0;
	}

DECLDIR int GetTorqueOnOff(int BoardNumber)
    {
		int BytesSent, BytesReceived, offset, Tonoff;
		CharBuff packetToBeSent, packetToReceive;
		SckID=boardSckID[BoardNumber];

		//Sends request
		buildTCPCommand(&packetToBeSent, GET_TORQUE_ONOFF, NULL);
		BytesSent = SendTCP(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;

		//Receives answer
		BytesReceived = ReceiveTCP(SckID, &packetToReceive);
		if (BytesReceived == -1)
			return -1;

		//PID output
		offset = getPayloadOffset(getDescriptor_TCPRep(REPLY_TORQUE_ONOFF));
		bytesToInt(&packetToReceive.content[offset], 1, &Tonoff);

		return Tonoff; //
	}
	
DECLDIR int SetMotorConfig(int BoardNumber, char *mConfig)
    {
		int BytesSent;
		char payload[2];
		CharBuff packetToBeSent;
		SckID=boardSckID[BoardNumber];
		
		sscanf_s(mConfig, "%d,%d", &payload[0], &payload[1]); //

		//Sends request
		buildTCPCommand(&packetToBeSent, SET_MOTOR_CONFIG, payload);
		BytesSent = SendTCP(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		else
			return 0;
	}

DECLDIR int GetMotorConfig(int BoardNumber)
    {
		int BytesSent, BytesReceived, offset, mConfig;
		CharBuff packetToBeSent, packetToReceive;
		SckID=boardSckID[BoardNumber];

		//Sends request
		buildTCPCommand(&packetToBeSent, GET_MOTOR_CONFIG, NULL);
		BytesSent = SendTCP(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;

		//Receives answer
		BytesReceived = ReceiveTCP(SckID, &packetToReceive);
		if (BytesReceived == -1)
			return -1;

		//
		offset = getPayloadOffset(getDescriptor_TCPRep(REPLY_MOTOR_CONFIG));
		bytesToInt(&packetToReceive.content[offset], 2, &mConfig);

		return mConfig;
	}

DECLDIR int SetPDlinkGains(int BoardNumber, char *Gains)
    {

		int BytesSent;
		long P, D;
		char payload[13];
		CharBuff packetToBeSent;
		SckID=boardSckID[BoardNumber];
		

		sscanf_s(Gains, "%d,%d", &P, &D); //Get the gains from the input array

		//Sends request
		longToBytes(P, payload, 4);
		longToBytes(D, payload+4, 4);
		

		buildTCPCommand(&packetToBeSent, SET_PD_LINK_GAINS, payload);
		BytesSent = SendTCP(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		else
			return 0;
	}

DECLDIR int GetPDlinkGains(int BoardNumber, long* GainsBuf)
    {
		int BytesSent, BytesReceived, offset;
		long p, d;
		CharBuff packetToBeSent, packetToReceive;
		SckID=boardSckID[BoardNumber];

		//Sends request
		buildTCPCommand(&packetToBeSent, GET_PD_LINK_GAINS, NULL);
		BytesSent = SendTCP(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;

		//Receives answer
		BytesReceived = ReceiveTCP(SckID, &packetToReceive);
		if (BytesReceived == -1)
			return -1;

		offset = getPayloadOffset(getDescriptor_TCPRep(REPLY_PD_LINK_GAINS));// + 1; //payload initial position
		bytesToLong(&packetToReceive.content[offset], 4, &p); //p gain
		bytesToLong(&packetToReceive.content[offset+4], 4, &d); //d gain
			
		GainsBuf[0] = p;
		GainsBuf[1] = d;

		return 0;
	}




/////////////////////////  FT Sensor functio implementation ///////////////////////////////////

//******************************************************************************

DECLDIR int CalibrateOffsets(int BoardNumber)
{
	int BytesSent;
		CharBuff packetToBeSent;
		SckID=boardSckID[BoardNumber];

		//Sends request		
		buildTCPCommand(&packetToBeSent, CALIBRATE_OFFSETS, NULL);
		BytesSent = SendTCP(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		else
			return 0;
}

//******************************************************************************//

DECLDIR int GetCalibrationOffsets(int BoardNumber)
{
	int BytesSent;
		CharBuff packetToBeSent;
		SckID=boardSckID[BoardNumber];

		//Sends request		
		buildTCPCommand(&packetToBeSent, GET_CALIBRATION_OFFSETS, NULL);
		BytesSent = SendTCP(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		else
			return 0;
}

//******************************************************************************//

DECLDIR int SetConversionFactors(int BoardNumber, char channel, int multiplier, int offset)
{
	int BytesSent;
	char payload[13];
	CharBuff packetToBeSent;
	SckID=boardSckID[BoardNumber];

	//Sends request
	payload[0] = channel;
	intToBytes(multiplier, payload+1, 4);
	intToBytes(offset, payload+5, 2);

	buildTCPCommand(&packetToBeSent, SET_CONVERSION_FACTORS, payload);
	BytesSent = SendTCP(SckID, &packetToBeSent);

	if (BytesSent == -1)
		return -1;
	else
		return 0;
}
//******************************************************************************//

DECLDIR int GetConversionFactors(int BoardNumber, char channel, int* FactorsBuf)
{
	int BytesSent, BytesReceived, offset;
		int rchannel, multiplier, par_offset;
		CharBuff packetToBeSent, packetToReceive;
		SckID=boardSckID[BoardNumber];

		//Sends request
		buildTCPCommand(&packetToBeSent, GET_CONVERSION_FACTORS, &channel);
		BytesSent = SendTCP(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;

		//Receives answer
		BytesReceived = ReceiveTCP(SckID, &packetToReceive);
		if (BytesReceived == -1)
			return -1;

		offset = getPayloadOffset(getDescriptor_TCPRep(REPLY_CONVERSION_FACTORS)) + 1; //payload initial position
		bytesToInt(&packetToReceive.content[offset], 1, &rchannel); //
		bytesToInt(&packetToReceive.content[offset+1], 4, &multiplier); //
		bytesToInt(&packetToReceive.content[offset+5], 2, &par_offset); //
			
		FactorsBuf[0] = rchannel;
		FactorsBuf[1] = multiplier;
		FactorsBuf[2] = par_offset;

		return 0;
}
//******************************************************************************//

DECLDIR int SetAvarageSamples(int BoardNumber, int samples)
{
	int BytesSent;
	char payload[13];
	CharBuff packetToBeSent;
	SckID=boardSckID[BoardNumber];

	//Sends request
	payload[0] = samples;

	buildTCPCommand(&packetToBeSent, SET_AVARAGE_SAMPLES, payload);
	BytesSent = SendTCP(SckID, &packetToBeSent);

	if (BytesSent == -1)
		return -1;
	else
		return 0;
}
//******************************************************************************//

DECLDIR int GetAvarageSamples(int BoardNumber, int samples)
{
	int BytesSent, BytesReceived, offset;
		CharBuff packetToBeSent, packetToReceive;
		SckID=boardSckID[BoardNumber];

		//Sends request
		buildTCPCommand(&packetToBeSent, GET_AVARAGE_SAMPLES, NULL);
		BytesSent = SendTCP(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;

		//Receives answer
		BytesReceived = ReceiveTCP(SckID, &packetToReceive);
		if (BytesReceived == -1)
			return -1;

		offset = getPayloadOffset(getDescriptor_TCPRep(REPLY_AVARAGE_SAMPLES)) + 1; //payload initial position
		bytesToInt(&packetToReceive.content[offset], 1, &samples); //
		
		return 0;
}
//******************************************************************************//

DECLDIR int SetMatrixRow(int BoardNumber, char row, float *matrixRow)
{
	int BytesSent;
	char payload[25];

	union float_convert
	{
		unsigned char cval[4];
		float fval;
	}fnum_co;

	CharBuff packetToBeSent;
	SckID=boardSckID[BoardNumber];

	//Sends request
	payload[0] = row;
	for(int i=0;i<=5;i++)
	{
		fnum_co.fval = matrixRow[i];
		payload[i*4+1] = fnum_co.cval[0];
		payload[i*4+2] = fnum_co.cval[1];
		payload[i*4+3] = fnum_co.cval[2];
		payload[i*4+4] = fnum_co.cval[3];
	}

	buildTCPCommand(&packetToBeSent, SET_MATRIX_ROW, payload);
	BytesSent = SendTCP(SckID, &packetToBeSent);

	if (BytesSent == -1)
		return -1;
	else
		return 0;
}
//******************************************************************************//

DECLDIR int GetMatrixRow(int BoardNumber, char row_req, char row_recv, float matrixRow[6])
{
	int BytesSent, BytesReceived, offset;
		CharBuff packetToBeSent, packetToReceive;
		SckID=boardSckID[BoardNumber];

	union float_convert
	{
		unsigned char cval[4];
		float fval;
	}fnum_co;

		//Sends request
		buildTCPCommand(&packetToBeSent, GET_MATRIX_ROW, &row_req);
		BytesSent = SendTCP(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;

		//Receives answer
		BytesReceived = ReceiveTCP(SckID, &packetToReceive);
		if (BytesReceived == -1)
			return -1;

		offset = getPayloadOffset(getDescriptor_TCPRep(REPLY_MATRIX_ROW)); //payload initial position
		row_recv = packetToReceive.content[offset];
		
		for(int i=0;i<6;i++)
		{
			fnum_co.cval[0] = packetToReceive.content[offset+1+i*4]; //
			fnum_co.cval[1] = packetToReceive.content[offset+1+i*4+1]; //
			fnum_co.cval[2] = packetToReceive.content[offset+1+i*4+2]; //
			fnum_co.cval[3] = packetToReceive.content[offset+1+i*4+3]; //
			matrixRow[i] = fnum_co.fval;
		}
		
		return 0;
}