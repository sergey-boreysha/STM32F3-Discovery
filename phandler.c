
#include "phandler.h"

//**************************************************************************

//Инициализация обработчика протокола USART1...
void __USART1_Protocol_Init(void){

	USART1_Status_Command = WAITING;	//Флаг состояния RX буфера - ожидание!
	USART1_RX_Buffer[0] = '\0';			//Очистка буфера приёмнка USART1
	USART1->CR1 |= 1<<5;				//Включить прерывание приёмника
}

//**************************************************************************

//Обработчик протокола USART1...
void __USART1_Protocol_Handler(void){

	if (USART1_Status_Command != COMMAND_RECEIVED) return;

	USART1_Send_String("\nUSART command recived->...");
	USART1_Send_String(USART1_RX_Buffer); //Печатаем то, что приняли!
	USART1_Send_Byte('\n');


//-----------------------------------------------------------------------

	if (_strcmp(USART1_RX_Buffer, (u8*)"test&")) nRF_Send_String((u8*)"test&");

	if (_strcmp(USART1_RX_Buffer, "rstnp&")) //Обнуляем счётчики переданных пакетов
		{
			nRF_number_of_TX_data_packets = 0;
			nRF_number_of_RX_data_packets = 0;
			nRF_number_of_LOST_data_packets = 0;
		}

//-----------------------------------------------------------------------

	__USART1_Protocol_Init(); //Инициализация обработчика протокола USART1...

}

//**************************************************************************
//**************************************************************************w

//Инициализация обработчика протокола nRF...
void __nRF_Protocol_Init(void){

	nRF_Protocol = WAITING_COMMAND;
	nRF_RX_buffer[0] = '\0'; //Очистка буфера приёмнка
}

//**************************************************************************

//Обработчик протокола nRF...
void __nRF_Protocol_Handler(void){

	if (nRF_Protocol != COMMAND_RECEIVED) return;

//	USART1_Send_String("nRF command recived->...");
//	USART1_Send_String(nRF_RX_buffer); //Печатаем то, что приняли!
//	USART1_Send_Byte('\n');

	nRF_Send_String((u8*)"answer&");

	__nRF_Protocol_Init(); //Инициализация обработчика протокола
}
