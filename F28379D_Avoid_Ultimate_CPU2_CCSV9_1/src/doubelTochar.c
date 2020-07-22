#include "stdio.h"

void floatTochar(float _data, char *_str, int decimal_point);
//第一个参数为待转换的数据
//第二个参数为保存转换后数据的字符型数组
//第三个参数为保留小数点的位数
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
        //说明_data值小于1
        for(i=0;i<decimal_point;i++)
            dat_float = dat_float * 10;//
        dat_int = dat_float;//强制转换为int型

        for (i = 0; i < decimal_point; ++i) {
            _str[1 + decimal_point - i] = dat_int % 10 + '0';
            dat_int = dat_int / 10;
        }

        _str[0]='0';
        _str[1]='.';
    } else {
        //数据大于1.0
        _data_int_part_len = 0;
        for (i = 0;; ++i) {
            _data_int_part_len ++;
            dat_int = dat_int / 10;
            if(dat_int == 0)
                break;
        }
        _str[_data_int_part_len] = '.';
        //开始转换小数点前面数据
        dat_int = _data;
        for (i = 0; i < _data_int_part_len; ++i) {
            _str[_data_int_part_len - i - 1] = dat_int % 10 + '0';
            dat_int = dat_int / 10;
        }
        //开始转换小数点后面数据
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



