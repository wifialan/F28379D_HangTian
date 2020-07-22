/**********************ͷ�ļ�**********************/
#ifndef INC_DEMOD_FSK_H_
#define INC_DEMOD_FSK_H_
#include <stdio.h>                  // C ���Ա�׼�������������
#include <math.h>                   // C ��ѧ������
#include "cfft_icfft.h"
/**********************ȫ�ֱ���**********************/
//#define PI               3.1415926
#define LFM_LENGTH      1600
#define INTERVAL        800         //LFM��FSK֮��ļ������
#define DATA_NUM        16
#define NSAMP           250         //ÿ����Ԫ�Ĳ�������
#define M               4
#define WIN_LENGTH      800  //�������ڳ���

typedef struct
{
    float Val;
    int Loc;
}maxStruct;

/**********************��������**********************/
void xCorr(float *fft_sigx, const float *fft_sigy);
void maxValue(maxStruct *ptrMax, const float *ptrData, int ArraySize);
int isPeak(float ptrMaxVal, float *ptrData, int ArraySize);
float SquareLawDetection(float * data, const float * desin, const float * decos, int ArraySize);
void bin2hex(unsigned char* ptrHex, const int* ptrData, int ArraySize);
void hex2int(unsigned char* ptrchar,uint32_t* ptrint);
#endif /* INC_DEMOD_FSK_H_ */
