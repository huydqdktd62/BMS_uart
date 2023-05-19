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

typedef struct SOC_Parameter_64_bit_t SOC_Parameter_64_bit;
struct SOC_Parameter_64_bit_t{
	int64_t estimate_state[UKF_STATE_DIM];
	int64_t state_covariance[UKF_STATE_DIM * UKF_STATE_DIM];
	int64_t sigma_points[UKF_STATE_DIM * UKF_SIGMA_FACTOR];
	int64_t sigma_state_error[UKF_STATE_DIM * UKF_SIGMA_FACTOR];
	int64_t priori_estimate_state[UKF_STATE_DIM];
	int64_t matrix_A[UKF_STATE_DIM * UKF_STATE_DIM];
	int64_t matrix_B[UKF_STATE_DIM * UKF_DYNAMIC_DIM];
	int64_t matrix_C[UKF_MEASUREMENT_DIM * UKF_STATE_DIM];
	int64_t matrix_D[UKF_DYNAMIC_DIM * UKF_MEASUREMENT_DIM];
	int64_t observed_measurement[UKF_MEASUREMENT_DIM];
	int64_t sigma_measurement[UKF_MEASUREMENT_DIM * UKF_SIGMA_FACTOR];
	int64_t estimate_measurement;
	int64_t sigma_measurement_error[UKF_MEASUREMENT_DIM * UKF_SIGMA_FACTOR];
	int64_t measurement_covariance;
	int64_t cross_covariance[UKF_STATE_DIM];
	int64_t kalman_gain[UKF_STATE_DIM];
	int64_t m_update_sigma_state[UKF_STATE_DIM];

};
SOC_Parameter_64_bit soc_parameter_64_bit;

int64_t priori_est_state_64_bit[UKF_STATE_DIM];
int64_t sigma_point_64_bit[UKF_STATE_DIM * UKF_SIGMA_FACTOR];
const int64_t m_weight_64_bit[UKF_SIGMA_FACTOR] = {     -2999,
                                    500,
                                    500,
                                    500,
                                    500,
                                    500,
                                    500 };

int64_t est_measurement_64_bit;
int64_t sigma_measurement_64_bit[UKF_SIGMA_FACTOR];

int64_t cross_covariance_64_bit[UKF_SIGMA_FACTOR];
int64_t sigma_state_err_64_bit[UKF_STATE_DIM * UKF_SIGMA_FACTOR];
int64_t sigma_measurement_err_64_bit[UKF_SIGMA_FACTOR];
const int64_t c_weight_64_bit[UKF_SIGMA_FACTOR] = {
		-3002,
		500,
		500,
		500,
		500,
		500,
		500
};

const int64_t default_measurement_covariance_64_bit = 447200000000;
int64_t measurement_covariance_64_bit;

const int64_t default_system_covariance_64_bit[] = {
		1000000, 0, 0,
		0, 10000, 0,
		0, 0, 200000000
};
int64_t state_covariance_64_bit[UKF_STATE_DIM * UKF_STATE_DIM];

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
	    int32_t j,k;
	    int32_t r,c;
	    int64_t buff;

		switch(NORMALIZED_TYPE){
		case SIGMA_POINT:
			for (j = 0; j < UKF_STATE_DIM; j++){
				soc_parameter_64_bit.estimate_state[i] = (int64_t)input_logger.estimate_state[j];
			}
			for (j = 0; j < UKF_STATE_DIM * UKF_STATE_DIM; j++){
				soc_parameter_64_bit.state_covariance[j] = (int64_t)input_logger.state_covariance[j];
			}
			for (j = 0; j < UKF_STATE_DIM; j++){
				for (k = 0; k < UKF_SIGMA_FACTOR; k++){
					soc_parameter_64_bit.sigma_points[j * UKF_SIGMA_FACTOR + k] = soc_parameter_64_bit.estimate_state[j];
				}
			}
//			hgenerate(battery_soc->param.est_state, UKF_STATE_DIM, battery_soc->param.g1_create_sigma_point);
//			chol(battery_soc->param.state_cov, battery_soc->param.g2_create_sigma_point);
//			scalar_multiply(battery_soc->param.g2_create_sigma_point, UKF_GAMMA_RATIO, battery_soc->param.g2_create_sigma_point);
//			sum(battery_soc->param.g1_create_sigma_point, battery_soc->param.g2_create_sigma_point, battery_soc->param.g1_create_sigma_point);
//			scalar_multiply(battery_soc->param.g2_create_sigma_point, 2, battery_soc->param.g2_create_sigma_point);
//			minus(battery_soc->param.g1_create_sigma_point, battery_soc->param.g2_create_sigma_point, battery_soc->param.g2_create_sigma_point);
//			htri_concat(battery_soc->param.est_state, battery_soc->param.g1_create_sigma_point, battery_soc->param.g2_create_sigma_point,
//					battery_soc->param.sigma_points);
			break;
		case SIGMA_STATE:
			for (j = 0; j < UKF_STATE_DIM * UKF_STATE_DIM; j++){
				soc_parameter_64_bit.matrix_A[j] = input_logger.matrix_A[j];
			}
			for (j = 0; j < UKF_STATE_DIM * UKF_DYNAMIC_DIM; j++){
				soc_parameter_64_bit.matrix_B[j] = input_logger.matrix_B[j];
			}
			for (j = 0; j < UKF_MEASUREMENT_DIM; j++){
				soc_parameter_64_bit.observed_measurement[j] = input_logger.observed_measurement[j];
			}
			for (j = 0; j < UKF_STATE_DIM * UKF_SIGMA_FACTOR; j++){
				soc_parameter_64_bit.sigma_points[j] = input_logger.sigma_points[j];
			}
			for (j = 0; j < UKF_STATE_DIM * UKF_DYNAMIC_DIM; j++){
				soc_parameter_64_bit.m_update_sigma_state[j] = 0;
			}

			multiply(battery_soc->param.matrix_B, battery_soc->param.observed_measurement, battery_soc->param.m_update_sigma_state);
			hgenerate(battery_soc->param.m_update_sigma_state, UKF_SIGMA_FACTOR, battery_soc->param.g_update_sigma_state);
			multiply(battery_soc->param.matrix_A, battery_soc->param.sigma_points, battery_soc->param.sigma_points);
			sum(battery_soc->param.sigma_points, battery_soc->param.g_update_sigma_state, battery_soc->param.sigma_points);

			break;
		case PRIOR_STATE:
		    for (j = 0; j < UKF_STATE_DIM * UKF_SIGMA_FACTOR; j++){
		    	soc_parameter_64_bit.sigma_points[j] = (int64_t)(input_logger.sigma_points[j]);
		    }
		    for (j = 0; j < UKF_STATE_DIM; j++){
		    	soc_parameter_64_bit.priori_estimate_state[j] = 0;
		    }
		    for (j = 0; j < UKF_SIGMA_FACTOR; j++) {
		    	soc_parameter_64_bit.priori_estimate_state[0] += m_weight_64_bit[j]
		                * sigma_point_64_bit[j];
		    	soc_parameter_64_bit.priori_estimate_state[1] += m_weight_64_bit[j]
		                * sigma_point_64_bit[UKF_SIGMA_FACTOR + j];
		    	soc_parameter_64_bit.priori_estimate_state[2] += m_weight_64_bit[j]
		                * sigma_point_64_bit[2 * UKF_SIGMA_FACTOR + j];
		    }
		    for (j = 0; j < UKF_STATE_DIM; j++){
		        output_logger.priori_estimate_state[j] = (int32_t)soc_parameter_64_bit.priori_estimate_state[j];
		    }
		    break;
		case SIGMA_STATE_ERROR:
			hgenerate(battery_soc->param.priori_est_state, UKF_SIGMA_FACTOR, battery_soc->param.sigma_state_err);
			minus(battery_soc->param.sigma_points, battery_soc->param.sigma_state_err, battery_soc->param.sigma_state_err);
			break;
		case PRIOR_STATE_COVARIANCE:
			for (j = 0; j < UKF_STATE_DIM * UKF_SIGMA_FACTOR; j++){
				soc_parameter_64_bit.sigma_state_error[j] = (int64_t)input_logger.sigma_state_error[j];
			}
			for (j = 0; j < UKF_STATE_DIM * UKF_STATE_DIM; j++){
				soc_parameter_64_bit.state_covariance[j] = default_system_covariance_64_bit[j];
			}
			for (j = 0; j < UKF_SIGMA_FACTOR; j++){
				for (r = 0; r < UKF_STATE_DIM; r++){
					for (c = 0; c < UKF_STATE_DIM; c ++){
						soc_parameter_64_bit.state_covariance[r * UKF_STATE_DIM + c] +=
								soc_parameter_64_bit.sigma_state_error[r * UKF_SIGMA_FACTOR
										+ j] * c_weight_64_bit[j]
										* soc_parameter_64_bit.sigma_state_error[c
												* UKF_SIGMA_FACTOR + j];
					}
				}
			}
			for (j = 0; j < UKF_STATE_DIM * UKF_STATE_DIM; j++){
				output_logger.state_covariance[j] = (int32_t)soc_parameter_64_bit.state_covariance[j];
			}
			break;
		case SYSTEM_PARAMETER:
			battery_soc->param.observed_measurement.entries[0] = battery_soc->param.cell_current;
			battery_soc->param.observed_measurement.entries[1] = (float) sign_f(battery_soc->param.cell_current);
			if (absolute_f(battery_soc->param.cell_current) < UKF_OBSERVED_CURRENT_THRESHOLD) { //here 0.1f
				battery_soc->param.observed_measurement.entries[1] = ZERO;
			}
			battery_soc->param.eta = UKF_DISCHARGE_ETA_RATIO;
			if (battery_soc->param.cell_current < ZERO) {
				battery_soc->param.eta = UKF_CHARGE_ETA_RATIO;
			}
			battery_soc->param.RC_param = exponent_f(-absolute_f((float)UKF_SAMPLE_TIME_s / (battery_param.R1*battery_param.C1)));

		//	aukf_update_matrix_a();
			battery_soc->param.matrix_A.entries[4] = battery_soc->param.RC_param;
			battery_soc->param.matrix_A.entries[8] = battery_soc->param.H_param;

		//	aukf_update_matrix_b();
			battery_soc->param.matrix_B.entries[0] = -(battery_soc->param.eta * (float)UKF_SAMPLE_TIME_s)
					/ (soh * UKF_NOMIMAL_CAPACITY_AS);
			battery_soc->param.matrix_B.entries[2] = ONE - battery_soc->param.RC_param;
			battery_soc->param.matrix_B.entries[5] = battery_soc->param.H_param - ONE;

		//	aukf_update_matrix_c();
			float d[UKF_SIGMA_FACTOR] = {ZERO};
			for (i = 0; i < UKF_SIGMA_FACTOR; i++) {
				d[i] = get_ratio_from_soc(battery_soc->param.sigma_points.entries[i]);
			}
			battery_soc->param.matrix_C.entries[0] = (d[0]+d[1]+d[2]+d[3]+d[4]+d[5]+d[6]) / UKF_SIGMA_FACTOR;
			battery_soc->param.matrix_C.entries[1] = -battery_param.R1;

		//	aukf_update_matrix_d();
			battery_soc->param.matrix_D.entries[0] = -battery_param.R0;

			break;
		case SIGMA_MEASUREMENT:
			multiply(battery_soc->param.matrix_C, battery_soc->param.sigma_points, battery_soc->param.sigma_measurements);
			float deltaMeasurement;
			deltaMeasurement = inner_multiply(battery_soc->param.matrix_D, battery_soc->param.observed_measurement);
			for (i = 0; i < UKF_SIGMA_FACTOR; i++){
				battery_soc->param.g_update_sigma_measurement.entries[i] = deltaMeasurement;
			}
			sum(battery_soc->param.sigma_measurements, battery_soc->param.g_update_sigma_measurement, battery_soc->param.sigma_measurements);
			break;
		case PREDICT_MEASUREMENT:
			soc_parameter_64_bit.estimate_measurement = 0;
		    for (j = 0; j < UKF_SIGMA_FACTOR; j++){
		    	soc_parameter_64_bit.sigma_measurement[j] = (int64_t)(input_logger.sigma_measurements[j]);
		    }
		    for (j = 0; j < UKF_SIGMA_FACTOR; j++) {
		    	soc_parameter_64_bit.estimate_measurement += m_weight_64_bit[j]
		                * soc_parameter_64_bit.sigma_measurement[j];
		    }
		    output_logger.est_measurement = (int32_t)soc_parameter_64_bit.estimate_measurement;
			break;
		case SIGMA_MEASUREMENT_ERROR:
			for (i = 0; i < UKF_SIGMA_FACTOR; i++){
				battery_soc->param.g_update_measurement_cov.entries[i] = battery_soc->param.est_measurement;
			}
			minus(battery_soc->param.sigma_measurements, battery_soc->param.g_update_measurement_cov,
					battery_soc->param.sigma_measurement_err);
			break;
		case MEASUREMENT_COVARIANCE:
			soc_parameter_64_bit.measurement_covariance = default_measurement_covariance_64_bit;
			for (j = 0; j < UKF_SIGMA_FACTOR; j++){
				soc_parameter_64_bit.sigma_measurement_error[j] = (int64_t)input_logger.sigma_measurement_error[j];
				soc_parameter_64_bit.measurement_covariance += c_weight_64_bit[j]
						* soc_parameter_64_bit.sigma_measurement_error[j]
						* soc_parameter_64_bit.sigma_measurement_error[j];
			}
			output_logger.measurement_cov = (int32_t)((float)soc_parameter_64_bit.measurement_covariance/1000000.0f);
			break;
		case CROSS_COVARIANCE:
			for (j = 0; j < UKF_STATE_DIM * UKF_SIGMA_FACTOR; j++){
				soc_parameter_64_bit.sigma_state_error[j] = (int64_t)(input_logger.sigma_state_error[j]);
			}
			for (j = 0; j < UKF_SIGMA_FACTOR; j++){
				soc_parameter_64_bit.sigma_measurement_error[j] = (int64_t)(input_logger.sigma_measurement_error[j]);
			}
			for (j = 0; j < UKF_STATE_DIM; j++){
				soc_parameter_64_bit.cross_covariance[j] = 0;
			}
			for (j = 0; j < UKF_STATE_DIM; j++){
				for (k = 0; k < UKF_SIGMA_FACTOR; k++){
					soc_parameter_64_bit.cross_covariance[j] +=
							(int64_t) (soc_parameter_64_bit.sigma_measurement_error[j * UKF_SIGMA_FACTOR + k])
									* c_weight_64_bit[k]
									* soc_parameter_64_bit.sigma_measurement_error[k];
				}
			}
			for (j = 0; j < UKF_STATE_DIM; j++){
				output_logger.cross_covariance[j] = (int32_t)((float)soc_parameter_64_bit.cross_covariance[j]/1000.0f);
			}
			break;
		case KALMAN_GAIN:
			scalar_multiply(battery_soc->param.cross_cov, (ONE / battery_soc->param.measurement_cov),
					battery_soc->param.kalman_gain);

			break;
		case POSTERIOR_STATE:
			scalar_multiply(battery_soc->param.kalman_gain,
					(battery_soc->param.cell_voltage - battery_soc->param.est_measurement), battery_soc->param.m_update_state);
			sum(battery_soc->param.priori_est_state, battery_soc->param.m_update_state, battery_soc->param.est_state);
			break;
		case POSTERIOR_STATE_COVARIANCE:
			for (i = 0; i < UKF_STATE_DIM; i++){
				battery_soc->param.t_update_state_cov.entries[i] = battery_soc->param.kalman_gain.entries[i];
			}
			multiply(battery_soc->param.kalman_gain, battery_soc->param.t_update_state_cov,
					battery_soc->param.m_update_state_cov);
			scalar_multiply(battery_soc->param.m_update_state_cov, battery_soc->param.measurement_cov,
					battery_soc->param.m_update_state_cov);
			minus(battery_soc->param.state_cov, battery_soc->param.m_update_state_cov, battery_soc->param.state_cov);
			battery_soc->output.SOC_f = battery_soc->param.est_state.entries[0]*SOC_NORMALIZED_GAIN;

			battery_soc->output.SOC = (uint32_t) roundf(battery_soc->output.SOC_f);
		    if(battery_soc->output.SOC > BMS_SOC_UPPER_LIMIT){
		    	battery_soc->output.SOC = BMS_SOC_UPPER_LIMIT;
		    }
			break;
		default:
			break;
		}



//		synchronize_in(&input_logger, &logger);
//		normalize(&logger, &processor, NORMALIZED_TYPE);
//		synchronize_out(&processor, &output_logger);
		save_data(output_ocv_data_file, out_file, NORMALIZED_TYPE, &input_logger, &output_logger);
		show_circle(i);
	}
	return 0;
}
