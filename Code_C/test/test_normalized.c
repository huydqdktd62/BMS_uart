/*
 * test_normalized.c
 *
 *  Created on: May 8, 2023
 *      Author: ADMIN
 */
#include <stdio.h>
#include "csv_loader.h"
#include "../soc_ukf/soc_ukf.h"
#include "soc_ukf_config.h"


#define INPUT_SAMPLE_SIZE				100000

const char *data_file_path = "input.csv";
const char *output_ocv_data_file = "normalized_output_BMS.csv";

static BMS_Input_Vector input_vector[INPUT_SAMPLE_SIZE];
FILE *out_file;

int8_t state_entries[7];
uint32_t soc = 0.0f;
int test_circle = 0;
SOC_Parameter soc_parameter;
SOC_UKF bms_soc;

static void show_circle(const int32_t sample) {
	printf("circle %d\n", sample);
}

static int32_t create_est_data(const char *file_path, FILE *out_file) {

	out_file = fopen(file_path, "w"); // write only

	if (out_file == NULL) {
		printf("Error! Could not write to file\n");
		return -1;
	}
	fprintf(out_file, "test_circle,state 0, state 1, state 2, state 3, state 4, state 5, state 6,SOC,SOC_f,");
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

static int32_t save_est_data(const char *file_path, const uint32_t pack_voltage, const int32_t pack_current,	FILE *out_file) {
	out_file = fopen(file_path, "a");
	int i;
	test_circle++;
	fprintf(out_file, "%i,", test_circle);
	for (i = 0; i < 7; i++){
		fprintf(out_file, "%d,", state_entries[i]);
		state_entries[i] = 0;
	}
	fprintf(out_file, "%d,%d,", bms_soc.output.SOC,(int32_t)(bms_soc.output.SOC_f*1000000.0f));
//	fprintf(out_file, "%d,%d,", bms_soc.input.pack_voltage,bms_soc.input.pack_current);
	fprintf(out_file, "%d,%d,", bms_soc.filter.avg_pack_voltage, bms_soc.filter.avg_pack_current);
	fprintf(out_file, "%d,", (int32_t)(soc_parameter.H_param*1000000.0f));

	for (i = 0; i < 3; i++)
		fprintf(out_file, "%d,", (int32_t)(soc_parameter.estimate_state_entries[i]*1000000.0f));

	for (i = 0; i < 9; i++)
		fprintf(out_file, "%d,", (int32_t)(soc_parameter.state_covariance_entries[i]*10000000000.0f));

	for (i = 0; i < 21; i++)
		fprintf(out_file, "%d,", (int32_t)(soc_parameter.sigma_points_entries[i]*1000000.0f));
	fprintf(out_file, "%d,%d,%d,", (int32_t)(soc_parameter.priori_estimate_state_entries[0]*1000000.0f),
            (int32_t)(soc_parameter.priori_estimate_state_entries[1]*1000000.0f),
            (int32_t)(soc_parameter.priori_estimate_state_entries[2]*1000000.0f));
	for (i = 0; i < 21; i++)
		fprintf(out_file, "%d,", (int32_t)(soc_parameter.sigma_state_error_entries[i]*1000000000.0f));
	fprintf(out_file, "%d,%d,", (int32_t)(soc_parameter.measurement_cov*1000000.0f),
            (int32_t)(soc_parameter.est_measurement*1000000.0f));
	for (i = 0; i < 3; i++)
		fprintf(out_file, "%d,", (int32_t)(soc_parameter.cross_covariance_entries[i]*1000000000.0f));
	for (i = 0; i < 3; i++)
		fprintf(out_file, "%d,", (int32_t)(soc_parameter.aukf_kalman_gain_entries[i]*1000000000.0f));
	for (i = 0; i < 9; i++)
		fprintf(out_file, "%d,", (int32_t)(soc_parameter.matrix_A_entries[i]*1000000.0f));
	for (i = 0; i < 6; i++)
		fprintf(out_file, "%d,", (int32_t)(soc_parameter.matrix_B_entries[i]*1000000.0f));
	for (i = 0; i < 3; i++)
		fprintf(out_file, "%d,", (int32_t)(soc_parameter.matrix_C_entries[i]*1000000.0f));
	for (i = 0; i < 2; i++)
		fprintf(out_file, "%d,", (int32_t)(soc_parameter.matrix_D_entries[i]*1000000.0f));
	fprintf(out_file, "%d,%d,", (int32_t)(soc_parameter.observed_measurement_entries[0]*1000000.0f),
            (int32_t)(soc_parameter.observed_measurement_entries[1]*1000000.0f));
	for (i = 0; i < 7; i++)
		fprintf(out_file, "%d,", (int32_t)(soc_parameter.sigma_measurements_entries[i]*1000000.0f));
	for (i = 0; i < 7; i++)
		fprintf(out_file, "%d,", (int32_t)(soc_parameter.sigma_measurement_error_entries[i]*1000000.0f));

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
	bms_soc.input.pack_voltage = input_vector[0].terminalVoltage;
	bms_soc.input.pack_current = input_vector[0].current;
	load_soc(&bms_soc, 100.0f*get_soc_from_ocv((float)bms_soc.input.pack_voltage/PACK_VOLTAGE_NORMALIZED_GAIN));
	ukf_parameters_create(&soc_parameter);
	ukf_init(&bms_soc);
	ukf_update(&bms_soc, 1.0f);
	for (int i = 0; i < sample_size; i++) {
		bms_soc.input.pack_voltage = input_vector[i].terminalVoltage;
		bms_soc.input.pack_current = input_vector[i].current;
		for (int j = 0; j < SOC_PERIOD; j++) {
			ukf_update(&bms_soc, 1.0f);
			switch (bms_soc.state) {
				case SOC_ST_INIT:
					state_entries[SOC_ST_INIT] = 1;
					break;
				case SOC_ST_IDLE:
					state_entries[SOC_ST_IDLE] = 1;
					break;
				case SOC_ST_UKF:
					state_entries[SOC_ST_UKF] = 1;
					break;
				case SOC_ST_COULOMB_COUNTER:
					state_entries[SOC_ST_COULOMB_COUNTER] = 1;
					break;
				case SOC_ST_CALIB:
					state_entries[SOC_ST_CALIB] = 1;
					break;
				case SOC_ST_SLEEP:
					state_entries[SOC_ST_SLEEP] = 1;
					break;
				case SOC_ST_FAULT:
					state_entries[SOC_ST_FAULT] = 1;
					break;
				default:
					break;
			}
		}
		save_est_data(output_ocv_data_file, bms_soc.input.pack_voltage, bms_soc.input.pack_current, out_file);
		soc = bms_soc.output.SOC;
		show_circle(i);
	}
	return 0;
}

