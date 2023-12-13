//
// Created by liuxi on 2023/11/25.
//

#include "ADS1292.h"
#include "spi.h"
#include "main.h"

uint8_t heartDetectRxBuffer[9];
int heartBeat = 0;
uint8_t test = 0;
uint16_t heartRawData;
uint8_t ads1292r_data_buff[9] = {0};
uint8_t device_id ;
signed long  ADS1292R_ECG_BUFFER[2];
signed short ECGRawData[2];
uint16_t heartRawData;
/*
	ADS1292初始化
*/
void ADS1292Init(){
    ADS1292CmdTransmit(ADS1292R_SDATAC);  // !!先发送停止读取数据的命令再进行复位
	HAL_GPIO_WritePin(heart_RST_GPIO_Port,heart_RST_Pin,GPIO_PIN_SET);
	HAL_Delay(500);
	printf("RESET Begin!\r\n");
	HAL_GPIO_WritePin(heart_RST_GPIO_Port,heart_RST_Pin,GPIO_PIN_RESET);
	HAL_Delay(1000);
	HAL_GPIO_WritePin(heart_RST_GPIO_Port,heart_RST_Pin,GPIO_PIN_SET);  //发出复位脉冲
	HAL_Delay(500);
	printf("RESET Complete!\r\n");
	HAL_GPIO_WritePin(heart_CS_GPIO_Port,heart_CS_Pin,GPIO_PIN_RESET); //拉低使能信号
	//发送停止读取数据的信息
    ADS1292CmdTransmit(ADS1292R_SDATAC);
    HAL_Delay(1000);
    while(device_id!=0x73 && device_id!=83)
    {
        device_id = ADS1292RegisterReadSingle(0x00);
        printf("errorID: %x\r\n",device_id);
        HAL_Delay(200);
    }
    printf("rightID: %x\r\n",device_id);
//    regReadAll();  //读取所有寄存器

/*    ADS1292RegisterWriteSingle(ADS1292R_CONFIG2,0xe0);
    HAL_Delay(10);
    ADS1292RegisterWriteSingle(ADS1292R_CONFIG1,0X01);
    ADS1292RegisterWriteSingle(ADS1292R_LOFF,0XF0);
    ADS1292RegisterWriteSingle(ADS1292R_LOFF_SENS,0X0F);
    if(test){
        ADS1292RegisterWriteSingle(ADS1292R_CONFIG2,0XA3);
        ADS1292RegisterWriteSingle(ADS1292R_CH1SET,0x05);
        ADS1292RegisterWriteSingle(ADS1292R_CH2SET,0x05);

    }else{
//        ADS1292RegisterWriteSingle(ADS1292R_CONFIG2,0xa0);
//        HAL_Delay(1000);
        ADS1292RegisterWriteSingle(ADS1292R_RLD_SENS,0xef);
        ADS1292RegisterWriteSingle(ADS1292R_RESP1,0xf2);
        ADS1292RegisterWriteSingle(ADS1292R_RESP2,0x83);
    }*/


/*    ADS1292RegisterWriteSingle(ADS1292R_CONFIG2,0Xe0);
    HAL_Delay(100);
    ADS1292RegisterWriteSingle(ADS1292R_CONFIG1,0x02);
    ADS1292RegisterWriteSingle(ADS1292R_CH1SET,0X00);
    ADS1292RegisterWriteSingle(ADS1292R_CH2SET, 0x00);
    ADS1292RegisterWriteSingle(ADS1292R_RLD_SENS,0x23);
    ADS1292RegisterWriteSingle(ADS1292R_RESP1,0x02);
    ADS1292RegisterWriteSingle(ADS1292R_RESP2,0x03);*/

    ADS1292RegisterWriteSingle(ADS1292R_CONFIG2,0xe0); //使用内部参考电压
    HAL_Delay(10); //等待稳定
    ADS1292RegisterWriteSingle(ADS1292R_CONFIG1,0x01);  //设置寄存器01 两通道采样为250SPS 连续读取模式
    ADS1292RegisterWriteSingle(ADS1292R_LOFF,0xF0);  //导联脱落检测寄存器=0xF0，正负极分别为70%和30%，实际未使用
    ADS1292RegisterWriteSingle(ADS1292R_CH1SET,0X00);  //通道1设置
    ADS1292RegisterWriteSingle(ADS1292R_CH2SET, 0x00);   //通道2设置
    ADS1292RegisterWriteSingle(ADS1292R_RLD_SENS,0x30);   //右腿驱动
    ADS1292RegisterWriteSingle(ADS1292R_LOFF_SENS,0x3F);  //导联脱落
    ADS1292RegisterWriteSingle(ADS1292R_RESP1,0xDE);   //设置寄存器 检测呼吸
    ADS1292RegisterWriteSingle(ADS1292R_RESP2,0x07);   //设置寄存器 检测呼吸

    HAL_Delay(500);
    ADS1292CmdTransmit(ADS1292R_RDATAC);
//    finalCountDown(2);
    ADS1292CmdTransmit(ADS1292R_START);
}

/*向ADS1292发送命令*/
int8_t ADS1292CmdTransmit(uint8_t cmd){
    SPI3_RWByte(cmd);
//    printf("cmd:%x\r\n",SPI3_RWByte(cmd));
    return SPI3_RWByte(cmd);
}

/*读取单个寄存器的值*/
uint8_t ADS1292RegisterReadSingle(uint8_t regAddr){
    uint8_t addr = 0x20 | regAddr;
    printf("read_addr:%x\r\n",addr);
    uint8_t num = 0x00;
    uint8_t res;
    SPI3_RWByte(addr);
    SPI3_RWByte(num);
    res = SPI3_RWByte(0xff);
    printf("read_data:%x\r\n",res);
    return res;
}

/*写单个寄存器的值*/
void ADS1292RegisterWriteSingle(uint8_t regAddr,uint8_t data){
    uint8_t addr = ADS1292R_WREG | regAddr;  //获取值以及指令
    printf("******\r\n");
    printf("write-addr:%x\r\n",addr);
    printf("write-cmd:%x\r\n",data);
    printf("******\r\n");
    uint8_t num = 0x00;  //写一个寄存器
    SPI3_RWByte(addr);  //第一位 命令+地址
    SPI3_RWByte(num);  //读写几位数据
    SPI3_RWByte(data);  //写指令
    HAL_Delay(1);  //等待1s
}

/*读取ADS心率数据*/
void ADS1292ReadData(){
//        for(uint8_t i = 0;i < 6;i++){
//        heartDetectRxBuffer[i] = SPI3_RWByte(0x00);
//    }
//    heartBeat = 0;
//    heartDetectRxBuffer[6] = SPI3_RWByte(0x00);
//    heartBeat = heartBeat | heartDetectRxBuffer[6];
//    heartBeat  = heartBeat << 8;
//    heartDetectRxBuffer[7] = SPI3_RWByte(0x00);
//    heartBeat = heartBeat | heartDetectRxBuffer[7];
//    heartDetectRxBuffer[8] = SPI3_RWByte(0x00);
//    heartRawData = (uint16_t)heartBeat;
//    printf("%d\n\r",heartRawData);

    int i,k;
    HAL_GPIO_WritePin(heart_CS_GPIO_Port,heart_CS_Pin,GPIO_PIN_RESET);

    for(k=0;k<50;k++);
    for(i=0;i<9;i++)
    {
        ads1292r_data_buff[i]=SPI3_RWByte(0X00);
    }
    for(k=0;k<50;k++);
//    printArray(ads1292r_data_buff,9);
    unsigned char number=0;
    for(number=0; number<2; number++)
    {
        ADS1292R_ECG_BUFFER[number]=(signed long)ads1292r_data_buff[3+3*number];
        ADS1292R_ECG_BUFFER[number]=ADS1292R_ECG_BUFFER[number]<<8;
        ADS1292R_ECG_BUFFER[number]|=ads1292r_data_buff[3+3*number+1];
        ADS1292R_ECG_BUFFER[number]=ADS1292R_ECG_BUFFER[number]<<8;
        ADS1292R_ECG_BUFFER[number]|=ads1292r_data_buff[3+3*number+2];
    }

    ADS1292R_ECG_BUFFER[0]=ADS1292R_ECG_BUFFER[0]>>8;
    ADS1292R_ECG_BUFFER[1]=ADS1292R_ECG_BUFFER[1]>>8;
    ADS1292R_ECG_BUFFER[0] &= 0xffff;
    ADS1292R_ECG_BUFFER[1] &= 0xffff;

    ECGRawData[0]=(short)ADS1292R_ECG_BUFFER[0];
    ECGRawData[1]=(short)ADS1292R_ECG_BUFFER[1];
    heartRawData = ECGRawData[1];
//    printf("%d\n\r",heartRawData);
//    static uint8_t temp1292r[9]={0}; //并没有什么卵用
//    HAL_SPI_TransmitReceive(&hspi3,temp1292r,ads1292r_data_buff,9,10);
//    HAL_SPI_Receive_DMA(&hspi3, ads1292r_data_buff, 9); // 使用DMA接收9个字节的数据
    HAL_GPIO_WritePin(heart_CS_GPIO_Port,heart_CS_Pin,GPIO_PIN_SET);

}

/*SPI写数据*/
int8_t SPI3_RWByte(uint8_t Tx){
    uint8_t Rx;
    HAL_SPI_TransmitReceive(&hspi3,&Tx,&Rx,sizeof(Tx),2);
    return Rx;
}

/*读取所有数据*/
void regReadAll(){
    uint8_t reg;
    for(reg = 0x00;reg <= 0x0B;reg++){
        ADS1292RegisterReadSingle(reg);
    }
}

/*工具：打印数组*/
void printArray(uint8_t array[],uint8_t size){
    if(size >= 1){
        for(uint8_t i = 6;i < size;i++){
            printf("|%x",array[i]);
        }
    }
    printf("|\r\n");
}

/*查看是哪个通道的数据*/
void printChannel(int array[]){
    printf("channel2：%d",array[1]);
    printf("\n");
}

void finalCountDown(uint8_t time){
    printf("Prep Complete!\r\n");
    for(uint8_t i = time;i > 0;i--)
    {printf("Initiate Sampling in %d sec.\r\n",i);
        HAL_Delay(1000);}
}