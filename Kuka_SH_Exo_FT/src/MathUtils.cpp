#include "MathUtils.h"
#include <algorithm> //std::max
#include <float.h> //copysign

double MathUtils::deg2rad(double x_deg)
{
	return x_deg*M_PI/180;
}

double MathUtils::rad2deg(double x_rad)
{
	return x_rad*180/M_PI;
}

void MathUtils::RotToRPY(CustomMatrix * pRot, double RPY[3]) //fills the array roll pitch yaw with the values extracted from the corresponding rotation matrix
{
    double roll;
    double pitch;
    double yaw;

    double r11,r21,r32,r33,r31;
	r11=pRot->getElement(1,1);
    r21=pRot->getElement(2,1);
    r31=pRot->getElement(3,1);
    r32=pRot->getElement(3,2);
    r33=pRot->getElement(3,3);

    pitch=atan2(-r31,sqrt(pow(r11,2)+pow(r21,2)));
    //printf("pitch before check: %.3f deg, %.3f rad \n",rad2deg(pitch), pitch);

    //Check for indetermination (pitch=+-90deg 0> cos(pitch)=0)
    if(pitch>deg2rad(89.5) && pitch<deg2rad(90) || pitch==deg2rad(90))    //Rq: -pi<atan2(x)<pi
    {
        pitch=deg2rad(89.5);
        //printf("Indetermination - pitch has been reset to 89.5deg \n");
    }
    else if(deg2rad(90)<pitch && pitch<deg2rad(90.5))
    {
        pitch=deg2rad(90.5);
        //printf("Indetermination - pitch has been reset to 90.5deg \n");
    }
    else if(deg2rad(-90)<pitch && pitch<deg2rad(-89.5) || pitch==deg2rad(-90))
    {
        pitch=deg2rad(-89.5);
        //printf("Indetermination - pitch has been reset to -89.5deg \n");
    }
    else if(deg2rad(-90.5)<pitch && pitch<deg2rad(-90))
    {
        pitch=deg2rad(-90.5);
        //printf("Indetermination - pitch has been reset to -90.5deg \n");
    }

    yaw=atan2(r21/cos(pitch),r11/cos(pitch));
    roll=atan2(r32/cos(pitch),r33/cos(pitch));

    RPY[0]=roll;
    RPY[1]=pitch;
    RPY[2]=yaw;

    //printf("[Optitrack::RotToRPY] new roll pitch yaw: %.3f \t %.3f \t %.3f \n",rollPitchYaw[0],rollPitchYaw[1],rollPitchYaw[2]);
}

void MathUtils::RPYtoRot(double RPY[3],CustomMatrix* res_pRot) //return rotation matrix corresponding to the roll-pitch-yaw angles provided
//Expressed in Coman torso ref frame since roll corresponds to rot about x_torso_coman, pitch corresponds to rot about y_torso_coman and yaw corresponds to rot about z_torso_coman
//Returns rotation matrix from torso ref frame to actual frame; so when roll=pitch=yaw=0, as actual frame corresponds to torso ref frame, it returns identity matrix.
{
    double roll=RPY[0];
    double pitch=RPY[1];
    double yaw=RPY[2];

	CustomMatrix * pRot_roll=new CustomMatrix (3,3); //rotation about x
    (*pRot_roll)(1,1)=1;
    (*pRot_roll)(2,1)=0;
    (*pRot_roll)(3,1)=0;
    (*pRot_roll)(1,2)=0;
    (*pRot_roll)(2,2)=cos(roll);
    (*pRot_roll)(3,2)=sin(roll);
    (*pRot_roll)(1,3)=0;
    (*pRot_roll)(2,3)=-sin(roll);
    (*pRot_roll)(3,3)=cos(roll);

    CustomMatrix * pRot_pitch=new CustomMatrix (3,3); //rotation about y
    (*pRot_pitch)(1,1)=cos(pitch);
    (*pRot_pitch)(2,1)=0;
    (*pRot_pitch)(3,1)=-sin(pitch);
    (*pRot_pitch)(1,2)=0;
    (*pRot_pitch)(2,2)=1;
    (*pRot_pitch)(3,2)=0;
    (*pRot_pitch)(1,3)=sin(pitch);
    (*pRot_pitch)(2,3)=0;
    (*pRot_pitch)(3,3)=cos(pitch);

	CustomMatrix * pRot_yaw=new CustomMatrix (3,3); //rotation about z
    (*pRot_yaw)(1,1)=cos(yaw);
    (*pRot_yaw)(2,1)=sin(yaw);
    (*pRot_yaw)(3,1)=0;
    (*pRot_yaw)(1,2)=-sin(yaw);
    (*pRot_yaw)(2,2)=cos(yaw);
    (*pRot_yaw)(3,2)=0;
    (*pRot_yaw)(1,3)=0;
    (*pRot_yaw)(2,3)=0;
    (*pRot_yaw)(3,3)=1;

	CustomMatrix* pInterm=CustomMatrix::MatrixProduct(pRot_pitch,pRot_roll);
	res_pRot=CustomMatrix::MatrixProduct(pRot_yaw,pInterm);//rot=rot_yaw*rot_pitch*rot_roll
}

//From www.thetenthplanet.de/archives/1994 -- evolution of Shoemake technique
//Construct a unit quaternion from rotation matrix
void MathUtils::RotToQuat(CustomMatrix * pRot, double res_quat[4]) //Checked ok
{
    double r11,r22,r33,r21,r12,r31,r13,r32,r23;
	r11=pRot->getElement(1,1);
	r22=pRot->getElement(2,2);
	r33=pRot->getElement(3,3);
    r21=pRot->getElement(2,1);
	r12=pRot->getElement(1,2);
    r31=pRot->getElement(3,1);
	r13=pRot->getElement(1,3);
    r32=pRot->getElement(3,2);
    r23=pRot->getElement(2,3);

	double qw,qx,qy,qz; //quat=qw+i.qx+j.qy+k.qz with i,j and k the Hamilton extended complex numbers

	qw=.5*sqrt(std::max(0.0,1+r11+r22+r33)); //0.0 since std::max needs to args of same type. Otherwise: std::max<double>(i,j)
	qx=.5*sqrt(std::max(0.0,1+r11-r22-r33));
	qy=.5*sqrt(std::max(0.0,1-r11+r22-r33));
	qz=.5*sqrt(std::max(0.0,1-r11-r22+r33));

	//sign
	qx=_copysign(qx,r32-r23);
	qy=_copysign(qy,r13-r31);
	qz=_copysign(qz,r21-r12);	

	//double _copysign(double x,double y): returns x with same sign as y
	
	res_quat[0]=qw;
	res_quat[1]=qx;
	res_quat[2]=qy;
    res_quat[3]=qz;

	//printf("[MathUtils::RotToQuat] %g %g %g %g \n",res_quat[0],res_quat[1],res_quat[2],res_quat[3]);
}

void MathUtils::getQuatNorm(double quat[4],double & res_norm)
{
	res_norm=quat[0]*quat[0]+quat[1]*quat[1]+quat[2]*quat[2]+quat[3]*quat[3];
}

//From Shoemake, 'Quaternions'
//void MathUtils::QuatToRot(double a_quat[4],CustomMatrix * res_pRot)
CustomMatrix* MathUtils::QuatToRot(double a_quat[4]) //Checked ok  
{
	double qw,qx,qy,qz;
	qw=a_quat[0];
	qx=a_quat[1];
	qy=a_quat[2];
	qz=a_quat[3];

	double Nq;//norm quat
	MathUtils::getQuatNorm(a_quat,Nq);

	///////////////////////////
	//Coman

	double s=(Nq>0.0?(2.0/Nq):0.0);
	double xs=qx*s;
	double ys=qy*s;
	double zs=qz*s;

	double wx=qw*xs;
	double wy=qw*ys;
	double wz=qw*zs;

	double xx=qx*xs;
	double xy=qx*ys;
	double xz=qx*zs;

	double yy=qy*ys;
	double yz=qy*zs;
	double zz=qz*zs;

	CustomMatrix * res_pRot=new CustomMatrix(3,3);
	(*res_pRot)(1,1)=1.0-(yy+zz);
	(*res_pRot)(2,1)=xy+wz;
	(*res_pRot)(3,1)=xz-wy;
	(*res_pRot)(1,2)=xy-wz;
	(*res_pRot)(2,2)=1.0-(xx+zz);
	(*res_pRot)(3,2)=yz+wx;
	(*res_pRot)(1,3)=xz+wy;
	(*res_pRot)(2,3)=yz-wx;
	(*res_pRot)(3,3)=1.0-(xx+yy);

	///////////////////////////////////////////////
	/*
	(*res_pRot)(1,1)=1.0-2*pow(qy,2)-2*pow(qz,2);
	(*res_pRot)(2,1)=2*qx*qy+2*qz*qw;
	(*res_pRot)(3,1)=2*qx*qz-2*qy*qw;
	(*res_pRot)(1,2)=2*qx*qy-2*qz*qw;
	(*res_pRot)(2,2)=1-2*pow(qx,2)-2*pow(qz,2);
	(*res_pRot)(3,2)=2*qy*qz+2*qx*qw;
	(*res_pRot)(1,3)=2*qx*qz+2*qy*qw;
	(*res_pRot)(2,3)=2*qy*qz-2*qx*qw;
	(*res_pRot)(3,3)=1-2*pow(qx,2)-2*pow(qy,2);*/

	
	//(*res_pRot)(1,1)=1.0-2*pow(qy,2)-2*pow(qz,2);
	//(*res_pRot)(2,1)=2*qx*qy-2*qz*qw;
	//(*res_pRot)(3,1)=2*qx*qz+2*qy*qw;

	//(*res_pRot)(1,2)=2*qx*qy+2*qz*qw;
	//(*res_pRot)(2,2)=1-2*pow(qx,2)-2*pow(qz,2);
	//(*res_pRot)(3,2)=2*qy*qz-2*qx*qw;

	//(*res_pRot)(1,3)=2*qx*qz-2*qy*qw;
	//(*res_pRot)(2,3)=2*qy*qz+2*qx*qw;
	//(*res_pRot)(3,3)=1-2*pow(qx,2)-2*pow(qy,2);


	///////////////////////////////////////////////////

	return res_pRot;

}

//product of quaternions
void MathUtils::Quat_Multiplication(double quat_a[4], double quat_b[4], double quat_res[4]) //to check
{
	double ql_w=quat_a[0];
	double ql_x=quat_a[1];
	double ql_y=quat_a[2];
	double ql_z=quat_a[3];

	double qr_w=quat_b[0];
	double qr_x=quat_b[1];
	double qr_y=quat_b[2];
	double qr_z=quat_b[3];

	quat_res[0]=ql_w*qr_w - ql_x-qr_x - ql_y*qr_y - ql_z*qr_z; // qw
	quat_res[1]=ql_w*qr_x + ql_x*qr_w + ql_y*qr_z - ql_z*qr_y; //qx
	quat_res[2]=ql_w*qr_y + ql_y*qr_w + ql_z*qr_x - ql_x*qr_z; //qy
	quat_res[3]=ql_w*qr_z + ql_z*qr_w + ql_x*qr_y - ql_y*qr_x; //qz
}

double MathUtils::Quat_euclidianDistance(double quat_a[4], double quat_b[4])
{
	double d; //euclidian distance between quat_a and quat_b
	d=sqrt( pow(quat_a[0]-quat_b[0],2) + pow(quat_a[1]-quat_b[1],2) + pow(quat_a[2]-quat_b[2],2) + pow(quat_a[3]-quat_b[3],2));
	return d;
}

void MathUtils::RotToQuat_closestNeighbour(CustomMatrix * pRot, double neighbour_quat[4], double res_quat[4])
{
	double interm_quat[4];

	//Compute rot to quat
	MathUtils::RotToQuat(pRot,interm_quat);
	//The rotation corresponds to either interm_quat or -interm_quat. Take the one that is closer to the neighbour.

	//Compute distance {interm,neighbour} 
	double d_neighbour_interm=MathUtils::Quat_euclidianDistance(neighbour_quat,interm_quat);

	//Compute distance {-interm,neighbour}
	double minusInterm_quat[4];
	for (int it=0;it<4;it++)
	{
		minusInterm_quat[it]=-interm_quat[it];
	}

	double d_neighbour_minusInterm=MathUtils::Quat_euclidianDistance(neighbour_quat,minusInterm_quat);

	//Choose between interm_quat and minusInterm_quat

	//This computes properly res_quat locally but does not modify the arg!
	//res_quat=(d_neighbour_interm<=d_neighbour_minusInterm?interm_quat:minusInterm_quat);
	//printf("[MathUtils::RotToQuat_closestNeighbour] %g %g %g %g \n",res_quat[0],res_quat[1],res_quat[2],res_quat[3]);

	if(d_neighbour_interm<=d_neighbour_minusInterm)
	{
		for (int it=0;it<4;it++)
		{
			res_quat[it]=interm_quat[it];
		}
	}
	else
	{
		for (int it=0;it<4;it++)
		{
			res_quat[it]=minusInterm_quat[it];
		}
	}
	//printf("[MathUtils::RotToQuat_closestNeighbour] %g %g %g %g \n",res_quat[0],res_quat[1],res_quat[2],res_quat[3]);

}

int MathUtils::convert_double2int(const double x) //ok
{
	//size(double)=8bytes whereas size(int)=4bytes so conversion double to int introduces some error
	//When doing
	//double a=70000;
	//int b=(int)(a);
	//printf("%g \n",b); we get 69999
	//To avoid this behavior we need to perform some correction
	//"Converting to an int truncates the double, i.e. drops any fraction bit. By adding 0.5 to positive numbers and -0.5 to negative numbers, we get the more conventional behavior. "
	//Related to the fact that cast to int rounds towards 0.

	double dx=x<0.0?-0.5:0.5;
	int res=static_cast<int>(x+dx);
	return res;

}

int MathUtils::compute_fittingCurve_movingAverage(double * raw_points_window,const int & a_windowSize,double & res_smoothValue_centerWindow) //returns -1 if error, 1 else
{
	//if non even, exit
	if((a_windowSize%2)==0) //If a_windowSize is even, that is, the remainder from the division of a_windowSize by 2 is zero 
	{
		printf("ERROR - the window used for computing the fitting curve must be uneven. Exit. [MathUtils::compute_fittingCurve_movingAverage]\n");
		return -1;
	}
	else
	{
		//Compute the average of the points in the window, which constitutes the value of the smoothed fnt at the center of the window
		double sum_loc=0;
		for(int it=0;it<a_windowSize;it++)
		{
			sum_loc+=raw_points_window[it];
		}
		res_smoothValue_centerWindow=sum_loc/a_windowSize;//this is the average value of the points in the considered window
		return 1;
	}
}

void MathUtils::compute_backward_finite_difference(const double data_points[2],const double & delta_t,double & res_valueDerivative)
{
	double y_prev=data_points[0]; //y(t-1)
	double y=data_points[1];//y(t)

	double y_dot;//y_dot(t)
	y_dot=(y-y_prev)/delta_t;
	res_valueDerivative=y_dot;
}

void MathUtils::compute_safePos(const double A[3],const double B[3],const double & a_maxDist,double res_C[3])
{
//res_C: Coordinates of point C, which is on the line from a_A to a_B at a distance of a_maxDist from a_A

		double xa=A[0];
		double ya=A[1];
		double za=A[2];
		double xb=B[0];
		double yb=B[1];
		double zb=B[2];

	double normAB=sqrt( pow(xb-xa,2) + pow(yb-ya,2) + pow(zb-za,2) );
	if(normAB<=a_maxDist)//If the distance from A to B is lower than a_maxDist, then result = B
	{
		for(int it=0;it<3;it++)
		{
			res_C[it]=B[it];
		}	
	}
	else
	{
		double alpha=a_maxDist/normAB;

		double xc=alpha*(xb-xa)+xa;
		double yc=alpha*(yb-ya)+ya;
		double zc=alpha*(zb-za)+za;

		res_C[0]=xc;
		res_C[1]=yc;
		res_C[2]=zc;

		//Check: norm of AC must be a_maxDist
		double normAC=sqrt( pow(xc-xa,2) + pow(yc-ya,2) + pow(zc-za,2) );
		//printf("Distance too large for Kuka, redesign \n");
		//printf("Norm AC: %g \n",normAC);
	
	}//else
}