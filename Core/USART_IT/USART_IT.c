/*
 * @Description:
 * @Author: MALossov
 * @Date: 2022-04-03 14:32:15
 * @LastEditTime: 2022-04-13 21:30:08
 * @LastEditors: MALossov
 * @Reference:
 */

#include "USART_IT.h"

extern UART_HandleTypeDef huart1;

extern uint8_t* rxPtr;
extern uint8_t aRxBuffer[2];
extern uint8_t rxFlg;
extern uint8_t rxStr[50];
uint8_t tmpCMDCmpr[50];
extern uint8_t sendFlag;
uint8_t stopADC;
uint8_t dctlName[5];    //操作所选项目的名称
uint8_t dctlOpt;    //对于所选项目的操作

extern uint32_t ADC_Values[1024];
extern uint32_t min_mag;

//字符串发送相关函数
extern uint8_t uartWavStr[1000];
extern uint8_t uartWavCache[1000];

extern uint8_t sinTableLX[384];

extern DataCTL uiDtc;

#define dctlOpt rxStr[3]

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
        ChkStr2DTC();
        OLED_ShowString(64, 6, rxStr, 16);
        rxPtr = rxStr;
    }
    else if (rxFlg && *aRxBuffer != '#' && *aRxBuffer != '$') {
        *rxPtr = *aRxBuffer;
        rxPtr++;
    }


    if (*huart2.pRxBuffPtr == 0x00) {
        HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);
        stopADC = 0;
        //ChkStr2DTC();
        HAL_ADC_Start_DMA(&hadc1, ADC_Values, 1024);
    }

    GetUARTReceiveITWork();
}

void Init_DataCTL() {
    uiDtc.dw = AUTO;
    uiDtc.fd = 1;
    uiDtc.sz = 1;
    uiDtc.py = 0;
    uiDtc.sj.choice = 2;
    uiDtc.sj.cy = DEFALULT;
    uiDtc.xs = DC;
}

void SendWav() {
    if (uiDtc.xs == DC) {
        static uint8_t tmpRnd;
        tmpRnd = uiDtc.sz;
        for (int i = 0;i < 400 / tmpRnd + 1;i++) {
            for (int j = 0;j < uiDtc.sz;j++) {
                uartWavCache[i * tmpRnd + j] = (uint8_t)(((((ADC_Values[i] >> 6) - (min_mag >> 6)) << uiDtc.fd) + uiDtc.py));
            }
        }

        //HAL_UART_Transmit(&huart1, uartWavStr, sizeof(uartWavStr), 400);

        HAL_UART_Transmit(&huart1, "addt 1,0,400\xff\xff\xff", 15, 50);
        HAL_UART_Transmit(&huart1, uartWavCache, 400, 100);
        HAL_UART_Transmit(&huart1, "\xff\xff\xff", 3, 10);
    }
    else {
        HAL_UART_Transmit(&huart1, uartWavStr, 320, 400);

        for (int i = 0;i < 400 / uiDtc.sz + 1;i++) {
            for (int j = 0;j < uiDtc.sz;j++) {
                printf("add 1,0,%d\xff\xff\xff", (uint8_t)(((((ADC_Values[i] >> 6) - (min_mag >> 6)) << uiDtc.fd) + uiDtc.py)));
            }
        }
    }
}

void ChkStr2DTC() {
    if (rxStr[0] == tmpCMDCmpr[0] && rxStr[3] == tmpCMDCmpr[3])
        return;

    //sscanf(rxStr, "%s\x2D%c", dctlName, &dctlOpt);    //以-分隔字符串
    // static uint8_t* strp;
    // strp = rxStr;

    // for (uint8_t tmp = 0;*strp != '-';tmp++) {
    //     dctlName[tmp] = *strp;
    //     strp++;
    // }
    // dctlOpt = *(++strp);

    dctlName[0] = rxStr[0];
    dctlName[1] = rxStr[1];


    switch (dctlName[0]) {
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
    case 'L':   //数据：理想：波形的选择
        uiDtc.sj.lx = dctlOpt;
        break;
    case 'C':   //采样的情况：带不带放大器
        uiDtc.sj.cy = dctlOpt;
        break;
    case 'M':   //数据模式
        uiDtc.sj.choice = dctlOpt;
        break;
    case 'R':
        uiDtc.Reset = 1;
        break;
    default:
        break;
    }
    tmpCMDCmpr[0] = rxStr[0];
    tmpCMDCmpr[3] = rxStr[3];
}

void SendTable(float duty) {
    if (uiDtc.sj.lx == 2) { //如果理想输出的波形为方波时
        //HAL_UART_Transmit(&huart1, uartWavStr, sizeof(uartWavStr), 400);

        for (int Round = 0;Round < 4;Round++) {
            for (int RoundPnt = 0;RoundPnt < 100;RoundPnt++) {
                if (RoundPnt < duty)
                    uartWavCache[RoundPnt + Round * 100] = 100;
                else uartWavCache[RoundPnt + Round * 100] = 0;
            }
        }
        if (uiDtc.xs == DC) {   //如果为单次出波时

            //HAL_UART_Transmit(&huart1, uartWavStr, sizeof(uartWavStr), 400);
            HAL_UART_Transmit(&huart1, "addt 1,0,320\xff\xff\xff", 15, 50);
            //while (*aRxBuffer != 0xFE || *aRxBuffer != 0xFF);
            HAL_UART_Transmit(&huart1, uartWavCache, 330, 100);
            HAL_UART_Transmit(&huart1, "\xff\xff\xff", 3, 10);
            //while (*aRxBuffer != 0xFF || *aRxBuffer != 0xFD);
        }
        else {
            for (int i = 0;i < 400;i++) {
                printf("add 1,0,%d\xff\xff\xff", uartWavCache[i]);
            }
        }
    }
    else {  //如果理想波形为正弦波时
        if (uiDtc.xs == DC) {
            HAL_UART_Transmit(&huart1, "addt 1,0,320\xff\xff\xff", 15, 50);
            //while (*aRxBuffer != 0xFE || *aRxBuffer != 0xFF);
            HAL_UART_Transmit(&huart1, sinTableLX, 320, 100);
            HAL_UART_Transmit(&huart1, "\xff\xff\xff", 3, 10);
            //while (*aRxBuffer != 0xFF || *aRxBuffer != 0xFD);
        }
        else {
            HAL_UART_Transmit(&huart1, uartWavStr, 320, 400);
            for (int i = 0;i < 320;i++) {
                printf("add 1,0,%d\xff\xff\xff", sinTableLX[i]);
            }
        }
    }
}

void GetUARTReceiveITWork() {
    if (HAL_UART_Receive_IT(&huart2, aRxBuffer, 1) != HAL_OK)
    {
        do {
            if (huart2.gState == HAL_UART_STATE_BUSY_TX)
            {
                continue;
            }
        } while (HAL_UART_Receive_IT(&huart2, aRxBuffer, 1) != HAL_BUSY);
    }
}