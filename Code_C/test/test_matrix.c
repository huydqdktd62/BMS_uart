/*
 * test_matrix.c
 *
 *  Created on: May 22, 2023
 *      Author: ADMIN
 */


#include "matrix.h"
#include "math_util.h"

Matrix a,b;
int i,j;

int hal_entry(void){
	a.col = 3;
	a.row = 3;
	b.col = 3;
	b.row = 3;
	a.entries = malloc(9 * sizeof(int64_t));
	b.entries = malloc(9 * sizeof(int64_t));

	a.entries[0] = 1401;
	a.entries[1] = 7;
	a.entries[2] = -1;
	a.entries[3] = 7;
	a.entries[4] = 7;
	a.entries[5] = -1;
	a.entries[6] = -1;
	a.entries[7] = -1;
	a.entries[8] = 284670;

	chol(a, b);
	for (i = 0; i < 3; i++){
		for (j = 0; j < 3; j++){
			printf("%d ,",a.entries[i * 3 + j]);
		}
		printf("\n");
	}
	for (i = 0; i < 3; i++){
		for (j = 0; j < 3; j++){
			printf("%d ,",b.entries[i * 3 + j]);
		}
		printf("\n");
	}
	return 1;
}

