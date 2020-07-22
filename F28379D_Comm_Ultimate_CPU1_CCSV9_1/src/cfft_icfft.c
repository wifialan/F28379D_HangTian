#include <stdio.h>                  // C 语言标准输入输出函数库
#include <math.h>                   // C 数学函数库
#include "fpu_cfft.h"
#include "cfft_icfft.h"


#ifdef __cplusplus
#pragma DATA_SECTION("CFFTdata1")
#else
#pragma DATA_SECTION(CFFTin1Buff,"CFFTdata1")
#endif //__cplusplus
//! \brief FFT Calculation Buffer
//! \note The input buffer needs to be aligned to a 4N word boundary
//! \note If the number of FFT stages is odd, the result of the FFT will
//! be written to this buffer
//!
float CFFTin1Buff[CFFT_SIZE*2];

#ifdef __cplusplus
#pragma DATA_SECTION("CFFTdata3")
#else
#pragma DATA_SECTION(CFFToutBuff,"CFFTdata3")
#endif //__cplusplus
//! \brief FFT Calculation Buffer
//! \note If the number of FFT stages is even, the result of the FFT will
//! be written to this buffer
//!
float CFFToutBuff[CFFT_SIZE*2];

#ifdef __cplusplus
#pragma DATA_SECTION("CFFTdata4")
#else
#pragma DATA_SECTION(CFFTF32Coef,"CFFTdata4")
#endif //__cplusplus
//! \brief Twiddle Factors
//!
float CFFTF32Coef[CFFT_SIZE];


CFFT_F32_STRUCT cfft;
CFFT_F32_STRUCT_Handle hnd_cfft = &cfft;

//extern uint16_t  FFTTwiddlesRunStart;
//extern uint16_t  FFTTwiddlesLoadStart;
//extern uint16_t  FFTTwiddlesLoadSize;

void cFFT(float* cfft_IO)
{
    /*
     * 复数快速傅里叶变换cFFT
     * 输入输出均存放在cfft_IO[CFFT_SIZE*2]中
     * 偶数项为实部，奇数项为虚部。
     * CurrentInPtr[0] = real[0]
     * CurrentInPtr[1] = imag[0]
     */

    int i;
    for(i=0;i<CFFT_SIZE*2;i++)
    {
        CFFTin1Buff[i]=cfft_IO[i];
    }
    //Input/output or middle stage of ping-pong buffer
    hnd_cfft->InPtr = CFFTin1Buff;
    //Output or middle stage of ping-pong buffer
    hnd_cfft->OutPtr = CFFToutBuff;
    hnd_cfft->Stages = CFFT_STAGES; // FFT stages
    hnd_cfft->FFTSize = CFFT_SIZE; // FFT size
    hnd_cfft->CoefPtr = CFFTF32Coef; //Twiddle factor table
    CFFT_f32_sincostable(hnd_cfft); // Calculate twiddle factor
    CFFT_f32(hnd_cfft);
    /*
     * hnd_cfft->CoefPtr = CFFT_f32_twiddleFactors; // Twiddle factor table
     * CFFT_f32t(hnd_cfft); // Calculate FFT
     * Depending on the number of FFT stages, the final output will be in either CFFTin1Buff (#Stages is odd(奇数)) or CFFToutBuff (#stages is even(偶数)).
     *
     * If number of Stages is ODD(奇数),
     *   currentInPtr=CFFTin1Buff, (Result of CFFT_f32)
     *   currentOutPtr=CFFToutBuff
     * If number of Stages is EVEN(偶数),
     *   currentInPtr=CFFToutBuff, (Result of CFFT_f32)
     * currentOutPtr=CFFTin1Buff
     */
    for(i=0;i<CFFT_SIZE*2;i++)
    {
        cfft_IO[i]=hnd_cfft->CurrentInPtr[i];
    }
}

void icFFT(float* icfft_IO)
{
    /*
     * 复数快速逆傅里叶变换icFFT
     * 输入输出均存放在cfft_IO[CFFT_SIZE*2]中
     * 偶数项为实部，奇数项为虚部。
     * CurrentInPtr[0] = real[0]
     * CurrentInPtr[1] = imag[0]
     */
    int i;
    for(i=0;i<CFFT_SIZE*2;i++)
    {
        CFFTin1Buff[i]=icfft_IO[i];
    }
    hnd_cfft->InPtr = CFFTin1Buff; /* Input data buffer */
    hnd_cfft->OutPtr = CFFToutBuff; /* FFT output buffer */
    hnd_cfft->CoefPtr = CFFTF32Coef; /* Twiddle factor buffer */
    hnd_cfft->FFTSize = CFFT_SIZE; /* FFT length */
    hnd_cfft->Stages = CFFT_STAGES; /* FFT Stages */
    CFFT_f32_sincostable(hnd_cfft); /* Initialize twiddle buffer */
    ICFFT_f32(hnd_cfft); /* Calculate Inverse FFT */
    for(i=0;i<CFFT_SIZE*2;i++)
    {
        icfft_IO[i]=hnd_cfft->CurrentInPtr[i];
    }
}
