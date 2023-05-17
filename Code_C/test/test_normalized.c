/*
 * test_normalized.c
 *
 *  Created on: May 8, 2023
 *      Author: ADMIN
 */
#include <stdio.h>
#include "csv_loader.h"
#include "data_logger.h"
#include "normalize.h"
#include "app_config.h"


#define INPUT_SAMPLE_SIZE				3000

const char *data_file_path = "normalized_input.csv";
const char *output_ocv_data_file = "normalized_output_PC.csv";

static Data_Logger input_vector[INPUT_SAMPLE_SIZE];
FILE *out_file;

SOC_Parameter_Entries logger_entries;
SOC_Parameter_Entries processor_entries;
SOC_UKF logger;
SOC_UKF processor;

int test_circle = 0;
Data_Logger input_logger;
Data_Logger output_logger;

//const int64_t m_weight[7] = {-2999, 500, 500, 500, 500, 500, 500};
//const int64_t c_weight[7] = {-3002, 500, 500, 500, 500, 500, 500};


static void show_circle(const int32_t sample) {
	printf("circle %d\n", sample);
}

int hal_entry(void) {

	int32_t sample_size = load_battery_data_from_csv_file(data_file_path,
			(BMS_Input_Vector*) input_vector, INPUT_SAMPLE_SIZE);
	if (sample_size <= 0) {
		return -1;
	}
	printf("Sample size: %d\n", sample_size);

	create_data(output_ocv_data_file, out_file, NORMALIZED_TYPE);
	load_soc(&processor, 80);
	load_soc(&logger, 80);
	ukf_parameters_init(&logger, &logger_entries);
	ukf_parameters_init(&processor, &processor_entries);
	ukf_init(66000, 0, &logger);
	ukf_init(66000, 0, &processor);
	for (int i = 0; i < sample_size; i++) {
		input_logger = input_vector[i];
		synchronize_in(&input_logger, &logger);
		normalize(&logger, &processor, NORMALIZED_TYPE);
		synchronize_out(&processor, &output_logger);
		save_data(output_ocv_data_file, out_file, NORMALIZED_TYPE, &input_logger, &output_logger);
		show_circle(i);
	}
	return 0;
}
