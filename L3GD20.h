
#ifndef _L3GD20_H
#define _L3GD20_H

//Подключаемые библиотеки...
#include "stm32_stdlib.h"	//Функции общего назначения
#include "dtype.h"			//Определение пользовательских типов данных
#include "usart.h"			//Библиотка USART

//#define PRINT_DEBUG_L3GD20 //Печатать отладочную информацию в сервисный порт

#define WHO_AM_I		0x0F //r 11010100
#define CTRL_REG1		0x20 //rw 00000111
#define CTRL_REG2		0x21 //rw 00000000
#define CTRL_REG3		0x22 //rw 00000000
#define CTRL_REG4		0x23 //rw 00000000
#define CTRL_REG5		0x24 //rw 00000000
#define REFERENCE		0x25 //rw 00000000
#define OUT_TEMP		0x26 //r output
#define STATUS_REG		0x27 //r output
#define OUT_X_L			0x28 //r output
#define OUT_X_H			0x29 //r output
#define OUT_Y_L			0x2A //r output
#define OUT_Y_H			0x2B //r output
#define OUT_Z_L			0x2C //r output
#define OUT_Z_H			0x2D //r output
#define FIFO_CTRL_REG	0x2E //rw 00000000
#define FIFO_SRC_REG	0x2F //r output
#define INT1_CFG		0x30 //rw 00000000
#define INT1_SRC		0x31 //r output
#define INT1_THS_XH		0x32 //rw 00000000
#define INT1_THS_XL		0x33 //rw 00000000
#define INT1_THS_YH		0x34 //rw 00000000
#define INT1_THS_YL		0x35 //rw 00000000
#define INT1_THS_ZH		0x36 //rw 00000000
#define INT1_THS_ZL		0x37 //rw 00000000
#define INT1_DURATION	0x38 //rw 00000000

#define READ	0b10000000
#define WRITE	0b00000000

#define L3GD20_CS_LOW_LEVEL {GPIOE->BSRR |= 1<<19;_delay(100);}	//Начало сеанса обмена по шине SPI1
#define L3GD20_CS_HIGH_LEVEL {_delay(100);GPIOE->BSRR |= 1<<3;}	//Завершение сеанса обмена по шине SPI1

//STATUS_REG description...
union
	{
		struct
			{
				u8 XDA:1;	//X axis new data available
				u8 YDA:1;	//Y axis new data available
				u8 ZDA:1;	//Z axis new data available
				u8 ZYXDA:1;	//X, Y, Z -axis new data available
				u8 XOR:1;	//X axis data overrun
				u8 YOR:1;	//Y axis data overrun
				u8 ZOR:1;	//Z axis data overrun
				u8 ZYXOR:1; //X, Y, Z -axis data overrun
			}bit;

		u8 reg;

	}_STATUS_REG;

//CTRL_REG1 description...
union
	{
		struct
			{
				u8 X_axis_enable:1;
				u8 Y_axis_enable:1;
				u8 Z_axis_enable:1;
				u8 Power_down_mode_enable:1;
				u8 Bandwidth_selection:2;
				u8 Output_data_rate_selection:2;
			}bit;

		u8 reg;

	}_CTRL_REG1;


//Прототипы функций...
void L3GD20_init();									//Инициализация L3GD20
u8 L3GD20_Exchange_Word(u8 cmd, u8 addr, u8 data);	//Чтение байта по адресу
void L3GD20_get_Register_Value(void);				//Читаем регистры L3GD20
void L3GD20_Config(void);							//Конфигурация L3GD20

#endif //_L3GD20_H
