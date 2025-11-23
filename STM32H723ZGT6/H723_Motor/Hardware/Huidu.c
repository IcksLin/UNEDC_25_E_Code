#include "main.h"
#include "gpio.h"
#include "Delay.h"
#define Huidu_Address 0x9E
#define Huidu_Num_Address 0xDD
 
 
void Huidu_W_SCL(uint8_t BitValue)
{
	HAL_GPIO_WritePin(GPIOF,GPIO_PIN_1,(GPIO_PinState)BitValue);
	Delay_us(10);
}
 
void Huidu_W_SDA(uint8_t BitValue)
{
	HAL_GPIO_WritePin(GPIOF,GPIO_PIN_0,(GPIO_PinState)BitValue);
	Delay_us(10);
}
 
uint8_t Huidu_R_SDA(void)
{
	uint8_t BitValue;
	BitValue = HAL_GPIO_ReadPin(GPIOF,GPIO_PIN_0);
	Delay_us(10);
	return BitValue;
}

void Huidu_Init(void)
{
  MX_GPIO_Init();
	Huidu_W_SCL(1);
	Huidu_W_SDA(1);
 
}
void Huidu_Start(void)
{
Huidu_W_SCL(1);
Huidu_W_SDA(1);
Huidu_W_SDA(0);
Huidu_W_SCL(0);
}
void Huidu_Stop(void)
{
Huidu_W_SCL(0);
Huidu_W_SDA(0);
Huidu_W_SCL(1);
Huidu_W_SDA(1);
}
void Huidu_SendByte(uint8_t Byte)
{
uint8_t i=0;
for(i=0;i<8;i++)
{
Huidu_W_SDA(Byte & (0x80>>i));
Huidu_W_SCL(1);
Huidu_W_SCL(0);
}
}
uint8_t Huidu_ReceiveByte(void)
{
    uint8_t Byte=0x00;
Huidu_W_SDA(1);
uint8_t i=0;
for(i=0;i<8;i++)
{
    Huidu_W_SCL(1);
   if(Huidu_R_SDA()==1)
   Byte|=(0x80>>i);
   Huidu_W_SCL(0);
}
return Byte;
}
 
uint8_t Huidu_ReceiveAck(void)
{
	uint8_t AckBit;
	Huidu_W_SDA(1);
	Huidu_W_SCL(1);
	AckBit = Huidu_R_SDA();
	Huidu_W_SCL(0);
	return AckBit;
}
void Huidu_SendAck(uint8_t AckBit)
{
Huidu_W_SDA(AckBit);
Huidu_W_SCL(1);
Huidu_W_SCL(0);
}
void Huidu_GetNum_Start(void)
{
	Huidu_Start();
	Huidu_SendByte(Huidu_Address);
	Huidu_ReceiveAck();
	Huidu_SendByte(Huidu_Num_Address);
	Huidu_ReceiveAck();
	Huidu_Stop();
}
char Huidu_GetNum(void)
{
	char GetNum;
	Huidu_Start();
	Huidu_SendByte(Huidu_Address | 0x9F);
	Huidu_ReceiveAck();
	GetNum = Huidu_ReceiveByte();
	Huidu_SendAck(1);
   Huidu_Stop();
return GetNum;
}