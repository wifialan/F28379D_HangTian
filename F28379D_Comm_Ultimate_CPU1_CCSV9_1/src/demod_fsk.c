/**********************ͷ�ļ�**********************/
#include "cfft_icfft.h"
#include "demod_fsk.h"

/**********************����ʵ��**********************/
void xCorr(float *fft_sigx, const float *fft_sigy)
{
    /*
     * ���������
     * �������ź�fft_sigx[CFFT_SIZE]�뱾���ź�fft_sigy[CFFT_SIZE]������أ���ȡ��ط�
     * ��������fft_sigx[CFFT_SIZE]��
     * ʹ��FFT���㷽����ע��FFT������оƬ�仯���仯��ע������
     * �Ա����ź���FFT��ȡ�����������Ľ��Ϊfft_sigx���fft_sigy��ʱ��
     */
    int counter;
    for(counter = 0; counter < CFFT_SIZE; counter++)
    {
        /* FFT֮����FFT_x�Ĺ�����FFT_y�ĳ˻�
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
    /* ���ֵ��Ѱ
     * ��������3��������
     *      ptrMax Ϊ maxStruct �ṹ��ָ��
     *      ptrData ΪĿ������ָ��
     *      ArraySize ΪĿ�������С
     * ������ѰptrData�е����ֵ���������ֵValue��Location����ptrMax��
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
     * ��ֵ�ж�
     * ��ֵ�ж��������ǣ�
     * �������ݶ��г������ֵ1/3�����ݵ���һ��������ʱ��
     * �ж������ֵΪ��ֵ
     * ��ֵ�ж����㷨����Ҫ����ĸĽ���Ҫô��Ӹ�����б𷽷�������֤��
     * Ҫô���ø�����ѧ�İ취
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
    if(bigger_counter <= 200 && bigger_counter > 5) //����3���趨��Ϊ�˷�ֹ�����о�Ϊ0��ĳһ�����������ֵ����
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
     * ����첨
     * ��������4������
     *      ����ָ��data�� ��Ҫ�����fsk�źŶΣ�����Ϊһ����Ԫ����
     *      ����ָ��desin��decos������ɽ���źţ���dataͬƵͬ���ȵ�sin��cos�ź�
     *      ����ArraySize�� ���鳤��
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
     * 2����ת16����
     * �����������������2��������ָ�� ptrData��
     * �������±�Խ�ͣ������Խ�ߣ�ÿ4λת��Ϊ
     * 16�����ַ�����ptrHex��
     * ArraySize ΪptrDataָ�������Ĵ�С
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
