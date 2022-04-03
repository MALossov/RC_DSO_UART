#include "USART_IT.h"

extern uint8_t* rxPtr;
extern uint8_t aRxBuffer[2];
extern uint8_t rxFlg;
extern uint8_t rxStr[200];
extern uint8_t sendFlag;
uint8_t stopADC;
extern uint32_t ADC_Values[1024];

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
    else if (rxFlg && *aRxBuffer != '#'&& *aRxBuffer != '$') {
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