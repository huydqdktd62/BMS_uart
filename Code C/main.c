/*
 * main.c
 *
 *  Created on: Feb 9, 2023
 *      Author: ADMIN
 */

#include <soc_ukf.h>
#include "soc_vffrls.h"
#include <stdio.h>
#include "csv_loader.h"
#include "battery_model.h"

#define INPUT_SAMPLE_SIZE				1000000
#define SOH_SAMPLE_SIZE					2000

#define STATE_DIM					3
#define SIGMA_FACTOR					(STATE_DIM*2+1)
#define MEASUREMENT_DIM			1
#define DYNAMIC_DIM				2

const char *data_file_path = "input.csv";
const char *output_ocv_data_file = "output.csv";

//static void task_300ms_update_battery_model(void);

static BMS_Input_Vector input_vector[INPUT_SAMPLE_SIZE];
FILE *out_file;


uint32_t soc = 0.0f;
float cell_vol = 0.0f;
float cell_cur = 0.0f;
float cell_soh = 0.0f;
float ref_soh = 1.0f;
int test_circle = 0;

static void show_ocv(const int32_t sample) {
	printf("SOC %d: %u\n", sample, soc);
}

static int32_t create_est_data(const char *file_path, FILE *out_file) {

	out_file = fopen(file_path, "w"); // write only

	if (out_file == NULL) {
		printf("Error! Could not write to file\n");
		return -1;
	}
	fprintf(out_file, "test_circle,SOC,SOC_f,");
	fprintf(out_file, "terminalVoltage,current,");
	fprintf(out_file, "H_param,");
	fprintf(out_file, "a_est_state1,a_est_state2,a_est_state3,");

	fprintf(out_file, "a_state_cov1,a_state_cov2,a_state_cov3,");
	fprintf(out_file, "a_state_cov4,a_state_cov5,a_state_cov6,");
	fprintf(out_file, "a_state_cov7,a_state_cov8,a_state_cov9,");

	fprintf(out_file, "sig1_1,sig1_2,sig1_3,sig1_4,sig1_5,sig1_6,sig1_7,");
	fprintf(out_file, "sig2_1,sig2_2,sig2_3,sig2_4,sig2_5,sig2_6,sig2_7,");
	fprintf(out_file, "sig3_1,sig3_2,sig3_3,sig3_4,sig3_5,sig3_6,sig3_7,");
	fprintf(out_file, "pri_state1,pri_state2,pri_state3,");
	fprintf(out_file,
			"e_sig1_1,e_sig1_2,e_sig1_3,e_sig1_4,e_sig1_5,e_sig1_6,e_sig1_7,");
	fprintf(out_file,
			"e_sig2_1,e_sig2_2,e_sig2_3,e_sig2_4,e_sig2_5,e_sig2_6,e_sig2_7,");
	fprintf(out_file,
			"e_sig3_1,e_sig3_2,e_sig3_3,e_sig3_4,e_sig3_5,e_sig3_6,e_sig3_7,");
	fprintf(out_file, "measur_cov,est_measur,");
	fprintf(out_file, "a_cross1,a_cross2,a_cross3,");
	fprintf(out_file, "a_kal1,a_kal2,a_kal3,");
	fprintf(out_file, "A1,A2,A3,A4,A5,A6,A7,A8,A9,");
	fprintf(out_file, "B1,B2,B3,B4,B5,B6,");
	fprintf(out_file, "C1,C2,C3,");
	fprintf(out_file, "D1,D2,");
	fprintf(out_file, "I,sign(I),");
	fprintf(out_file, "sig_m1,sig_m2,sig_m3,sig_m4,sig_m5,sig_m6,sig_m7,");
	fprintf(out_file,
			"e_sig_m1,e_sig_m2,e_sig_m3,e_sig_m4,e_sig_m5,e_sig_m6,e_sig_m7");

	fprintf(out_file, "\n");
	fclose(out_file);
	return 0;
}


//SOC_Input SOC_input;
//SOC_Output SOC_output;
/*
static int32_t save_est_data(const char *file_path, const uint32_t voltage,
		const int32_t current, const SOC_Output output, FILE *out_file) {
	out_file = fopen(file_path, "a");
	test_circle++;
	fprintf(out_file, "%i,%i,%d,%i,%f,%f,%f,%f", test_circle, voltage, current, output.SOC, output.OCV, battery_param.R0, battery_param.R1, battery_param.C1);

	fprintf(out_file, "\n");
	fclose(out_file);
	return 0;
}
*/


static int32_t save_est_data(const char *file_path, const uint32_t pack_voltage, const int32_t pack_current,	FILE *out_file) {
	out_file = fopen(file_path, "a");
	test_circle++;

	fprintf(out_file, "%i,%d,%f,", test_circle, soc_output.SOC,
			soc_output.SOC_f);
	fprintf(out_file, "%d,%d,", soc_input.pack_voltage,
			soc_input.pack_current);
	fprintf(out_file, "%.12f,", H_param);
	int i;
	for (i = 0; i < 3; i++)
		fprintf(out_file, "%.12f,", estimate_state_entries[i]);

	for (i = 0; i < 9; i++)
		fprintf(out_file, "%.12f,", state_covariance_entries[i]);

	for (i = 0; i < 21; i++)
		fprintf(out_file, "%.12f,", sigma_points_entries[i]);
	fprintf(out_file, "%f,%f,%f,", priori_estimate_state_entries[0],
			priori_estimate_state_entries[1],
			priori_estimate_state_entries[2]);
	for (i = 0; i < 21; i++)
		fprintf(out_file, "%.12f,", sigma_state_error_entries[i]);
	fprintf(out_file, "%.12f,%.12f,", measurement_cov,
			est_measurement);
	for (i = 0; i < 3; i++)
		fprintf(out_file, "%.12f,", cross_covariance_entries[i]);
	for (i = 0; i < 3; i++)
		fprintf(out_file, "%.12f,", aukf_kalman_gain_entries[i]);
	for (i = 0; i < 9; i++)
		fprintf(out_file, "%.12f,", matrix_A_entries[i]);
	for (i = 0; i < 6; i++)
		fprintf(out_file, "%.12f,", matrix_B_entries[i]);
	for (i = 0; i < 3; i++)
		fprintf(out_file, "%.12f,", matrix_C_entries[i]);
	for (i = 0; i < 2; i++)
		fprintf(out_file, "%.12f,", matrix_D_entries[i]);
	fprintf(out_file, "%.12f,%.12f,", observed_measurement_entries[0],
			observed_measurement_entries[1]);
	for (i = 0; i < 7; i++)
		fprintf(out_file, "%.12f,", sigma_measurements_entries[i]);
	for (i = 0; i < 7; i++)
		fprintf(out_file, "%.12f,", sigma_measurement_error_entries[i]);

	fprintf(out_file, "\n");
	fclose(out_file);
	return 0;
}


int main(void) {

	int32_t sample_size = load_battery_data_from_csv_file(data_file_path,
			(BMS_Input_Vector*) input_vector, INPUT_SAMPLE_SIZE);
	if (sample_size <= 0) {
		return -1;
	}
	printf("Sample size: %d", sample_size);

	create_est_data(output_ocv_data_file, out_file);
	cell_vol = input_vector[0].terminalVoltage;
	cell_cur = input_vector[0].current;

	ukf_init(cell_vol, cell_cur);

	for (int i = 1; i < sample_size; i++) {
		soc_input.pack_voltage = input_vector[i].terminalVoltage;
		soc_input.pack_current = input_vector[i].current;
		for (int j = 0; j<SOC_PERIOD; j++){
			ukf_update(soc_input, 1.0f);
		}

//		soc_input.pack_voltage = input_vector[i].terminalVoltage;
//		soc_input.pack_current = input_vector[i].current;
//		ref_soh = 1.0;
//		soc_output = ukf_update(soc_input, 1.0f);

		save_est_data(output_ocv_data_file, soc_input.pack_voltage, soc_input.pack_current, out_file);
		soc = soc_output.SOC;
		show_ocv(i);
	}
	return 0;
}

