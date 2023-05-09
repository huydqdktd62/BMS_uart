/*
 *
 *
 *  Created on: Jul 22, 2022
 *      Author: Minh Nhan
 */

#ifndef FIFO_H_
#define FIFO_H_


#include "stdio.h"
#include "stdlib.h"
#include "stdint.h"
#include "stdbool.h"
#include "string.h"

typedef struct FIFO_Handle_t FIFO_Handle;

struct FIFO_Handle_t{
    void *buff;
    size_t length;
    size_t size;
    volatile int head;
    volatile int tail;
    volatile unsigned int count;
};


void fifo_init(FIFO_Handle *p_fifo,size_t length,size_t size);
bool fifo_push(FIFO_Handle *p_fifo,void *item);
bool fifo_pop(FIFO_Handle *p_fifo,void *item);
void fifo_deinit(FIFO_Handle *p_fifo);
unsigned int fifo_get_num_item(FIFO_Handle *p_fifo);

#endif
