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


#define INPUT_SAMPLE_SIZE				3000

const char *data_file_path = "normalized_input.csv";
const char *output_ocv_data_file = "normalized_output_BMS.csv";

static Data_Logger input_vector[INPUT_SAMPLE_SIZE];
FILE *out_file;

SOC_Parameter_Entries logger_entries;
SOC_Parameter_Entries processor_entries;
SOC_UKF logger;
SOC_UKF processor;

int test_circle = 0;
Data_Logger input;
Data_Logger output;

const int64_t m_weight[7] = {-2999, 500, 500, 500, 500, 500, 500};
const int64_t c_weight[7] = {-3002, 500, 500, 500, 500, 500, 500};


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
	ukf_init(66000, 0, &logger);
	ukf_init(66000, 0, &processor);
	parameters_init(&logger, logger_entries);
	parameters_init(&processor, processor_entries);

	for (int i = 0; i < sample_size; i++) {
		input = input_vector[i];
		synchronize_in(&input, &logger);
		normalize(&logger, &processor, NORMALIZED_TYPE);
		synchronize_out(&processor, &output);
		save_data(output_ocv_data_file, out_file, NORMALIZED_TYPE, &input, &output);
		show_circle(i);
	}
	return 0;
}
