/*
 * soc-aukf_app.c
 *
 *  Created on: Sep 6, 2022
 *      Author: ADMIN
 */

#if 0
#include "soc_aukf_app.h"
#include "bms.h"
#include "afe_init.h"
#include "component/exflash/exflash.h"

#define CELL_SERIAL                             (16)
#define CELL_CHARGE_VOLTAGE_LOWER_LIMIT         (4180)
#define CELL_CHARGE_VOLTAGE_UPPER_LIMIT         (4200)
#define CELL_DISCHARGE_VOLTAGE_LIMIT            (2500)
#define PACK_CHARGE_VOLTAGE_LOWER_LIMIT         (CELL_SERIAL*CELL_CHARGE_VOLTAGE_LOWER_LIMIT)
#define PACK_CHARGE_VOLTAGE_UPPER_LIMIT         (CELL_SERIAL*CELL_CHARGE_VOLTAGE_UPPER_LIMIT)
#define PACK_DISCHARGE_VOLTAGE_LIMIT            (CELL_SERIAL*CELL_DISCHARGE_VOLTAGE_LIMIT)
#define PACK_CHARGE_CURRENT_UPPER_LIMIT         (180)
#define PACK_CHARGE_CURRENT_LOWER_LIMIT         (80)
#define PACK_REST_CURRENT_UPPER_LIMIT           (25)
#define PACK_REST_CURRENT_LOWER_LIMIT           (-25)
#define PACK_UPDATE_CURRENT_UPPER_LIMIT         (30)
#define PACK_UPDATE_CURRENT_LOWER_LIMIT         (-30)
#define SOC_UPPER_LIMIT                         (99.75f)

#define ALGORITHM_LOOPS_PER_MINUTE              (85)
#define ALGORITHM_REST_AFTER_MINUTES            (15)

#define SOH_UPDATE_LOWER_BOUND                  (70)

#define SOC_UPDATED                             (0b10111111)

#define CURRENT_DECAY_RATIO                     (1.0308f)

#define DEFAULT_SOH                             (100)

static VFFRLS_Input_Vector vffrls_input;
static VFFRLS_Output_Vector vffrls_output;
static VFFRLS_State_Vector vffrls_est;
static AUKF_Input_Vector aukf_input;
static AUKF_Output_Vector aukf_output;
static AUKF_State_Vector aukf_est;

static float soh;

static volatile uint8_t aukf_update_flag;
static volatile uint8_t soc_reset_flag;
static volatile uint8_t soc_update_flag;
static volatile uint8_t vffrls_update_flag;
static volatile uint8_t manual_update_flag;
static volatile uint8_t soh_updated_flag;

//static volatile uint32_t soc_update_counter;
static volatile uint32_t vffrls_update_counter;
static volatile uint32_t manual_update_counter;
static volatile uint32_t restCounterSecond;
static volatile uint32_t restCounterMinute;
static volatile uint32_t soh_update_counter;

static void bms_init_parameters_impl(BMS *p_bms);
static void bms_init_vffrls_impl(BMS *p_bms);
static void bms_init_aukf_impl(BMS *p_bms);

static void bms_reset_impl(BMS *p_bms);

static void bms_update_vffrls_impl(BMS *p_bms);
static void bms_update_aukf_impl(BMS *p_bms);
static void bms_update_soh_impl(BMS *p_bms);
static void bms_update_manual_impl(BMS *p_bms);
static void charge_boundary(BMS *p_bms);
static void full_charge_boundary(BMS *p_bms);
static void stop_charge_boundary(BMS *p_bms);

static void manual_update_timer(BMS *p_bms);
static void algorithm_update_timer(BMS *p_bms);
static void soh_update_timer(BMS *p_bms);
static void aukf_check_input(BMS *p_bms);

void bms_init_soc_aukf(BMS *p_bms)
{
    if (p_bms->afe->cell_array->cells[15].voltage > CELL_CHARGE_VOLTAGE_UPPER_LIMIT
            || p_bms->afe->cell_array->cells[15].voltage < CELL_DISCHARGE_VOLTAGE_LIMIT)
    {
        bms_update_cell_voltages (p_bms);
        bms_update_pack_voltage (p_bms);
        bms_update_current (p_bms);
//        bms_init_soc_aukf (p_bms);
    }
    bms_init_parameters_impl (p_bms);
    bms_init_vffrls_impl (p_bms);
    bms_init_aukf_impl (p_bms);

    soh = (float)p_bms->soh/100.0f;
}

static void bms_init_parameters_impl(BMS *p_bms)
{
    soc_update_flag = 1;
    aukf_update_flag = 1;
    vffrls_update_flag = 1;
    manual_update_flag = 0;
    soc_reset_flag = 0;

//    soc_update_counter = 0;
    vffrls_update_counter = 0;
    manual_update_counter = 0;

    restCounterSecond = 0;
    restCounterMinute = 0;

    soh_update_counter = 0;

    p_bms->vffrls_est = &vffrls_est;
    p_bms->aukf_est = &aukf_est;
    p_bms->vffrls_est->p_input = &vffrls_input;
    p_bms->vffrls_est->p_output = &vffrls_output;
    p_bms->aukf_est->p_input = &aukf_input;
    p_bms->aukf_est->p_output = &aukf_output;

    soh = 1.0f;
    if (p_bms->soh != 0)
    {
        soh = (float) p_bms->soh / 100.0f;
    }
    soh_updated_flag = 1;

    p_bms->aukf_error.AUKF_State = 0;
    p_bms->aukf_error.prior_Voltage = p_bms->afe->cell_array->cells[15].voltage;
    p_bms->aukf_error.prior_Current = p_bms->afe->current;
}

static void bms_init_vffrls_impl(BMS *p_bms)
{
    vffrls_update_input (p_bms->vffrls_est,
                         (float) p_bms->afe->cell_array->cells[0].voltage / CELL_VOLTAGE_NORMALIZED,
                         (CURRENT_DECAY_RATIO * (float) p_bms->current / CURRENT_NORMALIZED), 0.0f);
    vffrls_init (p_bms->vffrls_est);
}

static void bms_init_aukf_impl(BMS *p_bms)
{
    aukf_update_input (p_bms->aukf_est, vffrls_get_output (p_bms->vffrls_est),
                       (float) p_bms->afe->cell_array->cells[0].voltage / CELL_VOLTAGE_NORMALIZED,
                       (CURRENT_DECAY_RATIO * (float) p_bms->current / CURRENT_NORMALIZED), 1.0);
    aukf_init (p_bms->aukf_est);
}

void bms_soc_aukf_update_timer(BMS *p_bms)
{
    p_bms->aukf_error.AUKF_State = 0b00000000;
    aukf_check_input (p_bms);
    p_bms->aukf_error.AUKF_State = ALGORITHM_REST;
    if (p_bms->afe->pack_voltage >= PACK_CHARGE_VOLTAGE_LOWER_LIMIT
            && p_bms->afe->pack_voltage <= PACK_CHARGE_VOLTAGE_UPPER_LIMIT)
    {
        if (p_bms->afe->current <= PACK_CHARGE_CURRENT_UPPER_LIMIT
                && p_bms->afe->current > PACK_CHARGE_CURRENT_LOWER_LIMIT)
        {
            manual_update_timer (p_bms);
        }
    }
    algorithm_update_timer (p_bms);
    soh_update_timer (p_bms);
}

static void manual_update_timer(BMS *p_bms)
{
    if (p_bms->aukf_est->estimateState.entries[0] < SOC_UPPER_LIMIT)
    {
        if (soh_updated_flag == 0)
        {
            soh_update_counter++;
            soh_updated_flag = 1;
        }
        manual_update_flag = 1;
    }
    if (manual_update_flag == 1)
    {
        manual_update_counter++;
    }
}

static void algorithm_update_timer(BMS *p_bms)
{
    if (p_bms->afe->current <= PACK_REST_CURRENT_UPPER_LIMIT &&
            p_bms->afe->current >= PACK_REST_CURRENT_LOWER_LIMIT)
    {
        restCounterSecond++;
    }
    if (p_bms->afe->current >= PACK_UPDATE_CURRENT_UPPER_LIMIT ||
            p_bms->afe->current <= PACK_UPDATE_CURRENT_LOWER_LIMIT)
    {
        if (soc_update_flag == 0)
        {
            soc_reset_flag = 1;
        }
        soc_update_flag = 1;
        restCounterSecond = 0;
        restCounterMinute = 0;
    }

    if (restCounterSecond == ALGORITHM_LOOPS_PER_MINUTE)
    {
        restCounterMinute++;
        restCounterSecond = 0;
    }

    if (restCounterMinute == ALGORITHM_REST_AFTER_MINUTES)
    {
    	if(soc_update_flag!=0){
            soc_reset_flag = 1;
            soc_update_flag = 0;
    	}
        restCounterMinute = 0;
    }

    if (soc_update_flag == 1)
    {
        if (manual_update_flag != 1)
        {
            aukf_update_flag = 1;
        }
        vffrls_update_flag = 1;
    }

}

static void soh_update_timer(BMS *p_bms)
{
    if (soh_updated_flag != 0 && p_bms->soc < SOH_UPDATE_LOWER_BOUND)
    {
        soh_updated_flag = 0;
    }
}

void bms_soc_aukf_process(BMS *p_bms)
{
    if (soh_update_counter != 0)
    {
        bms_update_soh_impl (p_bms);
        p_bms->aukf_error.AUKF_State = p_bms->aukf_error.AUKF_State | SOH_UPDATE;
        soh_update_counter = 0;
    }
    if (soc_reset_flag != 0)
    {
        bms_reset_impl (p_bms);
        p_bms->aukf_error.AUKF_State = p_bms->aukf_error.AUKF_State | ALGORITHM_RESET;
        soc_reset_flag = 0;
    }
    if (p_bms->afe->pack_voltage != 0)
    {
        if (aukf_update_flag != 0)
        {
            bms_update_aukf_impl (p_bms);
            p_bms->aukf_error.AUKF_State = p_bms->aukf_error.AUKF_State | AUKF_UPDATED;
            p_bms->soc = (uint32_t) p_bms->aukf_est->p_output->SOC;
            p_bms->aukf_error.AUKF_State = p_bms->aukf_error.AUKF_State & SOC_UPDATED;
            aukf_update_flag = 0;
        }
        if (vffrls_update_flag != 0)
        {
            bms_update_vffrls_impl (p_bms);
            p_bms->aukf_error.AUKF_State = p_bms->aukf_error.AUKF_State | VFFRLS_UPDATED;
            vffrls_update_flag = 0;
        }
        if (manual_update_counter != 0)
        {
            charge_boundary (p_bms);
            bms_update_manual_impl (p_bms);
            p_bms->aukf_error.AUKF_State = p_bms->aukf_error.AUKF_State | MANUAL_UPDATED;
            p_bms->soc = (uint32_t) p_bms->aukf_est->p_output->SOC;
            p_bms->aukf_error.AUKF_State = p_bms->aukf_error.AUKF_State & SOC_UPDATED;
            manual_update_counter = 0;
        }
    }
}

static void bms_update_soh_impl(BMS *p_bms)
{
    float decay = 0.99f;
    if (p_bms->aukf_est->estimateState.entries[0] >= 99.0f)
    {
        decay = p_bms->aukf_est->estimateState.entries[0] / 100.0f;
    }
    soh = soh * decay;
    p_bms->soh = (uint32_t) (soh * 100.0f);
}

static void bms_reset_impl(BMS *p_bms)
{
    p_bms->aukf_est->estimateState.entries[0] = get_soc_from_ocv((float)p_bms->afe->pack_voltage/PACK_VOLTAGE_NORMALIZED);
    p_bms->soc = (uint32_t) roundf(p_bms->aukf_est->estimateState.entries[0]);
    p_bms->aukf_est->estimateState.entries[2] = 0.0f;
    p_bms->aukf_est->p_output->hysteresisVoltage = 0.0f;
    p_bms->vffrls_est->p_input->hysteresis = 0.0f;
    vffrls_parameters_init (p_bms->vffrls_est);
}

static void bms_update_vffrls_impl(BMS *p_bms)
{
    vffrls_update_input (p_bms->vffrls_est,
                         p_bms->aukf_est->p_input->terminalVoltage,
                         p_bms->aukf_est->p_input->current,
                         p_bms->aukf_est->p_output->hysteresisVoltage);
    vffrls_update (p_bms->vffrls_est);
}

static void bms_update_aukf_impl(BMS *p_bms)
{
    aukf_update_input (p_bms->aukf_est, vffrls_get_output (p_bms->vffrls_est),
                       (float) (p_bms->afe->pack_voltage) / PACK_VOLTAGE_NORMALIZED,
                       (CURRENT_DECAY_RATIO * (float) p_bms->afe->current) / CURRENT_NORMALIZED, soh);
    aukf_update (p_bms->aukf_est);
}

static void aukf_check_input(BMS *p_bms)
{
    p_bms->aukf_error.prior_Voltage = p_bms->aukf_error.input_Voltage;
    p_bms->aukf_error.prior_Current = p_bms->aukf_error.input_Current;
    p_bms->aukf_error.input_Voltage = p_bms->afe->pack_voltage;
    p_bms->aukf_error.input_Current = p_bms->afe->current;
    p_bms->aukf_error.voltage_Error_flag = 0;
    if ((int32_t) (p_bms->aukf_error.input_Voltage - p_bms->aukf_error.prior_Voltage) > 1000
            || (int32_t) (p_bms->aukf_error.input_Voltage - p_bms->aukf_error.prior_Voltage) < -1000)
    {
        p_bms->aukf_error.voltage_Error_flag = 1;
        p_bms->aukf_error.input_Voltage = p_bms->aukf_error.prior_Voltage;
        p_bms->aukf_error.input_Current = p_bms->aukf_error.prior_Current;
    }
    if (p_bms->afe->cell_array->cells[15].voltage < CELL_DISCHARGE_VOLTAGE_LIMIT
            || p_bms->afe->cell_array->cells[15].voltage > CELL_CHARGE_VOLTAGE_UPPER_LIMIT)
    {
        p_bms->aukf_error.AUKF_State = p_bms->aukf_error.AUKF_State | VOLTAGE_ERROR;
    }
//    if (p_bms->aukf_error.Input_Current - p_bms->aukf_error.Prior_Current > 10
//            || p_bms->aukf_error.Input_Current - p_bms->aukf_error.Prior_Current < -10)
//    {
//        p_bms->aukf_error.AUKF_State = p_bms->aukf_error.AUKF_State | CURRENT_ERROR;
//    }
}

static void charge_boundary(BMS *p_bms)
{
    full_charge_boundary (p_bms);
    stop_charge_boundary (p_bms);
}

static void full_charge_boundary(BMS *p_bms)
{
    if (manual_update_flag == 1)
    {
        if (p_bms->aukf_est->estimateState.entries[0] > SOC_UPPER_LIMIT)
        {
            manual_update_flag = 0;
        }
    }
}

static void stop_charge_boundary(BMS *p_bms)
{
    if (manual_update_flag == 1)
    {
        if ((p_bms->afe->pack_voltage) < PACK_CHARGE_VOLTAGE_LOWER_LIMIT
                || p_bms->afe->current > PACK_CHARGE_CURRENT_UPPER_LIMIT
                || p_bms->afe->current <= PACK_CHARGE_CURRENT_LOWER_LIMIT)
        {
            manual_update_flag = 0;
        }
    }
}

static void bms_update_manual_impl(BMS *p_bms)
{
    if (p_bms->aukf_est->estimateState.entries[0] >= 99.0f)
    {
        p_bms->aukf_est->estimateState.entries[0] += 0.003f;
    }
    else if (p_bms->aukf_est->estimateState.entries[0] >= 98.0f)
    {
        p_bms->aukf_est->estimateState.entries[0] += 0.004f;
    }
    else if (p_bms->aukf_est->estimateState.entries[0] >= 97.0f)
    {
        p_bms->aukf_est->estimateState.entries[0] += 0.006f;
    }
    else if (p_bms->aukf_est->estimateState.entries[0] >= 96.0f)
    {
        p_bms->aukf_est->estimateState.entries[0] += 0.009f;
    }
    else
    {
        p_bms->aukf_est->estimateState.entries[0] += 0.0012f;
    }

    p_bms->aukf_est->p_output->SOC = (uint32_t) roundf(p_bms->aukf_est->estimateState.entries[0]);
}

#endif
