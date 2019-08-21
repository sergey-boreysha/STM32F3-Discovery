
#ifndef _PHANDLER_H
#define _PHANDLER_H

//Подключаемые библиотеки...
//#include <stm32f30x.h>
//#include "stm32_stdlib.h"	//Функции общего назначения
#include "dtype.h"			//Определение пользовательских типов данных
//#include "usart.h"			//Библиотка USART
#include "nRF24L01.h"

//Прототипы функций...
void __USART1_Protocol_Init(void);		//Инициализация обработчика протокола USART1
void __USART1_Protocol_Handler(void);	//Обработчик протокола USART1

void __nRF_Protocol_Init(void);			//Инициализация обработчика протокола nRF
void __nRF_Protocol_Handler(void);		//Обработчик протокола nRF

//Определение переменных...
extern volatile enum ptcl USART1_Status_Command; //Флаг состояния коммандного регистра
extern u8 USART1_RX_Buffer[16]; //Буфер приёмнка UART1

extern volatile enum stat nRF_Status;
extern volatile enum ptcl nRF_Protocol;


#endif //_PHANDLER_H
