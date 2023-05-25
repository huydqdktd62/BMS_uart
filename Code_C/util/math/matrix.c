/*
// * matrix.c
// *
// *  Created on: Apr 18, 2022
// *      Author: quangnd
// */
//
#include "matrix.h"
#include "math_util.h"
/*
 * Function generate zeros diagonal matrix, identity matrix, ones diagonal matrix.
 * Ensure the result Matrix is a square matrix with
 * all value is initial.
 */

void zeros(Matrix dst_matrix) {
	uint8_t k;
	for (k = 1; k <= dst_matrix.row * dst_matrix.col; k++)
		dst_matrix.entries[k - 1] = 0;
}

void eye(Matrix dst_matrix) {
	uint8_t k;
	zeros(dst_matrix);
	for (k = 1; k <= dst_matrix.row; k++)
		dst_matrix.entries[(k - 1) * dst_matrix.col + k - 1] = 1;
}

void ones(Matrix dst_matrix) {
	uint8_t k;
	for (k = 1; k <= dst_matrix.row * dst_matrix.col; k++)
		dst_matrix.entries[k - 1] = 1.0f;
}

/*
 * Function get or set the value of an element of a matrix
 * locates at the r-th row and c-th column.
 * Ensure all value of matrix is initial.
 */

float get(Matrix src_matrix, uint8_t r, uint8_t c) {
	float d = src_matrix.entries[(r - 1) * src_matrix.col + c - 1];
	return d;
}

void set(Matrix dst_matrix, uint8_t r, uint8_t c, float d) {
	dst_matrix.entries[(r - 1) * dst_matrix.col + c - 1] = d;
}

/*
 *  Function matrix addition/subtraction with argument:
 *  	n: number of row	 m: number of column
 *  	matrix1: matrix n*m
 *  	matrix2: matrix n*m
 *  	result: matrix result n*m
 *  Ensure all value of matrix is initial.
 */

void sum(Matrix A, Matrix B, Matrix result) {
	uint8_t i;
	for (i = 1; i <= A.row * A.col; i++)
		result.entries[i - 1] = A.entries[i - 1] + B.entries[i - 1];
}

void minus(Matrix A, Matrix B, Matrix result) {
	uint8_t i;
	for (i = 1; i <= A.row * A.col; i++)
		result.entries[i - 1] = A.entries[i - 1] - B.entries[i - 1];
}

/*
 * Function copy the value of source matrix into result matrix.
 * Ensure all value of matrix is initial.
 */

void copy_value(Matrix src_matrix, Matrix dst_matrix) {
	uint8_t i;
	for (i = 1; i <= src_matrix.row * src_matrix.col; i++)
		dst_matrix.entries[i - 1] = src_matrix.entries[i - 1];
}

/*
 *  Function multiply two matrix with argument:
 * 		 n: number of row 1 	 m: number of column 1 	 p: number of column2
 *  	 matrix1: matrix n*m
 *  	 matrix2: matrix m*p
 *  	 result: matrix result n*p
 *  Ensure all value of matrix is initial.
 */

float inner_multiply(Matrix A, Matrix B) {
	float d = 0;
	uint8_t i;
	uint8_t n = A.row;
	if (A.col > n)
		n = A.col;
	for (i = 1; i <= n; i++)
		d += A.entries[i - 1] * B.entries[i - 1];
	return d;
}

void hadamard_product(Matrix A, Matrix B, Matrix result) {
	uint8_t i;
	for (i = 1; i <= result.row * result.col; i++)
		result.entries[i - 1] = A.entries[i - 1] * B.entries[i - 1];
}

void scalar_multiply(Matrix A, float c, Matrix result) {
	uint8_t i;
	for (i = 1; i <= A.row * A.col; i++)
		result.entries[i - 1] = A.entries[i - 1] * c;
}

void multiply(Matrix A, Matrix B, Matrix result) {
	Matrix R;
	R.row = result.row;
	R.col = result.col;
	R.entries = malloc(sizeof(float) * result.row * result.col);
	uint8_t i, j, k;
	for (i = 1; i <= A.row; i++)
		for (j = 1; j <= B.col; j++) {
			float de = 0;
			for (k = 1; k <= A.col; k++)
				de += A.entries[(i - 1) * A.col + k - 1]
						* B.entries[(k - 1) * B.col + j - 1];
			R.entries[(i - 1) * R.col + j - 1] = de;
		}
//    for(int i=1;i<=result.row*result.col;i++)
//    	result.entries[i-1]=R.entries[i-1];
	copy_value(R, result);
	free(R.entries);
}

/*
 *  Function matrix transpose with argument:
 *  	source matrix: matrix n*m
 *  	destination matrix: matrix m*n
 *  Ensure all value of matrix is initial.
 */

void transpose(Matrix src_matrix, Matrix dst_matrix) {
	Matrix R;
	R.row = dst_matrix.row;
	R.col = dst_matrix.col;
	R.entries = (float*) malloc(sizeof(float) * src_matrix.row * src_matrix.col);
	uint8_t i, j;
	uint8_t k = 0;
	for (i = 1; i <= dst_matrix.row; i++)
		for (j = 1; j <= dst_matrix.col; j++)
			R.entries[k++] = src_matrix.entries[(j - 1) * src_matrix.row + i - 1];
	copy_value(R, dst_matrix);
	free(R.entries);
}

/*
 * Function matrix truncate with argument:
 * 		source matrix: matrix n*m
 * 		destination matrix: matrix x*y
 * 		begin_row
 * 		begin_column
 * Ensure all value of matrix is initial.
 */

void truncate_matrix(Matrix src_matrix, Matrix dst_matrix, uint8_t begin_row,
		uint8_t begin_col) {
	uint8_t i, j;
	for (i = begin_row; i < (begin_row + dst_matrix.row); i++) {
		for (j = begin_col; j < (begin_col + dst_matrix.col); j++) {
			dst_matrix.entries[(i - begin_row) * dst_matrix.col + j - begin_col] =
					src_matrix.entries[(i - 1) * src_matrix.col + j - 1];
		}
	}
}

/*
 *  Function matrix horizon concat with argument:
 *  	matrix A: matrix n*a
 *  	matrix B: matrix n*b
 *  	result: matrix result n*(a+b)
 *  Ensure all value of matrix is initial.
 */

void hconcat(Matrix A, Matrix B, Matrix result) {
	uint8_t i, j;
	uint8_t k = 0;
	for (i = 1; i <= A.row; i++) {
		for (j = 1; j <= A.col; j++)
			result.entries[k++] = A.entries[(i - 1) * A.col + j - 1];
		for (j = 1; j <= B.col; j++)
			result.entries[k++] = B.entries[(i - 1) * B.col + j - 1];
	}
}

/*
 *  Function matrix horizon triple concat with argument:
 *  	matrix A: matrix n*a
 *  	matrix B: matrix n*b
 *  	matrix C: matrix n*c
 *  	result: matrix result n*(a+b+c)
 * 	Ensure all value of matrix is initial.
 */

void htri_concat(Matrix A, Matrix B, Matrix C, Matrix result) {
	uint8_t i, j;
	uint8_t k = 0;
	for (i = 1; i <= A.row; i++) {
		for (j = 1; j <= A.col; j++)
			result.entries[k++] = A.entries[(i - 1) * A.col + j - 1];
		for (j = 1; j <= B.col; j++)
			result.entries[k++] = B.entries[(i - 1) * B.col + j - 1];
		for (j = 1; j <= C.col; j++)
			result.entries[k++] = C.entries[(i - 1) * C.col + j - 1];
	}
}

/*
 *  Function matrix vertical concat with argument:
 *  	matrix A: matrix a*n
 *  	matrix B: matrix b*n
 *  	result: matrix result (a+b)*n
 * 	Ensure all value of matrix is initial.
 */

void vconcat(Matrix A, Matrix B, Matrix result) {
	uint8_t i, j;
	uint8_t k = 0;
	for (i = 1; i <= A.row; i++)
		for (j = 1; j <= A.col; j++)
			result.entries[k++] = A.entries[(i - 1) * A.col + j - 1];
	for (i = 1; i <= B.row; i++)
		for (j = 1; j <= B.col; j++)
			result.entries[k++] = B.entries[(i - 1) * B.col + j - 1];
}

/*
 * Function generate_matrix with each component is same value
 * Ensure all value of matrix is initial.
 */
void generate_matrix(Matrix result, float value) {
	uint8_t i, j;
	for (i = 0; i < result.row; i++) {
		for (j = 0; j < result.col; j++) {
			result.entries[i * result.col + j] = value;
		}
	}
}

/*
 * Function generate matrix with each component is same row/column
 * with argument:
 * 		source vector matrix A
 * 		number of row/column
 * 		result matrix
 * Ensure all value of matrix is initial.
 */

void hgenerate(Matrix A, uint8_t c, Matrix result) {
	uint8_t i, j;
	uint8_t k = 0;
	for (i = 1; i <= A.row; i++)
		for (j = 1; j <= c; j++)
			result.entries[k++] = A.entries[i - 1];
}

void vgenerate(Matrix A, uint8_t r, Matrix result) {
	uint8_t i, j;
	uint8_t k = 0;
	for (i = 1; i <= r; i++)
		for (j = 1; j <= A.col; j++)
			result.entries[k++] = A.entries[j - 1];
}

/*
 * Function generate diagonal matrix from a vector matrix with argument:
 * 		vector matrix: src_matrix
 * 		destination matrix: dst_matrix
 * Ensure all value of matrix is initial.
 */

void diag_matrix_col(Matrix src_matrix, Matrix dst_matrix) {
	uint8_t i, j;
	uint8_t m = src_matrix.col;
	if (m < src_matrix.row)
		m = src_matrix.row;
	for (i = 0; i < m; i++) {
		for (j = 0; j < m; j++) {
			dst_matrix.entries[i * dst_matrix.col + j] =
					(i == j) ? src_matrix.entries[i] : 0;
		}
	}
}

/*
 * Function cholesky decomposition with argument:
 * 		source matrix: src_matrix
 * 		destination matrix: dst_matrix
 * Ensure all value of matrix is initial.
 */

void chol(Matrix src_matrix, Matrix dst_matrix) {
	Matrix mediate;
	mediate.row = dst_matrix.row;
	mediate.col = dst_matrix.col;
	mediate.entries = (float*) malloc(
			sizeof(float) * mediate.row * mediate.col);
	while (mediate.entries == NULL) {
	};
	copy_value(src_matrix, mediate);

	uint8_t i, j, k;
	float de;
	for (i = 1; i <= mediate.row; i++) {
		for (j = 1; j < i; j++) {
			de = src_matrix.entries[(i - 1) * mediate.col + j - 1];
			for (k = 1; k < j; k++)
				de -= mediate.entries[(i - 1) * mediate.col + k - 1]
						* mediate.entries[(j - 1) * mediate.col + k - 1];
			mediate.entries[(i - 1) * mediate.col + j - 1] = de
					/ mediate.entries[(j - 1) * mediate.col + j - 1];
		}
		de = src_matrix.entries[(i - 1) * mediate.col + i - 1];
		for (j = 1; j < i; j++)
			de -= mediate.entries[(i - 1) * mediate.col + j - 1]
					* mediate.entries[(i - 1) * mediate.col + j - 1];
		mediate.entries[(i - 1) * mediate.col + i - 1] = square_root_f(
				absolute_f(de));
	}
	for (i = 2; i <= mediate.row; i++)
		for (j = 1; j < i; j++)
			mediate.entries[(j - 1) * mediate.col + i - 1] = 0;
	copy_value(mediate, dst_matrix);
	free(mediate.entries);
}
