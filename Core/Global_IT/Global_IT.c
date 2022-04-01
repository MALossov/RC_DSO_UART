/*
 * @Description:
 * @Author: MALossov
 * @Date: 2022-03-25 23:12:52
 * @LastEditTime: 2022-04-01 20:00:19
 * @LastEditors: MALossov
 * @Reference:
 */
#include "Global_IT.h"

uint32_t FFT_in[1024];     // fft输入
uint32_t FFT_out[512];     // fft输出
uint32_t FFT_out_mag[512]; // fft振幅
uint8_t str[40] = "55";
uint32_t sampling_rate_list[3] = { 1000, 5000, 10000 };
uint16_t tim3period[3] = { 999, 199, 99 };
extern uint8_t uartWavStr[10000];
uint8_t tmpWavPnt[20];

short save_statue = 0;

extern uint32_t ADC_Values[1024];
extern uint8_t aRxBuffer[2];

int fputc(int ch, FILE* f)
{
    uint8_t temp[1] = { ch };
    HAL_UART_Transmit(&huart1, temp, 1, 2);
    return 0;
}

void dsp_asm_powerMag(int32_t* IBUFOUT) //求谐波幅值
{
    s16 lx, ly;
    u32 i;
    for (i = 0; i < 1024 / 2; i++) //由于FFT的频域结果是关于奈奎斯特频率对称的，所以只计算一半
    {
        lx = (IBUFOUT[i] << 16) >> 16; //取低十六位，虚部
        ly = (IBUFOUT[i] >> 16);       //取高十六位，实部
        float X = 1024 * ((float)lx) / 32768;
        float Y = 1024 * ((float)ly) / 32768;
        float Mag = sqrt(X * X + Y * Y) / 1024;
        FFT_out_mag[i] = (u32)(Mag * 65536);
    }                                    //这些就是计算振幅IBUFOUT[i] = sqrt(lx*lx+ly*ly)*2/NPT
                                         //之所以先除以32768后乘以65536是为了符合浮点数的计算规律
    FFT_out_mag[0] = FFT_out_mag[0] / 2; //这个是直流分量，不需要乘以2
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    HAL_GPIO_TogglePin(LED0_GPIO_Port, LED0_Pin);
    HAL_ADC_Stop_DMA(&hadc1); //关闭adc

    HAL_UART_Transmit(&huart1, uartWavStr, strlen(uartWavStr), 1000);


    static uint8_t choice = 2, times = 0;
    static uint32_t sampling_rate = 10000;
    uint32_t max_f = 0, max_mag = 0, min_mag = 0; //最大振幅和相应频率和最小振幅
    uint32_t mid_mag = 0, duty_point = 0;         //电压中间值，高于间值的点数
    float max_f_r, vpp = 0, duty_cycle = 0;
    ; //真实的频率,峰峰值，占空比
    /*************************FFT********************************/
    for (uint16_t i = 0; i < 1024; i++)
    {
        FFT_in[i] = ((s16)ADC_Values[i] << 16);
    }                                          //转化
    cr4_fft_1024_stm32(FFT_out, FFT_in, 1024); // fft
    dsp_asm_powerMag(FFT_out);                 //求幅值
    for (int i = 0; i < 512; i++)              //造出最大的振幅和相应序号
    {
        if (max_mag < FFT_out_mag[i] && i != 0)
        {
            max_mag = FFT_out_mag[i];
            max_f = i;
        }
    }
    /********************计算峰峰值*******************************/
    max_mag = min_mag = ADC_Values[1]; //计算峰峰值
    for (int i = 0; i < 1024; i++)     //计算峰峰值
    {
        if (ADC_Values[i] > max_mag)
            max_mag = ADC_Values[i];
        else if (ADC_Values[i] < min_mag)
            min_mag = ADC_Values[i];
    }
    vpp = (float)((max_mag - min_mag) * 3.3 / 4096);
    /******************************计算占空比*********************************/
    mid_mag = (max_mag + min_mag) / 2; //找出中间值
    for (int i = 0; i < 1024; i++)
    {
        if (ADC_Values[i] > mid_mag)
            duty_point++;
    }
    duty_cycle = (float)((duty_point / 1024.0) * 100); //计算占空比
    /*************************计算实际频率*******************************/
    max_f_r = (float)max_f * sampling_rate / 1024; //计算实际频率
    /********************************输出**(vpp峰峰值，max_f_r频率，duty_cycle占空比)(ADCvalues:ADC采样值)***********************/
    OLED_Clear();
    sprintf(str, "Vpp=%.2fV,f=%.1fHZ,d:%.1f%%", vpp, max_f_r, duty_cycle); //实际电压和频率
    //printf("%s", str);
    OLED_ShowString(0, 0, str, 16);
    /******************************调档*****************************/
    if (max_f_r > 700)
        choice = 2;
    else if (max_f_r > 200)
        choice = 1;
    else
        choice = 0; //分档
    if (times < 10)
    {
        if (times == 8)
        {
            sampling_rate = 10000;
            htim3.Instance->ARR = 99;
        } //改档前，先使用10k采两次，防止采样率从低到高引发问题
        times++;
    }
    else
    {
        times = 0;
        sampling_rate = sampling_rate_list[choice];
        htim3.Instance->ARR = tim3period[choice];
    } //改档
    /****************************************************************/


   // 进行一波串口的DMA转换和发送
    // strcat(uartWavStr, endChar);
    // for (int i = 0;i < 320;i++) {
    //     sprintf(uartWavStr, "%s%x", uartWavStr, (uint8_t)(ADC_Values[i] >> 5));
    // }
    // sprintf(tmpWavPnt, "%x%s", (uint8_t)(ADC_Values[320] >> 5), endChar);
    // strcat(uartWavStr, tmpWavPnt);
    // HAL_UART_Transmit_DMA(&huart1, uartWavStr, strlen(uartWavStr));

    // sprintf(uartWavStr, "add 1,0,320%s", endChar);
    //while (huart1.gState != HAL_UART_STATE_READY);
    for (int i = 0;i < 320;i++)
        printf("add 1,0,%d\xff\xff\xff", (uint8_t)(ADC_Values[i] >> 5));

    HAL_ADC_Start_DMA(&hadc1, ADC_Values, 1024); //开启adc
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart)
{
    if (huart->Instance == &huart1)
    {
        printf("%c", aRxBuffer[0]);

        HAL_UART_Receive_IT(&huart1, &aRxBuffer, 1);
    }

}