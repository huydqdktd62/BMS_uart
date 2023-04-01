/*
 * app_co_manu_od.c
 *
 *  Created on: Jul 15, 2022
 *      Author: Dakaka
 *
 */

#include "manu_od.h"

#include "app_acc_profile.h"

/*******************************************************************************************
 * Define all application/manufactor-specific profile initializer sub-object
 * *******************************************************************************************/

/*--- Object 0x5000: Basic electric specifications ----------------------------*/
CO_Sub_Object obj_5000_basic_elec_specs[] = {
		{(void*)&od_temp_manu_profile_para.x500200_rated_volt_mV, ODA_SDO_R, 4, NULL, NULL},
		{(void*)&od_temp_manu_profile_para.x500201_rated_curr_mA, ODA_SDO_R, 4, NULL, NULL},
};

/*--- Object 0x5001: Basic CANopen node parameters ----------------------------*/
CO_Sub_Object obj_5001_basic_co_node_para[] = {
		{(void*)&od_temp_manu_profile_para.x500300_node_ID, ODA_SDO_RW|ODA_VWTE_after_reset, 1, NULL, (void*)&od_temp_manu_profile_para.x500300_node_ID},
		{(void*)&od_temp_manu_profile_para.x500301_baud_rate_kHz, ODA_SDO_RW|ODA_VWTE_after_reset, 2, NULL, (void*)&od_temp_manu_profile_para.x500301_baud_rate_kHz},
};

/* USER CODE 0 begin */

/* 0x2100 ARRAY Anti-theft -------------------------------------------------------------------------*/
/* 0x2110 Maintenance ------------------------------------------------------------------------------*/
CO_Sub_Object obj_2110_maintnc[]={
		{(void*)&mc.maintnc.throttle_cm_pu_1B, ODA_TPDO|ODA_SDO_R, 1, NULL, NULL},
		{(void*)&mc.maintnc.speed_rpm_2B, ODA_TPDO|ODA_SDO_R, 2, NULL, NULL},
		{(void*)&mc.error_code_sending, ODA_TPDO|ODA_SDO_R, 4, NULL, NULL}
};

/* 0x2120 Throttle ---------------------------------------------------------------------------------*/
CO_Sub_Object obj_2120_throttle[]={
		{(void*)&mc.throttle.min_voltage, ODA_TPDO, 4, NULL, NULL},
		{(void*)&mc.throttle.throttle_setpoint, ODA_TPDO, 4, NULL, NULL}
};

/* 0x2130 Acceleration profile mode ---------------------------------------------------------------------------------*/
static uint8_t shadow_mode_name;
static CO_Sub_Object_Ext_Confirm_Func_t confirm_func_0x213000_ready_update_acc_mode_impl(void)
{
	if ((shadow_mode_name >= AP_LIMIT_NUMBER_MODE) || (shadow_mode_name < 0))
	{
		return CO_EXT_CONFIRM_abort;
	}

	ap_limit_set_mode(&limit_acc_profile, shadow_mode_name);
	return CO_EXT_CONFIRM_success;
}

CO_Sub_Object_Ext_t sub_obj_ext_0x213000 =
{
        .p_shadow_data = &shadow_mode_name,
        .confirm_func = confirm_func_0x213000_ready_update_acc_mode_impl
};

CO_Sub_Object obj_2130_acc_profile_mode[]={
		{(void*)&limit_acc_profile.cur_mode.name, ODA_SDO_RW, 1, &sub_obj_ext_0x213000, NULL}
};

/* 0x2140 Limit/maximum speed ---------------------------------------------------------------------------------*/
static uint32_t shadow_data_0x214000;
#define COMMAND_0x214000_MAX_SPEED				(255U)

static CO_Sub_Object_Ext_Confirm_Func_t confirm_func_0x214000_ready_update_maximum_speed_impl(void)
{
    float ap_limit_speed_radps = ap_limit_get_max_speed_radps(&limit_acc_profile);
    if(shadow_data_0x214000 == COMMAND_0x214000_MAX_SPEED)
    {
    	od_cm__set_speed_limit_radps(&mc.od_cm, OD_CM__SPEED_LIMIT_RADPS__MAX);
        return CO_EXT_CONFIRM_success;
    }
    if (shadow_data_0x214000 >= OD_CM__SPEED_LIMIT_RADPS__MIN
			&& shadow_data_0x214000 <= ap_limit_speed_radps)
	{
    	od_cm__set_speed_limit_radps(&mc.od_cm, shadow_data_0x214000);
        return CO_EXT_CONFIRM_success;
    }
    return CO_EXT_CONFIRM_abort;
}
CO_Sub_Object_Ext_t sub_obj_ext_0x214000 =
{
        .p_shadow_data = (uint8_t*)&shadow_data_0x214000,
        .confirm_func = confirm_func_0x214000_ready_update_maximum_speed_impl
};
CO_Sub_Object obj_2140_maximum_speed[]={
		{(void*)&mc.speed_limit_radps, ODA_SDO_RW, 4, &sub_obj_ext_0x214000, NULL, NULL}
};




/* 0x3FF7 sub_test_mapping_tpdo --------------------------------------------------------------------*/
test_t od_data_test_mapping_tpdo =
{
		.test1 = 0x11,
		.test2 = 0x2222,
		.test3 = 0x33333333
};
CO_Sub_Object obj_3FF7_test_mapping_tpdo[]={
		{(void*)&od_data_test_mapping_tpdo.test1, ODA_TPDO, 1, NULL, NULL},
		{(void*)&od_data_test_mapping_tpdo.test2, ODA_TPDO, 2, NULL, NULL},
		{(void*)&od_data_test_mapping_tpdo.test3, ODA_TPDO, 4, NULL, NULL}
};

/* 0x3FF8 sub_test_expe_sdo ------------------------------------------------------------------------*/
uint8_t od_data_test_expe_sdo[2] = {0};
CO_Sub_Object obj_3FF8_test_expe_sdo[]={
		{(void*)od_data_test_expe_sdo, ODA_SDO_RW, 2, NULL, NULL}
};

/* 0x3FF9 od_data_test_normal_sdo ------------------------------------------------------------------*/
uint8_t od_data_test_normal_sdo[100] = {0};
CO_Sub_Object obj_3FF9_test_normal_sdo[]={
		{(void*)od_data_test_normal_sdo, ODA_SDO_RW, 100, NULL, NULL}
};

/* USER CODE 0 end */
