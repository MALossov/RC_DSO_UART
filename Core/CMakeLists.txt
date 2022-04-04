cmake_minimum_required(VERSION 3.21)
project(Core C)

set(CMAKE_C_STANDARD 11)

include_directories(.)
include_directories(DSP)
include_directories(Global_IT)
include_directories(Inc)
include_directories(OLED_IIC)
include_directories(Src)
include_directories(USART_IT)
include_directories(../Drivers/STM32F1xx_HAL_Driver/Inc)
include_directories(../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy)
include_directories(../Drivers/CMSIS/Include)
include_directories(D:/Program/Keil/ARM/ARMCLANG/include)
link_libraries(../Drivers/CMSIS/Lib)

add_executable(Core
        DSP/stm32_dsp.h
        DSP/table_fft.h
        Global_IT/Global_IT.c
        Global_IT/Global_IT.h
        Inc/adc.h
        Inc/dac.h
        Inc/dma.h
        Inc/gpio.h
        Inc/i2c.h
        Inc/main.h
        Inc/stm32f1xx_hal_conf.h
        Inc/stm32f1xx_it.h
        Inc/tim.h
        Inc/usart.h
        OLED_IIC/oled.c
        OLED_IIC/oled.h
        OLED_IIC/oledfont.h
        Src/adc.c
        Src/dac.c
        Src/dma.c
        Src/gpio.c
        Src/i2c.c
        Src/main.c
        Src/stm32f1xx_hal_msp.c
        Src/stm32f1xx_it.c
        Src/system_stm32f1xx.c
        Src/tim.c
        Src/usart.c
        USART_IT/USART_IT.c
        USART_IT/USART_IT.h)
