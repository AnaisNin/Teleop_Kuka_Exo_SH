#pragma once

//#include <windows.h>
#include <WinSock2.h>

#define BufLenS 8
#define BufLenR 100//78

class UDP_Communication
{
public:
	UDP_Communication(void);
	~UDP_Communication(void);

	void InitializeWinSock();
	void UDPSendMessage(char MessageBuf[BufLenS]);
	int UDPReceieveMessage(char RecvBuf[BufLenR]);
};
