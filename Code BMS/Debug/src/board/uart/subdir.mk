################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/board/uart/uart_hw.c 

C_DEPS += \
./src/board/uart/uart_hw.d 

OBJS += \
./src/board/uart/uart_hw.o 

SREC += \
Can2Mb.srec 

MAP += \
Can2Mb.map 


# Each subdirectory must supply rules for building sources it contributes
src/board/uart/%.o: ../src/board/uart/%.c
	$(file > $@.in,-mcpu=cortex-m33 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -O2 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal  -g -gdwarf-4 -D_RA_CORE=CM33 -D_RENESAS_RA_ -I"C:/Users/Admin/e2_studio/workspace4.1/Can2Mb/src" -I"C:/Users/Admin/e2_studio/workspace4.1/Can2Mb/ra/fsp/inc" -I"C:/Users/Admin/e2_studio/workspace4.1/Can2Mb/ra/fsp/inc/api" -I"C:/Users/Admin/e2_studio/workspace4.1/Can2Mb/ra/fsp/inc/instances" -I"C:/Users/Admin/e2_studio/workspace4.1/Can2Mb/ra/arm/CMSIS_5/CMSIS/Core/Include" -I"C:/Users/Admin/e2_studio/workspace4.1/Can2Mb/ra_gen" -I"C:/Users/Admin/e2_studio/workspace4.1/Can2Mb/ra_cfg/fsp_cfg/bsp" -I"C:/Users/Admin/e2_studio/workspace4.1/Can2Mb/ra_cfg/fsp_cfg" -I"C:/Users/Admin/e2_studio/workspace4.1/Can2Mb/src/board/gpio" -I"C:/Users/Admin/e2_studio/workspace4.1/Can2Mb/src/board/can" -I"C:/Users/Admin/e2_studio/workspace4.1/Can2Mb/src/board" -I"C:/Users/Admin/e2_studio/workspace4.1/Can2Mb/src/board/uart" -I"C:/Users/Admin/e2_studio/workspace4.1/Can2Mb/src/board/timer" -I"C:/Users/Admin/e2_studio/workspace4.1/Can2Mb/src/components/bp" -I"C:/Users/Admin/e2_studio/workspace4.1/Can2Mb/src/components/cabinet" -I"C:/Users/Admin/e2_studio/workspace4.1/Can2Mb/src/components/converter" -I"C:/Users/Admin/e2_studio/workspace4.1/Can2Mb/src/components/node_id" -I"C:/Users/Admin/e2_studio/workspace4.1/Can2Mb/src/service/canopen" -I"C:/Users/Admin/e2_studio/workspace4.1/Can2Mb/src/service/modbus/include" -I"C:/Users/Admin/e2_studio/workspace4.1/Can2Mb/src/service/modbus/rtu" -I"C:/Users/Admin/e2_studio/workspace4.1/Can2Mb/src/service/modbus/port" -I"C:/Users/Admin/e2_studio/workspace4.1/Can2Mb/src/app/can2mb_init" -I"C:/Users/Admin/e2_studio/workspace4.1/Can2Mb/src/app/canopen/init" -I"C:/Users/Admin/e2_studio/workspace4.1/Can2Mb/src/app/canopen/od" -I"C:/Users/Admin/e2_studio/workspace4.1/Can2Mb/src/app/mb_app" -I"C:/Users/Admin/e2_studio/workspace4.1/Can2Mb/src/app/canopen/od/comm_od" -I"C:/Users/Admin/e2_studio/workspace4.1/Can2Mb/src/app/canopen/od/manu_od" -I"C:/Users/Admin/e2_studio/workspace4.1/Can2Mb/src/service/fifo" -std=c99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" -x c "$<")
	@echo Building file: $< && arm-none-eabi-gcc @"$@.in"

