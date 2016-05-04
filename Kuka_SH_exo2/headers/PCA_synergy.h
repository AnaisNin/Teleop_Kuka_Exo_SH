#ifndef PCA_SYNERGY
#define PCA_SYNERGY

#include "CustomMatrix.h"
#include "Exoskeleton.h"
#include "Globals.h"
#include "SoftHandSyn.h"

class PCA_synergy  //PCA - Rq: syns=synergy space = pcs = principal component space (same)
{
	public:
		//Synergy Matrix
		static void set_synData_fromFile(); //OK - Read datafile from matrix - synergy matrix = coeff matrix from pca matlab - mean_dataCollection - min & max 1pc
		static CustomMatrix* get_pSynMatrix(){return pSynMatrix;}
		static void set_mean_dataCollection(double a_mean_dataCollection[Globals::n_pcs]);
		//User fnt
		static void update_syn_struct(Exoskeleton * pExoskeleton); //Extract data from exo + mean-centers
		static void compute_sigma_dot();
		static void compute_sigma1();
		static void compute_z();
		static void compute_F_syns(); 
		static void update_struct_from_syn(Exoskeleton * pExoskeleton);//Unstack vector F (18x1) and sets each finger Ftip
		//Getters - setters
		static double get_deltaSigma1(){return PCA_synergy::deltaSigma1;} //Error between pos at initial contact and current pos, in synergy space - delta_sigma1=sigma1_current - sigma1_initialContact if penetration, 0 else	
		static void set_deltaSigma1(const double & a_deltaSigma1);
		static void print_mean_dataCollection();
		static void get_z(double res_z[Globals::n_pcs]);
		static void get_F(double res_F[Globals::n_pcs]);

		//PUT BACK TO PRIVATE
		static double sigma1;//ref to softhand, from syn computation
		static double convert_sigmaScale2unitScale(const double & a_sigmaScale);//ok
		static double sigma1_range_dataCollection[2];//min(closed), max(open)
		//static void compute_posRef_syns(Exoskeleton * pExoskeleon,SoftHand * pSoftHand); //User fnt - extract data_ib from Exoskeleton, project on syns, extract 1PC, scale it between 0 and 1 and set softhand pos ref

	private:
	//fnt
		//Update syn members from exo and softhand
		static double convert_unitScale2sigmaScale(const double & a_unitScale);//to check
		static double convert_impObj_to_stiffSpring(const double & a_impObj);//TO CODE!
		
		static void update_x_dot_syns(Exoskeleton * pExoskeleton);
		static void update_x_syns(Exoskeleton * pExoskeleton);
		//Projection initial base <-> syns
		static void project_dataSet_ib2syns(double dataSet_ib[Globals::n_pcs],double res_dataSet_syns[Globals::n_pcs]); //called by compute_sigma_dot()
		static void project_force_syns2ib(const double a_force_syns[Globals::n_pcs],double res_force_ib[Globals::n_pcs]); //Called by compute_forceRef -- Sets force vector F=Sx * z with z=[z1 0 ... 0]'
		//Else
		static double compute_force_1stComp_syns(const double & stiff_VirtSpring,const double & deltaPos_syns);//called by compute_z() - WHAT ABOUT SCALING deltaPos?? return z1
		static void time_integrate(const double & x_dot,double & res_x); //TO CODE!
		//Check
		static double extract_fpc_scale_unitRange(double a_dataSet_syns[Globals::n_pcs]); //called by compute_posRef_syns - Extract and scale 1st PC s.t. 0 is palm opened, 1 is max closure
	

	//Variables - Rq: _syns refers to the dim (6x3=18), does not mean that the variable stored is expressed in synergistic coordinates
		//syn matrix
		static CustomMatrix* pSynMatrix; //(n_pcs x n_pcs) such that x_ib=SynMatrix * sc - Transpose of pEigenVectors
		static double mean_dataCollection[Globals::n_pcs];
		
		//pos
		static double x_dot_syns[Globals::n_pcs];//x_dot of three fingers stacked to constitute dataset to project on synergy space
		static double x_syns[Globals::n_pcs];//pos quat of three fingers stacked to constitute dataset to project
		static double sigma_dot_syns[Globals::n_pcs];
		static double sigma_syns[Globals::n_pcs];
		//static double sigma1;//current - projection of the current posture on the synergy space, 1st component
		//static double sigma1_initialContact; //updated from sofhand
		static double deltaSigma1;//interpenetration in the object - from softhand - in sigma scale
		//force
		static double stiff_vSpring_syns; //stiffness of the virtual spring used to compute the force reference from the postion error along first synergy s.t. z=stiff_spring_syns * delta_sc1
		static double z_syns[Globals::n_pcs];
		static double F_syns[Globals::n_pcs]; //force ref in initial base, computed as projection of force in synergy space z
		
		
		//Let's call Syn the synergy matrix S and sc the synergy coordinates, sc1 the first component ; the coordinates in the synergy space, that 
};

#endif

		//OLD - using coeff'
		//static void set_eigenVectors_fromFile(); //coeff'==S'
		//static CustomMatrix* get_pEigenVectors(){return pEigenVectors;} 
		//static double project_dataSet_pcs(double * dataSet_initialBase,double * res_dataSet_pcs); //return 1st component = res_dataSet_pcs[0]
		//static void update_z1_syns(); //Called by compute_forceRef -- z1_syns=stiff_vSpring_syns*delta_sc1
		//static void set_Is_softHand_contact(const bool & is_contact);
		//static void compute_forceRef_fingertip_ib(double res_forceRef[Globals::n_pcs]); //User fnt - Returns the an array of size n_pcs containing the force in base frame. Calls all the above fnt in proper order and returns the ref force vector
		//static void compute_forceRef_fingertip_ib(SoftHand * pSoftHand,Exoskeleton * pExoskeleton); //User fnt - Check if contact from softhand, if so compute force tip and set exo force vector 
		//static double z1_syns; //force along 1st synergy - due to position error in syn space and stiff virtual spring
		//static double dataSet_ib[Globals::n_pcs]; //initial base
		//static double dataSet_syns[Globals::n_pcs]; //principal component space==synergy space
		//static CustomMatrix* pEigenVectors; //(n_pcs x n_pcs) such that data_pcs=pEigenVectors * data_ib - so pEigenVectors=inv(Sx)=Sx', with Sx the synergy matrix such that x=Sx.s
