/*
 * @Description:
 * @Author: MALossov
 * @Date: 2022-04-03 14:32:15
 * @LastEditTime: 2022-04-03 21:35:31
 * @LastEditors: MALossov
 * @Reference:
 */
#include "USART_IT.h"

extern uint8_t* rxPtr;
extern uint8_t aRxBuffer[2];
extern uint8_t rxFlg;
extern uint8_t rxStr[200];
extern uint8_t sendFlag;
uint8_t stopADC;
extern uint32_t ADC_Values[1024];

extern DataCTL uiDtc;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart)
{
    HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);

    if (!stopADC) {
        HAL_ADC_Stop_DMA(&hadc1); //关闭adc
        stopADC = 1;
    }

    if ((!rxFlg) && *aRxBuffer == '$') {
        rxFlg = 1;
    }
    else if (rxFlg && *aRxBuffer == '#') {
        rxFlg = 0;
        *rxPtr = '\0';
        //printf("%s", rxStr);
        //sendFlag = 1;
        OLED_Clear();
        OLED_ShowString(0, 4, rxStr, 16);
        rxPtr = rxStr;
    }
    else if (rxFlg && *aRxBuffer != '#' && *aRxBuffer != '$') {
        *rxPtr = *aRxBuffer;
        rxPtr++;
    }


    if (*huart1.pRxBuffPtr == 0x00) {
        HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);
        stopADC = 0;
        HAL_ADC_Start_DMA(&hadc1, ADC_Values, 1024);
    }

    if (HAL_UART_Receive_IT(&huart1, aRxBuffer, 1) != HAL_OK)
    {
        do {
            if (huart1.gState == HAL_UART_STATE_BUSY_TX)
            {
                continue;
            }
        } while (HAL_UART_Receive_IT(&huart1, aRxBuffer, 1) != HAL_BUSY);
    }
}

void Init_DataCTL() {
    uiDtc.dw = AUTO;
    uiDtc.fd = 1;
    uiDtc.sz = 1;
    uiDtc.py = 75;
    uiDtc.sj.choice = 1;
    uiDtc.sj.cy = DEFALULT;
    uiDtc.xs = LX;
}

void sendWav() {
    if (uiDtc.xs = DC) {
        for (int i = 0;i < 325;i++) {
            for (int j = 0;j < uiDtc.sz;j++) {
                uartWavCache[i] = (uint8_t)((ADC_Values[i] >> (5 + uiDtc.fd) + uiDtc.py - 75));
            }
            i += uiDtc.sz - 1;
        }

        HAL_UART_Transmit(&huart1, uartWavStr, strlen(uartWavStr), 1000);

        HAL_UART_Transmit(&huart1, "addt 1,0,325\xff\xff\xff", 15, 50);
        while (*aRxBuffer != 0xFE || *aRxBuffer != 0xFF);
        HAL_UART_Transmit(&huart1, uartWavCache, 330, 100);
        HAL_UART_Transmit(&huart1, "\xff\xff\xff", 3, 10);
        while (*aRxBuffer != 0xFF || *aRxBuffer != 0xFD);
    }
    else {
        for (int i = 0;i < 320;i++) {
            for (int j = 0;j < uiDtc.sz;j++) {
                printf("add 1,0,%d\xff\xff\xff", ((ADC_Values[i] >> (5 + uiDtc.fd) + uiDtc.py - 75)));
            }
            i += uiDtc.sz - 1;
        }
    }
}