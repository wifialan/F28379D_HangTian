/**********************头文件**********************/
#ifndef INC_DEMOD_FSK_H_
#define INC_DEMOD_FSK_H_
#include <stdio.h>                  // C 语言标准输入输出函数库
#include <math.h>                   // C 数学函数库
#include "cfft_icfft.h"
/**********************全局变量**********************/
//#define PI               3.1415926
#define LFM_LENGTH      1600
#define INTERVAL        800         //LFM与FSK之间的间隔长度
#define DATA_NUM        16
#define NSAMP           250         //每个码元的采样点数
#define M               4
#define WIN_LENGTH      800  //滑动窗口长度

typedef struct
{
    float Val;
    int Loc;
}maxStruct;

/**********************函数声明**********************/
void xCorr(float *fft_sigx, const float *fft_sigy);
void maxValue(maxStruct *ptrMax, const float *ptrData, int ArraySize);
int isPeak(float ptrMaxVal, float *ptrData, int ArraySize);
float SquareLawDetection(float * data, const float * desin, const float * decos, int ArraySize);
void bin2hex(unsigned char* ptrHex, const int* ptrData, int ArraySize);
void hex2int(unsigned char* ptrchar,uint32_t* ptrint);
#endif /* INC_DEMOD_FSK_H_ */
