//
// Created by liuxi on 2023/11/25.
//

#ifndef QEA2_ADS1292_H
#define QEA2_ADS1292_H

#endif //QEA2_ADS1292_H
#include "gpio.h"
#include <string.h>

/*============ADS1292系统命令====================*/
/*系统命令*/
#define ADS1292R_WAKEUP	     0X02	//从待机模式唤醒
#define ADS1292R_STANDBY	 0X04	//进入待机模式
#define ADS1292R_ADSRESET    0X06	//复位ADS1292R
#define ADS1292R_START	     0X08	//启动或转换
#define ADS1292R_STOP	     0X0A	//停止转换
#define ADS1292R_OFFSETCAL	 0X1A	//通道偏移校准
/*数据读取命令*/
#define ADS1292R_RDATAC	     0X10	//启用连续的数据读取模式,默认使用此模式
#define ADS1292R_SDATAC	     0X11	//停止连续的数据读取模式
#define ADS1292R_RDATA	     0X12	//通过命令读取数据;支持多种读回。
/*寄存器读取命令*/
#define	ADS1292R_RREG	     0X20	//读取001r rrrr 000n nnnn  这里定义的只有高八位，低8位在发送命令时设置
#define ADS1292R_WREG	     0X40	//写入010r rrrr 000n nnnn
/*	r rrrr=要读、写的寄存器地址
	n nnnn=要读、写的数据		*/
/*ADS1292R内部寄存器地址定义	*/
#define ADS1292R_ID			 0X00	//ID控制寄存器
#define ADS1292R_CONFIG1	 0X01	//配置寄存器1
#define ADS1292R_CONFIG2	 0X02	//配置寄存器2
#define ADS1292R_LOFF		 0X03	//导联脱落控制寄存器
#define ADS1292R_CH1SET		 0X04	//通道1设置寄存器
#define ADS1292R_CH2SET		 0X05	//通道2设置寄存器
#define ADS1292R_RLD_SENS	 0X06	//右腿驱动选择寄存器
#define ADS1292R_LOFF_SENS	 0X07	//导联脱落检测选择寄存器
#define ADS1292R_LOFF_STAT	 0X08	//导联脱落检测状态寄存器
#define	ADS1292R_RESP1		 0X09	//呼吸检测控制寄存器1
#define	ADS1292R_RESP2		 0X0A	//呼吸检测控制寄存器2
#define	ADS1292R_GPIO		 0X0B	//GPIO控制寄存器
/*================================*/

#define HEART_BUFFER_SIZE 9
extern uint16_t heartRawData;
void ADS1292Init(void);
int8_t ADS1292CmdTransmit(uint8_t);
uint8_t ADS1292RegisterReadSingle(uint8_t);
void ADS1292ReadData(void);
void ADS1292RegisterWriteSingle(uint8_t,uint8_t);
int8_t SPI3_RWByte(uint8_t);
void regReadAll(void);
void printArray(uint8_t*,uint8_t);
void finalCountDown(uint8_t);
extern void printChannel(int array[]);
