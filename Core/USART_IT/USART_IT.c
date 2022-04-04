/*
 * @Description:
 * @Author: MALossov
 * @Date: 2022-04-03 14:32:15
 * @LastEditTime: 2022-04-04 14:18:20
 * @LastEditors: MALossov
 * @Reference:
 */

#include "USART_IT.h"

extern uint8_t* rxPtr;
extern uint8_t aRxBuffer[2];
extern uint8_t rxFlg;
extern uint8_t rxStr[50];
uint8_t* tmpCMDCmpr[50];
extern uint8_t sendFlag;
uint8_t stopADC;
uint8_t dctlName[5];    //操作所选项目的名称
uint8_t dctlOpt;    //对于所选项目的操作

extern uint32_t ADC_Values[1024];

//字符串发送相关函数
extern uint8_t uartWavStr[10000];
extern uint8_t uartWavCache[500];

extern uint8_t sinTableLX[384];

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
        ChkStr2DTC();
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

void SendWav() {
    if (uiDtc.xs == DC) {
        for (int i = 0;i < 325;i++) {
            for (int j = 0;j < uiDtc.sz;j++) {
                uartWavCache[i] = (uint8_t)(((ADC_Values[i] >> (5 + uiDtc.fd)) + uiDtc.py - 75));
            }
            i += uiDtc.sz - 1;
        }

        HAL_UART_Transmit(&huart1, uartWavStr, sizeof(uartWavStr), 1000);

        HAL_UART_Transmit(&huart1, "addt 1,0,320\xff\xff\xff", 15, 50);
        while (*aRxBuffer != 0xFE || *aRxBuffer != 0xFF);
        HAL_UART_Transmit(&huart1, uartWavCache, 320, 100);
        HAL_UART_Transmit(&huart1, "\xff\xff\xff", 3, 10);
        while (*aRxBuffer != 0xFF || *aRxBuffer != 0xFD);
    }
    else {
        for (int i = 0;i < 320;i++) {
            for (int j = 0;j < uiDtc.sz;j++) {
                printf("add 1,0,%d\xff\xff\xff", (((ADC_Values[i] >> (5 + uiDtc.fd)) + uiDtc.py - 75)));
            }
            i += uiDtc.sz - 1;
        }
    }
}

void ChkStr2DTC() {
    if (strcmp(rxStr, tmpCMDCmpr) == 0)
        return;

    sscanf(rxStr, "%s-%c", dctlName, &dctlOpt);

    switch (*dctlName) {
    case 'H':   //滑块，偏移量
        uiDtc.py = dctlOpt;
        break;
    case 'S':   //时轴偏量
        uiDtc.sz = dctlOpt;
        break;
    case 'D':   //档位
        uiDtc.dw = dctlOpt;
        break;
    case 'F':   //放大倍数
        uiDtc.fd = dctlOpt;
        break;
    case 'X':   //显示
        uiDtc.xs = dctlOpt;
    case 'L':   //数据：理想
        uiDtc.sj.lx = dctlOpt;
        break;
    case 'C':   //采样的情况：带不带放大器
        uiDtc.sj.cy = dctlOpt;
        break;
    case 'M':   //数据模式
        uiDtc.sj.choice = dctlOpt;
        break;
    }
    strcpy(tmpCMDCmpr, rxStr);
}

void SendTable(float duty) {
    if (uiDtc.sj.lx == 2) { //如果理想输出的波形为方波时
        for (int Round = 0;Round < 4;Round++) {
            for (int RoundPnt = 0;RoundPnt < 100;RoundPnt++) {
                for (int j = 0;j < uiDtc.sz;j++) {
                    if (RoundPnt < duty)
                        uartWavCache[RoundPnt + Round * 100] = 100;
                    else uartWavCache[RoundPnt + Round * 100] = 0;
                }
                RoundPnt += uiDtc.sz - 1;
            }
        }
        if (uiDtc.xs == DC) {   //如果为单次出波时

            HAL_UART_Transmit(&huart1, uartWavStr, sizeof(uartWavStr), 1000);
            HAL_UART_Transmit(&huart1, "addt 1,0,325\xff\xff\xff", 15, 50);
            while (*aRxBuffer != 0xFE || *aRxBuffer != 0xFF);
            HAL_UART_Transmit(&huart1, uartWavCache, 330, 100);
            HAL_UART_Transmit(&huart1, "\xff\xff\xff", 3, 10);
            while (*aRxBuffer != 0xFF || *aRxBuffer != 0xFD);
        }
        else {
            for (int i = 0;i < 320;i++) {
                printf("add 1,0,%d\xff\xff\xff", uartWavCache[i]);
            }
        }
    }
    else {  //如果理想波形为正弦波时
        if (uiDtc.xs == DC) {
            HAL_UART_Transmit(&huart1, "addt 1,0,320\xff\xff\xff", 15, 50);
            while (*aRxBuffer != 0xFE || *aRxBuffer != 0xFF);
            HAL_UART_Transmit(&huart1, sinTableLX, 320, 100);
            HAL_UART_Transmit(&huart1, "\xff\xff\xff", 3, 10);
            while (*aRxBuffer != 0xFF || *aRxBuffer != 0xFD);
        }
        else {
            for (int i = 0;i < 320;i++) {
                printf("add 1,0,%d\xff\xff\xff", sinTableLX[i]);
            }
        }
    }
}