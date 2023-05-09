/*
 * app_co_init.h
 *
 *  Created on: Jun 17, 2022
 *      Author: Dakaka
 */

#ifndef APP_APP_CANOPEN_APP_CO_INIT_H_
#define APP_APP_CANOPEN_APP_CO_INIT_H_

#include "CO.h"
#include "od_config.h"

extern void* tpdo_build_data_impl[TPDO_NUMBER];

void app_co_init(void);
void app_co_can_receive(const uint32_t can_id, uint8_t* data);
void co_process_rewrite(CO* p_co, uint16_t time_diff_ms);

/*** BS_TPDO1 ***/
/* BS TPDO BYTE 0*/
#define BS_TPDO_1_BYTE0_BIT7_BRAKE			0x80
#define BS_TPDO_1_BYTE0_BIT6				0x40
#define BS_TPDO_1_BYTE0_BIT5_PARKING		0x20
#define BS_TPDO_1_BYTE0_BIT4_ECO_SPORT		0x10
#define BS_TPDO_1_BYTE0_BIT3_XI_NHAN_RIGHT	0x08
#define BS_TPDO_1_BYTE0_BIT2_XI_NHAN_LEFT		0x04
#define BS_TPDO_1_BYTE0_BIT1_HORN			0x02
#define BS_TPDO_1_BYTE0_BIT0_KEY			0x01

typedef enum
{
	PDO_DRI_SPORT_DATA 	= 0x00U,
	PDO_DRI_ECO_DATA 		= 0x01U,
}PDO_DRIVE_MODE_DATA_BYTE1;

typedef enum
{
	PDO_DRI_SPORT	 	= 0x01U,
	PDO_DRI_ECO 		= 0x00U,
}PDO_DRIVE_MODE;

typedef enum
{
	PDO_DRI_NOT_UPDATE	= 0x00U,
	PDO_DRI_UPDATED		= 0x01U,
}PDO_DRIVE_MODE_UPDATE_STATUS;

typedef enum
{
	MC_CO_SYNC_STATUS_INACTIVE			= 0x00U,
	MC_CO_SYNC_STATUS_ACTIVE			= 0x01U,
}MC_CO_SYNC_SIGNAL_STATUS;

typedef struct PDO_DRIVE_MODE_t
{
	PDO_DRIVE_MODE 			mode;			///< Current mode
	PDO_DRIVE_MODE_UPDATE_STATUS 	update_status;	///< Driving mode update status
}PDO_DRIVE_MODE_t;

typedef struct CO_BS_TPDO1_t
{
	float				limited_discharge_cur_A;
	float				limited_charge_cur_A;
	PDO_DRIVE_MODE_t	dri_mode_cm;	///< Drive mode command from system
	CO_CAN_Msg 			rx_msg;
}BS_TPDO1;

extern BS_TPDO1 bs_tpdo1;

/* Low battery mode function */
static inline float app_co_bs_tpdo1_get_limited_discharge_current_A()
{
	return bs_tpdo1.limited_discharge_cur_A;
}
/* Driving mode command function */
static inline PDO_DRIVE_MODE app_co_bs_tpdo1_dri_get_current_mode()
{
	return bs_tpdo1.dri_mode_cm.mode;
}
static inline PDO_DRIVE_MODE_UPDATE_STATUS app_co_bs_tpdo1_dri_get_update_status()
{
	return bs_tpdo1.dri_mode_cm.update_status;
}
static inline void app_co_bs_tpdo1_dri_set_update_status(PDO_DRIVE_MODE_UPDATE_STATUS status)
{
	bs_tpdo1.dri_mode_cm.update_status = status;
}
#endif /* APP_APP_CANOPEN_APP_CO_INIT_H_ */

