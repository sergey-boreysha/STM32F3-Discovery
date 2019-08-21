
#ifndef _L3GD20_H
#define _L3GD20_H

//Подключаемые библиотеки...
//#include <stm32f30x.h>
#include "stm32_stdlib.h"	//Функции общего назначения
#include "dtype.h"			//Определение пользовательских типов данных
#include "usart.h"			//Библиотка USART

#define L3GD20_CS_LOW_LEVEL {GPIOE->BSRR |= 1<<19;_delay(100);}		//Начало сеанса обмена по шине SPI1
#define L3GD20_CS_HIGH_LEVEL {_delay(100);GPIOE->BSRR |= 1<<3;}		//Завершение сеанса обмена по шине SPI1

//Прототипы функций...
void L3GD20_init();										//Инициализация L3GD20
u8 L3GD20_Exchange_Word(u8 cmd, u8 addr, u8 data);		//Чтение байта по адресу


#endif //_L3GD20_H
