/*
 * porting.h
 *
 *  Created on: Apr 2, 2023
 *      Author: MinhNhan
 */

#ifndef RETAGET_PORTING_H_
#define RETAGET_PORTING_H_

void serial_init(void *arg);
void serial_putc(char c);
void serial_puts(char *s);


#endif /* RETAGET_PORTING_H_ */
