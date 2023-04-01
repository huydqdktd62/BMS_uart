/*
 * CO_SYNC.c
 *
 *  Created on: Jun 20, 2022
 *      Author: Dakaka
 */

#include "CO_SYNC.h"
#include "stdio.h"

static inline void CO_SYNC_send(CO_SYNC* p_sync);
static inline void CO_SYNC_update_aux_sync_reg(CO_SYNC* p_sync);

#if CO_SYNC_INIT_TYPE_1
void CO_SYNC_init(CO_SYNC *p_sync, co_id_t x1005_sync_cob_id, uint16_t x1006_cycle_period_ms,
		uint16_t x1007_sync_window_len, uint16_t x1019_sync_cter_over_flow_value,
		uint8_t x1030_sync_bit_pos_over_flow_value)
{
	p_sync->msg.data_len = 3;
	p_sync->msg.id = x1005_sync_cob_id;
	p_sync->msg.is_new = false;

	p_sync->cycle_period_ms = x1006_cycle_period_ms;
	p_sync->window_length_ms = x1007_sync_window_len;
	p_sync->max_sync_counter = x1019_sync_cter_over_flow_value;
	p_sync->max_aux_sync_counter = x1030_sync_bit_pos_over_flow_value;

	p_sync->time_in_cycle_ms = p_sync->index_in_cycle =0;
	p_sync->sync_counter = 0;
	p_sync->aux_sync_counter = 0;
	p_sync->aux_sync_reg = 0;
	p_sync->is_inside_window = false;
}
#else
void CO_SYNC_init(CO_SYNC *p_sync, OD_Temp_Comm_Profile_Para_t* p_od_comm_prof_init_data)
{
	p_sync->msg.data_len = 3;
	p_sync->msg.id = p_od_comm_prof_init_data->x1005_cob_id_sync_message;
	p_sync->msg.is_new = false;

	p_sync->cycle_period_ms = p_od_comm_prof_init_data->x1006_comm_cycle_period_ms;
	p_sync->window_length_ms = p_od_comm_prof_init_data->x1007_sync_window_length_ms;
	p_sync->max_sync_counter = p_od_comm_prof_init_data->x1019_sync_cter_over_flow_value;
	p_sync->max_aux_sync_counter = p_od_comm_prof_init_data->x1030_sync_bit_pos_over_flow_value;

	p_sync->time_in_cycle_ms = p_sync->index_in_cycle = 0;
	p_sync->sync_counter = 0;
	p_sync->aux_sync_counter = 0;
	p_sync->aux_sync_reg = 0;
	p_sync->is_inside_window = false;
	for(int i=0;i<MAX_SYNC_SLOTS_x32;i++){
		//p_sync->active_sync_mask[i]=0xFFFFFFFF;
		p_sync->active_sync_mask[i]=0x00000000;
	}
}

void CO_SYNC_disable_sync_slot(CO_SYNC* p_sync,const uint32_t slot){

	uint32_t slot_offset= slot/32;
	uint32_t slot_sub_offset=slot%32;

	p_sync->active_sync_mask[slot_offset] &= ~(1<<slot_sub_offset);
}

void CO_SYNC_enable_sync_slot(CO_SYNC* p_sync,const uint32_t slot){

	uint32_t slot_offset= slot/32;
	uint32_t slot_sub_offset=slot%32;

	p_sync->active_sync_mask[slot_offset] |=(1<<slot_sub_offset);
}

bool CO_SYNC_is_sync_slot_enable(const CO_SYNC* const p_sync,const uint32_t slot){

	uint32_t slot_offset= slot/32;
	uint32_t slot_sub_offset=slot%32;

	return (p_sync->active_sync_mask[slot_offset] & (1<<slot_sub_offset));
}

#endif
void CO_SYNC_process(CO_SYNC* p_sync, bool nmt_is_pre_or_operational, uint16_t time_diff_ms)
{
	(void) nmt_is_pre_or_operational;
	if(p_sync->msg.id.valid == CO_SYNC_VALID)
	{
	    /* Update sync timer, no overflow */
	    uint16_t timer_new = p_sync->time_in_cycle_ms + time_diff_ms;
	    if (timer_new >  p_sync->time_in_cycle_ms)
	    {
	    	p_sync->time_in_cycle_ms = timer_new;
	    }
	    uint16_t index_new = p_sync->index_in_cycle + 1;
	    if (index_new >  p_sync->index_in_cycle)
	    {
	    	p_sync->index_in_cycle = index_new;
	    }

	    bool is_sync_slot_enable=false;
	    uint32_t max_sync_slot=(p_sync->max_aux_sync_counter+1)* p_sync->max_sync_counter;
	    uint32_t sync_slot_index=0;
	    /* Update sync counter */
	    if(p_sync->msg.id.bit_30 == CO_SYNC_GEN)	//is producer
	    {
	        if(p_sync->time_in_cycle_ms >= p_sync->cycle_period_ms)
	        {
	        	for(int i=0;((i< max_sync_slot) && (!is_sync_slot_enable));i++){
					if(++p_sync->sync_counter > p_sync->max_sync_counter)
					{
						p_sync->sync_counter = 1;
						if(++p_sync->aux_sync_counter > p_sync->max_aux_sync_counter)
						{
							p_sync->aux_sync_counter = 0;
						}
						CO_SYNC_update_aux_sync_reg(p_sync);
					}

					sync_slot_index= p_sync->aux_sync_counter* p_sync->max_sync_counter + (p_sync->sync_counter-1);

					if(CO_SYNC_is_sync_slot_enable(p_sync, sync_slot_index)==true){
						CO_SYNC_send(p_sync);
						is_sync_slot_enable=true;
					}
	        	}
	        	p_sync->time_in_cycle_ms = p_sync->index_in_cycle = 0;
	        }
	    }
	    else // is consumer
	    {
	        if (p_sync->msg.is_new)
	        {
	        	p_sync->sync_counter = CO_getUint16(&p_sync->msg.data[0]);
	        	p_sync->aux_sync_counter = p_sync->msg.data[2];
	        	CO_SYNC_update_aux_sync_reg(p_sync);
	        	p_sync->time_in_cycle_ms = p_sync->index_in_cycle = 0;
	            p_sync->msg.is_new = false;
	        }
	    }
	    /* Synchronous PDO, SDO are allowed only inside time window */
	   	p_sync->is_inside_window = (p_sync->time_in_cycle_ms <= p_sync->window_length_ms);
	}
}
static inline void CO_SYNC_send(CO_SYNC* p_sync)
{
	CO_setUint16(&p_sync->msg.data[0], p_sync->sync_counter);
	p_sync->msg.data[2] = p_sync->aux_sync_counter;
	CO_CAN_send(&p_sync->msg);
}
static inline void CO_SYNC_update_aux_sync_reg(CO_SYNC* p_sync)
{
	p_sync->aux_sync_reg = 0x0001 << p_sync->aux_sync_counter;
}
