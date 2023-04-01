/*
 * app_co_od_config.h
 *
 *  Created on: Oct 8, 2022
 *      Author: Dakaka
 */

#include "CO.h"

/* Include header file contains CANopen object to here ----------*/
#include

#define	NODE_ID_DEFAULT												MC_NODE_ID
#define CO_DEVICE													mc.base		//<< Fill CANopen object
#define SW_VERSION													0x000003	//<< Use 3 byte
#define IS_MAIN_APP													1			//<< is main_app: set 1,
																				//<< is boot_app: set 0

/* OD STORAGE option : enable (set 1), disable (set 0) */
#define APP_OD_STORAGE__USE											1

#if APP_OD_STORAGE__USE
#define APP_OD_STORAGE__STORAGE_REGION__ADDR						(xxx)
#define APP_OD_STORAGE__STORAGE_REGION__SIZE						(xxx)
#endif
