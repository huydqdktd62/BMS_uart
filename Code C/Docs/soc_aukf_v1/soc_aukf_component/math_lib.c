/*
 * mathlib.c
 *
 *  Created on: Jul 1, 2022
 *      Author: ADMIN
 */

#if 0
#include "math_lib.h"

Matrix eye(Matrix result){
	result=zeros(result);
    for(unsigned int i=1;i<=result.row;i++)
        result.entries[(i-1)*result.col+i-1]=1;
    return result;
}

Matrix zeros(Matrix result){
	for(unsigned int k=1;k<=result.row*result.col;k++)
		result.entries[k-1]=0;
    return result;
}

Matrix ones(Matrix result){
	for(unsigned int k=1;k<=result.row*result.col;k++)
		result.entries[k-1]=1.0f;
    return result;
}

float get(Matrix M,unsigned int r,unsigned  int c){
    float d=M.entries[(r-1)*M.col+c-1];
    return d;
}

void set(Matrix M,unsigned int r,unsigned int c, float d){
    M.entries[(r-1)*M.col+c-1]=d;
}

Matrix sum(Matrix A, Matrix B, Matrix result){
    for(unsigned int i=1;i<=A.row*A.col;i++)
    	result.entries[i-1]=A.entries[i-1]+B.entries[i-1];
    return result;
}

Matrix minus(Matrix A, Matrix B, Matrix result){
    for(unsigned int i=1;i<=A.row*A.col;i++)
    	result.entries[i-1]=A.entries[i-1]-B.entries[i-1];
    return result;
}

float inner_multiply(Matrix A, Matrix B){
    float d=0;
    unsigned int n=A.row;
    if(A.col>n)
        n=A.col;
    for(unsigned int i=1;i<=n;i++)
        d+=A.entries[i-1]*B.entries[i-1];
    return d;
}

Matrix scalar_multiply(Matrix A, float c, Matrix result){
    for(unsigned int i=1;i<=A.row*A.col;i++)
        result.entries[i-1]=A.entries[i-1]*c;
    return result;
}

Matrix multiply(Matrix A, Matrix B, Matrix result){
    if(A.row==1&&A.col==1){
    	result=scalar_multiply(B,A.entries[0],result);
        return result;
    }else if(B.row==1&&B.col==1){
    	result=scalar_multiply(A,B.entries[0],result);
        return result;
    }
    Matrix R;
    R.row=result.row;
    R.col=result.col;
    R.entries=malloc(sizeof(float)*result.row*result.col);
    for(unsigned int i=1;i<=A.row;i++)
        for(unsigned int j=1;j<=B.col;j++){
            float de=0;
            for(unsigned int k=1;k<=A.col;k++)
                de+=A.entries[(i-1)*A.col+k-1]*B.entries[(k-1)*B.col+j-1];
            R.entries[(i-1)*R.col+j-1]=de;
        }
//    for(int i=1;i<=result.row*result.col;i++)
//    	result.entries[i-1]=R.entries[i-1];
    result=copy_value(R, result);
    free(R.entries);
    return result;
}

Matrix copy_value(Matrix A, Matrix result){
    for(unsigned int i=1;i<=A.row*A.col;i++)
    	result.entries[i-1]=A.entries[i-1];
    return result;
}

Matrix transpose(Matrix A, Matrix result){
	Matrix R;
	R.row=result.row;
	R.col=result.col;
	R.entries=(float*)malloc(sizeof(float)*A.row*A.col);
    int k=0;
    for(unsigned int i=1;i<=result.row;i++)
        for(unsigned int j=1;j<=result.col;j++)
            R.entries[k++]=A.entries[(j-1)*A.row+i-1];
    result=copy_value(R, result);
    free(R.entries);
    return result;
}

Matrix hconcat(Matrix A, Matrix B, Matrix result){
	int k=0;
	for(unsigned int i=1;i<=A.row;i++){
		for(unsigned int j=1;j<=A.col;j++)
			result.entries[k++]=A.entries[(i-1)*A.col+j-1];
		for(unsigned int j=1;j<=B.col;j++)
			result.entries[k++]=B.entries[(i-1)*B.col+j-1];
	}
	return result;
}

Matrix htri_concat(Matrix A, Matrix B, Matrix C, Matrix result){
	int k=0;
	for(unsigned int i=1;i<=A.row;i++){
		for(unsigned int j=1;j<=A.col;j++)
			result.entries[k++]=A.entries[(i-1)*A.col+j-1];
		for(unsigned int j=1;j<=B.col;j++)
			result.entries[k++]=B.entries[(i-1)*B.col+j-1];
		for(unsigned int j=1;j<=C.col;j++)
			result.entries[k++]=C.entries[(i-1)*C.col+j-1];
	}
	return result;
}

Matrix vconcat(Matrix A, Matrix B, Matrix result){
	unsigned int k=0;
	for(unsigned int i=1;i<=A.row;i++)
		for(unsigned int j=1;j<=A.col;j++)
			result.entries[k++]=A.entries[(i-1)*A.col+j-1];
	for(unsigned int i=1;i<=B.row;i++)
		for(unsigned int j=1;j<=B.col;j++)
			result.entries[k++]=B.entries[(i-1)*B.col+j-1];
	return result;
}

Matrix hgenerate(Matrix A,unsigned int c, Matrix result){
	unsigned int k=0;
	for(unsigned int i=1;i<=A.row;i++)
		for(unsigned int j=1;j<=c;j++)
			result.entries[k++]=A.entries[i-1];
	return result;
}

Matrix vgenerate(Matrix A,unsigned int r, Matrix result){
	unsigned int k=0;
	for(unsigned int i=1;i<=r;i++)
		for(unsigned int j=1;j<=A.col;j++)
			result.entries[k++]=A.entries[j-1];
	return result;
}

Matrix chol(Matrix A, Matrix result){
	Matrix mediate;
	mediate.row=result.row;
	mediate.col=result.col;
	mediate.entries=(float*)malloc(sizeof(float)*mediate.row*mediate.col);
	while(mediate.entries==NULL){};
	mediate=copy_value(A, mediate);

	unsigned int i,j;
    for(i=1;i<=mediate.row;i++){
        for(j=1;j<i;j++){
            float de=A.entries[(i-1)*mediate.col+j-1];
            for(unsigned int k=1;k<j;k++)
                de-=mediate.entries[(i-1)*mediate.col+k-1]*mediate.entries[(j-1)*mediate.col+k-1];
            mediate.entries[(i-1)*mediate.col+j-1]=de/mediate.entries[(j-1)*mediate.col+j-1];
        }
        float de=A.entries[(i-1)*mediate.col+i-1];
        for(j=1;j<i;j++)
            de-=mediate.entries[(i-1)*mediate.col+j-1]*mediate.entries[(i-1)*mediate.col+j-1];
        mediate.entries[(i-1)*mediate.col+i-1]=square_root(absolute(de));
    }
    for(i=2;i<=mediate.row;i++)
        for(j=1;j<i;j++)
            mediate.entries[(j-1)*mediate.col+i-1]=0;
    result=copy_value(mediate,result);
    free(mediate.entries);
    return result;
}

Matrix hadamard_product(Matrix A, Matrix B, Matrix result){
	for(unsigned int i=1;i<=result.row*result.col;i++)
		result.entries[i-1]=A.entries[i-1]*B.entries[i-1];
	return result;
}

int binary_search(float data, const float* array, int max_size){
	int lower_bound = max_size-1;
	int upper_bound = 0;
	int mid_point = -1;
	int index = -1;
	while(upper_bound <= lower_bound){
		mid_point = (lower_bound+upper_bound)/2;
		if(data==array[mid_point]){
			index = mid_point;
			break;
		}else{
			if(data<array[mid_point]){
				upper_bound = mid_point +1;
			}else{
				lower_bound = mid_point -1;
			}
		}
	}
	if(upper_bound>lower_bound){
		index = lower_bound;
	}
	return index;
}


float absolute(float x){
	return (x>=0)? x: -x;
}

float square_root(float x){
    float squareRoot = 1;
    while(absolute((squareRoot*squareRoot)/x - 1) >= 1e-6){
        squareRoot = ((x/squareRoot) + squareRoot)/2;
    }
    return squareRoot;
}

float exponent(float x){
	float sum = 1;
	int i = 1;
	float temp = x;
	while ( absolute(temp/x) > 1e-6){
		sum += temp;
		i++;
		temp = temp*x/(float)i;
	}
	return sum;
}

int sign(float x){
	return (x==0)? 0: ((x>0)? 1: -1);
}

float get_min(float value1, float value2){
	float min;
	min = (value1<value2)? value1 : value2;
	return min;
}

float get_max(float value1, float value2){
	float max;
	max = (value1>value2)? value1 : value2;
	return max;
}

void cholesky(int n, float matrix[n][n], float chol[n][n]){
	int i,j,k;
	chol[n][n]=matrix[n][n];
	chol[0][0]=square_root(matrix[0][0]);
	for(i=1;i<n;i++){
		for(j=0;j<i;j++){
			for(k=0;k<j;k++){
				chol[i][j]-=chol[i][k]*chol[j][k];
			}
			chol[i][j]=chol[i][j]/chol[j][j];
		}
		for(j=0;j<i;j++){
			chol[i][i]-=chol[i][j]*chol[i][j];
		}
		chol[i][i]=square_root(chol[i][i]);
	}
	for(i=1;i<n;i++){
		for(j=0;j<i;j++){
			chol[j][i]=0;
		}
	}
}

void multiply_matrix(uint8_t m, uint8_t p, uint8_t n, float matrix1[m][p], float matrix2[p][n], float result[m][n]){
	uint8_t i, j, k;
	float *L=(float*)calloc(m*n,sizeof(float));
	while(NULL==L){};
	for(i = 0; i < m; i++)
		for(j = 0; j < n; j++)
			for(k = 0; k < p; k++)
				L[i*m+j] += matrix1[i][k] * matrix2[k][j];
	memcpy(result,L,m*n);
}

void add_matrix(float sign, uint8_t m, uint8_t n, float matrix1[m][n], float matrix2[m][n], float result[m][n]){
	uint8_t i, j;
	for(i = 0; i < m; i++)
		for(j = 0; j < n; j++)
			result[i][j] = matrix1[i][j] + (sign) * matrix2[i][j];
}

void generate_matrix(uint8_t m, uint8_t n, float vector[m][1], float generated_matrix[m][n]){
	uint8_t i, j;
	for(i = 0; i < m; i++)
		for(j = 0; j < n; j++)
			generated_matrix[i][j] = vector[i][1];
}

void transpose_matrix(uint8_t m, uint8_t n, float origin_matrix[m][n], float transposed_matrix[n][m]){
	uint8_t i, j;
	float L[n*m];
	for(i = 0; i < m; i++)
		for(j = 0; j < n; j++)
			L[j*n+i] = origin_matrix[i][j];
	for(i = 0; i < m; i++)
		for(j = 0; j < n; j++)
			transposed_matrix[j][i]=L[j*n+i];
}

void scalar_multiply_matrix(int operator, uint8_t m, uint8_t n, float number, float matrix[m][n], float result[m][n]){
	uint8_t i,j;
	number = (operator)? number: (1.0f/number);
	for(i = 0; i < m; i++)
		for(j = 0; j < n; j++)
		result[i][j] = number * matrix[i][j];
}
#endif
