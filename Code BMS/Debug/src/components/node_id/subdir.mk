################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/components/node_id/node_id.c 

C_DEPS += \
./src/components/node_id/node_id.d 

OBJS += \
./src/components/node_id/node_id.o 


# Each subdirectory must supply rules for building sources it contributes
src/components/node_id/%.o: ../src/components/node_id/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU Arm Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m33 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -O2 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal  -g -gdwarf-4 -D_RA_CORE=CM33 -D_RENESAS_RA_ -I"D:\DamQuocHuy\BP\BP_Testing\Tool\BMS_uart\Code BMS\src" -I"D:\DamQuocHuy\BP\BP_Testing\Tool\BMS_uart\Code BMS\src\service\soc_ukf" -I"D:\DamQuocHuy\BP\BP_Testing\Tool\BMS_uart\Code BMS\src\service\math" -I"D:\DamQuocHuy\BP\BP_Testing\Tool\BMS_uart\Code BMS\ra\fsp\inc" -I"D:\DamQuocHuy\BP\BP_Testing\Tool\BMS_uart\Code BMS\ra\fsp\inc\api" -I"D:\DamQuocHuy\BP\BP_Testing\Tool\BMS_uart\Code BMS\ra\fsp\inc\instances" -I"D:\DamQuocHuy\BP\BP_Testing\Tool\BMS_uart\Code BMS\ra\arm\CMSIS_5\CMSIS\Core\Include" -I"D:\DamQuocHuy\BP\BP_Testing\Tool\BMS_uart\Code BMS\ra_gen" -I"D:\DamQuocHuy\BP\BP_Testing\Tool\BMS_uart\Code BMS\ra_cfg\fsp_cfg\bsp" -I"D:\DamQuocHuy\BP\BP_Testing\Tool\BMS_uart\Code BMS\ra_cfg\fsp_cfg" -I"D:\DamQuocHuy\BP\BP_Testing\Tool\BMS_uart\Code BMS\src\board\gpio" -I"D:\DamQuocHuy\BP\BP_Testing\Tool\BMS_uart\Code BMS\src\board\can" -I"D:\DamQuocHuy\BP\BP_Testing\Tool\BMS_uart\Code BMS\src\board" -I"D:\DamQuocHuy\BP\BP_Testing\Tool\BMS_uart\Code BMS\src\board\uart" -I"D:\DamQuocHuy\BP\BP_Testing\Tool\BMS_uart\Code BMS\src\board\timer" -I"D:\DamQuocHuy\BP\BP_Testing\Tool\BMS_uart\Code BMS\src\components\bp" -I"D:\DamQuocHuy\BP\BP_Testing\Tool\BMS_uart\Code BMS\src\components\cabinet" -I"D:\DamQuocHuy\BP\BP_Testing\Tool\BMS_uart\Code BMS\src\components\converter" -I"D:\DamQuocHuy\BP\BP_Testing\Tool\BMS_uart\Code BMS\src\components\node_id" -I"D:\DamQuocHuy\BP\BP_Testing\Tool\BMS_uart\Code BMS\src\service\canopen" -I"D:\DamQuocHuy\BP\BP_Testing\Tool\BMS_uart\Code BMS\src\service\modbus\include" -I"D:\DamQuocHuy\BP\BP_Testing\Tool\BMS_uart\Code BMS\src\service\modbus\rtu" -I"D:\DamQuocHuy\BP\BP_Testing\Tool\BMS_uart\Code BMS\src\service\modbus\port" -I"D:\DamQuocHuy\BP\BP_Testing\Tool\BMS_uart\Code BMS\src\app\can2mb_init" -I"D:\DamQuocHuy\BP\BP_Testing\Tool\BMS_uart\Code BMS\src\app\canopen\init" -I"D:\DamQuocHuy\BP\BP_Testing\Tool\BMS_uart\Code BMS\src\app\canopen\od" -I"D:\DamQuocHuy\BP\BP_Testing\Tool\BMS_uart\Code BMS\src\app\mb_app" -I"D:\DamQuocHuy\BP\BP_Testing\Tool\BMS_uart\Code BMS\src\app\canopen\od\comm_od" -I"D:\DamQuocHuy\BP\BP_Testing\Tool\BMS_uart\Code BMS\src\app\canopen\od\manu_od" -I"D:\DamQuocHuy\BP\BP_Testing\Tool\BMS_uart\Code BMS\src\service\fifo" -std=c99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" -x c "$<"
	@echo 'Finished building: $<'
	@echo ' '


