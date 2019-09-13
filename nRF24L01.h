
#ifndef _NRF24L01_H
#define _NRF24L01_H

//Подключаемые библиотеки...
//#include <stm32f30x.h>
#include "nRF_map.h"		//Определения регистров nRF
#include "stm32_stdlib.h"	//Функции общего назначения
#include "dtype.h"			//Определение пользовательских типов данных
#include "usart.h"			//Библиотка USART

#define PRINT_DEBUG_nRF24L01 //Печатать отладочную информацию в сервисный порт

#define nRF_FREQ 0 //Частота 2400МГц, +1 = +1МГц, max +127

#define nRF_CSN_LOW_LEVEL {GPIOB->BSRR |= 1<<28;_delay(1000);}	//Начало сеанса обмена по шине SPI2
#define nRF_CSN_HIGH_LEVEL {_delay(1000);GPIOB->BSRR |= 1<<12;}	//Завершение сеанса обмена по шине SPI2

//Режимы обмена данными nRF...
#define nRF_CE_HIGH_LEVEL GPIOB->BSRR |= 1<<11;	//Режим приёма, CE=1
#define nRF_CE_LOW_LEVEL GPIOB->BSRR |= 1<<27;	//Режим передачи, CE=0
#define nRF_TX_MODE nRF_Exchange_Word(W_REGISTER,CONFIG,(1<<EN_CRC)|(1<<CRCO)|(1<<PWR_UP)|(0<<PRIM_RX));
#define nRF_RX_MODE nRF_Exchange_Word(W_REGISTER,CONFIG,(1<<EN_CRC)|(1<<CRCO)|(1<<PWR_UP)|(1<<PRIM_RX));

//Очистка флагов прерывания
#define nRF_IRQ_FLAG_CLEAR nRF_Exchange_Word(W_REGISTER,STATUS,0b01110000);

//Очистка счётчика утерянных пакетов
#define nRF_OBSERVE_RESET nRF_Exchange_Word(W_REGISTER,RF_CH,nRF_FREQ);

//Очистка буфферов данных
#define nRF_FLUSH_TX_BUFFER nRF_Exchange_Byte(FLUSH_TX);
#define nRF_FLUSH_RX_BUFFER nRF_Exchange_Byte(FLUSH_RX);

//Начало передачи данных
#define nRF_START_TRANSMIT_DATA {nRF_CE_HIGH_LEVEL;_delay(100);nRF_CE_LOW_LEVEL;}

//Прототипы функций...
void nRF_Init(void);											//Инициализация nRF
void nRF_Config(void);											//Конфигурация nRF
void nRF_RX_Mode(void);											//Режим приёма данных nRF
u8 nRF_Exchange_Byte(u8 data); 									//Обмен байтом nRF
u8 nRF_Exchange_Word(u8 cmd, u8 addr, u8 data);					//Обмен словом nRF
void nRF_Send_String(u8 *data);									//Отправить строку nRF
void nRF_Write_Buffer(u8 cmd, u8 addr, u8 *buffer);				//Запись массива по адресу
void nRF_Read_Buffer(u8 cmd, u8 addr, u8 number_of_elements);	//Чтение массива по адресу
void nRF_get_Register_Value(void);								//Читаем регистры nRF

//Определение переменных...
volatile enum stat nRF_Status;
volatile enum ptcl nRF_Protocol;

uint8_t nRF_number_of_TX_data_packets; //Счётчики пакетов данных
uint8_t nRF_number_of_RX_data_packets;
uint8_t nRF_number_of_LOST_data_packets;

uint8_t nRF_RX_buffer[32]; //Буфер приёмника транссивера


#endif //_NRF24L01_H
