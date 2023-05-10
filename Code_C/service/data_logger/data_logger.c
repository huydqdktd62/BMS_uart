/*
 * data_logger.c
 *
 *  Created on: May 9, 2023
 *      Author: ADMIN
 */

#include "data_logger.h"
int test_circle;
Data_Logger input;
Data_Logger output;

int32_t create_data(const char *file_path, FILE *out_file, const Parameter type) {
	int i;
	out_file = fopen(file_path, "w"); // write only

	if (out_file == NULL) {
		printf("Error! Could not write to file\n");
		return -1;
	}
	fprintf(out_file, "test_circle,");

	switch(type){
	case SIGMA_POINT:
		for (i = 1; i < 4; i++){
			fprintf(out_file, "state %d,", i);
		}
		for (i = 1; i < 10; i++){
			fprintf(out_file, "state_covariance %d,", i);
		}
		for (i = 1; i < 22; i++){
			fprintf(out_file, "sigma_point %d,", i);
		}
		break;
	case SIGMA_STATE:
		for (i = 1; i < 10; i++){
			fprintf(out_file, "A %d,", i);
		}
		for (i = 1; i < 22; i++){
			fprintf(out_file, "sigma_point %d,", i);
		}
		for (i = 1; i < 7; i++){
			fprintf(out_file, "B %d,", i);
		}
		for (i = 1; i < 3; i++){
			fprintf(out_file, "observer %d,", i);
		}
		for (i = 1; i < 22; i++){
			fprintf(out_file, "sigma_state %d,", i);
		}
		break;
	case PRIOR_STATE:
		for (i = 1; i < 22; i++){
			fprintf(out_file, "sigma_state %d,", i);
		}
		for (i = 1; i < 4; i++){
			fprintf(out_file, "proir_state %d,", i);
		}
		break;
	case SIGMA_STATE_ERROR:
		for (i = 1; i < 22; i++){
			fprintf(out_file, "sigma_state %d,", i);
		}
		for (i = 1; i < 4; i++){
			fprintf(out_file, "proir_state %d,", i);
		}
		for (i = 1; i < 22; i++){
			fprintf(out_file, "sigma_state_error %d,", i);
		}
		break;
	case PRIOR_STATE_COVARIANCE:
		for (i = 1; i < 10; i++){
			fprintf(out_file, "state_covariance %d,", i);
		}
		for (i = 1; i < 22; i++){
			fprintf(out_file, "sigma_state_error %d,", i);
		}
		for (i = 1; i < 10; i++){
			fprintf(out_file, "prior_state_covariance %d,", i);
		}
		break;
	case SYSTEM_PARAMETER:
		for (i = 1; i < 3; i++){
			fprintf(out_file, "observer %d,", i);
		}
		for (i = 1; i < 8; i++){
			fprintf(out_file, "est_soc %d,", i);
		}
		for (i = 1; i < 10; i++){
			fprintf(out_file, "A %d,", i);
		}
		for (i = 1; i < 7; i++){
			fprintf(out_file, "B %d,", i);
		}
		for (i = 1; i < 4; i++){
			fprintf(out_file, "C %d,", i);
		}
		for (i = 1; i < 3; i++){
			fprintf(out_file, "D %d,", i);
		}
		break;
	case SIGMA_MEASUREMENT:
		for (i = 1; i < 4; i++){
			fprintf(out_file, "C %d,", i);
		}
		for (i = 1; i < 10; i++){
			fprintf(out_file, "prior_state_covariance %d,", i);
		}
		for (i = 1; i < 3; i++){
			fprintf(out_file, "D %d,", i);
		}
		for (i = 1; i < 3; i++){
			fprintf(out_file, "observer %d,", i);
		}
		for (i = 1; i < 8; i++){
			fprintf(out_file, "sigma_measurement %d,", i);
		}
		break;
	case PREDICT_MEASUREMENT:
		for (i = 1; i < 8; i++){
			fprintf(out_file, "sigma_measurement %d,", i);
		}
		fprintf(out_file, "predict_measurement,");
		break;
	case SIGMA_MEASUREMENT_ERROR:
		for (i = 1; i < 8; i++){
			fprintf(out_file, "sigma_measurement %d,", i);
		}
		fprintf(out_file, "predict_measurement,");
		for (i = 1; i < 8; i++){
			fprintf(out_file, "sigma_measurement_error %d,", i);
		}
		break;
	case MEASUREMENT_COVARIANCE:
		for (i = 1; i < 8; i++){
			fprintf(out_file, "sigma_measurement_error %d,", i);
		}
		fprintf(out_file, "measurement_covariance,");
		break;
	case CROSS_COVARIANCE:
		for (i = 1; i < 22; i++){
			fprintf(out_file, "sigma_state_error %d,", i);
		}
		for (i = 1; i < 8; i++){
			fprintf(out_file, "sigma_measurement_error %d,", i);
		}
		for (i = 1; i < 4; i++){
			fprintf(out_file, "cross_covariance %d,", i);
		}
		break;
	case KALMAN_GAIN:
		for (i = 1; i < 4; i++){
			fprintf(out_file, "cross_covariance %d,", i);
		}
		fprintf(out_file, "measurement_covariance,");
		for (i = 1; i < 4; i++){
			fprintf(out_file, "kalman_gain %d,", i);
		}
		break;
	case POSTERIOR_STATE:
		for (i = 1; i < 4; i++){
			fprintf(out_file, "proir_state %d,", i);
		}
		for (i = 1; i < 4; i++){
			fprintf(out_file, "kalman_gain %d,", i);
		}
		fprintf(out_file, "U,");
		fprintf(out_file, "predict_measurement,");
		for (i = 1; i < 4; i++){
			fprintf(out_file, "posterior_state %d,", i);
		}
		break;
	case POSTERIOR_STATE_COVARIANCE:
		for (i = 1; i < 10; i++){
			fprintf(out_file, "prior_state_covariance %d,", i);
		}
		for (i = 1; i < 4; i++){
			fprintf(out_file, "kalman_gain %d,", i);
		}
		fprintf(out_file, "measurement_covariance,");
		for (i = 1; i < 10; i++){
			fprintf(out_file, "posterior_state_covariance %d,", i);
		}
		break;
	default:
		fprintf(out_file, "\n");
		fclose(out_file);
		return -1;
	}

	fprintf(out_file, "\n");
	fclose(out_file);
	return 0;
}

int32_t save_data(const char *file_path, FILE *out_file, const Parameter type,
		const Data_Logger* logger, const Data_Logger* processor) {
	int i;
	out_file = fopen(file_path, "a");
	test_circle++;

	fprintf(out_file, "%i,", test_circle);

	switch(type){
	case SIGMA_POINT:
		for (i = 0; i < 3; i++){
			fprintf(out_file, "%d,", input.estimate_state[i]);
		}
		for (i = 0; i < 9; i++){
			fprintf(out_file, "%d,", input.state_covariance[i]);
		}
		for (i = 0; i < 21; i++){
			fprintf(out_file, "%d,", output.sigma_points[i]);
		}
		break;
	case SIGMA_STATE:
		for (i = 0; i < 9; i++){
			fprintf(out_file, "%d,", input.matrix_A[i]);
		}
		for (i = 0; i < 21; i++){
			fprintf(out_file, "%d,", input.sigma_points[i]);
		}
		for (i = 0; i < 6; i++){
			fprintf(out_file, "%d,", input.matrix_B[i]);
		}
		for (i = 0; i < 2; i++){
			fprintf(out_file, "%d,", input.observed_measurement[i]);
		}
		for (i = 0; i < 21; i++){
			fprintf(out_file, "%d,", output.sigma_points[i]);
		}
		break;
	case PRIOR_STATE:
		for (i = 0; i < 21; i++){
			fprintf(out_file, "%d,", input.sigma_points[i]);
		}
		for (i = 0; i < 3; i++){
			fprintf(out_file, "%d,", output.priori_estimate_state[i]);
		}
		break;
	case SIGMA_STATE_ERROR:
		for (i = 0; i < 21; i++){
			fprintf(out_file, "%d,", input.sigma_points[i]);
		}
		for (i = 0; i < 3; i++){
			fprintf(out_file, "%d,", input.priori_estimate_state[i]);
		}
		for (i = 0; i < 21; i++){
			fprintf(out_file, "%d,", output.sigma_state_error[i]);
		}
		break;
	case PRIOR_STATE_COVARIANCE:
		for (i = 0; i < 9; i++){
			fprintf(out_file, "%d,", input.state_covariance[i]);
		}
		for (i = 0; i < 21; i++){
			fprintf(out_file, "%d,", input.sigma_state_error[i]);
		}
		for (i = 0; i < 9; i++){
			fprintf(out_file, "%d,", output.state_covariance[i]);
		}
		break;
	case SYSTEM_PARAMETER:
		for (i = 0; i < 2; i++){
			fprintf(out_file, "%d,", input.observed_measurement[i]);
		}
		for (i = 0; i < 7; i++){
			fprintf(out_file, "%d,", input.sigma_points[i]);
		}
		for (i = 0; i < 9; i++){
			fprintf(out_file, "%d,", output.matrix_A[i]);
		}
		for (i = 0; i < 6; i++){
			fprintf(out_file, "%d,", output.matrix_B[i]);
		}
		for (i = 0; i < 3; i++){
			fprintf(out_file, "%d,", output.matrix_C[i]);
		}
		for (i = 0; i < 2; i++){
			fprintf(out_file, "%d,", output.matrix_D[i]);
		}
		break;
	case SIGMA_MEASUREMENT:
		for (i = 0; i < 3; i++){
			fprintf(out_file, "%d,", input.matrix_C[i]);
		}
		for (i = 0; i < 9; i++){
			fprintf(out_file, "%d,", input.state_covariance[i]);
		}
		for (i = 0; i < 2; i++){
			fprintf(out_file, "%d,", input.matrix_D[i]);
		}
		for (i = 0; i < 2; i++){
			fprintf(out_file, "%d,", input.observed_measurement[i]);
		}
		for (i = 0; i < 7; i++){
			fprintf(out_file, "%d,", output.sigma_measurements[i]);
		}
		break;
	case PREDICT_MEASUREMENT:
		for (i = 0; i < 7; i++){
			fprintf(out_file, "%d,", input.sigma_measurements[i]);
		}
		fprintf(out_file, "%d,", output.est_measurement);
		break;
	case SIGMA_MEASUREMENT_ERROR:
		for (i = 0; i < 7; i++){
			fprintf(out_file, "%d,", input.sigma_measurements[i]);
		}
		fprintf(out_file, "%d,", input.est_measurement);
		for (i = 0; i < 7; i++){
			fprintf(out_file, "%d,", output.sigma_measurement_error[i]);
		}
		break;
	case MEASUREMENT_COVARIANCE:
		for (i = 0; i < 7; i++){
			fprintf(out_file, "%d,", input.sigma_measurement_error[i]);
		}
		fprintf(out_file, "%d,", output.measurement_cov);
		break;
	case CROSS_COVARIANCE:
		for (i = 0; i < 21; i++){
			fprintf(out_file, "%d,", input.sigma_state_error[i]);
		}
		for (i = 0; i < 7; i++){
			fprintf(out_file, "%d,", input.sigma_measurement_error[i]);
		}
		for (i = 0; i < 3; i++){
			fprintf(out_file, "%d,", output.cross_covariance[i]);
		}
		break;
	case KALMAN_GAIN:
		for (i = 0; i < 3; i++){
			fprintf(out_file, "%d,", input.cross_covariance[i]);
		}
		fprintf(out_file, "%d,", input.measurement_cov);
		for (i = 0; i < 3; i++){
			fprintf(out_file, "%d,", output.aukf_kalman_gain[i]);
		}
		break;
	case POSTERIOR_STATE:
		for (i = 0; i < 3; i++){
			fprintf(out_file, "%d,", input.priori_estimate_state[i]);
		}
		for (i = 0; i < 3; i++){
			fprintf(out_file, "%d,", input.aukf_kalman_gain[i]);
		}
		fprintf(out_file, "%d,", input.pack_voltage);
		fprintf(out_file, "%d,", input.est_measurement);
		for (i = 0; i < 3; i++){
			fprintf(out_file, "%d,", output.estimate_state[i]);
		}
		break;
	case POSTERIOR_STATE_COVARIANCE:
		for (i = 0; i < 9; i++){
			fprintf(out_file, "%d,", input.cross_covariance[i]);
		}
		for (i = 0; i < 3; i++){
			fprintf(out_file, "%d,", input.aukf_kalman_gain[i]);
		}
		fprintf(out_file, "%d,", input.measurement_cov);
		for (i = 0; i < 9; i++){
			fprintf(out_file, "%d,", output.state_covariance[i]);
		}
		break;
	default:
		fprintf(out_file, "\n");
		fclose(out_file);
		return -1;
	}

	fprintf(out_file, "\n");
	fclose(out_file);
	return 0;
}


