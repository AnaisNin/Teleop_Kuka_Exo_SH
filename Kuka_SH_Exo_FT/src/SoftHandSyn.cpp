#include "SoftHandSyn.h"

bool SoftHandSyn::m_is_contact = false;
double SoftHandSyn::m_impedance_object = 0;
double SoftHandSyn::m_posRef_unitScaled = 0;
double SoftHandSyn::m_pos_unitScale = 0;//measured position
double SoftHandSyn::m_pos_1stContact_unitScaled = 0;
double SoftHandSyn::m_tau_interaction = 0;
double SoftHandSyn::m_posRef_unitScale_filtered = 0;
double SoftHandSyn::m_tau_interaction_filtered = 0;
int SoftHandSyn::m_pos_firstContact = 0;
double SoftHandSyn::m_stiff_contact = 0;
int SoftHandSyn::posMin = 1000000;
int SoftHandSyn::posMax = 2200000;
double SoftHandSyn::m_tau_interaction_max=1.6; //Nm - TO SET - depends on torque observer settings, from 1.5 to 2
//double SoftHandSyn::m_deltaPos_current_1stContact_unitScaled; //scaled


void SoftHandSyn::set_pos_firstContact(const int & a_pos_firstContact)
{
	SoftHandSyn::m_pos_firstContact = a_pos_firstContact;
}

double SoftHandSyn::get_pos_firstContact_unitScale()
{
	double pos_firstContact_unitScale_loc = SoftHandSyn::convert_pos_shScale2unitScale(SoftHandSyn::m_pos_firstContact);
	return pos_firstContact_unitScale_loc;
}

double SoftHandSyn::convert_pos_shScale2unitScale(const int & pos_shScale)
{
	//posMin==open corresponds to 0; posMax==closed corresponds to 1
	//pos_unitScale=a*pos_sh+b with a=1/(posMax-posMin) and b=-posMin/(posMax-posMin)
	double pos_unitScale;
	pos_unitScale = 1 / (posMax - posMin)*pos_shScale - posMin / (posMax - posMin);

	if (pos_unitScale<0)
	{
		pos_unitScale = 0;
	}
	else if (pos_unitScale>1)
	{
		pos_unitScale = 1;
	}
	return pos_unitScale;
}

void SoftHandSyn::set_stiff_contact(const double & a_stiff_contact)
{
	if (a_stiff_contact<0)//Stiff cannot be negative
	{
		SoftHandSyn::m_stiff_contact = 0;
	}
	else
	{
		SoftHandSyn::m_stiff_contact = a_stiff_contact;
	}

}

void SoftHandSyn::set_tau_interaction(const double & a_tau_int)
{
	if (a_tau_int<0)
	{
		SoftHandSyn::m_tau_interaction = 0;
	}
	else
	{
		SoftHandSyn::m_tau_interaction = a_tau_int;
	}
}

void SoftHandSyn::set_tau_interaction_filtered(const double & a_tau_int_filtered)
{
	if (a_tau_int_filtered<0)
	{
		SoftHandSyn::m_tau_interaction_filtered = 0;
	}
	else
	{
		SoftHandSyn::m_tau_interaction_filtered = a_tau_int_filtered;
	}
}

void SoftHandSyn::set_pos_unitScale(const double & a_pos_unitScale)
{
	if (a_pos_unitScale<0)
	{
		SoftHandSyn::m_pos_unitScale = 0;
	}
	else if (a_pos_unitScale>1)
	{
		SoftHandSyn::m_pos_unitScale = 1;
	}
	else
	{
		SoftHandSyn::m_pos_unitScale = a_pos_unitScale;
	}
}

void SoftHandSyn::set_contact_state(const bool & a_is_contact)
{
	SoftHandSyn::m_is_contact = a_is_contact;
}

bool SoftHandSyn::get_contact_state()
{
	return SoftHandSyn::m_is_contact;
}

void SoftHandSyn::set_posRef_unitScaled(const double & a_unitScaled_fpc)
{
	double pos = a_unitScaled_fpc;

	if (pos<0)
	{
		pos = 0;
	}
	else if (pos>1)
	{
		pos = 1;
	}
	SoftHandSyn::m_posRef_unitScaled = pos;
}

void SoftHandSyn::set_posRef_unitScale_filtered(const double & a_posRef_unitScale_filtered)
{
	double pos = a_posRef_unitScale_filtered;

	if (pos<0)
	{
		pos = 0;
	}
	else if (pos>1)
	{
		pos = 1;
	}
	SoftHandSyn::m_posRef_unitScale_filtered = pos;

}

void SoftHandSyn::updateClassMembers(bool a_contact_state, double a_impedance_object, double a_pos_firstContact_unitScale, double a_pos_unitScale, double a_tau_interaction)
{
	SoftHandSyn::set_contact_state(a_contact_state);
	SoftHandSyn::set_impedance_object(a_impedance_object);

	if (a_pos_firstContact_unitScale<0)
	{
		//pos_firstContact_unitScale_loc = 0;
		SoftHandSyn::set_pos_1stContact_unitScaled(0);
	}
	else if (a_pos_firstContact_unitScale>1)
	{
		//pos_firstContact_unitScale_loc = 1;

		SoftHandSyn::set_pos_1stContact_unitScaled(1);
	}
	else
		SoftHandSyn::set_pos_1stContact_unitScaled(a_pos_firstContact_unitScale);
	

	//measured pos unit scale
	SoftHandSyn::set_pos_unitScale(a_pos_unitScale);

	//Tau interact
	SoftHandSyn::set_tau_interaction(a_tau_interaction);
}

void SoftHandSyn::set_impedance_object(const double & a_imp_obj)
{
	SoftHandSyn::m_impedance_object = a_imp_obj;
}

void SoftHandSyn::set_pos_1stContact_unitScaled(const double & a_pos_1stContact_unitScaled)
{
	SoftHandSyn::m_pos_1stContact_unitScaled = a_pos_1stContact_unitScaled;
}

double SoftHandSyn::get_pos_initialContact_unitScaled()
{
	return SoftHandSyn::m_pos_1stContact_unitScaled;
}

double SoftHandSyn::get_impedance_object()
{
	return SoftHandSyn::m_impedance_object;
}


double SoftHandSyn::get_posRef_unitScaled()
{
	return SoftHandSyn::m_posRef_unitScaled;
}