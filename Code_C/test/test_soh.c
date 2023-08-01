/*
 * test_soh.c
 *
 *  Created on: Jul 14, 2023
 *      Author: ADMIN
 */

#include <stdio.h>
#include "csv_loader.h"
#include "../soc_ukf/soc_ukf.h"
#include "soc_ukf_config.h"
#include "soh_lsb.h"


#define INPUT_SAMPLE_SIZE				1000000

const char *data_file_path = "soh_test_case_2_5.csv";
const char *soh_save_data_file_path = "soh_save_data.csv";
const char *output_ocv_data_file = "soh_test_result_5.csv";

static BMS_Input_Vector input_vector[INPUT_SAMPLE_SIZE];
static SOH_Save_Data soh_input_vector[200];
FILE *out_file;

uint32_t soc = 0.0f;
int test_circle = 0;
SOC_Parameter soc_parameter;
SOC_UKF bms_soc;
SOH_Estimator bms_soh;
SOH_Save_Data soh_save_data;

static void show_circle(const int32_t sample) {
	printf("circle %d\n", sample);
}

static int32_t create_est_data(const char *file_path, FILE *out_file) {

	out_file = fopen(file_path, "w"); // write only

	if (out_file == NULL) {
		printf("Error! Could not write to file\n");
		return -1;
	}
	fprintf(out_file, "test_circle, SOC, SOC_f,");

	fprintf(out_file, "terminalVoltage,current,");
	fprintf(out_file, "SOH, SOH_f,");

	fprintf(out_file, "\n");
	fclose(out_file);
	return 0;
}

static int32_t save_est_data(const char *file_path, const uint32_t pack_voltage, const int32_t pack_current, FILE *out_file) {
	out_file = fopen(file_path, "a");
	test_circle++;
	fprintf(out_file, "%i,", test_circle);

	fprintf(out_file, "%d,%d,", bms_soc.output.SOC,(int32_t)(bms_soc.output.SOC_f*10000.0f));
	fprintf(out_file, "%d,%d,", bms_soc.filter.avg_pack_voltage, bms_soc.filter.avg_pack_current);
	fprintf(out_file, "%d,%f,", (int32_t)(bms_soh.soh), bms_soh.soh);

	fprintf(out_file, "\n");
	fclose(out_file);
	return 0;
}

static int32_t save_soh_data(const char *file_path, const SOH_Save_Data *data, FILE *out_file) {
	out_file = fopen(file_path, "a");

	fprintf(out_file, "%d,", (int32_t)soh_save_data.c1);
	fprintf(out_file, "%d,", (int32_t)soh_save_data.c2);
	fprintf(out_file, "%d,", (int32_t)soh_save_data.c3);
	fprintf(out_file, "%d,", (int32_t)soh_save_data.delta_x);
	fprintf(out_file, "%d,", (int32_t)soh_save_data.delta_y);
	fprintf(out_file, "%d,", (int32_t)soh_save_data.est_capacity);
	fprintf(out_file, "%d,", (int32_t)soh_save_data.last_soc);
	fprintf(out_file, "%d,", (int32_t)soh_save_data.soh);

	fprintf(out_file, "\n");
	fclose(out_file);
	return 0;
}

int save_cnt = 0;
int hal_entry(void) {

	int32_t sample_size = load_battery_data_from_csv_file(data_file_path,
			(BMS_Input_Vector*) input_vector, INPUT_SAMPLE_SIZE);
	if (sample_size <= 0) {
		return -1;
	}
	printf("Sample size: %d\n", sample_size);

	int32_t soh_size = load_soh_data_from_csv_file(soh_save_data_file_path, (SOH_Save_Data*) soh_input_vector, 200);
	if (soh_size <= 0) {
		return -1;
	}
	printf("SOH size: %d\n", soh_size);

	create_est_data(output_ocv_data_file, out_file);
	bms_soc.input.pack_voltage = input_vector[0].terminalVoltage;
	bms_soc.input.pack_current = input_vector[0].current;

    ukf_parameters_create(&soc_parameter);
    bms_soc.battery_model = &LG_Model;
	load_soc(&bms_soc, (float)soh_input_vector[soh_size - 1].last_soc/10.0f);
    ukf_init(&bms_soc, 100);

    soh_save_data.c1 = soh_input_vector[soh_size - 1].c1;
    soh_save_data.c2 = soh_input_vector[soh_size - 1].c2;
    soh_save_data.c3 = soh_input_vector[soh_size - 1].c3;
    soh_save_data.delta_x = soh_input_vector[soh_size - 1].delta_x;
    soh_save_data.delta_y = soh_input_vector[soh_size - 1].delta_y;
    soh_save_data.est_capacity = soh_input_vector[soh_size - 1].est_capacity;
    soh_save_data.last_soc = soh_input_vector[soh_size - 1].last_soc;
    soh_save_data.soh = soh_input_vector[soh_size - 1].soh;

    bms_set_soh(&bms_soh, &soh_save_data);
    bms_soh_init(&bms_soh, model_get_soc_from_ocv(bms_soc.battery_model,
			(float) bms_soc.input.pack_voltage / PACK_VOLTAGE_NORMALIZED_GAIN));
    bms_soc.soh = bms_soh.soh / 100.0f;
	for (int i = 0; i < sample_size; i++) {
		bms_soc.input.pack_voltage = (uint32_t)(input_vector[i].terminalVoltage);
		bms_soc.input.pack_current = (int32_t)(input_vector[i].current);
//		if(bms_soc.input.pack_voltage < 48000){
//			bms_soc.input.pack_voltage = 48000;
//			bms_soc.input.pack_current = 0;
//		}
		for (int j = 0; j < SOC_PERIOD; j++) {
		    ukf_update(&bms_soc);
		}
		soc = bms_soc.output.SOC;
		bms_update_soh(&bms_soh, (bms_soc.output.SOC_f/100.0f), bms_soc.filter.avg_pack_current);
	    bms_soc.soh = bms_soh.soh / 100.0f;
	    save_cnt++;
		if(save_cnt == 100){
		    save_est_data(output_ocv_data_file, bms_soc.input.pack_voltage, bms_soc.input.pack_current, out_file);
		    save_cnt = 0;
		}
//	    save_est_data(output_ocv_data_file, bms_soc.input.pack_voltage, bms_soc.input.pack_current, out_file);
		show_circle(i);
	}

    soh_save_data.c1 = (int32_t)(bms_soh.c1 * SOH_NORMALIZED_GAIN);
    soh_save_data.c2 = (int32_t)(bms_soh.c2 * SOH_NORMALIZED_GAIN);
    soh_save_data.c3 = (int32_t)(bms_soh.c3 * SOH_NORMALIZED_GAIN);
    soh_save_data.delta_x = (int32_t)(bms_soh.delta_x * SOH_NORMALIZED_GAIN);
    soh_save_data.delta_y = (int32_t)(bms_soh.delta_y * SOH_NORMALIZED_GAIN);
    soh_save_data.est_capacity = (int32_t)(bms_soh.est_capacity * SOH_NORMALIZED_GAIN);
    soh_save_data.soh = (int32_t)(bms_soh.soh * SOH_NORMALIZED_GAIN);
    soh_save_data.last_soc = (int32_t)(bms_soh.last_soc * SOH_NORMALIZED_GAIN);
	save_soh_data(soh_save_data_file_path, &soh_save_data, out_file);
	return 0;
}

