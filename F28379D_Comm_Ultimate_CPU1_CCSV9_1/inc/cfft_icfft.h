#ifndef CFFT_ICFFT_H_
#define CFFT_ICFFT_H_


#include <stdio.h>                  // C 语言标准输入输出函数库
#include <math.h>                   // C 数学函数库
#include "fpu_cfft.h"

#define CFFT_STAGES     10
#define CFFT_SIZE       1024
#define EPSILON         0.01

void cFFT(float* cfft_IO);
void icFFT(float* icfft_IO);

#endif /* CFFT_ICFFT_H_ */
