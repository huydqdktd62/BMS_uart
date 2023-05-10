/*
 * normalized.c
 *
 *  Created on: May 9, 2023
 *      Author: ADMIN
 */

#include "normalize.h"

void synchronize_matrix(const int32_t* entry, Matrix* dst);
void synchronize_entry(const Matrix* matrix, int32_t* dst);

static float m_weight[UKF_SIGMA_FACTOR] = { 	-2999.0f,
									500.0f,
									500.0f,
									500.0f,
									500.0f,
									500.0f,
									500.0f };
static Matrix m_weight_matrix = { .row = UKF_SIGMA_FACTOR, .col = 1, .entries =
		&m_weight[0] };
static const float c_weight[UKF_SIGMA_FACTOR] = { 	-2999.0f - 3.0f - 0.0002f,
									500.0f,
									500.0f,
									500.0f,
									500.0f,
									500.0f,
									500.0f };

static const float default_system_covariance[UKF_STATE_DIM * UKF_STATE_DIM] = {
		0.000001f, 0.0f, 0.0f,
		0.0f, 0.00000001f, 0.0f,
		0.0f, 0.0f, 0.0002f
};

void synchronize_in(const Data_Logger* input, SOC_UKF* logger){
	synchronize_matrix(&input->estimate_state[0], &logger->param.est_state);
	int32_t i;
	for(i = 0; i < logger->param.state_cov.col * logger->param.state_cov.row; i++){
		logger->param.state_cov.entries[i] = (float)input->state_covariance[i] / 1000000000000.0f;
	}
	synchronize_matrix(&input->sigma_points[0], &logger->param.sigma_points);
	synchronize_matrix(&input->priori_estimate_state[0], &logger->param.priori_est_state);
	synchronize_matrix(&input->matrix_A[0], &logger->param.matrix_A);
	synchronize_matrix(&input->matrix_B[0], &logger->param.matrix_B);
	synchronize_matrix(&input->matrix_C[0], &logger->param.matrix_C);
	synchronize_matrix(&input->matrix_D[0], &logger->param.matrix_D);
	synchronize_matrix(&input->sigma_state_error[0], &logger->param.sigma_state_err);
	synchronize_matrix(&input->observed_measurement[0], &logger->param.observed_measurement);
	synchronize_matrix(&input->sigma_measurements[0], &logger->param.sigma_measurements);
	synchronize_matrix(&input->sigma_measurement_error[0], &logger->param.sigma_measurement_err);
	synchronize_matrix(&input->cross_covariance[0], &logger->param.cross_cov);
	synchronize_matrix(&input->aukf_kalman_gain[0], &logger->param.kalman_gain);
	logger->param.est_measurement = (float)input->est_measurement / 1000000.0f;
	logger->param.measurement_cov = (float)input->measurement_cov / 1000000.0f;
	logger->param.cell_voltage = (float)input->pack_voltage / PACK_VOLTAGE_NORMALIZED_GAIN;
	logger->param.cell_current = (float)input->pack_current / PACK_CURRENT_NORMALIZED_GAIN;

}

void synchronize_matrix(const int32_t* entry, Matrix* dst){
	int32_t i;
	for(i = 0; i < dst->col * dst->row; i++){
		dst->entries[i] = (float)entry[i] / 1000000.0f;
	}
}

int normalize(const SOC_UKF *logger, SOC_UKF *processor, const Parameter type) {
	uint8_t i, j;
	switch (type) {
	case SIGMA_POINT:
		hgenerate(logger->param.est_state, UKF_STATE_DIM,
				processor->param.g1_create_sigma_point);
		chol(logger->param.state_cov,
				processor->param.g2_create_sigma_point);
		scalar_multiply(processor->param.g2_create_sigma_point,
				UKF_GAMMA_RATIO, processor->param.g2_create_sigma_point);
		sum(processor->param.g1_create_sigma_point,
				processor->param.g2_create_sigma_point,
				processor->param.g1_create_sigma_point);
		scalar_multiply(processor->param.g2_create_sigma_point, 2,
				processor->param.g2_create_sigma_point);
		minus(processor->param.g1_create_sigma_point,
				processor->param.g2_create_sigma_point,
				processor->param.g2_create_sigma_point);
		htri_concat(logger->param.est_state,
				processor->param.g1_create_sigma_point,
				processor->param.g2_create_sigma_point,
				processor->param.sigma_points);
		break;
	case SIGMA_STATE:
		multiply(logger->param.matrix_B,
				logger->param.observed_measurement,
				processor->param.m_update_sigma_state);
		hgenerate(processor->param.m_update_sigma_state, UKF_SIGMA_FACTOR,
				processor->param.g_update_sigma_state);
		multiply(logger->param.matrix_A, logger->param.sigma_points,
				processor->param.sigma_points);
		sum(processor->param.sigma_points,
				processor->param.g_update_sigma_state,
				processor->param.sigma_points);
		break;
	case PRIOR_STATE:
		for (i = 0; i < UKF_STATE_DIM; i++) {
			processor->param.priori_est_state.entries[i] = ZERO;
		}
		for (i = 0; i < UKF_SIGMA_FACTOR; i++) {
			processor->param.priori_est_state.entries[0] += m_weight[i]
					* logger->param.sigma_points.entries[i];
		}
		for (i = 0; i < UKF_SIGMA_FACTOR; i++) {
			processor->param.priori_est_state.entries[1] += m_weight[i]
					* logger->param.sigma_points.entries[UKF_SIGMA_FACTOR
							+ i];
		}
		for (i = 0; i < UKF_SIGMA_FACTOR; i++) {
			processor->param.priori_est_state.entries[2] += m_weight[i]
					* logger->param.sigma_points.entries[2
							* UKF_SIGMA_FACTOR + i];
		}
		break;
	case SIGMA_STATE_ERROR:
		hgenerate(logger->param.priori_est_state, UKF_SIGMA_FACTOR,
				processor->param.sigma_state_err);
		minus(logger->param.sigma_points,
				processor->param.sigma_state_err,
				processor->param.sigma_state_err);
		break;
	case PRIOR_STATE_COVARIANCE:
		for (i = 0; i < (UKF_STATE_DIM * UKF_STATE_DIM); i++) {
			processor->param.state_cov.entries[i] =
					default_system_covariance[i];
		}
		for (i = 0; i < UKF_SIGMA_FACTOR; i++) {
			for (j = 0; j < UKF_STATE_DIM; j++) {
				processor->param.m1_update_state_cov.entries[j] =
						logger->param.sigma_state_err.entries[j
								* UKF_SIGMA_FACTOR + i];
				processor->param.m2_update_state_cov.entries[j] =
						logger->param.sigma_state_err.entries[j
								* UKF_SIGMA_FACTOR + i];
			}
			multiply(processor->param.m1_update_state_cov,
					processor->param.m2_update_state_cov,
					processor->param.m3_update_state_cov);
			scalar_multiply(processor->param.m3_update_state_cov, c_weight[i],
					processor->param.m3_update_state_cov);
			sum(logger->param.state_cov,
					processor->param.m3_update_state_cov,
					processor->param.state_cov);
		}
		break;
	case SYSTEM_PARAMETER:
		//	aukf_update_system_parameters();
		processor->param.observed_measurement.entries[0] =
				logger->param.cell_current;
		processor->param.observed_measurement.entries[1] = (float) sign_f(
				logger->param.cell_current);
		if (absolute_f(
				logger->param.cell_current) < UKF_OBSERVED_CURRENT_THRESHOLD) { //here 0.1f
			processor->param.observed_measurement.entries[1] = ZERO;
		}
		processor->param.eta = UKF_DISCHARGE_ETA_RATIO;
		if (logger->param.cell_current < ZERO) {
			processor->param.eta = UKF_CHARGE_ETA_RATIO;
		}
		processor->param.RC_param = exponent_f(
				-absolute_f(
						(float) UKF_SAMPLE_TIME_s
								/ (battery_param.R1 * battery_param.C1)));
		processor->param.H_param = exponent_f(
				-(absolute_f(
						(float) (processor->param.eta * UKF_CAPACITY_RATIO
								* UKF_SAMPLE_TIME_s
								* logger->param.cell_current
								/ (1.0f * UKF_NOMIMAL_CAPACITY_AS)))));

		//	aukf_update_matrix_a();
		processor->param.matrix_A.entries[4] = processor->param.RC_param;
		processor->param.matrix_A.entries[8] = processor->param.H_param;

		//	aukf_update_matrix_b();
		processor->param.matrix_B.entries[0] = -(processor->param.eta
				* (float) UKF_SAMPLE_TIME_s) / (1.0f * UKF_NOMIMAL_CAPACITY_AS);
		processor->param.matrix_B.entries[2] = ONE
				- processor->param.RC_param;
		processor->param.matrix_B.entries[5] = processor->param.H_param
				- ONE;

		//	aukf_update_matrix_c();

		//	float d = 0.0f;
		//	for (i = 0; i < SIGMA_FACTOR; i++) {
		//		d += get_ratio_from_soc(battery_soc->param.sigma_points.entries[i]);
		//	}
		//	battery_soc->param.matrix_C.entries[0] = d / SIGMA_FACTOR;
		//	battery_soc->param.matrix_C.entries[1] = -battery_param.R1;

		float d[UKF_SIGMA_FACTOR] = { ZERO };
		for (i = 0; i < UKF_SIGMA_FACTOR; i++) {
			d[i] = get_ratio_from_soc(
					logger->param.sigma_points.entries[i]);
		}
		processor->param.matrix_C.entries[0] = (d[0] + d[1] + d[2] + d[3]
				+ d[4] + d[5] + d[6]) / UKF_SIGMA_FACTOR;
		processor->param.matrix_C.entries[1] = -battery_param.R1;

		//	aukf_update_matrix_d();
		processor->param.matrix_D.entries[0] = -battery_param.R0;
		break;
	case SIGMA_MEASUREMENT:
		multiply(logger->param.matrix_C, logger->param.sigma_points,
				processor->param.sigma_measurements);
		float deltaMeasurement;
		deltaMeasurement = inner_multiply(logger->param.matrix_D,
				logger->param.observed_measurement);
		for (i = 0; i < UKF_SIGMA_FACTOR; i++) {
			processor->param.g_update_sigma_measurement.entries[i] =
					deltaMeasurement;
		}
		sum(processor->param.sigma_measurements,
				processor->param.g_update_sigma_measurement,
				processor->param.sigma_measurements);
		break;
	case PREDICT_MEASUREMENT:
		processor->param.est_measurement = inner_multiply(
				logger->param.sigma_measurements, m_weight_matrix);
		break;
	case SIGMA_MEASUREMENT_ERROR:
		for (i = 0; i < UKF_SIGMA_FACTOR; i++) {
			processor->param.g_update_measurement_cov.entries[i] =
					logger->param.est_measurement;
		}
		minus(logger->param.sigma_measurements,
				processor->param.g_update_measurement_cov,
				processor->param.sigma_measurement_err);
		break;
	case MEASUREMENT_COVARIANCE:
		processor->param.measurement_cov = UKF_DEFAULT_MEASUREMENT_COVARIANCE;
		for (i = 0; i < UKF_SIGMA_FACTOR; i++) {
			processor->param.measurement_cov += c_weight[i]
					* logger->param.sigma_measurement_err.entries[i]
					* logger->param.sigma_measurement_err.entries[i];
		}
		break;
	case CROSS_COVARIANCE:
		for (i = 0; i < UKF_STATE_DIM; i++) {
			processor->param.cross_cov.entries[i] = ZERO;
		}
		for (i = 0; i < UKF_SIGMA_FACTOR; i++) {
			for (j = 0; j < UKF_STATE_DIM; j++) {
				processor->param.m1_update_cross_cov.entries[j] =
						logger->param.sigma_state_err.entries[j
								* UKF_SIGMA_FACTOR + i];
			}
			scalar_multiply(processor->param.m1_update_cross_cov,
					(c_weight[i]
							* logger->param.sigma_measurement_err.entries[i]),
							processor->param.m1_update_cross_cov);
			sum(logger->param.cross_cov,
					processor->param.m1_update_cross_cov,
					processor->param.cross_cov);
		}
		break;
	case KALMAN_GAIN:
		scalar_multiply(logger->param.cross_cov,
				(ONE / logger->param.measurement_cov),
				processor->param.kalman_gain);
		break;
	case POSTERIOR_STATE:
		scalar_multiply(logger->param.kalman_gain,
				(logger->param.cell_voltage
						- logger->param.est_measurement),
						processor->param.m_update_state);
		sum(logger->param.priori_est_state,
				processor->param.m_update_state,
				processor->param.est_state);
		break;
	case POSTERIOR_STATE_COVARIANCE:
		for (i = 0; i < UKF_STATE_DIM; i++) {
			processor->param.t_update_state_cov.entries[i] =
					logger->param.kalman_gain.entries[i];
		}
		multiply(logger->param.kalman_gain,
				processor->param.t_update_state_cov,
				processor->param.m_update_state_cov);
		scalar_multiply(processor->param.m_update_state_cov,
				logger->param.measurement_cov,
				processor->param.m_update_state_cov);
		minus(logger->param.state_cov,
				processor->param.m_update_state_cov,
				processor->param.state_cov);
		break;
	default:
		return -1;
	}
	return 0;
}

void synchronize_out(const SOC_UKF* processor, Data_Logger* output){
	synchronize_entry(&processor->param.est_state, &output->estimate_state[0]);
	synchronize_entry(&processor->param.state_cov, &output->state_covariance[0]);
	synchronize_entry(&processor->param.sigma_points, &output->sigma_points[0]);
	synchronize_entry(&processor->param.priori_est_state, &output->priori_estimate_state[0]);
	synchronize_entry(&processor->param.matrix_A, &output->matrix_A[0]);
	synchronize_entry(&processor->param.matrix_B, &output->matrix_B[0]);
	synchronize_entry(&processor->param.matrix_C, &output->matrix_C[0]);
	synchronize_entry(&processor->param.matrix_D, &output->matrix_D[0]);
	synchronize_entry(&processor->param.sigma_state_err, &output->sigma_state_error[0]);
	synchronize_entry(&processor->param.observed_measurement, &output->observed_measurement[0]);
	synchronize_entry(&processor->param.sigma_measurements, &output->sigma_measurements[0]);
	synchronize_entry(&processor->param.sigma_measurement_err, &output->sigma_measurement_error[0]);
	synchronize_entry(&processor->param.cross_cov, &output->cross_covariance[0]);
	synchronize_entry(&processor->param.kalman_gain, &output->aukf_kalman_gain[0]);
	output->est_measurement = (int32_t)(processor->param.est_measurement * 1000000.0f);
	output->measurement_cov = (int32_t)(processor->param.measurement_cov * 1000000.0f);
	output->pack_voltage = (int32_t)(processor->param.cell_voltage * PACK_VOLTAGE_NORMALIZED_GAIN);
	output->pack_current = (int32_t)(processor->param.cell_current * PACK_CURRENT_NORMALIZED_GAIN);
}

void synchronize_entry(const Matrix* matrix, int32_t* dst){
	int32_t i;
	for(i = 0; i < matrix->col * matrix->row; i++){
		matrix->entries[i] = (int32_t)(dst[i] * 1000000.0f);
	}
}
