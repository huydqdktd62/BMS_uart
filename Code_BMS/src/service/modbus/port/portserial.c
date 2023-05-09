/*
 * FreeModbus Libary: BARE Port
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id$
 */

#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"


#include "hal_data.h"

uint8_t receiver_char = 0;

/* ----------------------- static functions ---------------------------------*/
static void prvvUARTTxReadyISR( void );
static void prvvUARTRxISR( void );

#define BAUD_RATE 115200
#define MASTER_UART1_PRIORITY 1
/* ----------------------- Start implementation -----------------------------*/
void
vMBPortSerialEnable( BOOL xRxEnable, BOOL xTxEnable )
{
    /* If xRXEnable enable serial receive interrupts. If xTxENable enable
     * transmitter empty interrupts.
     */
	ENTER_CRITICAL_SECTION(  );
    if (xRxEnable == TRUE){

    }
    else {
        R_SCI_UART_Abort(&rs485_ctrl, UART_DIR_RX);
    }

    // Enable/Disable TX
    if (xTxEnable == TRUE)
    {
        pxMBFrameCBTransmitterEmpty();
    }
    else {
        R_SCI_UART_Abort(&rs485_ctrl, UART_DIR_TX);
    }
	EXIT_CRITICAL_SECTION(  );
}

BOOL
xMBPortSerialInit( UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity )
{

    (void)ucPORT;
    (void)ulBaudRate;
    (void)ucDataBits;
    (void)eParity;
    R_SCI_UART_Open(&rs485_ctrl,&rs485_cfg);
    return TRUE;
}

BOOL
xMBPortSerialPutByte( CHAR ucByte )
{
    /* Put a byte in the UARTs transmit buffer. This function is called
     * by the protocol stack if pxMBFrameCBTransmitterEmpty( ) has been
     * called. */
    fsp_err_t err = R_SCI_UART_Write(&rs485_ctrl,(uint8_t*)&ucByte,(uint32_t)1);
    if(err == FSP_SUCCESS)
        return TRUE;
    return FALSE;
}
BOOL xMBPortSerialPutBytes(volatile UCHAR *ucByte, USHORT usSize)
{
    fsp_err_t err = R_SCI_UART_Write(&rs485_ctrl,(uint8_t*)ucByte,(uint32_t)usSize);
    if(err == FSP_SUCCESS)
        return TRUE;
    return FALSE;
}
BOOL
xMBPortSerialGetByte( CHAR * pucByte )
{
    /* Return the byte in the UARTs receive buffer. This function is called
     * by the protocol stack after pxMBFrameCBByteReceived( ) has been called.
     */
    *pucByte = (CHAR)(receiver_char);
    return TRUE;
}

/* Create an interrupt handler for the transmit buffer empty interrupt
 * (or an equivalent) for your target processor. This function should then
 * call pxMBFrameCBTransmitterEmpty( ) which tells the protocol stack that
 * a new character can be sent. The protocol stack will then call 
 * xMBPortSerialPutByte( ) to send the character.
 */
static void prvvUARTTxReadyISR( void )
{
    pxMBFrameCBTransmitterEmpty(  );
}

/* Create an interrupt handler for the receive interrupt for your target
 * processor. This function should then call pxMBFrameCBByteReceived( ). The
 * protocol stack will then call xMBPortSerialGetByte( ) to retrieve the
 * character.
 */
static void prvvUARTRxISR( void )
{
    pxMBFrameCBByteReceived(  );
}

/* Callback function */
void rs485_callback(uart_callback_args_t *p_args)
{
    /* TODO: add your own code here */
    if(p_args->event == UART_EVENT_RX_CHAR){
        receiver_char = (uint8_t) p_args->data;
        prvvUARTRxISR();
    }
    if(p_args->event == UART_EVENT_TX_COMPLETE){
        prvvUARTTxReadyISR();
    }
    if(p_args->event == UART_EVENT_RX_COMPLETE){

    }
}
