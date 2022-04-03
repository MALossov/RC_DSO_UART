/*
 * @Description:
 * @Author: MALossov
 * @Date: 2022-04-03 14:32:04
 * @LastEditTime: 2022-04-03 21:26:18
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
#include "stdlib.h"
#include "tim.h"
#include "string.h"
#include "dma.h"
#include "Global_IT.h"

 //采样结构体：放大器，默认
typedef enum {
    NUL,    //空_采样子控制
    FD, //放大
    DEFALULT,   //默认
}CY;

//档位结构体
typedef enum {
    NUL_DW, //空_档位
    FAST,   //快速
    BALENCE,    //平衡
    SLOW,   //慢速
    AUTO    //自动
}DW;

//显示结构体：单次，连续
typedef enum {
    NUL_XS, //空
    DC, //单次
    LX, //连续
}XS;

//理想波形结构体
typedef enum {
    NUL_LX,
    ZX, //正弦波
    FB, //方波
}LXX;

//数据结构体：采样和理想的综合
typedef struct {
    int choice; //选择了哪个结构体:0:理想，1:采样；
    LXX lx;  //理想波形的控制面板
    CY cy;  //采样控制面板
}SJ;

typedef struct {
    DW dw;//= AUTO; //档位
    uint8_t sz;//= 1;//时轴
    uint8_t fd;//= 1;//放大
    XS xs;//= 2;    //显示模式
    SJ sj;  //数据模式，使用数据结构体
    uint8_t py; //偏移量
}DataCTL;



void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart);
void Init_DataCTL();