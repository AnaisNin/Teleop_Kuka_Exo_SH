#include "PCA_synergy.h"
#include <stdio.h> // for FILE
#include <cmath>

//Initialization class members - Reminder: 'static' is specified in the .h but not repeated in the .cpp
double PCA_synergy::stiff_vSpring_syns=1; //TO SET!
//double PCA_synergy::sigma1_initialContact=0;
double PCA_synergy::sigma1=0;
double PCA_synergy::sigma_dot_syns[Globals::n_pcs];
double PCA_synergy::z_syns[Globals::n_pcs];
double PCA_synergy::F_syns[Globals::n_pcs]={0};
double PCA_synergy::x_dot_syns[Globals::n_pcs];
double PCA_synergy::x_syns[Globals::n_pcs];
CustomMatrix* PCA_synergy::pSynMatrix=new CustomMatrix(Globals::n_pcs,Globals::n_pcs);
double PCA_synergy::mean_dataCollection[Globals::n_pcs];
double PCA_synergy::sigma1_range_dataCollection[2];
double PCA_synergy::sigma_syns[Globals::n_pcs];
double  PCA_synergy::deltaSigma1;//from softhand
double PCA_synergy::m_forceGains[Globals::numFingers]={1};
double PCA_synergy::F_syns_scaled[Globals::n_pcs]; //force ref in initial base, computed as projection of force in synergy space z


//TO CHECK
//double PCA_synergy::get_delta_sigma1() 
//{
//	double delta_sigma1_loc=0;
//	printf("CHECK: if sigma1 decreases when closing the hand, change signs [PCA_synergy::get_delta_sigma1]\n");
//	//We assume here that sc increases when we close the hand. If not, change signs.
//	if(PCA_synergy::sigma1<PCA_synergy::sigma1_initialContact) //penetration
//	{
//		delta_sigma1_loc=PCA_synergy::sigma1-PCA_synergy::sigma1_initialContact;
//		//printf("[PCA_synergy::get_delta_sigma1] Penetration in the object \n");
//	}
//	else
//	{
//		delta_sigma1_loc=0;
//		//printf("[PCA_synergy::get_delta_sigma1] NO penetration in the object \n");
//	}
//
//	return delta_sigma1_loc;
//}

void PCA_synergy::set_deltaSigma1(const double & a_deltaSigma1)
{

	double deltaSigma1_max=PCA_synergy::sigma1_range_dataCollection[1]-PCA_synergy::sigma1_range_dataCollection[0];
	if(a_deltaSigma1>deltaSigma1_max)
	{
			PCA_synergy::deltaSigma1=deltaSigma1_max;
	}
	 PCA_synergy::deltaSigma1=a_deltaSigma1;
}


void PCA_synergy::set_synData_fromFile() //OK
{
		FILE * pFilePCA;
		if( (pFilePCA=fopen("log/File_synergyMatrix_fromMatlab.txt","r")) == NULL ) // C4996
			printf( "ERROR while opening log/File_synergyMatrix_fromMatlab.txt \n" );
		else
		{
			double row[Globals::n_pcs];

			for(int iti=0;iti<Globals::n_pcs;iti++)
			{
				//Reads line by line
				if(Globals::IDF_ACTIVE_FINGERS==1) //index finger only
				{
					//fscanf(pFilePCA, "%lg %lg %lg %lg %lg %lg %lg",row,row+1,row+2,row+3,row+4,row+5,row+6); // %lg == %lf == double; %g == %f == floating point number with no length=precision specification, cf http://www.cplusplus.com/reference/cstdio/scanf/
					fscanf(pFilePCA, "%lg %lg %lg",row,row+1,row+2); // %lg == %lf == double; %g == %f == floating point number with no length=precision specification, cf http://www.cplusplus.com/reference/cstdio/scanf/

				}
				else if(Globals::IDF_ACTIVE_FINGERS==3)
				{
					//fscanf(pFilePCA, "%lg %lg %lg %lg %lg %lg %lg %lg %lg %lg %lg %lg %lg %lg %lg %lg %lg %lg %lg %lg %lg",row,row+1,row+2,row+3,row+4,row+5,row+6,row+7,row+8,row+9,row+10,row+11,row+12,row+13,row+14,row+15,row+16,row+17,row+18,row+19,row+20); // %lg == %lf == double; %g == %f == floating point number with no length=precision specification, cf http://www.cplusplus.com/reference/cstdio/scanf/
					fscanf(pFilePCA, "%lg %lg %lg %lg %lg %lg %lg %lg %lg",row,row+1,row+2,row+3,row+4,row+5,row+6,row+7,row+8); // %lg == %lf == double; %g == %f == floating point number with no length=precision specification, cf http://www.cplusplus.com/reference/cstdio/scanf/

				}
				else
				{
					printf("ERROR: IDF_ACTIVE_FINGERS should be either 1 (1 index finger) or 3 (3 fingers). Exit. [PCA_synergy::set_synData_fromFile]\n");
				}
				//Extract column
				for(int itj=0;itj<Globals::n_pcs;itj++)
				{
					(*PCA_synergy::pSynMatrix)(iti+1,itj+1)=row[itj];
				}
			}
			//Next raw - data mean
			fscanf(pFilePCA, "%lg %lg %lg %lg %lg %lg %lg %lg %lg",row,row+1,row+2,row+3,row+4,row+5,row+6,row+7,row+8);
			set_mean_dataCollection(row);
			//Next raw - min max for 1pc
			fscanf(pFilePCA, "%lg %lg",PCA_synergy::sigma1_range_dataCollection,PCA_synergy::sigma1_range_dataCollection+1);

			//Printf
			//printf("End of fscanf. Synergy matrix: \n");
			//PCA_synergy::pSynMatrix->printf_Matrix();
			//printf("DataCollection mean: \n");
			//for(int it=0;it<Globals::n_pcs;it++)
			//{
			//	printf("%g \t",PCA_synergy::mean_dataCollection[it]);
			//}
			//printf("\n Range of 1PC from dataCollection: %g %g\n",PCA_synergy::sigma1_range_dataCollection[0],PCA_synergy::sigma1_range_dataCollection[1]);

			fclose(pFilePCA);

		}//else: valid file
}


void PCA_synergy::print_mean_dataCollection()
{
	printf("Mean data collection [PCA_synergy]: \n");
	for(int it=0;it<Globals::n_pcs;it++)
	{
		printf("%g \t",PCA_synergy::mean_dataCollection[it]);
	}
	printf("\n");
}

//TO CHECK
void PCA_synergy::project_dataSet_ib2syns(double dataSet_ib[Globals::n_pcs],double res_dataSet_syns[Globals::n_pcs]) //Unscaled
{
	//ib=initial base, syns=synergy space
	//Put initial data set in a column vector
	CustomMatrix * pX_ib=new CustomMatrix(Globals::n_pcs,1);//data set in initial base, column vector (n_pcs x 1)
	for (int it=0;it<Globals::n_pcs;it++)
	{
		(*pX_ib)(it+1,1)=dataSet_ib[it];
	}
	//printf("Data to project: [PCA_synergy::project_DataSet_ib2syns]\n");
	//pX_ib->printf_Matrix();

	//Project on principal component space=synergy space 
	//x=S.sc -> sc=S'.x with sc=synergy components/coefficients

	//Compute S'
	CustomMatrix * p_SynMatrix_transpose=PCA_synergy::pSynMatrix->transpose();

	//Compute p_sc = synergy coeff
	CustomMatrix * p_sc=CustomMatrix::MatrixProduct(p_SynMatrix_transpose,pX_ib);
	//printf("[PCA_synergy::project_dataSet_ib2syns] Projected data in syns (should be a n_pcs x 1 column vector): \n");
	//p_sc->printf_Matrix();

	//Copy result in the arg array
	for (int it=0;it<Globals::n_pcs;it++)
	{
		res_dataSet_syns[it]=(*p_sc)(it+1,1);
	}
}

void PCA_synergy::update_syn_struct(Exoskeleton * pExoskeleton) //Stack - RQ: everything in softhand is static so no need to get the object
{
//Exoskeleton
	//PCA_synergy::update_x_dot_syns(pExoskeleton);//Updates x_dot_syns (stack x_dot 3 fingers)
	PCA_synergy::update_x_syns(pExoskeleton); //Stacked pos thumb,index,middle - mean_dataCollection
	/*printf("x_syns: [PCA_synergy::update_syn_struct] \n");
	for(int it=0;it<Globals::n_pcs;it++)
	{
		printf("%g \t ",PCA_synergy::x_syns[it]);
	}
	printf("\n");*/

//SoftHand
	//k virtual spring
	double impObj_loc=SoftHandSyn::get_impedance_object();
	double stiff_vSpring_loc=PCA_synergy::convert_impObj_to_stiffSpring(impObj_loc);
	PCA_synergy::stiff_vSpring_syns=stiff_vSpring_loc;
	//delta_sigma1
	double sigma_initContact_unitScale_loc=SoftHandSyn::get_pos_initialContact_unitScaled();
	double sigma_initContact_loc=PCA_synergy::convert_unitScale2sigmaScale(sigma_initContact_unitScale_loc);

	double sigma_unitScale_loc=SoftHandSyn::get_pos_unitScale();//to code! measure pos
	double sigma_loc=PCA_synergy::convert_unitScale2sigmaScale(sigma_unitScale_loc);

	double delta_sigma1_loc=sigma_initContact_loc-sigma_loc;
    PCA_synergy::set_deltaSigma1(delta_sigma1_loc);
}

double PCA_synergy::convert_impObj_to_stiffSpring(const double & a_impObj)//TO CODE!
{
	//printf("TO CODE! [PCA_synergy::convert_impObj_to_stiffSpring]\n");
	double stiff_spring_loc=a_impObj;
	return stiff_spring_loc;
}

//void PCA_synergy::update_sigma1_initialContact(const double pos_unitScale_initialContact) //A lot of imbrication here..
//{
//	PCA_synergy::sigma1_initialContact=PCA_synergy::convert_unitScale2sigmaScale(pos_unitScale_initialContact);
//}

void PCA_synergy::update_x_dot_syns(Exoskeleton * pExoskeleton) //Stack vel vectors of the three fingers to create the dataset to project
{
	if(Globals::IDF_ACTIVE_FINGERS==1) //one finger (index)
		{
			for(int it=0;it<Globals::mCart_kins_pf;it++)
			{
				PCA_synergy::x_dot_syns[it]=pExoskeleton->aFingers[1].velTip_rt[it];
			}
		}
		else if(Globals::IDF_ACTIVE_FINGERS==3) //3fingers
		{
			int iti;
			//Thumb
			for(int it=0;it<Globals::mCart_kins_pf;it++)
			{
				PCA_synergy::x_dot_syns[it]=pExoskeleton->aFingers[0].velTip_rt[it];
			}
			//Index
			for(int it=Globals::mCart_kins_pf;it<2*Globals::mCart_kins_pf;it++)
			{
				iti=it-Globals::mCart_kins_pf;
				PCA_synergy::x_dot_syns[it]=pExoskeleton->aFingers[1].velTip_rt[iti];
			}
			//Middle
			for(int it=2*Globals::mCart_kins_pf;it<3*Globals::mCart_kins_pf;it++)
			{
				iti=it-2*Globals::mCart_kins_pf;
				PCA_synergy::x_dot_syns[it]=pExoskeleton->aFingers[2].velTip_rt[iti];
			}
		}//if 3fingers
		else
		{
			printf("ERROR: IDF_ACTIVE_FINGERS should be either 1 (1 index finger) or 3 (3 fingers). Exit. [PCA_synergy::update_x_dot_syns]\n");
		}	
}

void PCA_synergy::update_x_syns(Exoskeleton * pExoskeleton) //If using pos-quat instead of vel
{
	//if(Globals::IDF_ACTIVE_FINGERS==1) //one finger (index)
	//	{
	//		PCA_synergy::x_syns[0]=pExoskeleton->aFingers[1].posTip[0];
	//		PCA_synergy::x_syns[1]=pExoskeleton->aFingers[1].posTip[1];
	//		PCA_synergy::x_syns[2]=pExoskeleton->aFingers[1].posTip[2];

	//		PCA_synergy::x_syns[3]=pExoskeleton->aFingers[1].quatTip[0];
	//		PCA_synergy::x_syns[4]=pExoskeleton->aFingers[1].quatTip[1];
	//		PCA_synergy::x_syns[5]=pExoskeleton->aFingers[1].quatTip[2];
	//		PCA_synergy::x_syns[6]=pExoskeleton->aFingers[1].quatTip[3];
	//	}
	//	else if(Globals::IDF_ACTIVE_FINGERS==3) //3fingers
	//	{
	//		//Thumb
	//		PCA_synergy::x_syns[0]=pExoskeleton->aFingers[0].posTip[0];
	//		PCA_synergy::x_syns[1]=pExoskeleton->aFingers[0].posTip[1];
	//		PCA_synergy::x_syns[2]=pExoskeleton->aFingers[0].posTip[2];
	//		PCA_synergy::x_syns[3]=pExoskeleton->aFingers[0].quatTip[0];
	//		PCA_synergy::x_syns[4]=pExoskeleton->aFingers[0].quatTip[1];
	//		PCA_synergy::x_syns[5]=pExoskeleton->aFingers[0].quatTip[2];
	//		PCA_synergy::x_syns[6]=pExoskeleton->aFingers[0].quatTip[3];

	//		//Index
	//		PCA_synergy::x_syns[7]=pExoskeleton->aFingers[1].posTip[0];
	//		PCA_synergy::x_syns[8]=pExoskeleton->aFingers[1].posTip[1];
	//		PCA_synergy::x_syns[9]=pExoskeleton->aFingers[1].posTip[2];
	//		PCA_synergy::x_syns[10]=pExoskeleton->aFingers[1].quatTip[0];
	//		PCA_synergy::x_syns[11]=pExoskeleton->aFingers[1].quatTip[1];
	//		PCA_synergy::x_syns[12]=pExoskeleton->aFingers[1].quatTip[2];
	//		PCA_synergy::x_syns[13]=pExoskeleton->aFingers[1].quatTip[3];

	//		//Middle
	//		PCA_synergy::x_syns[14]=pExoskeleton->aFingers[2].posTip[0];
	//		PCA_synergy::x_syns[15]=pExoskeleton->aFingers[2].posTip[1];
	//		PCA_synergy::x_syns[16]=pExoskeleton->aFingers[2].posTip[2];
	//		PCA_synergy::x_syns[17]=pExoskeleton->aFingers[2].quatTip[0];
	//		PCA_synergy::x_syns[18]=pExoskeleton->aFingers[2].quatTip[1];
	//		PCA_synergy::x_syns[19]=pExoskeleton->aFingers[2].quatTip[2];
	//		PCA_synergy::x_syns[20]=pExoskeleton->aFingers[2].quatTip[3];
	//	}
	//	else
	//	{
	//		printf("ERROR: IDF_ACTIVE_FINGERS should be either 1 (1 index finger) or 3 (3 fingers). Exit. [PCA_synergy::update_x_syns]\n");
	//	}

	//Using pos only
	PCA_synergy::x_syns[0]=pExoskeleton->aFingers[0].posTip[0];
	PCA_synergy::x_syns[1]=pExoskeleton->aFingers[0].posTip[1];
	PCA_synergy::x_syns[2]=pExoskeleton->aFingers[0].posTip[2];
	PCA_synergy::x_syns[3]=pExoskeleton->aFingers[1].posTip[0];
	PCA_synergy::x_syns[4]=pExoskeleton->aFingers[1].posTip[1];
	PCA_synergy::x_syns[5]=pExoskeleton->aFingers[1].posTip[2];
	PCA_synergy::x_syns[6]=pExoskeleton->aFingers[2].posTip[0];
	PCA_synergy::x_syns[7]=pExoskeleton->aFingers[2].posTip[1];
	PCA_synergy::x_syns[8]=pExoskeleton->aFingers[2].posTip[2];

	//Substract mean to get mean-centered data
	for(int it=0;it<Globals::n_pcs;it++)
	{
		PCA_synergy::x_syns[it]-=PCA_synergy::mean_dataCollection[it];
	}
}



//TO IMPROVE
double PCA_synergy::extract_fpc_scale_unitRange(double a_dataSet_syns[Globals::n_pcs])
{
	//unscaled: 0 for opened, -0.3 for max closure
	//desired output: 0 for opened, 1 for max closure

	//x axis=input=unscaled 1PC, y axis=output=scaled 1PC
	//point A=opened, point B=max closure
	//desired yA=0 , yB=1
	//output= a.input + b, a?b?
	// yA = a.xA + b
	// yB = a.xB + b
	// so a = (yB-yA)/(xB-xA)=(1-0)/(xB-xA)=1/(xB-xA)
	// and b = yA - a.xA = 0 - a.xA

	//Values observed for first component unscaled, to adjust if necessary 
	double xA=0; //value of 1PC for open hand - unscaled
	double xB=-0.030; //value of 1PC for max closure - unscaled
	//Ensure that the arg is in the proper range
	double input=a_dataSet_syns[0]; //1st PC unscaled
	if(input<xB)
	{
		printf("[PCA_synergy::scaling] Value of raw 1st PC out of range, apply saturation \n");
		input=xB;
	}
	else if(input>xA)
	{
		printf("[PCA_synergy::scaling] Value of raw 1st PC out of range, apply saturation \n");
		input=xA;
	}

	//Compute output=scaled value
	double a=1/(xB-xA); //slope
	double b=-a*xA; //intersection with 0 axis
	double scaled_value= a * input + b;

	// Double chck: ensure that result is between 0 and 1
	if(scaled_value<0)
	{
		printf("[PCA_synergy::scaling] Abnormal saturation of scaled value, CHECK \n");
		scaled_value=0;
	}
	else if(scaled_value>1)
	{
		printf("[PCA_synergy::scaling] Abnormal saturation of scaled value, CHECK \n");
		scaled_value=1;
	}
	return scaled_value; //1st PC scaled between 0 and 1
}

//TO FINISH!
//void PCA_synergy::update_stiff_vSpring_syns(const double a_objectImpedance)
//{
//	printf("ADD proper scaling between objectImpedance and k [PCA_synergy::update_stiff_vSpring]\n");
//	//...
//	PCA_synergy::stiff_vSpring_syns=a_objectImpedance;
//}

void PCA_synergy::compute_sigma_dot() //project from initial base to synergy base - 
{
	//x_dot=S.sigma_dot, everything in syns dim
	//sigma_dot=S^t.x_dot

	PCA_synergy::project_dataSet_ib2syns(PCA_synergy::x_dot_syns,PCA_synergy::sigma_dot_syns);
}

void PCA_synergy::compute_sigma1()//double for integration
{
	//Pos
	PCA_synergy::project_dataSet_ib2syns(PCA_synergy::x_syns,PCA_synergy::sigma_syns);
	//printf("Data projected to synergy space: [PCA_synergy::compute_sigma1()]");
	//for(int it=0;it<Globals::n_pcs;it++)
	//{
	//	printf("%g \t",PCA_synergy::sigma_syns[it]);
	//}
	PCA_synergy::sigma1=PCA_synergy::sigma_syns[0];
	//printf("sigma1: %g [PCA_synergy::compute_sigma1()\n]",PCA_synergy::sigma1);
	
	//Vel
	//PCA_synergy::time_integrate(PCA_synergy::sigma_dot_syns[0],PCA_synergy::sigma1);
}

void PCA_synergy::time_integrate(const double & x_dot,double & res_x) //Adjust param list and so on. Internal fnt, use arg - called by PCA_synergy::compute_sigma
{
	printf("TO CODE! Time integration [PCA_synergy::time_integrate]\n");
	res_x=0;
}

void PCA_synergy::compute_z()
{
	//Case z1=stiff*delta_sigma1
	//double z1=PCA_synergy::compute_force_1stComp_syns(PCA_synergy::stiff_vSpring_syns,PCA_synergy::deltaSigma1);
	
	//Case z1=tau_interaction
	//double z1=SoftHandSyn::get_tau_interaction();//Nm
	double z1=SoftHandSyn::get_tau_interaction_filtered();//Nm
	
	PCA_synergy::z_syns[0]=z1;
	for(int it=1;it<Globals::n_pcs;it++)
	{
		z_syns[it]=0;
	}
}

void PCA_synergy::get_z(double res_z[Globals::n_pcs])
{
	for(int it=0;it<Globals::n_pcs;it++)
	{
		res_z[it]=PCA_synergy::z_syns[it];
	}
}

double PCA_synergy::compute_force_1stComp_syns(const double & a_stiff_VirtSpring,const double & a_deltaPos_syns)//WHAT ABOUT SCALING??
{
	double z1=a_stiff_VirtSpring * a_deltaPos_syns;
	//printf("[PCA_synergy::compute_force_syns] z1: %g \n",z1);
	return z1;
}

void PCA_synergy::compute_F_syns()
{
	//F=S^(-T).z
	//As S is ortho, S'=S^(-1) so S^(-T)=(S^T)^T=S
	//So F=S.z
	PCA_synergy::project_force_syns2ib(PCA_synergy::z_syns,PCA_synergy::F_syns);//Sets F_syns
}

//TO CHECK
void PCA_synergy::project_force_syns2ib(const double a_force_syns[Globals::n_pcs],double res_force_ib[Globals::n_pcs]) // //Sets force vector F=Sx * z with z=[z1 0 ... 0]'
{
	//Convert array to column vector for force in syns
	CustomMatrix * pZ_syns=new CustomMatrix(Globals::n_pcs,1); //(nx1)
	for(int it=0;it<Globals::n_pcs;it++)
	{
		(*pZ_syns)(it+1,1)=a_force_syns[it];
	}
	//printf("[PCA_synergy::project_force_syns2ib] Force vector in syns z: \n");
	//pZ_syns->printf_Matrix();

	//Project z on ib through synergy matrix: F=Sx*z
	CustomMatrix * pF=CustomMatrix::MatrixProduct(PCA_synergy::pSynMatrix,pZ_syns);
    //printf("[PCA_synergy::project_force_syns2ib] Force vector in cartesian space F (should be n_pcs x 1): \n");
	//pF->printf_Matrix();

	//Set arg
	for (int it=0;it<Globals::n_pcs;it++)
	{
		res_force_ib[it]=(*pF)(it+1,1);
	}
}

void PCA_synergy::update_struct_from_syn(Exoskeleton * pExoskeleton)
{
	//Exo - force ref
	//pExoskeleton->set_force_tip_des_base(PCA_synergy::F_syns);
	pExoskeleton->set_force_tip_des_base(PCA_synergy::F_syns_scaled);

	//SoftHand - pos ref
	double posRef_unitScale_loc=PCA_synergy::convert_sigmaScale2unitScale(PCA_synergy::sigma1);
	SoftHandSyn::set_posRef_unitScaled(posRef_unitScale_loc);
}

void PCA_synergy::get_F(double res_F[Globals::n_pcs])
{
	for(int it=0;it<Globals::n_pcs;it++)
	{
		res_F[it]=PCA_synergy::F_syns[it];
	}
}

double PCA_synergy::convert_sigmaScale2unitScale(const double & a_sigmaScale) //ok - 0 for fully opened, 1 for fully closed
{
	//To get 1 for closed, 0 for open
	//sigma_min=closed, corresponds to 1, sigma_max=opened, corresponds to 0
	//y=ax+b - Y1=0,x1=sigma_max, y2=1,x2=sigma_min 
	//a=(y1-y2)/(x1-x2)
	//b=y1-a.x1
	
	//double a=1/(PCA_synergy::sigma1_range_dataCollection[1]-PCA_synergy::sigma1_range_dataCollection[0]);//1/(sigma_max-sigma_min)
	//double b=1-a*PCA_synergy::sigma1_range_dataCollection[1];//sigma_min
	//double x_unitScale_loc=a*a_sigmaScale+b;

	double a=(0-1)/(PCA_synergy::sigma1_range_dataCollection[1]-PCA_synergy::sigma1_range_dataCollection[0]);//-1/(sigma_max-sigma_min)
	double b=0-a*PCA_synergy::sigma1_range_dataCollection[1];//0-a*sigma_max
	double x_unitScale_loc=a*a_sigmaScale+b;

	if(x_unitScale_loc>1)
	{
		x_unitScale_loc=1;
		//printf("Sigma1 out of range, set to sigma max [PCA_synergy::conver_sigmaScale2unitScale]\n");
	}
	else if(x_unitScale_loc<0)
	{
		x_unitScale_loc=0;
		//printf("Sigma1 out of range, set to sigma min [PCA_synergy::conver_sigmaScale2unitScale]\n");
	}

	//printf("UnitScaled sigma1: %g \n",x_unitScale_loc);
	//double x_unitScale_loc=a_sigmaScale;
	return x_unitScale_loc;
}
double PCA_synergy::convert_unitScale2sigmaScale(const double & a_unitScale)
{
	//To get 1 for closed, 0 for open
	//sigma_min=closed, corresponds to 1, sigma_max=opened, corresponds to 0
	//pos_sigmaScale=a*pos_unitScale + b with a=sigma_min-sigma_max and b=sigma_max

	double pos_sigmaScale=(PCA_synergy::sigma1_range_dataCollection[0] - PCA_synergy::sigma1_range_dataCollection[1]) * a_unitScale + PCA_synergy::sigma1_range_dataCollection[1];
	if(pos_sigmaScale>PCA_synergy::sigma1_range_dataCollection[1])
	{
		pos_sigmaScale=PCA_synergy::sigma1_range_dataCollection[1];
	}
	else if(pos_sigmaScale <PCA_synergy::sigma1_range_dataCollection[0])
	{
		pos_sigmaScale=PCA_synergy::sigma1_range_dataCollection[0];
	}
	return pos_sigmaScale;
}

void PCA_synergy::set_mean_dataCollection(double a_mean_dataCollection[Globals::n_pcs])
{
	for(int it=0;it<Globals::n_pcs;it++)
	{
		PCA_synergy::mean_dataCollection[it]=a_mean_dataCollection[it];
	}
}

void PCA_synergy::compute_forceGains(Exoskeleton * pExoskeleton) //TO CHECK
{

//1. Compute Fmax
	//Zmax (force max in synergy space): zmax=[tau_interaction_max 0 .. 0]'
	CustomMatrix * pZmax_syns=new CustomMatrix(Globals::n_pcs,1); //(nx1)
	(*pZmax_syns)(1,1)=SoftHandSyn::get_tau_interaction_max();
	for(int it=1;it<Globals::n_pcs;it++)
	{
		(*pZmax_syns)(it+1,1)=0;
	}
	//printf("Force_max vector in syns z: [PCA_synergy::compute_forceGains]  \n");
	//pZmax_syns->printf_Matrix();

	//Project z on ib through synergy matrix: F=Sx*z
	CustomMatrix * pFmax=CustomMatrix::MatrixProduct(PCA_synergy::pSynMatrix,pZmax_syns);//nx1
	//printf("Force_max vector (Projection of tau_int_max on syn space before scaling)  \n");
	//pFmax->printf_Matrix();

//2. Compute J(q_closed)^T * Fmax
	
	//double l_Fmax_finger[Globals::mCart_kins_pf]={0};
	CustomMatrix * p_Fmax_finger_l=new CustomMatrix(Globals::mCart_kins_pf,1);
	double l_scalingGains[Globals::numFingers]={0};
	CustomMatrix * interm_l=new CustomMatrix(Globals::mCart_kins_pf,1);
	//For checking only
	double l_normFmax[Globals::numFingers]={0};//not used, to check
	double tau_exo_ref_max_l[Globals::numFingers]={0};

	for(int it_f=0;it_f<Globals::numFingers;it_f++)//for each finger
	{
		//Get 6x1 force vector
		for(int it=0;it<3;it++)
		{
			(*p_Fmax_finger_l)(it+1,1)=(*pFmax)(it+1+3*it_f,1);//3 first terms=force unstacked from pFmax, 3 last terms=0
		}

		//Check
		l_normFmax[it_f]=sqrt(pow((*p_Fmax_finger_l)(1,1),2)+pow((*p_Fmax_finger_l)(2,1),2)+pow((*p_Fmax_finger_l)(3,1),2));//N

		//Compute J(q_closed)^T*Fmax for each finger
		//double * interm_l=CustomMatrix::MatrixProduct(pExoskeleton->aFingers[it_f].pJacobianTip->transpose(),l_Fmax_finger);//6x1
		interm_l=CustomMatrix::MatrixProduct(pExoskeleton->aFingers[it_f].pJacobianTip->transpose(),p_Fmax_finger_l);//6x1

//3. Set k_finger=tau_exo_max/ 1st component of J(q_closed)^T*Fmax
		PCA_synergy::m_forceGains[it_f]=pExoskeleton->getMaxTorque()/(*interm_l)(1,1);//Take abs value, interm_l(1,1) is negative 

		//Check that tau_exo is the same for all fingers (after scaling)
		interm_l=CustomMatrix::MatrixProduct(pExoskeleton->aFingers[it_f].pJacobianTip->transpose(),p_Fmax_finger_l);//6x1
		tau_exo_ref_max_l[it_f]=(*interm_l)(1,1)*PCA_synergy::m_forceGains[it_f];
	}//for it_f

	//Printf
	//printf(" [PCA_synergy::compute_forceGains] \n");
	//printf("\t Fmax norm per finger without scaling (Projection of tau_int_max on syn space before scaling): %g \t %g \t %g \n",l_normFmax[0],l_normFmax[1],l_normFmax[2]);
	//printf("\t Force gains: %g \t %g \t %g \n",PCA_synergy::m_forceGains[0],PCA_synergy::m_forceGains[1],PCA_synergy::m_forceGains[2]);
	//printf("\t Tau exo ref after scaling, max case: %g \t %g \t %g  \n",tau_exo_ref_max_l[0],tau_exo_ref_max_l[1],tau_exo_ref_max_l[2]);
	//printf("\t Fmax norm per finger after scaling (Projection of tau_int_max on syn space + scaling): %g \t %g \t %g \n",l_normFmax[0]*PCA_synergy::m_forceGains[0],l_normFmax[1]*PCA_synergy::m_forceGains[1],l_normFmax[2]*PCA_synergy::m_forceGains[2]);
	//Ok. Note that this scaling method ensures that tau_exo is equal for all fingers (and equal to tau_exo_max) to render tau_int_max. This is meant to use the full functionalities of the exo. 
	//However this means that the force applied at the fingertip is different across fingers (kinematics differences -> larger for thumb)
	

	//printf("m_forceGains: %g \t %g \t %g \n[PCA_synergy.h]",PCA_synergy::m_forceGains[0],PCA_synergy::m_forceGains[1],PCA_synergy::m_forceGains[2]);
	delete p_Fmax_finger_l;
	delete pZmax_syns;
	delete pFmax;
	delete interm_l;
}

void PCA_synergy::compute_F_syns_scaled()
{
	for(int it=0;it<Globals::n_pcs;it++)
	{
		if(it<3)
		{
			PCA_synergy::F_syns_scaled[it]=PCA_synergy::F_syns[it]*PCA_synergy::m_forceGains[0];
		}
		else if(it>=3 && it<6)
		{
			PCA_synergy::F_syns_scaled[it]=PCA_synergy::F_syns[it]*PCA_synergy::m_forceGains[1];
		}
		else if(it>=6 && it<9)
		{
			PCA_synergy::F_syns_scaled[it]=PCA_synergy::F_syns[it]*PCA_synergy::m_forceGains[2];
		}
		else
		{
			printf("ERROR - invalid case [PCA_synergy::compute_F_syns_scaled] \n");
		}
			
	}//for
}

//void PCA_synergy::compute_forceRef_fingertip_ib(SoftHand * pSoftHand,Exoskeleton * pExoskeleton)
//{
//	double generalized_force_ib_loc[Globals::n_pcs]={0}; //generalized force at fingertips - ref for exo in cart space
//	//If one finger active, dim=6, if 3 fingers active, dim=18 
//
//	if(pSoftHand->get_contact_state()==true)
//	{
//		printf("[PCA_synergy::compute_forceRef_fingertips] Contact at softhand ; ensure that sc1_firstContact and stiff_vSpring have been properly setted \n");
//		//PCA_synergy::update_delta_sc1(); //from BoardControl::stopPositionControl() and sigma1
//
//		//Compute 1st component of force in syn space
//		double z1_syns_loc=PCA_synergy::compute_force_1stComp_syns(pSoftHand->get_impedance_object(),pSoftHand->get_deltaPos_current_1stContact());//CHECK SCALING FPC!
//		printf("[PCA_synergy::compute_forceRef_fingertip] Check scaling!! \n ");
//
//		//Map to cart space
//		//PCA_synergy::project_force_syns2ib(z1_syns_loc,generalized_force_ib_loc);
//	}
//	else //no contact
//	{
//		for(int it=0;it<Globals::n_pcs;it++)
//		{
//			generalized_force_ib_loc[it]=0; 
//		}
//	}
//	
//	//This is actually done in Exoskeleton::set_force_tip_des_base
//	//set Exoskeleton class member force
//	//if(Globals::IDF_ACTIVE_FINGERS==1)//only index, then dim(PCA_synergy::generalized_force_)
//	//{
//	//	for(int it=0;it<Globals::n_pcs;it++)
//	//	{
//	//		pExoskeleton->aFingers[1].force_tip_des_base[it]=generalized_force_ib_loc[it];
//	//	}
//	//}
//	//else if(Globals::IDF_ACTIVE_FINGERS==3)//3fingers, unstack corresponding force vectors
//	//{
//	//	for(int it=0;it<Globals::mCart_kins_pf;it++)//thumb
//	//	{
//	//		pExoskeleton->aFingers[0].force_tip_des_base[it]=generalized_force_ib_loc[it];
//	//	}
//	//	for(int it=Globals::mCart_kins_pf;it<2*Globals::mCart_kins_pf;it++)
//	//	{
//	//		pExoskeleton->aFingers[1].force_tip_des_base[it-Globals::mCart_kins_pf]=generalized_force_ib_loc[it];
//	//	}
//	//	for(int it=2*Globals::mCart_kins_pf;it<3*Globals::mCart_kins_pf;it++)
//	//	{
//	//		pExoskeleton->aFingers[2].force_tip_des_base[it-2*Globals::mCart_kins_pf]=generalized_force_ib_loc[it];
//	//	}
//	//}
//	//else
//	//{
//	//	printf("ERROR, invalid number of active fingers. Exit. [PCA_synergy::computeForceRef_fingerTip]\n");
//	//}
//
//	pExoskeleton->set_force_tip_des_base(generalized_force_ib_loc);
//
//}

//void PCA_synergy::compute_forceRef_fingertip_ib(double res_forceRef[Globals::n_pcs])
//{
//	if(PCA_synergy::is_softHand_contact==true)
//	{
//		printf("[PCA_synergy::compute_forceRef_fingertips] Contact at softhand ; ensure that sc1_firstContact and stiff_vSpring have been properly setted \n");
//		PCA_synergy::update_delta_sc1(); //from sc1_firstContact and sigma
//		PCA_synergy::update_z1_syns(); // z1=stiff_vSpring * delta_sc1
//		PCA_synergy::project_z_to_F_ib(); //F=S*z
//	}
//	else //no contact
//	{
//		for(int it=0;it<Globals::n_pcs;it++)
//		{
//			PCA_synergy::F_ib[it]=0; //set class member to 0
//		}
//	}
//	
//	//set arg
//	for(int it=0;it<Globals::n_pcs;it++)
//	{
//		res_forceRef[it]=PCA_synergy::F_ib[it];
//	}
//}

//OLD - to remove
//double PCA_synergy::project_dataSet_pcs(double * dataSet_initialBase,double * res_dataSet_pcs)
//{
//	//Put initial data set in a column vector
//	CustomMatrix * p_Si=new CustomMatrix(Globals::n_pcs,1);//data set in initial base, column vector (n_pcs x 1)
//	for (int iti=0;iti<Globals::n_pcs;iti++)
//	{
//		(*p_Si)(iti+1,1)=dataSet_initialBase[iti];
//	}
//
//	//Project on principal component space: Spc= coeff' x Si with coeff' = (*pEigenVectors) copied from matlab
//	//CustomMatrix * p_Spc=new CustomMatrix(PCA_synergy::n_pcs,1); //data set in pc=principal component base, column vector (n_pcs x 1)	
//	CustomMatrix * p_Spc=CustomMatrix::MatrixProduct(PCA_synergy::pEigenVectors,p_Si);
//	//printf("Result of the projection (should be a n_pcs x 1 column vector): \n");
//	//p_Spc->printf_Matrix();
//
//	//Copy p_Spc in the arg array
//	for (int iti=0;iti<Globals::n_pcs;iti++)
//	{
//		res_dataSet_pcs[iti]=(*p_Spc)(iti+1,1);
//	}
//
//	return res_dataSet_pcs[0];
//}

//OLD - coeff'
//void PCA_synergy::set_eigenVectors_fromFile() 
//{
//		FILE * pFilePCA;
//		if( (pFilePCA=fopen("log/File_synergyMatrix_fromMatlab.txt","r")) == NULL ) // C4996
//			printf( "ERROR while opening log/File_synergyMatrix_fromMatlab.txt \n" );
//		else
//		{
//			double row[Globals::n_pcs];
//
//			//To use fscanf:
//			//double a,b;
//			//fscanf(pFilePCA,"%lf %lf",&a,&b);
//			//printf("%g %g\n",a,b);
//
//			//fscanf takes a pointer on double as arg. 
//			//row is a pointer on 1st element of array, row+1 is a pointer on 2nd element of array, cf http://www.tutorialspoint.com/cprogramming/c_pointer_to_an_array.htm
//			//double row[7] == double * row : row is a pointer on the first element of the array == row contains the address of the first element. Thus row+1 contains the address of the second element of the array. 
//
//			for(int iti=0;iti<Globals::n_pcs;iti++)
//			{
//				fscanf(pFilePCA, "%lg %lg %lg %lg %lg %lg %lg",row,row+1,row+2,row+3,row+4,row+5,row+6); // %lg == %lf == double; %g == %f == floating point number with no length=precision specification, cf http://www.cplusplus.com/reference/cstdio/scanf/
//				for(int itj=0;itj<Globals::n_pcs;itj++)
//				{
//					(*PCA_synergy::pEigenVectors)(iti+1,itj+1)=row[itj];
//				}
//				//fscanf(pFilePCA, "%g %g %g %g %g %g %g",(*pEigenVectors)(iti,1),(*pEigenVectors)(iti,2),(*pEigenVectors)(iti,3),(*pEigenVectors)(iti,4),(*pEigenVectors)(iti,5),(*pEigenVectors)(iti,6),(*pEigenVectors)(iti,7));
//				//iti+=1;
//			}
//			//printf("End of fscanf. EigenVectors matrix: \n");
//			//PCA_synergy::pEigenVectors->printf_Matrix();
//
//			fclose(pFilePCA);
//
//		}//else   
//}

//void PCA_synergy::compute_posRef_syns(Exoskeleton * pExoskeleton,SoftHand * pSoftHand) //return a value between 0 (opened) and 1 (max closure) - sets SoftHand::posRef_unitScale
//{
	//PCA_synergy::extract_dataSet_ib(pExoskeleton); //Extract data set in initial base and set PCA_synergy::dataSet_ib
	//PCA_synergy::project_dataSet_ib2syns(PCA_synergy::dataSet_ib,PCA_synergy::dataSet_syns); //project data from ib to syns and sets dataSet_syns
	//double fpc_unitScaled_loc=PCA_synergy::extract_fpc_scale_unitRange(PCA_synergy::dataSet_syns);
	//pSoftHand->set_posRef_unitScaled(fpc_unitScaled_loc);
//}