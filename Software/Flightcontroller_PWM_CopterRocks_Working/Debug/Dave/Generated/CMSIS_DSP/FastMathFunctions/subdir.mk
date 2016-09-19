################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Dave/Generated/CMSIS_DSP/FastMathFunctions/arm_cos_f32.c \
../Dave/Generated/CMSIS_DSP/FastMathFunctions/arm_cos_q15.c \
../Dave/Generated/CMSIS_DSP/FastMathFunctions/arm_cos_q31.c \
../Dave/Generated/CMSIS_DSP/FastMathFunctions/arm_sin_f32.c \
../Dave/Generated/CMSIS_DSP/FastMathFunctions/arm_sin_q15.c \
../Dave/Generated/CMSIS_DSP/FastMathFunctions/arm_sin_q31.c \
../Dave/Generated/CMSIS_DSP/FastMathFunctions/arm_sqrt_q15.c \
../Dave/Generated/CMSIS_DSP/FastMathFunctions/arm_sqrt_q31.c 

OBJS += \
./Dave/Generated/CMSIS_DSP/FastMathFunctions/arm_cos_f32.o \
./Dave/Generated/CMSIS_DSP/FastMathFunctions/arm_cos_q15.o \
./Dave/Generated/CMSIS_DSP/FastMathFunctions/arm_cos_q31.o \
./Dave/Generated/CMSIS_DSP/FastMathFunctions/arm_sin_f32.o \
./Dave/Generated/CMSIS_DSP/FastMathFunctions/arm_sin_q15.o \
./Dave/Generated/CMSIS_DSP/FastMathFunctions/arm_sin_q31.o \
./Dave/Generated/CMSIS_DSP/FastMathFunctions/arm_sqrt_q15.o \
./Dave/Generated/CMSIS_DSP/FastMathFunctions/arm_sqrt_q31.o 

C_DEPS += \
./Dave/Generated/CMSIS_DSP/FastMathFunctions/arm_cos_f32.d \
./Dave/Generated/CMSIS_DSP/FastMathFunctions/arm_cos_q15.d \
./Dave/Generated/CMSIS_DSP/FastMathFunctions/arm_cos_q31.d \
./Dave/Generated/CMSIS_DSP/FastMathFunctions/arm_sin_f32.d \
./Dave/Generated/CMSIS_DSP/FastMathFunctions/arm_sin_q15.d \
./Dave/Generated/CMSIS_DSP/FastMathFunctions/arm_sin_q31.d \
./Dave/Generated/CMSIS_DSP/FastMathFunctions/arm_sqrt_q15.d \
./Dave/Generated/CMSIS_DSP/FastMathFunctions/arm_sqrt_q31.d 


# Each subdirectory must supply rules for building sources it contributes
Dave/Generated/CMSIS_DSP/FastMathFunctions/%.o: ../Dave/Generated/CMSIS_DSP/FastMathFunctions/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM-GCC C Compiler'
	"C:\DAVEv4\DAVE-4.1.2\eclipse\ARM-GCC-49/bin/arm-none-eabi-gcc" -MMD -MT "$@" -DXMC4500_F100x1024 -I"C:\Workspaces\DAVE-4.1\Flightcontroller_PWM_CopterRocks_Working\Libraries\XMCLib\inc" -I"C:\Workspaces\DAVE-4.1\Flightcontroller_PWM_CopterRocks_Working/Libraries/CMSIS/Include" -I"C:\Workspaces\DAVE-4.1\Flightcontroller_PWM_CopterRocks_Working/Libraries/CMSIS/Infineon/XMC4500_series/Include" -I"C:\Workspaces\DAVE-4.1\Flightcontroller_PWM_CopterRocks_Working" -I"C:\Workspaces\DAVE-4.1\Flightcontroller_PWM_CopterRocks_Working\Dave\Generated" -I"C:\Workspaces\DAVE-4.1\Flightcontroller_PWM_CopterRocks_Working\Libraries" -O0 -ffunction-sections -fdata-sections -Wall -std=gnu99 -mfloat-abi=softfp -Wa,-adhlns="$@.lst" -pipe -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d) $@" -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mthumb -g -gdwarf-2 -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo.

