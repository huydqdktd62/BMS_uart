/*
 * mathlibs.h
 *
 *  Created on: Jul 1, 2022
 *      Author: ADMIN
 */

#if 0
#ifndef SERVICE_MATH_LIB_H_
#define SERVICE_MATH_LIB_H_

#include "stdint.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#include "math.h"

#define NEGATIVE							(-1.0f)
#define POSITIVE							(1.0f)
#define MULTIPLY							1
#define DIVIDE								0

typedef struct Matrix_t Matrix;

struct Matrix_t{
    float* entries;
    unsigned int row;
    unsigned int col;
};

Matrix eye(Matrix result);
Matrix zeros(Matrix result);
Matrix ones(Matrix result);
float get(Matrix M,unsigned int r,unsigned int c);
void set(Matrix M,unsigned int r,unsigned int c, float d);
Matrix sum(Matrix A, Matrix B, Matrix result);
Matrix minus(Matrix A, Matrix B, Matrix result);
float inner_multiply(Matrix A, Matrix B);
Matrix scalar_multiply(Matrix A, float c, Matrix result);
Matrix multiply(Matrix A, Matrix B, Matrix result);
Matrix copy_value(Matrix A, Matrix result);
Matrix transpose(Matrix A, Matrix result);
Matrix hconcat(Matrix A, Matrix B, Matrix result);
Matrix vconcat(Matrix A, Matrix B, Matrix result);
Matrix htri_concat(Matrix A, Matrix B, Matrix C, Matrix result);

Matrix hgenerate(Matrix A,unsigned int c, Matrix result);
Matrix vgenerate(Matrix A,unsigned int r, Matrix result);
Matrix chol(Matrix A, Matrix result);
Matrix hadamard_product(Matrix A, Matrix B, Matrix result);

int binary_search(float data, const float* array,int max_size);
float absolute(float x);
float square_root(float x);
float exponent(float x);
int sign(float x);
float get_min(float value1, float value2);
float get_max(float value1, float value2);
void cholesky(int n, float matrix[n][n], float chol[n][n]);
void multiply_matrix(uint8_t m, uint8_t p, uint8_t n, float matrix1[m][p], float matrix2[p][n], float result[m][n]);
void add_matrix(float sign, uint8_t m, uint8_t n, float matrix1[m][n], float matrix[m][n], float result[m][n]);
void generate_matrix(uint8_t m, uint8_t n, float vector[m][1], float generated_matrix[m][n]);
void transpose_matrix(uint8_t m, uint8_t n, float origin_matrix[m][n], float transposed_matrix[n][m]);
void scalar_multiply_matrix(int operator, uint8_t m, uint8_t n, float number, float matrix[m][n], float result[m][n]);

#endif /* SERVICE_MATH_LIB_H_ */
#endif
