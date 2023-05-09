/*
 * modbus_app.c
 *
 *  Created on: Mar 28, 2023
 *      Author: Admin
 */

#include "port.h"
#include "mb.h"
#include  "modbus_app.h"
#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"


USHORT usRegInputStart = REG_INPUT_START;
USHORT usRegInputBuf[REG_INPUT_NREGS];

USHORT usRegHoldingStart = REG_HOLDING_START;
USHORT usRegHoldingBuf[REG_HOLDING_NREGS];

void mb_app_init(){
	memset(usRegInputBuf,0,REG_INPUT_NREGS);
	eMBErrorCode eStatus = eMBInit( MB_RTU, 1, 3, 115200, MB_PAR_NONE );
	eStatus = eMBEnable();
}

void mb_app_run(){
	eMBPoll();
}

eMBErrorCode
eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex;

    if( ( usAddress >= REG_INPUT_START )
        && ( usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS ) )
    {
        iRegIndex = ( int )( usAddress - usRegInputStart );
        while( usNRegs > 0 )
        {
            *pucRegBuffer++ =
                ( unsigned char )( usRegInputBuf[iRegIndex] >> 8 );
            *pucRegBuffer++ =
                ( unsigned char )( usRegInputBuf[iRegIndex] & 0xFF );
            iRegIndex++;
            usNRegs--;
        }

    }
    else
    {
        eStatus = MB_ENOREG;
    }

    return eStatus;
}

eMBErrorCode
eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs,
                 eMBRegisterMode eMode )
{

    eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex;

    if( ( usAddress >= REG_HOLDING_START )
        && ( usAddress + usNRegs <= REG_HOLDING_START + REG_HOLDING_NREGS ) )
    {
        iRegIndex = ( int )( usAddress - usRegHoldingStart );
        while( usNRegs > 0 )
        {
					if(eMode == MB_REG_READ)
					{
            *pucRegBuffer++ =
                ( unsigned char )( usRegHoldingBuf[iRegIndex] >> 8 );
            *pucRegBuffer++ =
                ( unsigned char )( usRegHoldingBuf[iRegIndex] & 0xFF );
					}
					else if(eMode == MB_REG_WRITE)
					{
						usRegHoldingBuf[iRegIndex] =  ((USHORT)(*pucRegBuffer)<<8)|((*(pucRegBuffer+1))&0xFF);
						pucRegBuffer += 2;
					}
					iRegIndex++;
          usNRegs--;
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }
    return eStatus;
}


eMBErrorCode
eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils,
               eMBRegisterMode eMode )
{
    return MB_ENOREG;
}

eMBErrorCode
eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )
{
    return MB_ENOREG;
}
