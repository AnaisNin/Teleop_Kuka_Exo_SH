#ifndef CUSTOMMATRIX_H
#define CUSTOMMATRIX_H

class CustomMatrix
{
    public:

    CustomMatrix(int numRows,int numCols); //constructor
    ~CustomMatrix(); //destructor

    double& operator()(int n, int m)
    {
        return array_elements[m_numCols*(n-1)+m-1];// to return the element in matrix
    }

    void getSize(int & a_numRows,int & a_numCols);
    void printf_Matrix();
    void setElementsValue(double * a_elements,const int numElements);
    CustomMatrix* transpose();
    double* getDiag(int & p_numElmts); //returns array with diagonal elements of the matrix and sets arg with number of elements
    void getArrayElements(double * a_arrayElements);
    double getElement(int i,int j);
    CustomMatrix* MultiplyByScalar(double a_gain);//Does not modify the first matrix, return arg
    CustomMatrix * getAbsMatrix();
	CustomMatrix * getInverse();

    static CustomMatrix* MatrixProduct(CustomMatrix * p_matrixA,CustomMatrix * p_matrixB);
	static double* MatrixProduct(CustomMatrix * p_matrixA,const double a_vector[3]);
	
    private:

    double * array_elements;
    int m_numRows;
    int m_numCols;

};

#endif // CUSTOMMATRIX_H
