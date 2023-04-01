/*
 * CO_slave.h
 *
 *  Created on: Jul 15, 2022
 *      Author: Dakaka
 */

#ifndef LIBS_SELEX_LIBC_CANOPEN_CLIB_CO_SLAVE_H_
#define LIBS_SELEX_LIBC_CANOPEN_CLIB_CO_SLAVE_H_


#include "CO_config.h"
#include "CO.h"
#include "stdint.h"
typedef struct CO_Slave_t CO_Slave;

#define SLAVE_INACTIVE_TIME_THRESHOLD_mS				(1000UL)

typedef enum CO_SLAVE_NET_STATE{
	CO_SLAVE_CON_ST_DISCONNECT	=	0,
	CO_SLAVE_CON_ST_ASSIGNING	=	1,
	CO_SLAVE_CON_ST_AUTHORIZING	=	2,
	CO_SLAVE_CON_ST_CONNECTED	=	3

}CO_SLAVE_NET_STATE;

typedef void (*CO_Slave_handle)(CO_Slave* sl);

typedef struct I_CO_SLave_t{

	CO_Slave_handle select;
	CO_Slave_handle deselect;
	CO_Slave_handle on_disconnected;
	CO_Slave_handle on_connected;
}I_CO_Slave;


typedef enum CO_SLAVE_ASSIGN_STATE{

	/* slave need to assign node id */
    CO_ASSIGN_ST_WAIT_REQUEST=0,
	/* start assign process */
	CO_ASSIGN_ST_START=1,
	/* slave select (set node id = low ) to select one slave
	 * from the list to start assigning process */
	CO_ASSIGN_ST_SLAVE_SELECT=2,
	/* slave ackknowledge the selection from master */
	CO_ASSIGN_ST_SELECT_ACK=3,
	/* after sending set node id message, wait at this state for
	 * confirming response from slave */
	CO_ASSIGN_ST_NODEID_CONFIRM=4,
	/* wait for authorizing at slave to finish ( sdo session of reading serial number of slave
	 *
	 * finish successfully
	 */
	CO_ASSIGN_ST_AUTHORIZING=5,
	CO_ASSIGN_ST_CONNECTED=6,
	/* slave is disconnect from the network */
	CO_ASSIGN_ST_DISCONNECTED=7,
	CO_ASSIGN_ST_FAIL=8,
	CO_ASSIGN_ST_REASSIGN=9,
	/* ask slave to prepare data and enter authorizing process by rising node id */
	CO_ASSIGN_ST_AUTHORIZING_REQ=10,
	CO_ASSIGN_ST_LOST_CONNECTED=11,
	/* start authorizing process by send sdo to read serial number of slave */
	CO_ASSIGN_ST_AUTHORIZING_START=12,
    CO_ASSIGN_ST_INIT=255

} CO_SLAVE_ASSIGN_STATE;

struct CO_Slave_t{
	CO base;
	uint8_t id;
	uint32_t assign_timeout;
	uint32_t assign_time_ms;
	uint32_t inactive_time_ms;
	uint32_t sdo_server_address;
	uint32_t authorizing_wait_time;
	CO_SLAVE_ASSIGN_STATE assign_state;
	bool     is_reconnect;
	I_CO_Slave interface;
};


static inline bool co_slave_is_assign_timeout(CO_Slave* p_sl){
	return (p_sl->assign_time_ms>=p_sl->assign_timeout);
}

static inline void co_slave_set_assign_state(CO_Slave* p_sl, CO_SLAVE_ASSIGN_STATE state){
        p_sl->assign_state=state;
}

static inline CO_SLAVE_ASSIGN_STATE co_slave_get_assign_state(const CO_Slave*  const p_sl){
	return p_sl->assign_state;
}

static inline void co_slave_reset_inactive_timer(CO_Slave* p_slave){
    p_slave->inactive_time_ms = 0;
}

static inline void co_slave_disconnect_handle(CO_Slave* p_slave){
	p_slave->interface.on_disconnected(p_slave);
}

static inline void co_slave_connect_handle(CO_Slave* p_slave){
	p_slave->interface.on_connected(p_slave);
}

static inline void co_slave_select(CO_Slave* p_slave){
		p_slave->interface.select(p_slave);
}

static inline void co_slave_deselect(CO_Slave* p_slave){
		p_slave->interface.deselect(p_slave);
}

static inline void co_slave_update_inactive_timer(CO_Slave* p_slave, const uint32_t timeout){
    if(p_slave->assign_state != CO_ASSIGN_ST_CONNECTED) return;

    p_slave->inactive_time_ms++;
    if(p_slave->inactive_time_ms > timeout){
    	co_slave_disconnect_handle(p_slave);
        co_slave_reset_inactive_timer(p_slave);
        co_slave_set_assign_state(p_slave,CO_ASSIGN_ST_DISCONNECTED);
    }
}

static inline bool co_slave_is_assign_finish(const CO_Slave* const p_sl){

	CO_SLAVE_ASSIGN_STATE state=co_slave_get_assign_state(p_sl);
	if((state==CO_ASSIGN_ST_CONNECTED) || (state==CO_ASSIGN_ST_DISCONNECTED)
			|| (state==CO_ASSIGN_ST_FAIL)){
		return true;
	}
	return false;
}

#endif /* LIBS_SELEX_LIBC_CANOPEN_CLIB_CO_SLAVE_H_ */
