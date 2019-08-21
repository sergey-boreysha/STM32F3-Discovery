
#ifndef _STM32_STDLIB_H
#define _STM32_STDLIB_H

//Подключаемые библиотеки...
#include <stm32f30x.h>
#include "dtype.h"

//Прототипы функций...
   	    void _delay(u32 index); 			//Программная пауза
		  u8 _strlen (uc8 *str); 			//Возвращает длинну строки
enum boolean _strcmp(u8 *str1, u8 *str2);	//Сравниваем две строки

		//void _delay_ms(uint8_t index); 	//Программная пауза
  	    //void _delay_us(uint32_t index); 	//Программная пауза


#endif //_STM32_STDLIB_H
