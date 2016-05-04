//#pragma once
#ifndef SOFTHANDSYN_H
#define SOFTHANDSYN_H

class SoftHandSyn
{
public:
	//Set class members from softhand 
	static void set_contact_state(const bool & a_is_contact); //Set from softhand
	static void set_impedance_object(const double & a_imp_obj); //Set from softhand

	static void set_posRef_unitScaled(const double & a_unitScaled_fpc); //Set from exo posture and PCA_synergy
	static void set_pos_1stContact_unitScaled(const double & a_pos_1stContact_unitScaled); //BUILD ENV FOR THIS FNT
	static void set_pos_unitScale(const double & a_pos_unitScale);
	static void set_pos_firstContact(const int & a_pos_firstContact);
	static double get_pos_firstContact_unitScale();
	static double convert_pos_shScale2unitScale(const int & pos_shScale);
	static void set_stiff_contact(const double & a_stiff_contact);
	static void set_tau_interaction(const double & a_tau_int);
	static void set_tau_interaction_filtered(const double & a_tau_int_filtered);
	static void set_posRef_unitScale_filtered(const double & a_posRef_unitScale_filtered);

	static void updateClassMembers(bool a_contact_state, double a_impedance_object, double a_pos_firstContact_unitScale, double a_pos_unitScale, double a_tau_interaction);

	static int get_pos_firstContact()
	{
		return m_pos_firstContact;
	}


	static int posMin;
	static int posMax;

	//Getters
	static bool get_contact_state();
	static double get_posRef_unitScaled(); //Sent to linux
	static double get_posRef_unitScale_filtered()
	{
		return SoftHandSyn::m_posRef_unitScale_filtered;
	} //Sent to linux

	static double get_impedance_object();
	static double get_pos_initialContact_unitScaled();//from linux
	static double get_pos_unitScale()
	{
		return SoftHandSyn::m_pos_unitScale;
	}//measured, from linux
	static double get_tau_interaction()
	{
		return SoftHandSyn::m_tau_interaction;
	}
	static double get_tau_interaction_filtered()
	{
		return SoftHandSyn::m_tau_interaction_filtered;
	}
	//static double get_deltaPos_current_1stContact();
	//static void update_deltaPos_current_1stContact();

private:
	static bool m_is_contact;
	static double m_impedance_object;
	static double m_pos_unitScale;//measured position
	static double m_posRef_unitScaled; //scaled fpc from syn analysis, 0 for open, 1 for maxClosure
	static double m_posRef_unitScale_filtered;
	static double m_pos_1stContact_unitScaled; //corresponds to sigma_firstContact_unitScale
	static double m_tau_interaction;
	static double m_tau_interaction_filtered;
	static int m_pos_firstContact;
	static double m_stiff_contact;
	//static double m_deltaPos_current_1stContact_unitScaled; //scaled
	//Unpack function: set is_in_
	//Find a way to check if it is FIRST contact (check previous state), if it is then update m_pos_1st_contact_scaled

};
#endif