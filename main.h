
#ifndef _MAIN_H
#define _MAIN_H

//Макроопределения...
#define F_CPU 72000000 //Тактируется от STLINK - 8MHz, PLL:ON, PLLMUL:0111, input clock x 9

#define ON 	1 //Операторы управленияw
#define OFF 0

//Индикатор выполнения основного цикла...
#define SYSTEM_BLNK {GPIOE->ODR |= 1<<15;_delay(2000000);GPIOE->ODR &= ~(1<<15);_delay(2000000);}

//Подключаемые библиотеки...
#include "init.h"		//Библиотка инициализации переферии
#include "usart.h"		//Библиотка USART
#include "phandler.h"	//Обработчик комманд USART
#include "L3GD20.h" 	//Библиотка SPI
//#include "LSM303D.h";	//Библиотка LSM303D
#include "nRF24L01.h"	//Библиотека трансивера nRF24L01
//#include "adc.h" 		//Библиотка ADC

//Прототипы функций...
int main(void); //Основная функция

//Объявить переменных...
extern volatile enum ptcl USART1_Status_Command; //Флаг состояния коммандного регистра
extern u8 USART1_RX_Buffer[16]; //Буфер приёмнка UART1


#endif //_MAIN_H
