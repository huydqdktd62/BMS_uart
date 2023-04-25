################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/service/canopen/CO.c \
../src/service/canopen/CO_CAN_Msg.c \
../src/service/canopen/CO_CRC.c \
../src/service/canopen/CO_EMCY.c \
../src/service/canopen/CO_FLASH.c \
../src/service/canopen/CO_NMT.c \
../src/service/canopen/CO_OD.c \
../src/service/canopen/CO_OD_sign.c \
../src/service/canopen/CO_OD_storage.c \
../src/service/canopen/CO_Object.c \
../src/service/canopen/CO_PDO.c \
../src/service/canopen/CO_RPDO.c \
../src/service/canopen/CO_SDO.c \
../src/service/canopen/CO_SDOclient.c \
../src/service/canopen/CO_SDOserver.c \
../src/service/canopen/CO_SYNC.c \
../src/service/canopen/CO_TPDO.c \
../src/service/canopen/can_master.c 

C_DEPS += \
./src/service/canopen/CO.d \
./src/service/canopen/CO_CAN_Msg.d \
./src/service/canopen/CO_CRC.d \
./src/service/canopen/CO_EMCY.d \
./src/service/canopen/CO_FLASH.d \
./src/service/canopen/CO_NMT.d \
./src/service/canopen/CO_OD.d \
./src/service/canopen/CO_OD_sign.d \
./src/service/canopen/CO_OD_storage.d \
./src/service/canopen/CO_Object.d \
./src/service/canopen/CO_PDO.d \
./src/service/canopen/CO_RPDO.d \
./src/service/canopen/CO_SDO.d \
./src/service/canopen/CO_SDOclient.d \
./src/service/canopen/CO_SDOserver.d \
./src/service/canopen/CO_SYNC.d \
./src/service/canopen/CO_TPDO.d \
./src/service/canopen/can_master.d 

OBJS += \
./src/service/canopen/CO.o \
./src/service/canopen/CO_CAN_Msg.o \
./src/service/canopen/CO_CRC.o \
./src/service/canopen/CO_EMCY.o \
./src/service/canopen/CO_FLASH.o \
./src/service/canopen/CO_NMT.o \
./src/service/canopen/CO_OD.o \
./src/service/canopen/CO_OD_sign.o \
./src/service/canopen/CO_OD_storage.o \
./src/service/canopen/CO_Object.o \
./src/service/canopen/CO_PDO.o \
./src/service/canopen/CO_RPDO.o \
./src/service/canopen/CO_SDO.o \
./src/service/canopen/CO_SDOclient.o \
./src/service/canopen/CO_SDOserver.o \
./src/service/canopen/CO_SYNC.o \
./src/service/canopen/CO_TPDO.o \
./src/service/canopen/can_master.o 


# Each subdirectory must supply rules for building sources it contributes
src/service/canopen/%.o: ../src/service/canopen/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU Arm Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m33 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -O2 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal  -g -gdwarf-4 -D_RA_CORE=CM33 -D_RENESAS_RA_ -I"D:\DamQuocHuy\BP\BP_Testing\Tool\BMS_uart\Code BMS\src" -I"D:\DamQuocHuy\BP\BP_Testing\Tool\BMS_uart\Code BMS\src\service\soc_ukf" -I"D:\DamQuocHuy\BP\BP_Testing\Tool\BMS_uart\Code BMS\src\service\math" -I"D:\DamQuocHuy\BP\BP_Testing\Tool\BMS_uart\Code BMS\ra\fsp\inc" -I"D:\DamQuocHuy\BP\BP_Testing\Tool\BMS_uart\Code BMS\ra\fsp\inc\api" -I"D:\DamQuocHuy\BP\BP_Testing\Tool\BMS_uart\Code BMS\ra\fsp\inc\instances" -I"D:\DamQuocHuy\BP\BP_Testing\Tool\BMS_uart\Code BMS\ra\arm\CMSIS_5\CMSIS\Core\Include" -I"D:\DamQuocHuy\BP\BP_Testing\Tool\BMS_uart\Code BMS\ra_gen" -I"D:\DamQuocHuy\BP\BP_Testing\Tool\BMS_uart\Code BMS\ra_cfg\fsp_cfg\bsp" -I"D:\DamQuocHuy\BP\BP_Testing\Tool\BMS_uart\Code BMS\ra_cfg\fsp_cfg" -I"D:\DamQuocHuy\BP\BP_Testing\Tool\BMS_uart\Code BMS\src\board\gpio" -I"D:\DamQuocHuy\BP\BP_Testing\Tool\BMS_uart\Code BMS\src\board\can" -I"D:\DamQuocHuy\BP\BP_Testing\Tool\BMS_uart\Code BMS\src\board" -I"D:\DamQuocHuy\BP\BP_Testing\Tool\BMS_uart\Code BMS\src\board\uart" -I"D:\DamQuocHuy\BP\BP_Testing\Tool\BMS_uart\Code BMS\src\board\timer" -I"D:\DamQuocHuy\BP\BP_Testing\Tool\BMS_uart\Code BMS\src\components\bp" -I"D:\DamQuocHuy\BP\BP_Testing\Tool\BMS_uart\Code BMS\src\components\cabinet" -I"D:\DamQuocHuy\BP\BP_Testing\Tool\BMS_uart\Code BMS\src\components\converter" -I"D:\DamQuocHuy\BP\BP_Testing\Tool\BMS_uart\Code BMS\src\components\node_id" -I"D:\DamQuocHuy\BP\BP_Testing\Tool\BMS_uart\Code BMS\src\service\canopen" -I"D:\DamQuocHuy\BP\BP_Testing\Tool\BMS_uart\Code BMS\src\service\modbus\include" -I"D:\DamQuocHuy\BP\BP_Testing\Tool\BMS_uart\Code BMS\src\service\modbus\rtu" -I"D:\DamQuocHuy\BP\BP_Testing\Tool\BMS_uart\Code BMS\src\service\modbus\port" -I"D:\DamQuocHuy\BP\BP_Testing\Tool\BMS_uart\Code BMS\src\app\can2mb_init" -I"D:\DamQuocHuy\BP\BP_Testing\Tool\BMS_uart\Code BMS\src\app\canopen\init" -I"D:\DamQuocHuy\BP\BP_Testing\Tool\BMS_uart\Code BMS\src\app\canopen\od" -I"D:\DamQuocHuy\BP\BP_Testing\Tool\BMS_uart\Code BMS\src\app\mb_app" -I"D:\DamQuocHuy\BP\BP_Testing\Tool\BMS_uart\Code BMS\src\app\canopen\od\comm_od" -I"D:\DamQuocHuy\BP\BP_Testing\Tool\BMS_uart\Code BMS\src\app\canopen\od\manu_od" -I"D:\DamQuocHuy\BP\BP_Testing\Tool\BMS_uart\Code BMS\src\service\fifo" -I"D:\DamQuocHuy\BP\BP_Testing\Tool\BMS_uart\Code BMS\src\service\retaget" -std=c99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" -x c "$<"
	@echo 'Finished building: $<'
	@echo ' '


