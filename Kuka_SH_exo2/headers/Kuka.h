#ifndef KUKA_H
#define KUKA_H

class Kuka
{
	public: 
		static double m_desPos_EE[3]; //optitrack
		static double m_currentPos_EE[3]; //current pos
		static double m_refPos_EE[3];//limit distance between 2 consecutive points EE to avoid Kuka to break - pos that is optitrack if distance from current pos is not too large, otherwise go to safe pos on the same direction, cf MathUtils
		static double m_refPos_workspace_EE[3];//Define a spherical workspace around the initial kuka pos (safety measure)
		
		static double m_maxDist_EE;
		static double m_boxSize_EE;



};

#endif