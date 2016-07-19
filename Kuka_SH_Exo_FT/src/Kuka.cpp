#include "Kuka.h"

double Kuka::m_desPos_EE[3]={0};
double Kuka::m_currentPos_EE[3]={0};

double Kuka::m_refPos_EE[3]={0};
double Kuka::m_refPos_workspace_EE[3]={0};

//Limit distances, to set
double Kuka::m_maxDist_EE=0.1;//m 0.1 works (not max, but this follows my fastest movements) - 0.014 for slow and safe operations - TO SET! max dist between 2 consecutive points sent to Kuka without make it break
double Kuka::m_boxSize_EE=0.5;//m

//For box 0.05


