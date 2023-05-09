/*
 * test_normalized.c
 *
 *  Created on: May 8, 2023
 *      Author: ADMIN
 */
#include <stdio.h>
#include "csv_loader.h"

#define INPUT_SAMPLE_SIZE				1000000

const char *data_file_path = "normalized_input_PC.csv";
const char *output_ocv_data_file = "normalized_output_PC.csv";

static BMS_Input_Vector input_vector[INPUT_SAMPLE_SIZE];
FILE *out_file;

int test_circle = 0;

static void show_circle(const int32_t sample) {
	printf("circle %d\n", sample);
}

static int32_t create_est_data(const char *file_path, FILE *out_file) {

	int i;
	out_file = fopen(file_path, "w"); // write only

	if (out_file == NULL) {
		printf("Error! Could not write to file\n");
		return -1;
	}
	fprintf(out_file, "test_circle,");
	for (i = 1; i < 22; i++){
		fprintf(out_file, "input %d,", i);
	}
//	for (i = 1; i < 22; i++){
//		fprintf(out_file, "normalized_input %d,", i);
//	}
	fprintf(out_file, "summation 1, summation 2, summation 3,");
	fprintf(out_file, "result 1, result 2, result 3");
	fprintf(out_file, "\n");
	fclose(out_file);
	return 0;
}

int64_t normalized_input[21];
int64_t summation[3], normalized_result[3];
float result[3];
const int64_t m_weight[7] = {-2999, 500, 500, 500, 500, 500, 500};
const int64_t c_weight[7] = {-30020002, 5000000, 5000000, 5000000, 5000000, 5000000, 5000000};


static int32_t save_est_data(const char *file_path, FILE *out_file) {
	int i;
	out_file = fopen(file_path, "a");
	test_circle++;

	fprintf(out_file, "%i,", test_circle);
	for (i = 0; i < 21; i++){
		fprintf(out_file, "%d,", (int32_t)input_vector[test_circle].sigma_point[i]);
	}
//	for (i = 0; i < 21; i++){
//		fprintf(out_file, "%ld,", normalized_input[i]);
//	}
	for (i = 0; i < 3; i++){
		fprintf(out_file, "%d,", (int32_t)summation[i]);
	}
	for (i = 0; i < 3; i++){
		fprintf(out_file, "%.12f,", result[i]);
	}
	fprintf(out_file, "\n");
	fclose(out_file);
	return 0;
}


int hal_entry(void) {

	int32_t sample_size = load_battery_data_from_csv_file(data_file_path,
			(BMS_Input_Vector*) input_vector, INPUT_SAMPLE_SIZE);
	if (sample_size <= 0) {
		return -1;
	}
	printf("Sample size: %d\n", sample_size);

	create_est_data(output_ocv_data_file, out_file);

	for (int i = 1; i < sample_size; i++) {
		for (int t = 0; t < 3; t++){
			summation[t] = 0;
		}
//		for (int j = 0; j < 21; j++){
//			normalized_input[j] = (int64_t)(input_vector[i].sigma_point[j]);
//		}
		for (int k = 0; k < 7; k++){
			summation[0] += m_weight[k]*input_vector[i].sigma_point[k];
			summation[1] += m_weight[k]*input_vector[i].sigma_point[7 + k];
			summation[2] += m_weight[k]*input_vector[i].sigma_point[14 + k];
		}
		result[0] = (float)summation[0] / 10000000.0f;
		result[1] = (float)summation[1] / 10000000.0f;
		result[2] = (float)summation[2] / 10000000.0f;

		save_est_data(output_ocv_data_file, out_file);
		show_circle(i);
	}
	return 0;
}
