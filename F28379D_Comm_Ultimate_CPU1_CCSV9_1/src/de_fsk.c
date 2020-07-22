/*
 * de_fsk.c
 *
 *  Created on: 2019年12月31日
 *      Author: multimicro
 */
#include "F28x_Project.h"
#include "demod_fsk.h"
#include "adc.h"
#include "data.h"
#include "sys_status.h"
#include "sci.h"

int comm_data[WIN_LENGTH];
int data_in_bin[2 * DATA_NUM];
unsigned char data_in_hex[DATA_NUM / 2];
uint32_t data_in_uint;
float sigx[CFFT_SIZE*2];
float xcorr[CFFT_SIZE];

int status;     //状态字

extern ESCI *scib_pdev;
extern Uint16 results0Index;
extern Uint16 Adca0Results[RESULTS0_BUFFER_SIZE];

Uint16 j=0;

extern struct sys_status sys;
extern void ipc_send_data(uint32_t data);

void demod_fsk()
{
    int counter=0,flag=0,lfm_sp=0,fsk_sp=0,skip_t=0,findit=0;
    int NOdata=0,start_p=0,rp=0,i=0;
    float *sigx_TF,*xcorr_FT;
    float decisionvector[M];
    maxStruct maxval, output;

    status=6;   //初始状态
    NOdata=0;   findit=0;   start_p=0;  rp=0;
    lfm_sp=0;   fsk_sp=0;   skip_t=0;   flag=0;
    for(counter=0;counter<CFFT_SIZE;counter++)
    {
        sigx[2*counter]=0;
        sigx[2*counter+1] = 0;
    }

    //Prepare receive adc data

    sys.demod_state = STATUS_DEMOD_BEGIN;

    //这一层while作用是循环解调，只要系统状态为接收，那么会一直运行
    while(sys.send_or_rece_state == STATUS_RECE){

        if( sys.demod_state == STATUS_DEMOD_BEGIN){

            j++;
            //            for (i = 0; i < 8; ++i) {
            //                data_in_hex[i]=0;
            //            }

            //这一层while是循环解调一组数据，解调完一组数据后，程序内用break跳出
            while(1){
                //    FILE *fp;
                //    fp = fopen("E:\\test2.bin","rb");
                //    if(fp!=NULL){
                ////        fseek(fp,10,SEEK_SET);
                //        fread(comm_data, sizeof(int),1024,fp);
                ////        fscanf(fp,"%d",counter);
                //        fclose(fp);
                //    }

                //comm_data is the adc data

                while(1){
                    if (results0Index == 0) {
                        break;
                    }
                }
                for(i=0;i<800;i++){
                    comm_data[i] = Adca0Results[i];
                }

                if(findit==0){
                    for(counter = 0; counter < WIN_LENGTH; counter++)
                    {
                        int ssp = counter+CFFT_SIZE-WIN_LENGTH;
                        sigx[2*ssp] = comm_data[counter];
                        sigx[2*ssp+1] = 0;
                    }
                    sigx_TF=sigx;
                    cFFT(sigx_TF);
                    xCorr(sigx_TF, lfm_fft);
                    xcorr_FT = sigx_TF;
                    icFFT(xcorr_FT);
                    for(counter = 0; counter < CFFT_SIZE; counter++)
                    {
                        float real,imag;
                        int swp;
                        real = xcorr_FT[2*counter];
                        imag = xcorr_FT[2*counter+1];
                        swp = (counter+CFFT_SIZE/2)%CFFT_SIZE;
                        xcorr[swp] =sqrt(real*real+imag*imag);
                    }
                    maxValue(&maxval, xcorr, CFFT_SIZE);
                    flag = isPeak(maxval.Val, xcorr, CFFT_SIZE);
                    lfm_sp = (maxval.Loc+CFFT_SIZE/2)%CFFT_SIZE-1;
                    if(flag==1 && lfm_sp <WIN_LENGTH)
                    {
                        status=2;       //当前数据有信号
                        findit = 1;
                        fsk_sp = lfm_sp + LFM_LENGTH + INTERVAL+10;
                        skip_t = (fsk_sp - CFFT_SIZE)/WIN_LENGTH;
                        start_p = fsk_sp - CFFT_SIZE;
                        start_p = start_p - WIN_LENGTH *skip_t;
                    }
                    else
                    {
                        status=1;       //当前数据无信号
                        int wp = 2*WIN_LENGTH-CFFT_SIZE;
                        for(counter=0;counter<CFFT_SIZE-WIN_LENGTH;counter++)
                        {
                            sigx[2*counter]=comm_data[counter+wp];
                            sigx[2*counter+1] = 0;
                        }
                        if(sys.send_or_rece_state == STATUS_SEND)
                        {
                            break;
                        }
                    }
                }//if(findit==0)
                else if(skip_t !=0){
                    status=3;       //跳过状态
                    findit=1;
                    skip_t--;
                }
                else//当先数据帧包含fsk数据
                {
                    for(counter=NOdata;counter<DATA_NUM;counter++)
                    {
                        if(start_p+NSAMP>=WIN_LENGTH){
                            status=5;       //数据不足，读取下一帧
                            rp=WIN_LENGTH-start_p;
                            for(i=0;i<rp;i++)
                                sigx[i]=comm_data[start_p+i];
                            NOdata=counter;
                            start_p=-rp;
                            break;
                        }
                        else{//数据足够，开始解调
                            status=4;       //开始解调
                            for(i=rp;i<NSAMP;i++)
                                sigx[i]=comm_data[start_p+i];
                            rp=0;
                            start_p+=NSAMP+50;
                            decisionvector[0] = SquareLawDetection(sigx, F20Sin, F20Cos, NSAMP);
                            decisionvector[1] = SquareLawDetection(sigx, F22Sin, F22Cos, NSAMP);
                            decisionvector[2] = SquareLawDetection(sigx, F24Sin, F24Cos, NSAMP);
                            decisionvector[3] = SquareLawDetection(sigx, F26Sin, F26Cos, NSAMP);
                            maxValue(&output, decisionvector, M);
                            switch(output.Loc)
                            {
                            case 0:
                                data_in_bin[2 * (DATA_NUM - 1 - counter)]      = 0;
                                data_in_bin[2 * (DATA_NUM - 1 - counter) + 1 ] = 0;
                                break;
                            case 1:
                                data_in_bin[2 * (DATA_NUM - 1 - counter)]      = 0;
                                data_in_bin[2 * (DATA_NUM - 1 - counter) + 1 ] = 1;
                                break;
                            case 2:
                                data_in_bin[2 * (DATA_NUM - 1 - counter)]      = 1;
                                data_in_bin[2 * (DATA_NUM - 1 - counter) + 1 ] = 0;
                                break;
                            case 3:
                                data_in_bin[2 * (DATA_NUM - 1 - counter)]      = 1;
                                data_in_bin[2 * (DATA_NUM - 1 - counter) + 1 ] = 1;
                                break;
                            default:
                                status=8;       //错误
                                break;
                            }//switch(output.Loc)
                        }//else{//数据足够，开始解调
                    }//for(counter=NOdata;counter<DATA_NUM;counter++)
                    if(counter == DATA_NUM){
                        bin2hex(data_in_hex, data_in_bin, 2*DATA_NUM);
                        hex2int(data_in_hex, &data_in_uint);
#ifndef CPU2_CTRL_UARTB
                        scib_pdev->master->write_string(scib_pdev,"DSP 2 demoded Data: ",-1);
                        scib_pdev->master->write_string(scib_pdev,data_in_hex,-1);
#endif
#ifdef CPU2_CTRL_UARTB
                        ipc_send_data(data_in_uint);//通过IPC传输数据给CPU2
#endif
                        status=6;       //解调完成
                        NOdata=0;   findit=0;   start_p=0;  rp=0;
                        lfm_sp=0;   fsk_sp=0;   skip_t=0;   flag=0;
                        break;
                    }
                }//else//当先数据帧包含fsk数据
            }// while(1)
        }
    }
}
