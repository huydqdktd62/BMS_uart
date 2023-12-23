#include "core_hw.h"
#include "string_util.h"
#define DEV_ID_BASE_ADDR                        0x1FFFF7AC

void core_hw_init(void){
	SystemInit();
}
int32_t core_read_id(char* id){
	memcpy( id, (uint8_t*)SERIAL_NUMBER_MEM_ADDR, SERIAL_NUMBER_MEM_SIZE );
	return SERIAL_NUMBER_MEM_SIZE;
}

/* Callback function */

