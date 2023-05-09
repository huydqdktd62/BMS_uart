/*
 * cfifo.c
 *
 *  Created on: Jul 22, 2022
 *      Author: Minh Nhan
 */



#include <fifo.h>

void fifo_init(FIFO_Handle *p_fifo,size_t length,size_t size){

    if(p_fifo == NULL){
        p_fifo = (FIFO_Handle*)malloc(sizeof(FIFO_Handle));
    }

    p_fifo->length = length+1;
    p_fifo->size = size;
    p_fifo->count = 0;
    p_fifo->head = -1;
    p_fifo->tail = -1;
    p_fifo->buff = malloc(p_fifo->length*p_fifo->size);
    if(p_fifo->buff == NULL){
    	//printf("fifo init false\r\n");
    	return;
    }
}
void fifo_deinit(FIFO_Handle *p_fifo){
	p_fifo->count = 0;
	p_fifo->head = 0;
	p_fifo->tail = 0;
	p_fifo->length = 0;
	free(p_fifo->buff);
	if(p_fifo->buff != NULL){
		//printf("deinit false\r\n");
	}
}
bool fifo_push(FIFO_Handle *p_fifo,void *item){
    unsigned int next = (unsigned int) (p_fifo->head + 1) % p_fifo->length;
    if (next != p_fifo->tail) {
//        p_fifo->buff[p_fifo->head] = c;
        memcpy((p_fifo->buff+p_fifo->head*p_fifo->size),item,p_fifo->size);
        p_fifo->head = next;
        p_fifo->count += 1;
        return true;
    }
    //printf("fifo push false\r\n");
    return false;
}
bool fifo_pop(FIFO_Handle *p_fifo,void *item){
	if (p_fifo->head != p_fifo->tail) {
		// data[0] = p_fifo->buff[p_fifo->tail];
        memcpy(item,(p_fifo->buff+p_fifo->tail*p_fifo->size),p_fifo->size);
		p_fifo->tail = (unsigned int) (p_fifo->tail + 1) % p_fifo->length;
		p_fifo->count -= 1;
		return true;
	}
	//printf("fifo pop false\r\n");
	return false;
}
unsigned int fifo_get_num_item(FIFO_Handle *p_fifo){
	return p_fifo->count;
}
