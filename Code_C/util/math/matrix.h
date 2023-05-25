/*
 * matrix.h
 *
 *  Created on: Apr 18, 2022
 *      Author: quangnd
 */

#ifndef UTIL_MATH_MATRIX_H_
#define UTIL_MATH_MATRIX_H_

#include <math.h>
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "stdint.h"


typedef struct Matrix_t Matrix;

struct Matrix_t{
    float* entries;
    uint8_t row;
    uint8_t col;
};

void zeros(Matrix dst_matrix);
void eye(Matrix dst_matrix);
void ones(Matrix dst_matrix);
float get(Matrix src_matrix,uint8_t r,uint8_t c);
void set(Matrix dst_matrix,uint8_t r,uint8_t c, float d);
void sum(Matrix A, Matrix B, Matrix result);
void minus(Matrix A, Matrix B, Matrix result);
void copy_value(Matrix src_matrix, Matrix dst_matrix);
float inner_multiply(Matrix A, Matrix B);
void hadamard_product(Matrix A, Matrix B, Matrix result);
void scalar_multiply(Matrix A, float c, Matrix result);
void multiply(Matrix A, Matrix B, Matrix result);
void transpose(Matrix src_matrix, Matrix dst_matrix);
void truncate_matrix(Matrix src_matrix, Matrix dst_matrix, uint8_t begin_row,
		uint8_t begin_col);
void hconcat(Matrix A, Matrix B, Matrix result);
void vconcat(Matrix A, Matrix B, Matrix result);
void htri_concat(Matrix A, Matrix B, Matrix C, Matrix result);
void generate_matrix(Matrix result, float value);
void hgenerate(Matrix A,uint8_t c, Matrix result);
void vgenerate(Matrix A,uint8_t r, Matrix result);
void diag_matrix_col(Matrix src_matrix, Matrix dst_matrix);
void chol(Matrix A, Matrix result);

/*
void matrix_multiply(uint8_t n, uint8_t m, uint8_t p,
         float matrix1[n][m], float matrix2[m][p], float result[n][p]);
void matrix_scalar_multiply(uint8_t n, uint8_t m, float number,
         float matrix[n][m], float result[n][m]);
void additon_matrix(uint8_t n, uint8_t m, float matrix1[n][m],
         float matrix2[n][m], float result[n][m]);
void truncate_matrix(uint8_t n, uint8_t m, float src_matrix[n][m],
         uint8_t x, uint8_t y, float dst_matrix[x][y], uint8_t begin_row,
         uint8_t begin_col);
void generate_matrix(uint8_t m, uint8_t n, float matrix[n][m],
         float value);
void transpose_matrix(uint8_t n, uint8_t m, float src_matrix[n][m],
         float dst_matrix[m][n]);
void diag_matrix_col(uint8_t m, float src_matrix[1][m],
                float dst_matrix[m][m]);
*/

#endif /* UTIL_MATH_MATRIX_H_ */
