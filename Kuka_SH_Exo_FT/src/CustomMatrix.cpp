#include "CustomMatrix.h"
#include <stdio.h>

CustomMatrix::CustomMatrix(int a_numRows,int a_numCols)
{
    this->m_numRows=a_numRows;
    this->m_numCols=a_numCols;
    this->array_elements=new double [m_numRows*m_numCols]; //allocate memory - must be delete after use

    for (int iti=0;iti<m_numRows;iti++)
    {
        for (int itj=0;itj<m_numCols;itj++)
        {
            (*this)(iti+1,itj+1)=0.0; //first: (1,1) and not (0,0)
        }
    }
}

CustomMatrix::~CustomMatrix()
{
    delete [] (this->array_elements);
    //delete pointer;  releases the memory of a single element allocated using new
    //delete[] pointer; releases the memory allocated for arrays of elements using new (int * foo;foo = new int [5];)

}

void CustomMatrix::getSize(int & p_numRows,int & p_numCols)
{
    p_numRows=this->m_numRows;
    p_numCols=this->m_numCols;
}

//CustomMatrix* CustomMatrix::MultiplyByMatrix(CustomMatrix * p_matrix_rhs)
CustomMatrix* CustomMatrix::MatrixProduct(CustomMatrix * p_matrixA,CustomMatrix * p_matrixB)//ok
{
    int numRowsA;
    int numColsA;
    int numRowsB;
    int numColsB;
    p_matrixA->getSize(numRowsA,numColsA);
    p_matrixB->getSize(numRowsB,numColsB);

    CustomMatrix * p_matrixRes=new CustomMatrix(numRowsA,numColsB);

     double sum=0;

    if(numColsA!=numRowsB)
    {
        printf("ERROR: Unproper size for matrix product - numColsA != numRowsB - [CustomMatrix::MultiplyByMatrix] \n");
        printf("Exit fnt \n");
        return 0;
    }
    else
    {
        int numCommonAB=numColsA;
        //iti: num rows res = num rows A
        //itj: num cols res=num cols B
        //itk: num cols A=num rows B
        for(int iti=1;iti<=numRowsA;iti++)
        {
            for(int itj=1;itj<=numColsB;itj++)
            {
                sum=0;
                for(int itk=1;itk<=numCommonAB;itk++)
                {
                    sum+=(*p_matrixA)(iti,itk)*(*p_matrixB)(itk,itj);
                }
                (*p_matrixRes)(iti,itj)=sum;
            }
        }
        return p_matrixRes;

    }
}

CustomMatrix* CustomMatrix::MultiplyByScalar(double a_gain)//Does not modify the first matrix, return arg
{
    int numRows=this->m_numRows;
    int numCols=this->m_numCols;

    CustomMatrix * p_matrixRes=new CustomMatrix(numRows,numCols);

    for(int iti=1;iti<=numRows;iti++)
        {
            for(int itj=1;itj<=numCols;itj++)
            {
                (*p_matrixRes)(iti,itj)=a_gain*(*this)(iti,itj);
            }
        }
        return p_matrixRes;
}

CustomMatrix * CustomMatrix::getAbsMatrix() //return matrix with abs value of elements
{
    int numRows=this->m_numRows;
    int numCols=this->m_numCols;

    CustomMatrix * p_matrixRes=new CustomMatrix(numRows,numCols);

    for(int iti=1;iti<=numRows;iti++)
        {
            for(int itj=1;itj<=numCols;itj++)
            {
                (*p_matrixRes)(iti,itj)=((*this)(iti,itj)<0?(-1)*(*this)(iti,itj):(*this)(iti,itj));
            }
        }
        return p_matrixRes;
}

double CustomMatrix::getElement(int i,int j)
{
    return (*this)(i,j);
}

void CustomMatrix::setElementsValue(double * a_elements,const int a_numElements) //ok- a_elements should contain 1st row then 2nd row etc
{
    if(this->m_numCols*this->m_numRows!=a_numElements)
    {
        printf("ERROR: unvalid number of elements - in [CustomMatrix::setElementsValue] \n");
        printf(".. exit. \n");
        return; //exit from void function
    }
    else
    {
        int ita=0;
        for (int iti=1;iti<=this->m_numRows;iti++)
        {
            for (int itj=1;itj<=this->m_numCols;itj++)
            {
                (*this)(iti,itj)=a_elements[ita];
                ita+=1;
            }
        }
    }
}

void CustomMatrix::printf_Matrix()
{
    for (int iti=1;iti<=this->m_numRows;iti++)
    {
        for (int itj=1;itj<=this->m_numCols;itj++)
        {
            printf("%g \t",(*this)(iti,itj));
        }
        printf("\n");
    }
}

CustomMatrix* CustomMatrix::transpose() //ok
{
    int numRows_res=this->m_numCols;
    int numCols_res=this->m_numRows;
    CustomMatrix * resMatrix=new CustomMatrix(numRows_res,numCols_res);
    for (int iti=1;iti<=numRows_res;iti++)
    {
        for (int itj=1;itj<=numCols_res;itj++)
        {
            (*resMatrix)(iti,itj)=(*this)(itj,iti);
        }
    }
    return resMatrix;
}

double* CustomMatrix::getDiag(int & p_numElmts) //returns diag and
{
    p_numElmts=this->m_numCols;
    double * a_diag=new double[this->m_numCols];

    if(this->m_numCols!=this->m_numRows)
    {
        printf("ERROR - non square matrix - exit [CustomMatrix::getDiag]");
        return 0;
    }
    else
    {
        for(int it=0;it<this->m_numRows;it++)
        {
            a_diag[it]=(*this)(it+1,it+1);
        }
        return a_diag;
    }
}

//To check
void CustomMatrix::getArrayElements(double * a_arrayElements)
{
    int numElements=this->m_numCols*this->m_numRows;
    for(int it=0;it<numElements;it++)
    {
        a_arrayElements[it]=this->array_elements[it];
    }
}

//TO CHECK
double* CustomMatrix::MatrixProduct(CustomMatrix * p_matrixA,const double a_vectorB[3])
{
	double res[3]={0};

	int l_numRows,l_numCols;
	p_matrixA->getSize(l_numRows,l_numCols);
	if(l_numRows!=3 | l_numCols!=3)
	{
		printf("ERROR - matrix should be 3x3 - Exit. [CustomMatrix::MatrixProduct] \n");
		return res;
	}
	else
	{
		for(int iti=0;iti<3;iti++)
		{
			res[iti]=0;
				for(int itj=0;itj<3;itj++)
				{
					res[iti]+=(*p_matrixA)(iti+1,itj+1)*a_vectorB[itj];
				}
		}
		return res;
	}

}

//To check
CustomMatrix * CustomMatrix::getInverse()
{
	CustomMatrix * p_result=new CustomMatrix(3,3);
	double det;

	det= (*this)(1,1)*((*this)(2,2)*(*this)(3,3)-(*this)(3,2)*(*this)(2,3))-(*this)(1,2)*((*this)(2,1)*(*this)(3,3)-(*this)(3,1)*(*this)(2,3))+(*this)(1,3)*((*this)(2,1)*(*this)(3,2)-(*this)(3,1)*(*this)(2,2));

	(*p_result)(1,1) =  1.0/det*((*this)(2,2)*(*this)(3,3)-(*this)(3,2)*(*this)(2,3));
	(*p_result)(2,1) = -1.0/det*((*this)(2,1)*(*this)(3,3)-(*this)(3,1)*(*this)(2,3));
	(*p_result)(3,1) =  1.0/det*((*this)(2,1)*(*this)(3,2)-(*this)(3,1)*(*this)(2,2));

	(*p_result)(1,2) = -1.0/det*((*this)(1,2)*(*this)(3,3)-(*this)(3,2)*(*this)(1,3));
	(*p_result)(2,2) =  1.0/det*((*this)(1,1)*(*this)(3,3)-(*this)(3,1)*(*this)(1,3));
	(*p_result)(3,2) = -1.0/det*((*this)(1,1)*(*this)(3,2)-(*this)(3,1)*(*this)(1,2));

	(*p_result)(1,3) =  1.0/det*((*this)(1,2)*(*this)(2,3)-(*this)(2,2)*(*this)(1,3));
	(*p_result)(2,3) = -1.0/det*((*this)(1,1)*(*this)(2,3)-(*this)(2,1)*(*this)(1,3));
	(*p_result)(3,3) =  1.0/det*((*this)(1,1)*(*this)(2,2)-(*this)(2,1)*(*this)(1,2));
	 
	return p_result;
}