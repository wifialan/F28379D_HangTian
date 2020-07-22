/**********************头文件**********************/
#include "cfft_icfft.h"
#include "demod_fsk.h"

/**********************函数实体**********************/
void xCorr(float *fft_sigx, const float *fft_sigy)
{
    /*
     * 互相关运算
     * 将接收信号fft_sigx[CFFT_SIZE]与本地信号fft_sigy[CFFT_SIZE]做互相关，获取相关峰
     * 结果存放在fft_sigx[CFFT_SIZE]中
     * 使用FFT运算方法，注意FFT函数随芯片变化而变化，注意适配
     * 对本地信号做FFT后取共轭，这样运算的结果为fft_sigx相对fft_sigy的时延
     */
    int counter;
    for(counter = 0; counter < CFFT_SIZE; counter++)
    {
        /* FFT之后，求FFT_x的共轭与FFT_y的乘积
         * x = a + jb; y = c + jd
         * conj(x).*y = (ac + bd) + j(ad - bc)
         * x.*conj(y) = (ac + bd) + j(bc - ad)
         */
        float a, b, c, d;
        a = fft_sigx[2 * counter];
        b = fft_sigx[2 * counter +  1];
        c = fft_sigy[2 * counter];
        d = fft_sigy[2 * counter + 1];

        fft_sigx[2 * counter] = a*c + b*d;
        fft_sigx[2 * counter + 1] = b*c - a*d;
    }
}
void maxValue(maxStruct *ptrMax, const float *ptrData, int ArraySize)
{
    /* 最大值找寻
     * 函数接收3个参数：
     *      ptrMax 为 maxStruct 结构体指针
     *      ptrData 为目标数组指针
     *      ArraySize 为目标数组大小
     * 函数找寻ptrData中的最大值，并将最大值Value及Location存入ptrMax中
     */
    int counter = 0;
    ptrMax->Val = -1;
    ptrMax->Loc = -1;
    for (counter = 0; counter < ArraySize; counter ++)
    {
        if(ptrMax->Val < ptrData[counter])
        {
            ptrMax->Val = ptrData[counter];
            ptrMax->Loc = counter;
        }
    }
}
int isPeak(float ptrMaxVal, float *ptrData, int ArraySize)
{
    /*
     * 峰值判定
     * 峰值判定的依据是：
     * 整个数据段中超过最大值1/3的数据低于一定的门限时，
     * 判定该最大值为峰值
     * 峰值判定的算法仍需要极大的改进，要么添加更多的判别方法互相佐证，
     * 要么采用更加数学的办法
     */
    int bigger_counter = 0;
    int counter;
    ptrMaxVal = ptrMaxVal / 3;
    for(counter=0; counter < ArraySize; counter++)
    {
        if(ptrMaxVal < ptrData[counter])
        {
            bigger_counter++;
        }
    }
    if(bigger_counter <= 200 && bigger_counter > 5) //大于3的设定是为了防止数组中均为0或某一个数持续出现的情况
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
float SquareLawDetection(float * data, const float * desin, const float * decos, int ArraySize)
{
    /*
     * 包络检波
     * 函数接收4个参数
     *      数组指针data： 需要解调的fsk信号段，长度为一个码元长度
     *      数组指针desin、decos：非相干解调信号，与data同频同长度的sin，cos信号
     *      整形ArraySize： 数组长度
     */
    int counter = 0;
    float desintemp = 0, decostemp = 0, powtemp = 0;
    while(counter < ArraySize)
    {
        decostemp = data[counter] * decos[counter] + decostemp;
        desintemp = data[counter] * desin[counter] + desintemp;
        counter++;
    }

    powtemp = decostemp * decostemp + desintemp * desintemp;
    return powtemp;
}

void bin2hex(unsigned char* ptrHex, const int* ptrData, int ArraySize)
{
    /*
     * 2进制转16进制
     * 函数接收正序输入的2进制数组指针 ptrData，
     * 即数组下标越低，其阶数越高，每4位转换为
     * 16进制字符数组ptrHex；
     * ArraySize 为ptrData指向的数组的大小
     */
    int counter, decTemp, i = 0;
    for(counter = 0; counter < ArraySize; counter = counter + 4)
    {
        decTemp = ptrData[counter + 3] + 2 * ptrData[counter + 2] + 4 * ptrData[counter + 1] + 8 * ptrData[counter];
        if(decTemp < 10)
        {
            ptrHex[i] = '0' + decTemp;
        }
        else
        {
            ptrHex[i] = 'A' + decTemp - 10;
        }
        i++;
    }
}

void hex2int(unsigned char* ptrchar,uint32_t *ptrint)
{
    unsigned char i;
    uint32_t tmp_data=0;
    *ptrint = 0;// clear origin data
    for (i = 0; i < 8; ++i) {
        if (ptrchar[i]>='0'&&ptrchar[i]<='9') {         //'0' is 48
            tmp_data = (ptrchar[i]-48)<<(1*(7-i));
        } else if (ptrchar[i]>='A'&&ptrchar[i]<='F') {//'A' is 65
            tmp_data = (ptrchar[i]-55)<<(1*(7-i));
        }
        tmp_data = tmp_data << (3*(7-i));
        *ptrint |= tmp_data;
    }
}
