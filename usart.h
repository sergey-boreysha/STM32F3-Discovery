
#ifndef _USART_H
#define _USART_H

//Подключаемые библиотеки...
//#include <stm32f30x.h>
#include "stm32_stdlib.h"	//Функции общего назначения
#include "dtype.h"			//Определение пользовательских типов данных

//Прототипы функций...
void USART1_Init(void);										//Инициализация USART1...
void USART1_Send_Byte(u8 txData);							//Передаём байт по USART1
void USART1_Send_String(u8 *data);							//Передаём строку по USART1
void USART1_Print_Byte(u8 data, enum type number_system); 	//Преобразование байта в текст и вывод в USART1
void USART1_Print_Word(u16 data); 							//Преобразование слова в текст и вывод в USART1
void USART1_IRQHandler(void); 								//Обработчик прерывания USART1


#endif //_USART_H
