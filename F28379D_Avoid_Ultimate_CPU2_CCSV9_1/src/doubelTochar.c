#include "stdio.h"

void floatTochar(float _data, char *_str, int decimal_point);
//��һ������Ϊ��ת��������
//�ڶ�������Ϊ����ת�������ݵ��ַ�������
//����������Ϊ����С�����λ��
//The interface fucation
//The first parameter is Pending conversion data
//The second parameter is character array that used to save the converted data
//The third parameter is the decimal number that require


void floatTochar(float _data, char *_str, int decimal_point){

    int dat_int = _data;
    int dat_all;
    float dat_float = _data;
    int _data_int_part_len;
    int i;
    if (dat_int == 0) {
        //˵��_dataֵС��1
        for(i=0;i<decimal_point;i++)
            dat_float = dat_float * 10;//
        dat_int = dat_float;//ǿ��ת��Ϊint��

        for (i = 0; i < decimal_point; ++i) {
            _str[1 + decimal_point - i] = dat_int % 10 + '0';
            dat_int = dat_int / 10;
        }

        _str[0]='0';
        _str[1]='.';
    } else {
        //���ݴ���1.0
        _data_int_part_len = 0;
        for (i = 0;; ++i) {
            _data_int_part_len ++;
            dat_int = dat_int / 10;
            if(dat_int == 0)
                break;
        }
        _str[_data_int_part_len] = '.';
        //��ʼת��С����ǰ������
        dat_int = _data;
        for (i = 0; i < _data_int_part_len; ++i) {
            _str[_data_int_part_len - i - 1] = dat_int % 10 + '0';
            dat_int = dat_int / 10;
        }
        //��ʼת��С�����������
        dat_int = _data;
        for(i=0;i<decimal_point;i++)
            dat_float = dat_float * 10;//
        dat_all = dat_float;
        for(i=0;i<decimal_point;i++)
            dat_int = dat_int * 10;//
        dat_int = dat_all - dat_int;
        for (i = 0; i < decimal_point; ++i) {
            _str[_data_int_part_len + decimal_point - i ] = dat_int % 10 + '0';
            dat_int = dat_int / 10;
        }
    }
}



