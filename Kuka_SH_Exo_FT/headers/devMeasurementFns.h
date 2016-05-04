#pragma once
#include "UDP_Communication.h"


class devMeasurementFns
{
public:
	devMeasurementFns(void);
	~devMeasurementFns(void);

	void RequestFTsensorData();
	void BiasFTsensor();
	int ReadFTsensorData();
	void GetFTData(double (&FTData)[6]);
	void GetRawFTData(double (&FTData)[6]);
};

extern UDP_Communication *UDPcomm;
