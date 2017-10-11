#ifndef _GPIO_H_
#define _GPIO_H_
#include "config.h"
void GPIOinput_init(void);//GPIOB口用寄存器直接初始化为输入
void GPIOoutput_init(void);//GPIOA口用寄存器直接初始化为输出
void GPIOoutput_config(void);//GPIOA口用固件库初始化为输出
void GPIOinput_config(void);//GPIOB口用固件库初始化为输入
#endif
