/*
 * @Description:
 * @Author: MALossov
 * @Date: 2022-03-25 23:12:36
 * @LastEditTime: 2022-04-01 09:42:33
 * @LastEditors: MALossov
 * @Reference:
 */


#include "stm32f1xx_it.h"
#include "adc.h"
#include "dac.h"
#include "stdio.h"
#include "stm32f1xx_hal.h"
#include "usart.h"
#include "oled.h"
#include "stm32_dsp.h"
#include "math.h"
#include "stdlib.h"
#include "tim.h"
#include "string.h"


void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef* huart);
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc);
// void HAL_DAC_ConvCpltCallbackCh1(DAC_HandleTypeDef *hdac);