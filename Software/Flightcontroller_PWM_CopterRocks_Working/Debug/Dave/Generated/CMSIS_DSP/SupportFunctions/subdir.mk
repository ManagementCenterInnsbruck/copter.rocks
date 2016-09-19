################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Dave/Generated/CMSIS_DSP/SupportFunctions/arm_copy_f32.c \
../Dave/Generated/CMSIS_DSP/SupportFunctions/arm_copy_q15.c \
../Dave/Generated/CMSIS_DSP/SupportFunctions/arm_copy_q31.c \
../Dave/Generated/CMSIS_DSP/SupportFunctions/arm_copy_q7.c \
../Dave/Generated/CMSIS_DSP/SupportFunctions/arm_fill_f32.c \
../Dave/Generated/CMSIS_DSP/SupportFunctions/arm_fill_q15.c \
../Dave/Generated/CMSIS_DSP/SupportFunctions/arm_fill_q31.c \
../Dave/Generated/CMSIS_DSP/SupportFunctions/arm_fill_q7.c \
../Dave/Generated/CMSIS_DSP/SupportFunctions/arm_float_to_q15.c \
../Dave/Generated/CMSIS_DSP/SupportFunctions/arm_float_to_q31.c \
../Dave/Generated/CMSIS_DSP/SupportFunctions/arm_float_to_q7.c \
../Dave/Generated/CMSIS_DSP/SupportFunctions/arm_q15_to_float.c \
../Dave/Generated/CMSIS_DSP/SupportFunctions/arm_q15_to_q31.c \
../Dave/Generated/CMSIS_DSP/SupportFunctions/arm_q15_to_q7.c \
../Dave/Generated/CMSIS_DSP/SupportFunctions/arm_q31_to_float.c \
../Dave/Generated/CMSIS_DSP/SupportFunctions/arm_q31_to_q15.c \
../Dave/Generated/CMSIS_DSP/SupportFunctions/arm_q31_to_q7.c \
../Dave/Generated/CMSIS_DSP/SupportFunctions/arm_q7_to_float.c \
../Dave/Generated/CMSIS_DSP/SupportFunctions/arm_q7_to_q15.c \
../Dave/Generated/CMSIS_DSP/SupportFunctions/arm_q7_to_q31.c 

OBJS += \
./Dave/Generated/CMSIS_DSP/SupportFunctions/arm_copy_f32.o \
./Dave/Generated/CMSIS_DSP/SupportFunctions/arm_copy_q15.o \
./Dave/Generated/CMSIS_DSP/SupportFunctions/arm_copy_q31.o \
./Dave/Generated/CMSIS_DSP/SupportFunctions/arm_copy_q7.o \
./Dave/Generated/CMSIS_DSP/SupportFunctions/arm_fill_f32.o \
./Dave/Generated/CMSIS_DSP/SupportFunctions/arm_fill_q15.o \
./Dave/Generated/CMSIS_DSP/SupportFunctions/arm_fill_q31.o \
./Dave/Generated/CMSIS_DSP/SupportFunctions/arm_fill_q7.o \
./Dave/Generated/CMSIS_DSP/SupportFunctions/arm_float_to_q15.o \
./Dave/Generated/CMSIS_DSP/SupportFunctions/arm_float_to_q31.o \
./Dave/Generated/CMSIS_DSP/SupportFunctions/arm_float_to_q7.o \
./Dave/Generated/CMSIS_DSP/SupportFunctions/arm_q15_to_float.o \
./Dave/Generated/CMSIS_DSP/SupportFunctions/arm_q15_to_q31.o \
./Dave/Generated/CMSIS_DSP/SupportFunctions/arm_q15_to_q7.o \
./Dave/Generated/CMSIS_DSP/SupportFunctions/arm_q31_to_float.o \
./Dave/Generated/CMSIS_DSP/SupportFunctions/arm_q31_to_q15.o \
./Dave/Generated/CMSIS_DSP/SupportFunctions/arm_q31_to_q7.o \
./Dave/Generated/CMSIS_DSP/SupportFunctions/arm_q7_to_float.o \
./Dave/Generated/CMSIS_DSP/SupportFunctions/arm_q7_to_q15.o \
./Dave/Generated/CMSIS_DSP/SupportFunctions/arm_q7_to_q31.o 

C_DEPS += \
./Dave/Generated/CMSIS_DSP/SupportFunctions/arm_copy_f32.d \
./Dave/Generated/CMSIS_DSP/SupportFunctions/arm_copy_q15.d \
./Dave/Generated/CMSIS_DSP/SupportFunctions/arm_copy_q31.d \
./Dave/Generated/CMSIS_DSP/SupportFunctions/arm_copy_q7.d \
./Dave/Generated/CMSIS_DSP/SupportFunctions/arm_fill_f32.d \
./Dave/Generated/CMSIS_DSP/SupportFunctions/arm_fill_q15.d \
./Dave/Generated/CMSIS_DSP/SupportFunctions/arm_fill_q31.d \
./Dave/Generated/CMSIS_DSP/SupportFunctions/arm_fill_q7.d \
./Dave/Generated/CMSIS_DSP/SupportFunctions/arm_float_to_q15.d \
./Dave/Generated/CMSIS_DSP/SupportFunctions/arm_float_to_q31.d \
./Dave/Generated/CMSIS_DSP/SupportFunctions/arm_float_to_q7.d \
./Dave/Generated/CMSIS_DSP/SupportFunctions/arm_q15_to_float.d \
./Dave/Generated/CMSIS_DSP/SupportFunctions/arm_q15_to_q31.d \
./Dave/Generated/CMSIS_DSP/SupportFunctions/arm_q15_to_q7.d \
./Dave/Generated/CMSIS_DSP/SupportFunctions/arm_q31_to_float.d \
./Dave/Generated/CMSIS_DSP/SupportFunctions/arm_q31_to_q15.d \
./Dave/Generated/CMSIS_DSP/SupportFunctions/arm_q31_to_q7.d \
./Dave/Generated/CMSIS_DSP/SupportFunctions/arm_q7_to_float.d \
./Dave/Generated/CMSIS_DSP/SupportFunctions/arm_q7_to_q15.d \
./Dave/Generated/CMSIS_DSP/SupportFunctions/arm_q7_to_q31.d 


# Each subdirectory must supply rules for building sources it contributes
Dave/Generated/CMSIS_DSP/SupportFunctions/%.o: ../Dave/Generated/CMSIS_DSP/SupportFunctions/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM-GCC C Compiler'
	"$(TOOLCHAIN_ROOT)/bin/arm-none-eabi-gcc" -MMD -MT "$@" -DXMC4500_F100x1024 -I"$(PROJECT_LOC)/Libraries/XMCLib/inc" -I"$(PROJECT_LOC)/Libraries/CMSIS/Include" -I"$(PROJECT_LOC)/Libraries/CMSIS/Infineon/XMC4500_series/Include" -I"$(PROJECT_LOC)" -I"$(PROJECT_LOC)/Dave/Generated" -I"$(PROJECT_LOC)/Libraries" -O0 -ffunction-sections -fdata-sections -Wall -std=gnu99 -mfloat-abi=softfp -Wa,-adhlns="$@.lst" -pipe -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d) $@" -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mthumb -g -gdwarf-2 -o "$@" "$<" 
	@echo 'Finished building: $<'
	@echo.

