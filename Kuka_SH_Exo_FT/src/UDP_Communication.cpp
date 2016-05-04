/*	This class implements functions that allow to communicate	*/
/*	over the network with the FT sensor.						*/


#include "stdafx.h"
#include "UDP_Communication.h"


//	Initialization of NETWORK variables
WSADATA wsaData;

SOCKET SendRecvSocket;

sockaddr_in Address;

int Port = 49152;

UDP_Communication::UDP_Communication(void)
{
}

UDP_Communication::~UDP_Communication(void)
{
}

void UDP_Communication::InitializeWinSock()
{
	//	This function initializes the Socket for UDP communication.	//

	// Initialize Winsock
	WSAStartup(MAKEWORD(2,2), &wsaData);

	// Create a socket for sending data
//WORKING	SendRecvSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	SendRecvSocket = WSASocket(AF_INET, SOCK_DGRAM, IPPROTO_UDP, NULL, 0, 0);



	// Create a receiver socket to receive datagrams
	Address.sin_family = AF_INET;
	//Insert the IP address of the FT Sensor

	//Address.sin_addr.s_addr = inet_addr("10.255.32.18");	//ATI 45		NetBox MAC:00 16 BD 00 03 0F (Jody's IP)
	//Address.sin_addr.s_addr = inet_addr("10.255.43.21");	//ATI 45		NetBox MAC:00 16 BD 00 03 0F
	//Address.sin_addr.s_addr = inet_addr("169.254.89.60");	//ATI 45		NetBox MAC:00 16 BD 00 03 0F
	Address.sin_addr.s_addr = inet_addr("192.168.0.15");	//ATI 45		NetBox MAC:00 16 BD 00 03 0F
	//Address.sin_addr.s_addr = inet_addr("10.255.32.133");	//ATI nano25	NetBox MAC:00 16 BD 00 03 AF (Iqbal's IP)
	Address.sin_port = htons(Port);

	int ATIcon = connect(SendRecvSocket,				// Socket
			(LPSOCKADDR)&Address,						// Server address
			sizeof(struct sockaddr));

}
void UDP_Communication::UDPSendMessage(char MessageBuf[BufLenS])
{
	//	This function sends a message over the network	//

	send(SendRecvSocket,				// Connected socket
				MessageBuf,			// Data buffer
				BufLenS,			// Length of data
				0);
}
int UDP_Communication::UDPReceieveMessage(char RecvBuf[BufLenR])
{
	//	This function receives a message from the network	//
	fd_set fds ;
	int n ;
	struct timeval tv ;

	// Set up the file descriptor set.
	FD_ZERO(&fds) ;
	FD_SET(SendRecvSocket, &fds) ;

	// Set up the struct timeval for the timeout.
	tv.tv_sec = 0 ;
	tv.tv_usec = 5000 ;

	// Wait until timeout or data received.
		n = select (SendRecvSocket, &fds, NULL, NULL, &tv ) ;
		if ( n == 0)
		{ 
		//printf("Timeout..\n");
		return 0 ;
		//	exit(0);
		}
		else if( n == -1 )
		{
		//printf("Error..\n");
		return -1;   
		}
		else{
		recv(SendRecvSocket, RecvBuf, BufLenR, 0);
		return 1;
		}

}