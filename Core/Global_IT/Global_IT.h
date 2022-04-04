/*
 * @Description:
 * @Author: MALossov
 * @Date: 2022-03-25 23:12:36
 * @LastEditTime: 2022-04-04 22:41:31
 * @LastEditors: MALossov
 * @Reference:
 */

#ifndef GLOBAL_IT_H
#define GLOBAL_IT_H 

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
#include "USART_IT.h"

void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef* huart);
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc);
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim);
void SendTxtData(float max_f_r, float vpp, float duty_cycle, uint8_t choice);
uint8_t	get_maxandmin(uint32_t* max_mag, uint32_t* min_mag);
#endif // !GLOBAL_IT_H