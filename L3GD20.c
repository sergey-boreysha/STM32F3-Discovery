
#include "L3GD20.h"

//**************************************************************************
//**************************************************************************

//Инициализация SPI1...
void L3GD20_init(){

	//After power supply is applied, the L3GD20 performs a 10 ms boot procedure

	USART1_Send_String((u8*)"Initialization SPI1...\n");

	RCC->AHBENR |= RCC_AHBENR_GPIOEEN;	//Включаем тактирование GPIOE
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN; //Включаем тактирование SPI1

	//Линия INT1 (PE0)...
		GPIOE->MODER |= (0<<1|0<<0);				//PE0 - Input mode

	//Линия DRDY/INT2 (PE1)...
		GPIOE->MODER |= (0<<3|0<<2);				//PE1 - Input mode

	//Линия CS (PE3)...
		GPIOE->MODER |= (0<<7|1<<6);				//PE3 - General purpose output mode

	//Линия SCK (PA5)...
		GPIOA->MODER |= (1<<11|0<<10);				//PA5 - Alternate function mode
		GPIOA->AFRL |= (0<<23|1<<22|0<<21|1<<20);	//Alternate function AF5

	//Линия MISO (PA6)...
		GPIOA->MODER |= (1<<13|0<<12);				//PA6 - Alternate function mode
		GPIOA->AFRL |= (0<<27|1<<26|0<<25|1<<24);	//Alternate function AF5

	//Линия MOSI (PA7)...
		GPIOA->MODER |= (1<<15|0<<14);				//PA6 - Alternate function mode
		GPIOA->AFRL |= (0<<31|1<<30|0<<29|1<<28);	//Alternate function AF5

	L3GD20_CS_HIGH_LEVEL;

		SPI1->CR1 |= 1<<0;						//Бит CPHA
		SPI1->CR1 |= 1<<1;						//Бит CPOL
		SPI1->CR1 |= 1<<2;						//Master configuration
		SPI1->CR1 |= (1<<5|1<<4|1<<3);			//Baud rate control - fPCLK/256 = 281,25kHz

		SPI1->CR2 |= 1<<2;						//Вывод NSS управляется аппаратным обеспечением
		//SPI1->CR2 |= (0<<11|1<<10|1<<9|1<<8);	//Формат передаваемых данных

		SPI1->CR1 |= 1<<6;						//SPI1 enable

	while (SPI1->SR & (1<<7));					//Ждем пока SPI1 занят

//Читаем регистр WHOAMI, L3GD20
	if (L3GD20_Exchange_Word(READ, WHO_AM_I, NULL) == 0xD4)
		USART1_Send_String((u8*)"Initialization L3GD20 successful!...\n");
	else
		USART1_Send_String((u8*)"Initialization L3GD20 error...\n");

}

//**************************************************************************
//**************************************************************************

//Конфигурация L3GD20...
void L3GD20_Config(void){



#ifdef PRINT_DEBUG_L3GD20
	USART1_Send_String((u8*)"\nThe state registers L3GD20 after reset...\n");
	L3GD20_get_Register_Value();	//Читаем регистры L3GD20
#endif //PRINT_DEBUG_L3GD20

}

//**************************************************************************
//**************************************************************************

//Чтение/запись байта по адресу, SPIx...
u8 L3GD20_Exchange_Word(u8 cmd, u8 addr, u8 data){

	uint8_t registr = 0;

		L3GD20_CS_LOW_LEVEL;

			SPI1->DR = cmd | addr; //Передаём адрес целевого регистра + комманда записи
			while (SPI1->SR & (1<<7)); //Ждем пока SPI1 занят

					registr = SPI1->DR;
					_delay(500); //Пауза 15мкс

				SPI1->DR = data; //Передаём байт
				while (SPI1->SR & (1<<7)); //Ждем пока SPI1 занят

			registr = SPI1->DR;

		L3GD20_CS_HIGH_LEVEL;

	return registr;
}

//**************************************************************************
//**************************************************************************

//Читаем регистры L3GD20...
void L3GD20_get_Register_Value(void){

	USART1_Send_String((u8*)"WHO_AM_I........");
	USART1_Print_Byte(L3GD20_Exchange_Word(READ, WHO_AM_I, NULL), BIN);
	USART1_Send_Byte('\n');

//	USART1_Send_String((u8*)"WHO_AM_I........");
//	USART1_Print_Byte(L3GD20_Exchange_Word(READ, WHO_AM_I, NULL), BIN);
//	USART1_Send_Byte('\n');
//
//	USART1_Send_String((u8*)"WHO_AM_I........");
//	USART1_Print_Byte(L3GD20_Exchange_Word(READ, WHO_AM_I, NULL), BIN);
//	USART1_Send_Byte('\n');
//
//	USART1_Send_String((u8*)"WHO_AM_I........");
//	USART1_Print_Byte(L3GD20_Exchange_Word(READ, WHO_AM_I, NULL), BIN);
//	USART1_Send_Byte('\n');



	USART1_Send_Byte('\n');
	USART1_Send_Byte('\n');
}
