/*
 * node_id.h
 *
 *  Created on: Dec 21, 2022
 *      Author: Admin
 */

#ifndef MAIN_COMPONENTS_NODE_ID_NODE_ID_H_
#define MAIN_COMPONENTS_NODE_ID_NODE_ID_H_

#include "stdint.h"
#include "gpio_hw.h"
typedef struct Node_ID_t Node_ID;

struct Node_ID_t{
	uint32_t port;
	uint32_t pin;
};

typedef enum{
	NODE_LOW = 0,
	NODE_HIGH
}Node_ID_State;

void node_id_init(Node_ID *node);
void node_id_set(Node_ID *node,Node_ID_State state);
void node_id_high(Node_ID *node);
void node_id_low(Node_ID *node);


#endif /* MAIN_COMPONENTS_NODE_ID_NODE_ID_H_ */
