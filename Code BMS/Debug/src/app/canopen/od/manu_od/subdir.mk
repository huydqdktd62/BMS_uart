################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/app/canopen/od/manu_od/manu_od.c \
../src/app/canopen/od/manu_od/manu_od_default_value.c 

C_DEPS += \
./src/app/canopen/od/manu_od/manu_od.d \
./src/app/canopen/od/manu_od/manu_od_default_value.d 

OBJS += \
./src/app/canopen/od/manu_od/manu_od.o \
./src/app/canopen/od/manu_od/manu_od_default_value.o 


# Each subdirectory must supply rules for building sources it contributes
src/app/canopen/od/manu_od/%.o: ../src/app/canopen/od/manu_od/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU Arm Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m33 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -O2 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal  -g -gdwarf-4 -D_RA_CORE=CM33 -D_RENESAS_RA_ -I"D:\DamQuocHuy\BP\BP_Testing\Tool\BMS_uart\Can2Mb\src" -I"D:\DamQuocHuy\BP\BP_Testing\Tool\BMS_uart\Can2Mb\src\service\soc_ukf" -I"D:\DamQuocHuy\BP\BP_Testing\Tool\BMS_uart\Can2Mb\src\service\math" -I"D:\DamQuocHuy\BP\BP_Testing\Tool\BMS_uart\Can2Mb\ra\fsp\inc" -I"D:\DamQuocHuy\BP\BP_Testing\Tool\BMS_uart\Can2Mb\ra\fsp\inc\api" -I"D:\DamQuocHuy\BP\BP_Testing\Tool\BMS_uart\Can2Mb\ra\fsp\inc\instances" -I"D:\DamQuocHuy\BP\BP_Testing\Tool\BMS_uart\Can2Mb\ra\arm\CMSIS_5\CMSIS\Core\Include" -I"D:\DamQuocHuy\BP\BP_Testing\Tool\BMS_uart\Can2Mb\ra_gen" -I"D:\DamQuocHuy\BP\BP_Testing\Tool\BMS_uart\Can2Mb\ra_cfg\fsp_cfg\bsp" -I"D:\DamQuocHuy\BP\BP_Testing\Tool\BMS_uart\Can2Mb\ra_cfg\fsp_cfg" -I"D:\DamQuocHuy\BP\BP_Testing\Tool\BMS_uart\Can2Mb\src\board\gpio" -I"D:\DamQuocHuy\BP\BP_Testing\Tool\BMS_uart\Can2Mb\src\board\can" -I"D:\DamQuocHuy\BP\BP_Testing\Tool\BMS_uart\Can2Mb\src\board" -I"D:\DamQuocHuy\BP\BP_Testing\Tool\BMS_uart\Can2Mb\src\board\uart" -I"D:\DamQuocHuy\BP\BP_Testing\Tool\BMS_uart\Can2Mb\src\board\timer" -I"D:\DamQuocHuy\BP\BP_Testing\Tool\BMS_uart\Can2Mb\src\components\bp" -I"D:\DamQuocHuy\BP\BP_Testing\Tool\BMS_uart\Can2Mb\src\components\cabinet" -I"D:\DamQuocHuy\BP\BP_Testing\Tool\BMS_uart\Can2Mb\src\components\converter" -I"D:\DamQuocHuy\BP\BP_Testing\Tool\BMS_uart\Can2Mb\src\components\node_id" -I"D:\DamQuocHuy\BP\BP_Testing\Tool\BMS_uart\Can2Mb\src\service\canopen" -I"D:\DamQuocHuy\BP\BP_Testing\Tool\BMS_uart\Can2Mb\src\service\modbus\include" -I"D:\DamQuocHuy\BP\BP_Testing\Tool\BMS_uart\Can2Mb\src\service\modbus\rtu" -I"D:\DamQuocHuy\BP\BP_Testing\Tool\BMS_uart\Can2Mb\src\service\modbus\port" -I"D:\DamQuocHuy\BP\BP_Testing\Tool\BMS_uart\Can2Mb\src\app\can2mb_init" -I"D:\DamQuocHuy\BP\BP_Testing\Tool\BMS_uart\Can2Mb\src\app\canopen\init" -I"D:\DamQuocHuy\BP\BP_Testing\Tool\BMS_uart\Can2Mb\src\app\canopen\od" -I"D:\DamQuocHuy\BP\BP_Testing\Tool\BMS_uart\Can2Mb\src\app\mb_app" -I"D:\DamQuocHuy\BP\BP_Testing\Tool\BMS_uart\Can2Mb\src\app\canopen\od\comm_od" -I"D:\DamQuocHuy\BP\BP_Testing\Tool\BMS_uart\Can2Mb\src\app\canopen\od\manu_od" -I"D:\DamQuocHuy\BP\BP_Testing\Tool\BMS_uart\Can2Mb\src\service\fifo" -std=c99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" -x c "$<"
	@echo 'Finished building: $<'
	@echo ' '


