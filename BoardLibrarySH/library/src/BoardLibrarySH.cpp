//Added in "project/properties/configuration properties/linker/input/additional dependencies": WS2_32.lib


#include "../include/BoardLibrarySH.h"
#include <stdlib.h>


//Comunication
//#include "../include/stdafx.h"




static const unsigned int Port = 23;
int activeBoardIDs[MAX_BOARDS_SH];
int numberOfActiveBoards;

	 //WSADATA              wsaData;
	 SOCKET               boardSckID[MAX_BOARDS_SH], SckID, UDPSckID;
	 
	 SOCKADDR_IN          ReceiverAddr;/*, SrcInfo*/;

	 
//******************************************************************************

	 DECLDIR int InitLibrary_SH(int boardsCount)
	 {
		 // Initialize Winsock version 2.2
		//WSADATA wsaData;
		//WSAStartup(MAKEWORD(2,2), &wsaData);

		 // Initialize phil_board.c
		init(boardsCount);
		return 0;
	 }

//******************************************************************************
    DECLDIR int ConnectTCP_SH(int BoardNumber)
	{
		
		char ip_addr_str[14];
		int  RetCode;
		SOCKADDR_IN ServerAddr;


		// Initialize Winsock version 2.2
		WSADATA wsaData;
		WSAStartup(MAKEWORD(2,2), &wsaData);
		
		sprintf(ip_addr_str, "169.254.89.%d", BASEADDR_SH+BoardNumber);
				
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


	DECLDIR int SendTCP_SH(SOCKET SckID, CharBuff* packetToBeSent)
	{
		int BytesSent;

		BytesSent = send(SckID, packetToBeSent->content, packetToBeSent->size, 0);
		if(BytesSent == SOCKET_ERROR)
			return -1;
		else
			return BytesSent;
	}

	
//******************************************************************************

	DECLDIR int ReceiveTCP_SH(SOCKET SckID, CharBuff* packetToReceive)
	{
		int BytesReceived;

		BytesReceived = recv(SckID, packetToReceive->content, sizeof(packetToReceive->content), 0);
		packetToReceive->size = BytesReceived;
		return BytesReceived;
	}

	
//******************************************************************************

	DECLDIR int GetBoardType_SH(int BoardNumber)
	{
		int BytesSent, BytesReceived, BoardType;
		CharBuff packetToBeSent, packetToReceive;
		SckID=boardSckID[BoardNumber];
		
		//Sends request
		buildTCPCommand(&packetToBeSent, GET_BOARD_TYPE, NULL);
		BytesSent = SendTCP_SH(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;

		//Receives answer
		BytesReceived = ReceiveTCP_SH(SckID, &packetToReceive);
		if (BytesReceived == -1)
			return -1;
		
		//TODO use descriptor information to retrieve the payload, not constants:
		BoardType = (unsigned char)(packetToReceive.content[3]);

		return BoardType;
	}

	
//******************************************************************************

	DECLDIR float GetFirmVersion_SH(int BoardNumber)
	{
		int BytesSent, BytesReceived, Firm0, Firm1;
		char Firm[3];
		float fFirm;
		CharBuff packetToBeSent, packetToReceive;
		SckID=boardSckID[BoardNumber];

		//Sends request		
		buildTCPCommand(&packetToBeSent, GET_FIRMWARE_VERSION, NULL);
		BytesSent = SendTCP_SH(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;

		//Receives answer
		BytesReceived = ReceiveTCP_SH(SckID, &packetToReceive);
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

	DECLDIR int SetFirmVersion_SH(int BoardNumber, char * firm) //TODO: board doesn't update the firmaware version.
	{
		int BytesSent;
		char payload[2];
		CharBuff packetToBeSent;
		SckID=boardSckID[BoardNumber];

		sscanf_s(firm, "%d.%d", &payload[0], &payload[1]);

		//Sends request
		buildTCPCommand(&packetToBeSent, SET_FIRMWARE_VERSION, payload);
		BytesSent = SendTCP_SH(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		else
			return 0;
	}

//******************************************************************************

	DECLDIR int ClearBoardFault_SH(int BoardNumber)
	{
		int BytesSent;
		CharBuff packetToBeSent;
		SckID=boardSckID[BoardNumber];

		//Sends request		
		buildTCPCommand(&packetToBeSent, CLEAR_BOARD_FAULT, NULL);
		BytesSent = SendTCP_SH(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		else
			return 0;

	}

//******************************************************************************

	DECLDIR int GetBoardFault_SH(int BoardNumber)
	{
		int BytesSent, BytesReceived, offset, Faults;
		CharBuff packetToBeSent, packetToReceive;
		SckID=boardSckID[BoardNumber];

		//Sends request
		buildTCPCommand(&packetToBeSent, GET_BOARD_FAULT, NULL);
		BytesSent = SendTCP_SH(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;

		//Receives answer
		BytesReceived = ReceiveTCP_SH(SckID, &packetToReceive);
		if (BytesReceived == -1)
			return -1;

		offset = getPayloadOffset(getDescriptor_TCPRep(REPLY_BOARD_FAULT)); //payload initial position
		bytesToInt(&packetToReceive.content[offset], 2, &Faults); //converts the payload bytes to an integer

		return Faults;
	}

//******************************************************************************

	DECLDIR int SetPidGains_SH(int BoardNumber, char gainSet, char * Gains)
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
		BytesSent = SendTCP_SH(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		else
			return 0;
	}

//******************************************************************************

	DECLDIR int GetPidGains_SH(int BoardNumber, char gainSet, long* GainsBuf)
    {
		int BytesSent, BytesReceived, offset;
		long p, i, d;
		CharBuff packetToBeSent, packetToReceive;
		SckID=boardSckID[BoardNumber];

		//Sends request
		buildTCPCommand(&packetToBeSent, GET_PID_GAINS, &gainSet);
		BytesSent = SendTCP_SH(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;

		//Receives answer
		BytesReceived = ReceiveTCP_SH(SckID, &packetToReceive);
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

	DECLDIR int SetPidGainScale_SH(int BoardNumber, char gainSet, char * GainsScale)
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
		BytesSent = SendTCP_SH(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		else
			return 0;
	}

//******************************************************************************

	DECLDIR int GetPidGainScale_SH(int BoardNumber, char gainSet, long* GainsBuf)
    {
		int BytesSent, BytesReceived, offset;
		long p, i, d;
		CharBuff packetToBeSent, packetToReceive;
		SckID=boardSckID[BoardNumber];

		//Sends request
		buildTCPCommand(&packetToBeSent, GET_PID_GAIN_SCALE, &gainSet);
		BytesSent = SendTCP_SH(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;

		//Receives answer
		BytesReceived = ReceiveTCP_SH(SckID, &packetToReceive);
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

	DECLDIR int SetILimGain_SH(int BoardNumber, char gainSet, long ILim)
    {
		int BytesSent;
		char payload[5];
		CharBuff packetToBeSent;
		SckID=boardSckID[BoardNumber];

		//Sends request
		payload[0] = gainSet;
		longToBytes(ILim, payload+1, sizeof(payload)-1);

		buildTCPCommand(&packetToBeSent, SET_ILIM_GAIN, payload);
		BytesSent = SendTCP_SH(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		else
			return 0;
	}

//******************************************************************************

	DECLDIR long GetILimGain_SH(int BoardNumber, char gainSet)
    {
		int BytesSent, BytesReceived, offset;
		long limit;
		CharBuff packetToBeSent, packetToReceive;
		SckID=boardSckID[BoardNumber];


		//Sends request
		buildTCPCommand(&packetToBeSent, GET_ILIM_GAIN, &gainSet);
		BytesSent = SendTCP_SH(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;

		//Receives answer
		BytesReceived = ReceiveTCP_SH(SckID, &packetToReceive);
		if (BytesReceived == -1)
			return -1;
			
		//Limit
		offset = getPayloadOffset(getDescriptor_TCPRep(REPLY_ILIM_GAIN)) + 1; //Add one to the payload offset to skip the first byte, which is the gain set:
		bytesToLong(&packetToReceive.content[offset], 4, &limit);

		return limit;
	}

//******************************************************************************

	DECLDIR long GetPidError_SH(int BoardNumber)
    {
		int BytesSent, BytesReceived, offset;
		long e;
		CharBuff packetToBeSent, packetToReceive;
		SckID=boardSckID[BoardNumber];

		//Sends request
		buildTCPCommand(&packetToBeSent, GET_PID_ERROR, NULL);
		BytesSent = SendTCP_SH(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		
		//Receives answer
		BytesReceived = ReceiveTCP_SH(SckID, &packetToReceive);
		if (BytesReceived == -1)
			return -1;

		//PID error
		offset = getPayloadOffset(getDescriptor_TCPRep(REPLY_PID_ERROR));
		bytesToLong(&packetToReceive.content[offset], 4, &e);

		return e ; //TODO: what means this number?
	}

//******************************************************************************

	DECLDIR int GetPidOutput_SH(int BoardNumber)
    {
		int BytesSent, BytesReceived, offset, u;
		CharBuff packetToBeSent, packetToReceive;
		SckID=boardSckID[BoardNumber];

		//Sends request
		buildTCPCommand(&packetToBeSent, GET_PID_OUTPUT, NULL);
		BytesSent = SendTCP_SH(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;

		//Receives answer
		BytesReceived = ReceiveTCP_SH(SckID, &packetToReceive);
		if (BytesReceived == -1)
			return -1;

		//PID output
		offset = getPayloadOffset(getDescriptor_TCPRep(REPLY_PID_OUTPUT));
		bytesToInt(&packetToReceive.content[offset], 2, &u);

		return u; //is it the duty cycle?
	}

//******************************************************************************

	DECLDIR int GetPidOffset_SH(int BoardNumber)
    {
		int BytesSent, BytesReceived, offset, off;
		CharBuff packetToBeSent, packetToReceive;
		SckID=boardSckID[BoardNumber];

		//Sends request
		buildTCPCommand(&packetToBeSent, GET_PID_OFFSET, NULL);
		BytesSent = SendTCP_SH(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;

		//Receives answer
		BytesReceived = ReceiveTCP_SH(SckID, &packetToReceive);
		if (BytesReceived == -1)
			return -1;

		//PID offset
		offset = getPayloadOffset(getDescriptor_TCPRep(REPLY_PID_OFFSET));
		bytesToInt(&packetToReceive.content[offset], 2, &off);

		return off;
	}

//******************************************************************************

	DECLDIR long GetPosition_SH(int BoardNumber)
    {
		int BytesSent, BytesReceived, offset;
		long pos;
		CharBuff packetToBeSent, packetToReceive;
		SckID=boardSckID[BoardNumber];

		//Sends request
		buildTCPCommand(&packetToBeSent, GET_ENCODER_POSITION, NULL);
		BytesSent = SendTCP_SH(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;

		//Receives answer
		BytesReceived = ReceiveTCP_SH(SckID, &packetToReceive);
		if (BytesReceived == -1)
			return -1;

		offset = getPayloadOffset(getDescriptor_TCPRep(REPLY_ENCODER_POSITION));
		bytesToLong(&packetToReceive.content[offset], 4, &pos);

		return pos;
	}

//******************************************************************************

	DECLDIR int GetVelocity_SH(int BoardNumber)
    {
		int BytesSent, BytesReceived, offset, v;
		CharBuff packetToBeSent, packetToReceive;
		SckID=boardSckID[BoardNumber];

		//Sends request
		buildTCPCommand(&packetToBeSent, GET_ENCODER_VELOCITY , NULL);
		BytesSent = SendTCP_SH(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		
		//Receives answer
		BytesReceived = ReceiveTCP_SH(SckID, &packetToReceive);
		if (BytesReceived == -1)
			return -1;

		offset = getPayloadOffset(getDescriptor_TCPRep(REPLY_ENCODER_VELOCITY));
		bytesToInt(&packetToReceive.content[offset], 2, &v);

		return v;
	}

//******************************************************************************

	DECLDIR int GetTorque_SH(int BoardNumber)	
    {
		int BytesSent, BytesReceived, offset, torque;
		CharBuff packetToBeSent, packetToReceive;
		SckID=boardSckID[BoardNumber];

		//Sends request
		buildTCPCommand(&packetToBeSent, GET_MEASURED_TORQUE , NULL);
		BytesSent = SendTCP_SH(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		
		//Receives answer
		BytesReceived = ReceiveTCP_SH(SckID, &packetToReceive);
		if (BytesReceived == -1)
			return -1;

		offset = getPayloadOffset(getDescriptor_TCPRep(REPLY_MEASURED_TORQUE));
		bytesToInt(&packetToReceive.content[offset], 2, &torque);

		return torque;
	}

//******************************************************************************

	DECLDIR long GetDesiredPosition_SH(int BoardNumber)
    {
		int BytesSent, BytesReceived, offset;
		long dp;
		CharBuff packetToBeSent, packetToReceive;
		SckID=boardSckID[BoardNumber];

		//Sends request
		buildTCPCommand(&packetToBeSent, GET_DESIRED_POSITION, NULL);
		BytesSent = SendTCP_SH(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;

		//Receives answer
		BytesReceived = ReceiveTCP_SH(SckID, &packetToReceive);
		if (BytesReceived == -1)
			return -1;

		offset = getPayloadOffset(getDescriptor_TCPRep(REPLY_DESIRED_POSITION));
		bytesToLong(&packetToReceive.content[offset], 4, &dp);

		return dp;
	}

//******************************************************************************

	DECLDIR int GetDesiredVelocity_SH(int BoardNumber)
    {
		int BytesSent, BytesReceived, offset, dv;
		CharBuff packetToBeSent, packetToReceive;
		SckID=boardSckID[BoardNumber];

		//Sends request
		buildTCPCommand(&packetToBeSent, GET_DESIRED_VELOCITY, NULL);
		BytesSent = SendTCP_SH(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;

		//Receives answer
		BytesReceived = ReceiveTCP_SH(SckID, &packetToReceive);
		if (BytesReceived == -1)
			return -1;

		offset = getPayloadOffset(getDescriptor_TCPRep(REPLY_DESIRED_VELOCITY));
		bytesToInt(&packetToReceive.content[offset], 2, &dv);

		return dv;
	}

//******************************************************************************

	DECLDIR int GetDesiredTorque_SH(int BoardNumber)
    {
		int BytesSent, BytesReceived, offset, dt;
		CharBuff packetToBeSent, packetToReceive;
		SckID=boardSckID[BoardNumber];

		//Sends request
		buildTCPCommand(&packetToBeSent, GET_DESIRED_TORQUE, NULL);
		BytesSent = SendTCP_SH(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;

		//Receives answer
		BytesReceived = ReceiveTCP_SH(SckID, &packetToReceive);
		if (BytesReceived == -1)
			return -1;

		offset = getPayloadOffset(getDescriptor_TCPRep(REPLY_DESIRED_TORQUE));
		bytesToInt(&packetToReceive.content[offset], 2, &dt);

		return dt;
	}

//******************************************************************************

	DECLDIR int SetAccel_SH(int BoardNumber, int Accel)
    {
		int BytesSent;
		char payload[2];
		CharBuff packetToBeSent;
		SckID=boardSckID[BoardNumber];

		//Sends request
		intToBytes(Accel, payload, sizeof(payload));

		buildTCPCommand(&packetToBeSent, SET_ACCEL, payload);
		BytesSent = SendTCP_SH(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		else
			return 0;
	}

//******************************************************************************

	DECLDIR int GetAccel_SH(int BoardNumber)
    {
		int BytesSent, BytesReceived, offset, a;
		CharBuff packetToBeSent, packetToReceive;
		SckID=boardSckID[BoardNumber];

		//Sends request
		buildTCPCommand(&packetToBeSent, GET_ACCEL, NULL);
		BytesSent = SendTCP_SH(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;

		//Receives answer
		BytesReceived = ReceiveTCP_SH(SckID, &packetToReceive);
		if (BytesReceived == -1)
			return -1;

		offset = getPayloadOffset(getDescriptor_TCPRep(REPLY_ACCEL));
		bytesToInt(&packetToReceive.content[offset], 2, &a);

		return a;
	}

//******************************************************************************

	DECLDIR int SetMinPosition_SH(int BoardNumber, long MinPosition)
    {
		int BytesSent;
		char payload[4];
		CharBuff packetToBeSent;
		SckID=boardSckID[BoardNumber];

		//Sends request
		longToBytes(MinPosition, payload, sizeof(payload));

		buildTCPCommand(&packetToBeSent, SET_MIN_POSITION, payload);
		BytesSent = SendTCP_SH(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		else
			return 0;
	}

//******************************************************************************

	DECLDIR long GetMinPosition_SH(int BoardNumber)
    {
		int BytesSent, BytesReceived, offset;
		long minPos;
		CharBuff packetToBeSent, packetToReceive;
		SckID=boardSckID[BoardNumber];

		//Sends request
		buildTCPCommand(&packetToBeSent, GET_MIN_POSITION, NULL);
		BytesSent = SendTCP_SH(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;

		//Receives answer
		BytesReceived = ReceiveTCP_SH(SckID, &packetToReceive);
		if (BytesReceived == -1)
			return -1;

		offset = getPayloadOffset(getDescriptor_TCPRep(REPLY_MIN_POSITION));
		bytesToLong(&packetToReceive.content[offset], 4, &minPos);

		return minPos;
	}

//******************************************************************************

	DECLDIR int SetMaxPosition_SH(int BoardNumber, long MaxPosition)
    {
		int BytesSent;
		char payload[4];
		CharBuff packetToBeSent;
		SckID=boardSckID[BoardNumber];

		//Sends request
		longToBytes(MaxPosition, payload, sizeof(payload));

		buildTCPCommand(&packetToBeSent, SET_MAX_POSITION, payload);
		BytesSent = SendTCP_SH(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		else
			return 0;
	}

//******************************************************************************

	DECLDIR long GetMaxPosition_SH(int BoardNumber)
    {
		int BytesSent, BytesReceived, offset;
		long maxPos;
		CharBuff packetToBeSent, packetToReceive;
		SckID=boardSckID[BoardNumber];

		//Sends request
		buildTCPCommand(&packetToBeSent, GET_MAX_POSITION, NULL);
		BytesSent = SendTCP_SH(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;

		//Receives answer
		BytesReceived = ReceiveTCP_SH(SckID, &packetToReceive);
		if (BytesReceived == -1)
			return -1;

		offset = getPayloadOffset(getDescriptor_TCPRep(REPLY_MAX_POSITION));
		bytesToLong(&packetToReceive.content[offset], 4, &maxPos);

		return maxPos;
	}

//******************************************************************************

	DECLDIR int SetMaxVelocity_SH(int BoardNumber, int MaxVelocity)
    {
		int BytesSent;
		char payload[2];
		CharBuff packetToBeSent;
		SckID=boardSckID[BoardNumber];

		//Sends request
		intToBytes(MaxVelocity, payload, sizeof(payload));

		buildTCPCommand(&packetToBeSent, SET_MAX_VELOCITY, payload);
		BytesSent = SendTCP_SH(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		else
			return 0;
	}

//******************************************************************************

	DECLDIR int GetMaxVelocity_SH(int BoardNumber)
    {
		int BytesSent, BytesReceived, offset, maxVel;
		CharBuff packetToBeSent, packetToReceive;
		SckID=boardSckID[BoardNumber];

		//Sends request
		buildTCPCommand(&packetToBeSent, GET_MAX_VELOCITY, NULL);
		BytesSent = SendTCP_SH(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;

		//Receives answer
		BytesReceived = ReceiveTCP_SH(SckID, &packetToReceive);
		if (BytesReceived == -1)
			return -1;

		offset = getPayloadOffset(getDescriptor_TCPRep(REPLY_MAX_VELOCITY));
		bytesToInt(&packetToReceive.content[offset], 2, &maxVel);

		return maxVel;
	}

//******************************************************************************

	DECLDIR int SetMaxTorque_SH(int BoardNumber, int MaxTorque)
    {
		int BytesSent;
		char payload[2];
		CharBuff packetToBeSent;
		SckID=boardSckID[BoardNumber];

		//Sends request
		intToBytes(MaxTorque, payload, sizeof(payload));

		buildTCPCommand(&packetToBeSent, SET_MAX_TORQUE, payload);
		BytesSent = SendTCP_SH(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		else
			return 0;
	}

//******************************************************************************

	DECLDIR int GetMaxTorque_SH(int BoardNumber)
    {
		int BytesSent, BytesReceived, offset, maxT;
		CharBuff packetToBeSent, packetToReceive;
		SckID=boardSckID[BoardNumber];

		//Sends request	
		buildTCPCommand(&packetToBeSent, GET_MAX_TORQUE, NULL);
		BytesSent = SendTCP_SH(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;

		//Receives answer
		BytesReceived = ReceiveTCP_SH(SckID, &packetToReceive);
		if (BytesReceived == -1)
			return -1;

		offset = getPayloadOffset(getDescriptor_TCPRep(REPLY_MAX_TORQUE));
		bytesToInt(&packetToReceive.content[offset], 2, &maxT);

		return maxT;
	}

//******************************************************************************

	DECLDIR int SetMinVelocity_SH(int BoardNumber, int MinVelocity)
    {
		int BytesSent;
		char payload[2];
		CharBuff packetToBeSent;
		SckID=boardSckID[BoardNumber];

		//Sends request
		intToBytes(MinVelocity, payload, sizeof(payload));

		buildTCPCommand(&packetToBeSent, SET_MIN_VELOCITY, payload);
		BytesSent = SendTCP_SH(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		else
			return 0;
	}

//******************************************************************************

	DECLDIR int GetMinVelocity_SH(int BoardNumber)
    {
		int BytesSent, BytesReceived, offset, minVel;
		CharBuff packetToBeSent, packetToReceive;
		SckID=boardSckID[BoardNumber];

		//Sends request
		buildTCPCommand(&packetToBeSent, GET_MIN_VELOCITY, NULL);
		BytesSent = SendTCP_SH(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;

		//Receives answer
		BytesReceived = ReceiveTCP_SH(SckID, &packetToReceive);
		if (BytesReceived == -1)
			return -1;

		offset = getPayloadOffset(getDescriptor_TCPRep(REPLY_MIN_VELOCITY));
		bytesToInt(&packetToReceive.content[offset], 2, &minVel);

		return minVel;
	}

//******************************************************************************

	DECLDIR int SetCurrentLimit_SH(int BoardNumber, int CurrentLimit)
    {
		int BytesSent;
		char payload[2];
		CharBuff packetToBeSent;
		SckID=boardSckID[BoardNumber];

		//Sends request
		intToBytes(CurrentLimit, payload, sizeof(payload));

		buildTCPCommand(&packetToBeSent, SET_CURRENT_LIMIT, payload);
		BytesSent = SendTCP_SH(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		else
			return 0;
	}

//******************************************************************************

	DECLDIR int GetCurrentLimit_SH(int BoardNumber)
    {
		int BytesSent, BytesReceived, offset, iLim;
		CharBuff packetToBeSent, packetToReceive;
		SckID=boardSckID[BoardNumber];

		//Sends request
		buildTCPCommand(&packetToBeSent, GET_CURRENT_LIMIT, NULL);
		BytesSent = SendTCP_SH(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;

		//Receives answer
		BytesReceived = ReceiveTCP_SH(SckID, &packetToReceive);
		if (BytesReceived == -1)
			return -1;

		offset = getPayloadOffset(getDescriptor_TCPRep(REPLY_CURRENT_LIMIT));
		bytesToInt(&packetToReceive.content[offset], 2, &iLim);

		return iLim;
	}

//******************************************************************************

//DECLDIR int SetBCastRate_SH(int BoardNumber, int BCastRate, char TwinSatus)
DECLDIR int SetBCastRate_SH(int BoardNumber, int BCastRate)
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
		BytesSent = SendTCP_SH(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		else
			return 0;
	}

DECLDIR int StopBCast_SH(int BoardNumber)
{
		int BytesSent;
		char payload[2];
		CharBuff packetToBeSent;
		SckID=boardSckID[BoardNumber];

		//Sends request
		payload[0] = 10;   // this can be anything since the boards will stop  broadcasting
		payload[1] = 0;  //"0" means stop the broadcast 
		buildTCPCommand(&packetToBeSent, SET_BCAST_RATE, payload);
		BytesSent = SendTCP_SH(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		else
			return 0;
}

//******************************************************************************

	DECLDIR int GetBCastRate_SH(int BoardNumber)
    {
		int BytesSent, BytesReceived, offset, bc;
		CharBuff packetToBeSent, packetToReceive;
		SckID=boardSckID[BoardNumber];

		//Sends request
		buildTCPCommand(&packetToBeSent, GET_BCAST_RATE, NULL);
		BytesSent = SendTCP_SH(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;

		//Receives answer
		BytesReceived = ReceiveTCP_SH(SckID, &packetToReceive);
		if (BytesReceived == -1)
			return -1;

		offset = getPayloadOffset(getDescriptor_TCPRep(REPLY_BCAST_RATE));
		bytesToInt(&packetToReceive.content[offset], 1, &bc);

		return bc;
	}

//******************************************************************************

	DECLDIR int SetBCastPolicy_SH(int BoardNumber, int BCastPolicy)
    {
		int BytesSent;
		char payload[2];
		CharBuff packetToBeSent;
		SckID=boardSckID[BoardNumber];

		//Sends request
		intToBytes(BCastPolicy, payload, sizeof(payload));

		buildTCPCommand(&packetToBeSent, SET_BCAST_POLICY, payload);
		BytesSent = SendTCP_SH(SckID, &packetToBeSent);


		int extraBCastPolicy=3584;
		//Sends request
		intToBytes(extraBCastPolicy, payload, sizeof(payload));

		buildTCPCommand(&packetToBeSent, SET_EXTRA_BCAST_POLICY, payload);
		BytesSent = SendTCP_SH(SckID, &packetToBeSent);
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
//		BytesSent = SendTCP_SH(SckID, &packetToBeSent);
//		if (BytesSent == -1)
//			return -1;
//		else
//			return 0;
//	}

//******************************************************************************

	DECLDIR int GetBCastPolicy_SH(int BoardNumber)
    {
		int BytesSent, BytesReceived, offset, bc;
		CharBuff packetToBeSent, packetToReceive;
		SckID=boardSckID[BoardNumber];

		//Sends request		
		buildTCPCommand(&packetToBeSent, GET_BCAST_POLICY, NULL);
		BytesSent = SendTCP_SH(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;

		//Receives answer
		BytesReceived = ReceiveTCP_SH(SckID, &packetToReceive);
		if (BytesReceived == -1)
			return -1;

		offset = getPayloadOffset(getDescriptor_TCPRep(REPLY_BCAST_POLICY));
		bytesToInt(&packetToReceive.content[offset], 2, &bc);

		return bc;
	}

//******************************************************************************

	DECLDIR int SetEncoderLines_SH(int BoardNumber, long EncoderLines)
    {
		int BytesSent;
		char payload[4];
		CharBuff packetToBeSent;
		SckID=boardSckID[BoardNumber];

		//Sends request
		longToBytes(EncoderLines, payload, sizeof(payload));

		buildTCPCommand(&packetToBeSent, SET_ENCODER_LINES, payload);
		BytesSent = SendTCP_SH(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		else
			return 0;
	}

//******************************************************************************

	DECLDIR long GetEncoderLines_SH(int BoardNumber)
    {
		int BytesSent, BytesReceived, offset;
		long el;
		CharBuff packetToBeSent, packetToReceive;
		SckID=boardSckID[BoardNumber];

		//Sends request
		buildTCPCommand(&packetToBeSent, GET_ENCODER_LINES, NULL);
		BytesSent = SendTCP_SH(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;

		//Receives answer
		BytesReceived = ReceiveTCP_SH(SckID, &packetToReceive);
		if (BytesReceived == -1)
			return -1;

		offset = getPayloadOffset(getDescriptor_TCPRep(REPLY_ENCODER_LINES));
		bytesToLong(&packetToReceive.content[offset], 4, &el);

		return el;
	}

//******************************************************************************

	DECLDIR int SetMotorPoles_SH(int BoardNumber, char poles)
    {
		int BytesSent;
		CharBuff packetToBeSent;
		SckID=boardSckID[BoardNumber];

		//Sends request
		buildTCPCommand(&packetToBeSent, SET_MOTOR_POLES, &poles);
		BytesSent = SendTCP_SH(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		else
			return 0;
	}

//******************************************************************************

	DECLDIR int GetMotorPoles_SH(int BoardNumber)
    {
		int BytesSent, BytesReceived, offset, mp;
		CharBuff packetToBeSent, packetToReceive;
		SckID=boardSckID[BoardNumber];

		//Sends request
		buildTCPCommand(&packetToBeSent, GET_MOTOR_POLES, NULL);
		BytesSent = SendTCP_SH(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;

		//Receives answer
		BytesReceived = ReceiveTCP_SH(SckID, &packetToReceive);
		if (BytesReceived == -1)
			return -1;

		offset = getPayloadOffset(getDescriptor_TCPRep(REPLY_MOTOR_POLES));
		bytesToInt(&packetToReceive.content[offset], 1, &mp);

		return mp;
	}

//******************************************************************************

	DECLDIR int SetAnalogInputs_SH(int BoardNumber, char anInputs)
    {
		int BytesSent;
		CharBuff packetToBeSent;
		SckID=boardSckID[BoardNumber];

		//Sends request
		buildTCPCommand(&packetToBeSent, SET_ANALOG_INPUTS, &anInputs);
		BytesSent = SendTCP_SH(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		else
			return 0;
	}
//******************************************************************************
	DECLDIR int DoRecalculatePosition_SH(int BoardNumber, char Calib)
    {
		int BytesSent;
		CharBuff packetToBeSent;
		SckID=boardSckID[BoardNumber];

		//Sends request
		buildTCPCommand(&packetToBeSent, DO_RECALCULATE_POSITION, &Calib);
		BytesSent = SendTCP_SH(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		else
			return 0;
	}

//******************************************************************************

	DECLDIR int GetAnalogInputs_SH(int BoardNumber)
    {
		int BytesSent, BytesReceived, offset, ai;
		CharBuff packetToBeSent, packetToReceive;
		SckID=boardSckID[BoardNumber];

		//Sends request		
		buildTCPCommand(&packetToBeSent, GET_ANALOG_INPUTS, NULL);
		BytesSent = SendTCP_SH(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;

		//Receives answer
		BytesReceived = ReceiveTCP_SH(SckID, &packetToReceive);
		if (BytesReceived == -1)
			return -1;

		offset = getPayloadOffset(getDescriptor_TCPRep(REPLY_ANALOG_INPUTS));
		bytesToInt(&packetToReceive.content[offset], 1, &ai);

		return ai;
	}

//******************************************************************************

	DECLDIR int CmdUpgrade_SH(int BoardNumber)
    {
		int BytesSent;
		CharBuff packetToBeSent;
		SckID=boardSckID[BoardNumber];

		//Sends request		
		buildTCPCommand(&packetToBeSent, CMD_UPGRADE, NULL);
		BytesSent = SendTCP_SH(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		else
			return 0;
	}

//******************************************************************************

	DECLDIR int SaveParamsToFlash_SH(int BoardNumber)
    {
		int BytesSent;
		CharBuff packetToBeSent;
		SckID=boardSckID[BoardNumber];

		//Sends request		
		buildTCPCommand(&packetToBeSent, SAVE_PARAMS_TO_FLASH, NULL);
		BytesSent = SendTCP_SH(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		else
			return 0;
	}

//******************************************************************************

	DECLDIR int LoadParamsFromFlash_SH(int BoardNumber) //TODO: does it receive any answer?
    {
		int BytesSent;
		CharBuff packetToBeSent;
		SckID=boardSckID[BoardNumber];

		//Sends request		
		buildTCPCommand(&packetToBeSent, LOAD_PARAMS_FROM_FLASH, NULL);
		BytesSent = SendTCP_SH(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		else
			return 0;
	}

//******************************************************************************

	DECLDIR int LoadDefaultParams_SH(int BoardNumber)
    {
		int BytesSent;
		CharBuff packetToBeSent;
		SckID=boardSckID[BoardNumber];

		//Sends request		
		buildTCPCommand(&packetToBeSent, LOAD_DEFAULT_PARAMS, NULL);
		BytesSent = SendTCP_SH(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		else
			return 0;
	}

//******************************************************************************

    DECLDIR int ControllerRun_SH(int BoardNumber)
    {
		int BytesSent;
		CharBuff packetToBeSent;
		SckID=boardSckID[BoardNumber];

		//Sends request		
		buildTCPCommand(&packetToBeSent, CONTROLLER_RUN, NULL);
		BytesSent = SendTCP_SH(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		else
			return 0;
	}

//******************************************************************************

    DECLDIR int ControllerIdle_SH(int BoardNumber)
	{
		int BytesSent;
		CharBuff packetToBeSent;
		SckID=boardSckID[BoardNumber];

		//Sends request
		buildTCPCommand(&packetToBeSent, CONTROLLER_IDLE, NULL);
		BytesSent = SendTCP_SH(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		else
			return 0;
	}

//******************************************************************************

	DECLDIR int EnablePwmPad_SH(int BoardNumber)
	{
		int BytesSent;
		CharBuff packetToBeSent;
		SckID=boardSckID[BoardNumber];

		//Sends request
		buildTCPCommand(&packetToBeSent, ENABLE_PWM_PAD, NULL);
		BytesSent = SendTCP_SH(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		else
			return 0;
	}

//******************************************************************************

	DECLDIR int DisablePwmPad_SH(int BoardNumber)
	{
		int BytesSent;
		CharBuff packetToBeSent;
		SckID=boardSckID[BoardNumber];

		//Sends request
		buildTCPCommand(&packetToBeSent, DISABLE_PWM_PAD, NULL);
		BytesSent = SendTCP_SH(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		else
			return 0;
	}

//******************************************************************************

	DECLDIR int DoCalibrate_SH(int BoardNumber, int direction, long initPos)
	{
		int BytesSent;
		char payload[5];
		CharBuff packetToBeSent;
		SckID=boardSckID[BoardNumber];

		//Sends request
		payload[0] = direction;
		longToBytes(initPos, payload+1, sizeof(payload)-1);

		buildTCPCommand(&packetToBeSent, DO_CALIBRATE, payload);
		BytesSent = SendTCP_SH(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		else
			return 0;
	}

//******************************************************************************

	DECLDIR int SetCalibrationCurrent_SH(int BoardNumber, int i)
    {
		int BytesSent;
		char payload[2];
		CharBuff packetToBeSent;
		SckID=boardSckID[BoardNumber];

		//Sends request
		intToBytes(i, payload, sizeof(payload));

		buildTCPCommand(&packetToBeSent, SET_CALIBRATION_CURRENT, payload);
		BytesSent = SendTCP_SH(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		else
			return 0;
	}

//******************************************************************************

	DECLDIR int GetCalibrationCurrent_SH(int BoardNumber)
	{
		int BytesSent, BytesReceived, offset, i;
		CharBuff packetToBeSent, packetToReceive;
		SckID=boardSckID[BoardNumber];

		//Sends request
		buildTCPCommand(&packetToBeSent, GET_CALIBRATION_CURRENT, NULL);
		BytesSent = SendTCP_SH(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
	
		//Receives answer
		BytesReceived = ReceiveTCP_SH(SckID, &packetToReceive);
		if (BytesReceived == -1)
			return -1;

		offset = getPayloadOffset(getDescriptor_TCPRep(REPLY_CALIBRATION_CURRENT));
		bytesToInt(&packetToReceive.content[offset], 2, &i);

		return i;
	}

//******************************************************************************

	DECLDIR int SetAbsoluteZero_SH(int BoardNumber, int absZero)
	{
		int BytesSent;
		char payload[2];
		CharBuff packetToBeSent;
		SckID=boardSckID[BoardNumber];

		//Sends request
		intToBytes(absZero, payload, sizeof(payload));

		buildTCPCommand(&packetToBeSent, SET_ABSOLUTE_ZERO, payload);
		BytesSent = SendTCP_SH(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		else
			return 0;
	}


//******************************************************************************

	DECLDIR int GetAbsoluteZero_SH(int BoardNumber)
    {
		int BytesSent, BytesReceived, offset, az;
		CharBuff packetToBeSent, packetToReceive;
		SckID=boardSckID[BoardNumber];

		//Sends request
		buildTCPCommand(&packetToBeSent, GET_ABSOLUTE_ZERO, NULL);
		BytesSent = SendTCP_SH(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;

		//Receives answer
		BytesReceived = ReceiveTCP_SH(SckID, &packetToReceive);
		if (BytesReceived == -1)
			return -1;

		offset = getPayloadOffset(getDescriptor_TCPRep(REPLY_ABSOLUTE_ZERO));
		bytesToInt(&packetToReceive.content[offset], 2, &az);

		return az;
	}

//******************************************************************************

	DECLDIR int SetTorqueAccel_SH(int BoardNumber, char tAccel)
	{
		int BytesSent;
		CharBuff packetToBeSent;
		SckID=boardSckID[BoardNumber];

		//Sends request
		buildTCPCommand(&packetToBeSent, SET_TORQUE_ACCEL, &tAccel);
		BytesSent = SendTCP_SH(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		else
			return 0;
	}


//******************************************************************************

	DECLDIR int GetTorque_SHAccel_SH(int BoardNumber)
    {
		int BytesSent, BytesReceived, offset, ta;
		CharBuff packetToBeSent, packetToReceive;
		SckID=boardSckID[BoardNumber];

		//Sends request
		buildTCPCommand(&packetToBeSent, GET_TORQUE_ACCEL, NULL);
		BytesSent = SendTCP_SH(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;

		//Receives answer
		BytesReceived = ReceiveTCP_SH(SckID, &packetToReceive);
		if (BytesReceived == -1)
			return -1;

		offset = getPayloadOffset(getDescriptor_TCPRep(REPLY_TORQUE_ACCEL));
		bytesToInt(&packetToReceive.content[offset], 2, &ta);

		return ta;
	}

//******************************************************************************

	//MotorType: 1 for Brushed DC Motor and 0 for Brushless DC Motor
	DECLDIR int SetMotorType_SH(int BoardNumber, char motorType)
	{
		int BytesSent;
		CharBuff packetToBeSent;
		SckID=boardSckID[BoardNumber];

		//Sends request
		buildTCPCommand(&packetToBeSent, SET_MOTOR_TYPE , &motorType);
		BytesSent = SendTCP_SH(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		else
			return 0;

	}

//******************************************************************************

	DECLDIR int GetMotorType_SH(int BoardNumber)
	{
		int BytesSent, BytesReceived, offset, mt;
		CharBuff packetToBeSent, packetToReceive;
		SckID=boardSckID[BoardNumber];

		//MotorType: 1 for Brushed DC Motor and 0 for Brushless DC Motor

		//Sends request
		buildTCPCommand(&packetToBeSent, GET_MOTOR_TYPE , NULL);
		BytesSent = SendTCP_SH(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;

		//Receives answer
		BytesReceived = ReceiveTCP_SH(SckID, &packetToReceive);
		if (BytesReceived == -1)
			return -1;

		offset = getPayloadOffset(getDescriptor_TCPRep(REPLY_MOTOR_TYPE));
		bytesToInt(&packetToReceive.content[offset], 1, &mt);

		return mt;

	}

//******************************************************************************

	DECLDIR void CloseSockets_SH(void)
	{
		int b=0;
		for(b=0;b<MAX_BOARDS_SH;b++)
			closesocket(boardSckID[b]);
		closesocket(UDPSckID);
	}


DECLDIR int SetMinAbsPosition_SH(int BoardNumber, int minAbsPos)
	{
		int BytesSent;
		char payload[2];
		CharBuff packetToBeSent;
		SckID=boardSckID[BoardNumber];

		//Sends request
		intToBytes(minAbsPos, payload, sizeof(payload));

		buildTCPCommand(&packetToBeSent, SET_MIN_ABS_POSITION, payload);
		BytesSent = SendTCP_SH(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		else
			return 0;
	}


//******************************************************************************

	DECLDIR int GetMinAbsPosition_SH(int BoardNumber)
    {
		int BytesSent, BytesReceived, offset, minAbsPos;
		CharBuff packetToBeSent, packetToReceive;
		SckID=boardSckID[BoardNumber];

		//Sends request
		buildTCPCommand(&packetToBeSent, GET_MIN_ABS_POSITION, NULL);
		BytesSent = SendTCP_SH(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;

		//Receives answer
		BytesReceived = ReceiveTCP_SH(SckID, &packetToReceive);
		if (BytesReceived == -1)
			return -1;

		offset = getPayloadOffset(getDescriptor_TCPRep(REPLY_MIN_ABS_POSITION));
		bytesToInt(&packetToReceive.content[offset], 2, &minAbsPos);

		return minAbsPos;
	}

	DECLDIR int SetMaxAbsPosition_SH(int BoardNumber, int maxAbsPos)
	{
		int BytesSent;
		char payload[2];
		CharBuff packetToBeSent;
		SckID=boardSckID[BoardNumber];

		//Sends request
		intToBytes(maxAbsPos, payload, sizeof(payload));

		buildTCPCommand(&packetToBeSent, SET_MAX_ABS_POSITION, payload);
		BytesSent = SendTCP_SH(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		else
			return 0;
	}


//******************************************************************************

	DECLDIR int GetMaxAbsPosition_SH(int BoardNumber)
    {
		int BytesSent, BytesReceived, offset, maxAbsPos;
		CharBuff packetToBeSent, packetToReceive;
		SckID=boardSckID[BoardNumber];

		//Sends request
		buildTCPCommand(&packetToBeSent, GET_MAX_ABS_POSITION, NULL);
		BytesSent = SendTCP_SH(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;

		//Receives answer
		BytesReceived = ReceiveTCP_SH(SckID, &packetToReceive);
		if (BytesReceived == -1)
			return -1;

		offset = getPayloadOffset(getDescriptor_TCPRep(REPLY_MAX_ABS_POSITION));
		bytesToInt(&packetToReceive.content[offset], 2, &maxAbsPos);

		return maxAbsPos;
	}




//******************************************************************************
// -------------------------------- UDP ---------------------------------------
//******************************************************************************

	//DECLDIR int ConnectUDP_SH(void)
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

	DECLDIR int ConnectUDP_SH(void)
	{
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

		iOptVal = 1;
		iOptVal2=100;//set receive buffer to make communication faster
		iOptLen = sizeof(int);

		// if (setsockopt(UDPSckID, SOL_SOCKET, SO_BROADCAST, (char*)&iOptVal, iOptLen) == SOCKET_ERROR) //Allow Broadcast Failed
		if (setsockopt(UDPSckID, SOL_SOCKET, SO_BROADCAST, (char*)&iOptVal, iOptLen) == 
			SOCKET_ERROR||setsockopt(UDPSckID, SOL_SOCKET, SO_RCVBUF, (char*)&iOptVal2, iOptLen) == SOCKET_ERROR) //Allow Broadcast Failed

			return -2;

		ReceiverAddr.sin_family = AF_INET;
		ReceiverAddr.sin_port = htons(Port);
		ReceiverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

		// Associate the address information with the socket using bind.
		// At this point you can receive datagrams on your bound socket.

		if (bind(UDPSckID, (SOCKADDR *)&ReceiverAddr, sizeof(ReceiverAddr)) == SOCKET_ERROR) //Error at bind()
		{
			closesocket(UDPSckID);
			WSACleanup();
			return -3;
		}

		ReceiverAddr.sin_addr.s_addr = inet_addr(ip_addr_str);

		return UDPSckID;
	}
	//-------------------------------------------------------------------------------



//******************************************************************************

	DECLDIR int SendUDP_SH(SOCKET UDPSckID, CharBuff* packetToBeSent)
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

	DECLDIR int recvfromTimeOutUDP_SH(SOCKET socket, long sec, long usec)

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

	DECLDIR int ReceiveUDP_SH(SOCKET UDPSckID, CharBuff* packetToReceive)
	{
		SOCKADDR_IN SenderAddr;
		int BytesReceived, SelectTiming, SenderAddrSize;
		
		SelectTiming = recvfromTimeOutUDP_SH(UDPSckID, 1, 100);
		
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

	DECLDIR int ReceiveUDP_SHWithTimeOut_SH(SOCKET UDPSckID, CharBuff* packetToReceive)
	{
		SOCKADDR_IN SenderAddr;
		int BytesReceived, SelectTiming, SenderAddrSize;
		
		SelectTiming = recvfromTimeOutUDP_SH(UDPSckID, 2, 1000);
		
		

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




	DECLDIR int WriteBCastData_SH(FILE * stream)
	{
		int BytesReceived, i;
		CharBuff packetToReceive;
		
		
		//TODO: I don't know why, but we can receive broadcast data just if we send some data first.
		//I chose to send the command "get active boards" (called in the interface)
		
/*		int activeBoards = GetActiveBoards_SH();
		if (activeBoards == -1)
			return -1;
		BytesReceived = ReceiveUDP_SH(UDPSckID, &packetToReceive); */

		BytesReceived = ReceiveUDP_SH(UDPSckID, &packetToReceive);

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


 DECLDIR int GetBCastData_SH(CharBuff &packetToReceive)
	{
		int BytesReceived;
		
		//TODO: I don't know why, but we can receive broadcast data just if we send some data first.
		//I chose to send the command "get active boards" (called in the interface)
		
	/*	int activeBoards = GetActiveBoards_SH();
		if (activeBoards == -1)
			return -1;
		BytesReceived = ReceiveUDP_SH(UDPSckID, &packetToReceive); */

		BytesReceived = ReceiveUDP_SH(UDPSckID, &packetToReceive);
		return BytesReceived;
		
	}


		
//******************************************************************************

	DECLDIR int GetActiveBoards_SH(int *boardIDs) //TODO: for more than 1 board
	{
		int i=0;

		if(numberOfActiveBoards>0)
			for(i=0;i<MAX_BOARDS_SH;i++)
				boardIDs[i] = activeBoardIDs[i];

		return numberOfActiveBoards;
	}



//******************************************************************************
DECLDIR int ScanForActiveBoards_SH(void)
{

		int BytesSent, BytesReceived, BoardNum;
		CharBuff packetToBeSent, packetToReceive;
		int i;
		


		//Sends request
		buildUDPCommand(&packetToBeSent, GET_ACTIVE_BOARDS , NULL);
		BytesSent = SendUDP_SH(UDPSckID, &packetToBeSent);


		if (BytesSent == -1)
			return -1;

	
	//	for(i=0;i<60000;i++);
		//Receives answer
		BytesReceived = ReceiveUDP_SHWithTimeOut_SH(UDPSckID, &packetToReceive); //Drops the first answer if it is equal to the sent one (due broadcast)
		if (BytesReceived == -1)
			return -2;

		numberOfActiveBoards=0;
		for(i=0;i<MAX_BOARDS_SH;i++)
			activeBoardIDs[i]=0;

		do{
			BytesReceived=0;
			BytesReceived = ReceiveUDP_SHWithTimeOut_SH(UDPSckID, &packetToReceive);
			if(BytesReceived>0)
				if(packetToReceive.content[2] == (char)0x82)   // if it is a board response packet
				  {
					BoardNum = (unsigned char)(packetToReceive.content[4]);
					activeBoardIDs[BoardNum-1]=BoardNum; // save board id
					numberOfActiveBoards++;
				  }
		}while(BytesReceived>0);

	//	activeBoardIDs[7]=8;
	//	numberOfActiveBoards++;
		if(numberOfActiveBoards>0)
			return numberOfActiveBoards;
		else	
			return -3;
}


//******************************************************************************

	//DECLDIR int SetDesiredPosition_SH(int DesPos[MAX_BOARDS_SH]) //TODO: to send the message if there is more than 1 board
	//{
	//	int BytesSent, BoardNum, i;
	//	char payload[PACKET_MAX_SIZE];
	//	CharBuff packetToBeSent;

	//	//Getting active board
	//	//BoardNum = GetActiveBoards_SH();

	//	BoardNum = MAX_BOARDS_SH;

	//	//Sends request
	//	for (i = 0; i<BoardNum; i++)
	//	{
	//		if(activeBoardIDs[i]<=0)
	//		{
	//			payload[4*i] = 0x00;
	//			payload[4*i+1] = 0x00;
	//			payload[4*i+2] = 0x00;
	//			payload[4*i+3] = 0x00;
	//		}	
	//		else
	//		{
	//			payload[4*i] =	 (DesPos[i] & 0x000000ff);
	//			payload[4*i+1] = ((DesPos[i]>>8) & 0x000000ff);
	//			payload[4*i+2] = ((DesPos[i]>>16) & 0x000000ff);
	//			payload[4*i+3] = ((DesPos[i]>>24) & 0x000000ff);

	//		/*	payload[4*i]   = DesPos[i]%256;
	//			payload[4*i+1] = DesPos[i]/256;
	//			payload[4*i+2] = DesPos[i]/65536;
	//			payload[4*i+3] = DesPos[i]/16777216; */

	//		}
	//	}

	///*	for (i = 0; i < (4*BoardNum-4); i++)
	//		payload[i] = 0x00;
	//	
	//	payload[4*BoardNum-4] = DesPos%256;
	//	payload[4*BoardNum-3] = DesPos/256;
	//	payload[4*BoardNum-2] = DesPos/65536;
	//	payload[4*BoardNum-1] = DesPos/16777216;  */

	//	buildUDPCommand(&packetToBeSent, SET_DESIRED_POSITION , payload);
	//	BytesSent = SendUDP_SH(UDPSckID, &packetToBeSent);
	//	if (BytesSent == -1)
	//		return -1;
	//	
	//	return 0;
	//}

//Modified for optitrack
	//DECLDIR int SetDesiredPosition_SH(int DesPos[MAX_BOARDS_SH],CharBuff * a_packetToBeSent) //TODO: to send the message if there is more than 1 board
	DECLDIR int SetDesiredPosition_SH(int DesPos[MAX_BOARDS_SH])
	{
		
		int BytesSent, BoardNum, i;
		char payload[PACKET_MAX_SIZE];
		CharBuff packetToBeSent;

		//Getting active board
		//BoardNum = GetActiveBoards_SH();

		BoardNum = MAX_BOARDS_SH;
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

		buildUDPCommand(&packetToBeSent, SET_DESIRED_POSITION , payload);
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
		
	//	return packetToBeSent;
		
		BytesSent = SendUDP_SH(UDPSckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		
		return 0;
		
	}

DECLDIR int SetDesiredStiffnessDamping_SH(int DesSiff[MAX_BOARDS_SH], int DesDamp[MAX_BOARDS_SH]) //TODO: to send the message if there is more than 1 board
	{
		int BytesSent, BoardNum, i;
		char payload[PACKET_MAX_SIZE];
		CharBuff packetToBeSent;

		//Getting active board
		//BoardNum = GetActiveBoards_SH();

		BoardNum = MAX_BOARDS_SH;

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
	
		BytesSent = SendUDP_SH(UDPSckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		
		return 0;
	}
//******************************************************************************

	DECLDIR int SetDesiredVelocity_SH(int DesVel[MAX_BOARDS_SH]) //TODO: to send the message if there is more than 1 board
	{
		int BytesSent, BoardNum, i;
		char payload[PACKET_MAX_SIZE];
		CharBuff packetToBeSent;

		//Getting active board
		//BoardNum = GetActiveBoards_SH();

		BoardNum = MAX_BOARDS_SH;

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
	
		BytesSent = SendUDP_SH(UDPSckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		
		return 0;
		
	}

	int SetDesiredTorque_SH(int DesTor[MAX_BOARDS_SH])
	{
		int BytesSent, BoardNum, i;
		char payload[PACKET_MAX_SIZE];
		CharBuff packetToBeSent;

		//Getting active board
		//BoardNum = GetActiveBoards_SH();

		BoardNum = MAX_BOARDS_SH;

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
	
		BytesSent = SendUDP_SH(UDPSckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		
		return 0;
	}

//******************************************************************************
DECLDIR int SetPidOffset_SH(int DesOffset[MAX_BOARDS_SH])
{

		int BytesSent, BoardNum, i;
		char payload[PACKET_MAX_SIZE];
		CharBuff packetToBeSent;

		//Getting active board
		//BoardNum = GetActiveBoards_SH();

		BoardNum = MAX_BOARDS_SH;

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
	
		BytesSent = SendUDP_SH(UDPSckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		
		return 0;
		
	}


//******************************************************************************

	DECLDIR int StartPositionControl_SH(void) //TODO: to send the message if there is more than 1 board
	{
		int BytesSent, BoardNum, i;
		char payload[PACKET_MAX_SIZE];
		CharBuff packetToBeSent;

		//Getting active board
	//	BoardNum = GetActiveBoards_SH();
		BoardNum = MAX_MOTOR_BOARDS_SH;

		//Sends request
		for (i = 0; i<BoardNum; i++)
			if(activeBoardIDs[i]<=0)
				payload[i] = 0x00;
			else
				payload[i] = 0x03;
		
		buildUDPCommand(&packetToBeSent, POSITION_MOVE , payload);
		BytesSent = SendUDP_SH(UDPSckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		
		return 0;
	}


//******************************************************************************

DECLDIR int StopPositionControl_SH(void) //TODO: to send the message if there is more than 1 board
	{
		int BytesSent, BoardNum, i;
		char payload[PACKET_MAX_SIZE];
		CharBuff packetToBeSent;

		//Getting active board
	//	BoardNum = GetActiveBoards_SH();
		BoardNum = MAX_MOTOR_BOARDS_SH;

		//Sends request
		for (i = 0; i<BoardNum; i++)
				payload[i] = 0x01;
		
		buildUDPCommand(&packetToBeSent, POSITION_MOVE , payload);
		BytesSent = SendUDP_SH(UDPSckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		
		return 0;
	}


//******************************************************************************

DECLDIR int StartBoardPositionControl_SH(int boardNumber)
	{
		int BytesSent, BoardNum, i;
		char payload[PACKET_MAX_SIZE];
		CharBuff packetToBeSent;

		//Getting active board
	//	BoardNum = GetActiveBoards_SH();
		BoardNum = MAX_MOTOR_BOARDS_SH;
		//Sends request
		for (i = 0; i<BoardNum; i++)     //stop all boards
			payload[i] = 0x00;   
		payload[boardNumber-1] = 0x03;   // start the selected board
		buildUDPCommand(&packetToBeSent, POSITION_MOVE , payload);
		BytesSent = SendUDP_SH(UDPSckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		
		return 0;
	}



//******************************************************************************

	DECLDIR int StartVelocityControl_SH(void) //TODO: to send the message if there is more than 1 board
	{
		int BytesSent, BoardNum, i;
		char payload[PACKET_MAX_SIZE];
		CharBuff packetToBeSent;

		//Getting active board
	//	BoardNum = GetActiveBoards_SH();
		BoardNum = MAX_BOARDS_SH;

		//Sends request
		for (i = 0; i<BoardNum; i++)
			if(activeBoardIDs[i]<=0)
				payload[i] = 0x00;
			else
				payload[i] = 0x03;
		
		buildUDPCommand(&packetToBeSent, VELOCITY_MOVE , payload);
		BytesSent = SendUDP_SH(UDPSckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		
		return 0;
	}

//******************************************************************************

	DECLDIR int StopMotorUDP_SH(void) //TODO: to send the message if there is more than 1 board
	{
		int BytesSent, BoardNum, i;
		char payload[PACKET_MAX_SIZE];
		CharBuff packetToBeSent;

		//Getting active board
//		BoardNum = GetActiveBoards_SH();
		BoardNum = MAX_BOARDS_SH;

		//Sends request
		for (i = 0; i < (BoardNum-1); i++)
			payload[i] = 0x00;
		
		payload[BoardNum - 1] = 0x01;
		
		buildUDPCommand(&packetToBeSent, POSITION_MOVE , payload);
		BytesSent = SendUDP_SH(UDPSckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		
		return 0;
	}

//******************************************************************************
	
//}

	//	NEW FUNCTIONS FOR IMPEDANCE CONTROL	//

DECLDIR int SetTorqueOnOff_SH(int BoardNumber, int torqueFlag)
    {
		int BytesSent;
		char payload[1];
		CharBuff packetToBeSent;
		SckID=boardSckID[BoardNumber];

		//Sends request
		intToBytes(torqueFlag, payload, sizeof(payload));

		buildTCPCommand(&packetToBeSent, SET_TORQUE_ONOFF, payload);
		BytesSent = SendTCP_SH(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		else
			return 0;
	}

DECLDIR int GetTorque_SHOnOff_SH(int BoardNumber)
    {
		int BytesSent, BytesReceived, offset, Tonoff;
		CharBuff packetToBeSent, packetToReceive;
		SckID=boardSckID[BoardNumber];

		//Sends request
		buildTCPCommand(&packetToBeSent, GET_TORQUE_ONOFF, NULL);
		BytesSent = SendTCP_SH(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;

		//Receives answer
		BytesReceived = ReceiveTCP_SH(SckID, &packetToReceive);
		if (BytesReceived == -1)
			return -1;

		//PID output
		offset = getPayloadOffset(getDescriptor_TCPRep(REPLY_TORQUE_ONOFF));
		bytesToInt(&packetToReceive.content[offset], 1, &Tonoff);

		return Tonoff; //
	}
	
DECLDIR int SetMotorConfig_SH(int BoardNumber, char *mConfig)
    {
		int BytesSent;
		char payload[2];
		CharBuff packetToBeSent;
		SckID=boardSckID[BoardNumber];
		
		sscanf_s(mConfig, "%d,%d", &payload[0], &payload[1]); //

		//Sends request
		buildTCPCommand(&packetToBeSent, SET_MOTOR_CONFIG, payload);
		BytesSent = SendTCP_SH(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		else
			return 0;
	}

DECLDIR int GetMotorConfig_SH(int BoardNumber)
    {
		int BytesSent, BytesReceived, offset, mConfig;
		CharBuff packetToBeSent, packetToReceive;
		SckID=boardSckID[BoardNumber];

		//Sends request
		buildTCPCommand(&packetToBeSent, GET_MOTOR_CONFIG, NULL);
		BytesSent = SendTCP_SH(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;

		//Receives answer
		BytesReceived = ReceiveTCP_SH(SckID, &packetToReceive);
		if (BytesReceived == -1)
			return -1;

		//
		offset = getPayloadOffset(getDescriptor_TCPRep(REPLY_MOTOR_CONFIG));
		bytesToInt(&packetToReceive.content[offset], 2, &mConfig);

		return mConfig;
	}

DECLDIR int SetPDlinkGains_SH(int BoardNumber, char *Gains)
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
		BytesSent = SendTCP_SH(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		else
			return 0;
	}

DECLDIR int GetPDlinkGains_SH(int BoardNumber, long* GainsBuf)
    {
		int BytesSent, BytesReceived, offset;
		long p, d;
		CharBuff packetToBeSent, packetToReceive;
		SckID=boardSckID[BoardNumber];

		//Sends request
		buildTCPCommand(&packetToBeSent, GET_PD_LINK_GAINS, NULL);
		BytesSent = SendTCP_SH(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;

		//Receives answer
		BytesReceived = ReceiveTCP_SH(SckID, &packetToReceive);
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

DECLDIR int CalibrateOffsets_SH(int BoardNumber)
{
	int BytesSent;
		CharBuff packetToBeSent;
		SckID=boardSckID[BoardNumber];

		//Sends request		
		buildTCPCommand(&packetToBeSent, CALIBRATE_OFFSETS, NULL);
		BytesSent = SendTCP_SH(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		else
			return 0;
}

//******************************************************************************//

DECLDIR int GetCalibrationOffsets_SH(int BoardNumber)
{
	int BytesSent;
		CharBuff packetToBeSent;
		SckID=boardSckID[BoardNumber];

		//Sends request		
		buildTCPCommand(&packetToBeSent, GET_CALIBRATION_OFFSETS, NULL);
		BytesSent = SendTCP_SH(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;
		else
			return 0;
}

//******************************************************************************//

DECLDIR int SetConversionFactors_SH(int BoardNumber, char channel, int multiplier, int offset)
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
	BytesSent = SendTCP_SH(SckID, &packetToBeSent);

	if (BytesSent == -1)
		return -1;
	else
		return 0;
}
//******************************************************************************//

DECLDIR int GetConversionFactors_SH(int BoardNumber, char channel, int* FactorsBuf)
{
	int BytesSent, BytesReceived, offset;
		int rchannel, multiplier, par_offset;
		CharBuff packetToBeSent, packetToReceive;
		SckID=boardSckID[BoardNumber];

		//Sends request
		buildTCPCommand(&packetToBeSent, GET_CONVERSION_FACTORS, &channel);
		BytesSent = SendTCP_SH(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;

		//Receives answer
		BytesReceived = ReceiveTCP_SH(SckID, &packetToReceive);
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

DECLDIR int SetAvarageSamples_SH(int BoardNumber, int samples)
{
	int BytesSent;
	char payload[13];
	CharBuff packetToBeSent;
	SckID=boardSckID[BoardNumber];

	//Sends request
	payload[0] = samples;

	buildTCPCommand(&packetToBeSent, SET_AVARAGE_SAMPLES, payload);
	BytesSent = SendTCP_SH(SckID, &packetToBeSent);

	if (BytesSent == -1)
		return -1;
	else
		return 0;
}
//******************************************************************************//

DECLDIR int GetAvarageSamples_SH(int BoardNumber, int samples)
{
	int BytesSent, BytesReceived, offset;
		CharBuff packetToBeSent, packetToReceive;
		SckID=boardSckID[BoardNumber];

		//Sends request
		buildTCPCommand(&packetToBeSent, GET_AVARAGE_SAMPLES, NULL);
		BytesSent = SendTCP_SH(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;

		//Receives answer
		BytesReceived = ReceiveTCP_SH(SckID, &packetToReceive);
		if (BytesReceived == -1)
			return -1;

		offset = getPayloadOffset(getDescriptor_TCPRep(REPLY_AVARAGE_SAMPLES)) + 1; //payload initial position
		bytesToInt(&packetToReceive.content[offset], 1, &samples); //
		
		return 0;
}
//******************************************************************************//

DECLDIR int SetMatrixRow_SH(int BoardNumber, char row, float *matrixRow)
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
	BytesSent = SendTCP_SH(SckID, &packetToBeSent);

	if (BytesSent == -1)
		return -1;
	else
		return 0;
}
//******************************************************************************//

DECLDIR int GetMatrixRow_SH(int BoardNumber, char row_req, char row_recv, float matrixRow[6])
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
		BytesSent = SendTCP_SH(SckID, &packetToBeSent);
		if (BytesSent == -1)
			return -1;

		//Receives answer
		BytesReceived = ReceiveTCP_SH(SckID, &packetToReceive);
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