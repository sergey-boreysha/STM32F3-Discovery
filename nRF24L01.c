
#include "nRF24L01.h"

//**************************************************************************
//**************************************************************************

//Инициализация nRF24L01...
void nRF_Init(void){

	USART1_Send_String((u8*)"Initialization SPI2...\n");

	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;	//Включаем тактирование GPIOB
	RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;	//Включаем тактирование SPI2
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN; //Включаем тактирование SYSCFG

		GPIOB->MODER &= ~(1<<21|1<<20);	//Линия INT(PB10) - Input mode

		SYSCFG->EXTICR[2] |= SYSCFG_EXTICR3_EXTI10_PB;	//Линия PB10 подключена к EXTI15_10
		EXTI->IMR |= EXTI_IMR_MR10;						//Разрешаем прерывание по EXTI15_10
		EXTI->FTSR |= EXTI_FTSR_TR10;					//Прерывание PB10 по срезу
		NVIC_EnableIRQ(EXTI15_10_IRQn); 				//Разрешение прерывания EXTI15_10

		GPIOB->MODER |= (0<<23|1<<22);	//Линия CE(PB11) - Output mode
		GPIOB->MODER |= (0<<25|1<<24);	//Линия SS(PB12) - Output mode

	//Линия SCK(PB13, AF5)...
		GPIOB->MODER |= (1<<27|0<<26);				//PB13 - Alternate function mode
		GPIOB->AFRH |= (0<<23|1<<22|0<<21|1<<20);	//Alternate function AF5

	//Линия MISO(PB14, AF5)...
		GPIOB->MODER |= (1<<29|0<<28);				//PB14 - Alternate function mode
		GPIOB->AFRH |= (0<<27|1<<26|0<<25|1<<24);	//Alternate function AF5

	//Линия MOSI(PB15, AF5)...
		GPIOB->MODER |= (1<<31|0<<30);				//PB15 - Alternate function mode
		GPIOB->AFRH |= (0<<31|1<<30|0<<29|1<<28);	//Alternate function AF5

	nRF_CSN_HIGH_LEVEL;

		SPI2->CR1 |= 0<<0;						//Бит CPHA
		SPI2->CR1 |= 0<<1;						//Бит CPOL
		SPI2->CR1 |= 1<<2;						//Master configuration
		SPI2->CR1 |= (1<<5|1<<4|1<<3);			//Baud rate control - fPCLK/256 = 281,25kHz

		SPI2->CR2 |= 1<<2;						//Вывод NSS управляется аппаратным обеспечением
		SPI2->CR2 |= (0<<11|1<<10|1<<9|1<<8);	//Формат передаваемых данных
		SPI2->CR2 |= 1<<12;						//Размер кадра 8 бит

		SPI2->CR1 |= 1<<6;						//SPI2 enable

	while (SPI2->SR & (1<<7)); 					//Ждем пока SPI2 занят

//Читаем регистр WHOAMI, L3GD20
	//if (L3GD20_Exchange_Word(READ, WHO_AM_I, NULL) == 0xD4)
		USART1_Send_String((u8*)"Initialization nRF24L01 successful!...\n");
	//else
		//USART1_Send_String((u8*)"Initialization L3GD20 error...\n");

	#ifdef PRINT_DEBUG_nRF24L01
		USART1_Send_String((u8*)"\nThe initial state registers nRF after reset...\n");
		nRF_get_Register_Value(); //Читаем регистры nRF
	#endif //PRINT_DEBUG_nRF24L01

		nRF_Config(); //Конфигурация nRF24L01
}

//**************************************************************************
//**************************************************************************

//Конфигурация nRF24L01...
void nRF_Config(void){

	nRF_Status = IDLE;		//Транссивер в режиме ожидания
	nRF_Protocol = WAITING; 	//Обработчик событий nRF - ожидание

	//nRF_number_of_TX_data_packets = 0; //Обнуляем счётчики переданных пакетов
	//nRF_number_of_RX_data_packets = 0;
	//nRF_number_of_LOST_data_packets = 0;

	//nRF_frame_number = 0; //Сброс счётчика событий

	nRF_CE_LOW_LEVEL;	//Режим передачи nRF
	nRF_CSN_HIGH_LEVEL;	//Завершение сеанса обмена по шине SPI

	nRF_Exchange_Word(W_REGISTER,EN_AA,0b00000001);			//Автоподтверждение
	nRF_Exchange_Word(W_REGISTER,EN_RXADDR,0b00000001);		//Включение каналов
	nRF_Exchange_Word(W_REGISTER,SETUP_AW,0b00000011);		//Установка длинны адреса
	nRF_Exchange_Word(W_REGISTER,SETUP_RETR,0b11111111);	//15-ть попыток, пауза 4ms
	nRF_Exchange_Word(W_REGISTER,RF_CH,nRF_FREQ);			//Выбор частотты RF
	nRF_Exchange_Word(W_REGISTER,RF_SETUP,0b00100110);		//250kbps, 0dBm
	nRF_Exchange_Word(W_REGISTER,DYNPD,1<<DPL_P0);			//Разрешение ДДПД
	nRF_Exchange_Word(W_REGISTER,FEATURE,1<<EN_DPL);		//Глобальное разрешение ДДПД

	nRF_IRQ_FLAG_CLEAR;		//Очищаем флаги прерываний
	nRF_OBSERVE_RESET;		//Очищаем счётчик утерянных пакетов
	nRF_FLUSH_RX_BUFFER;	//Очищаем RX буффер
	nRF_FLUSH_TX_BUFFER;	//Очищаем TX буффер

	nRF_RX_Mode(); //Режим приёма данных nRF

	#ifdef PRINT_DEBUG_nRF24L01
		USART1_Send_String((u8*)"The state registers nRF after configuration...\n");
		nRF_get_Register_Value(); //Читаем регистры nRF
	#endif //PRINT_DEBUG_nRF24L01
}

//**************************************************************************
//**************************************************************************

//Режим приёма данных nRF...
void nRF_RX_Mode(void){

	nRF_RX_MODE;
	nRF_CE_HIGH_LEVEL;	//Режим приёма
	_delay(100); 		//Пауза на переключения режима
}

//**************************************************************************
//**************************************************************************

//Обработчик событий nRF...
void EXTI15_10_IRQHandler(void){

//Читаем регистр статуса...
	uint8_t status = nRF_Exchange_Word(R_REGISTER,STATUS,NOP);

	//Данные переданы...
		if (status & 1<<TX_DS) {

			nRF_number_of_TX_data_packets++;
			nRF_RX_Mode();
		}

		//Данные приняты...
			if (status & 1<<RX_DR) {

				nRF_number_of_RX_data_packets++;
				nRF_Protocol = COMMAND_RECEIVED;

			//Выгружаем принятые данные в буффер...
				nRF_Read_Buffer(R_REGISTER,R_RX_PAYLOAD,nRF_Exchange_Word(R_RX_PL_WID,NULL,NOP));
			}

			//Данные потеряны...
				if (status & 1<<MAX_RT) {

					nRF_number_of_LOST_data_packets++;

					nRF_FLUSH_TX_BUFFER;
					nRF_RX_Mode();
				}

	nRF_IRQ_FLAG_CLEAR;

	USART1_Send_String((u8*)"TX|RX|LOST->");
	USART1_Print_Byte(nRF_number_of_TX_data_packets, HEX);
	USART1_Send_Byte('|');
	USART1_Print_Byte(nRF_number_of_RX_data_packets, HEX);
	USART1_Send_Byte('|');
	USART1_Print_Byte(nRF_number_of_LOST_data_packets, HEX);
	USART1_Send_Byte('\n');

#ifdef PRINT_DEBUG_nRF24L01
	USART1_Send_String((u8*)"\nThe state registers nRF after event...\n");
	nRF_get_Register_Value(); //Читаем регистры nRF
#endif //PRINT_DEBUG_nRF24L01

	EXTI->PR = 1<<10; //Cбрасываем флаг прерывания
}

//**************************************************************************
//**************************************************************************

//Обмен байтом nRF...
u8 nRF_Exchange_Byte(u8 data){

	u8 registr = 0;

		nRF_CSN_LOW_LEVEL;

		*(volatile uint8_t *)&SPI2->DR = data; //Передаём байт
				while (SPI2->SR & (1<<7)); //Ждем пока SPI2 занят

			registr = *(volatile uint8_t *)&SPI2->DR;

		nRF_CSN_HIGH_LEVEL;

	return registr;
}

//**************************************************************************
//**************************************************************************

//Обмен словом nRF...
u8 nRF_Exchange_Word(u8 cmd, u8 addr, u8 data){

	uint8_t registr = 0;

		nRF_CSN_LOW_LEVEL;

		*(volatile uint8_t *)&SPI2->DR = cmd | addr; //Передаём адрес целевого регистра + комманда записи
			while (SPI2->SR & (1<<7)); //Ждем пока SPI2 занят

					registr = *(volatile uint8_t *)&SPI2->DR;
					_delay(500); //Пауза 15мкс

				*(volatile uint8_t *)&SPI2->DR = data; //Передаём байт
				while (SPI2->SR & (1<<7)); //Ждем пока SPI2 занят

			registr = *(volatile uint8_t *)&SPI2->DR;

		nRF_CSN_HIGH_LEVEL;

	return registr;
}

//**************************************************************************
//**************************************************************************

//Отправить строку по nRF...
void nRF_Send_String(u8 *data){

	//if(nRF_Status == BUSY) return USART1_Send_String((u8*)"ERROR - nRF is BUSY!...\n");
	//nRF_Status = BUSY;

	nRF_Write_Buffer(W_REGISTER,W_TX_PAYLOAD,data); //Загружаем данные

	nRF_CE_LOW_LEVEL;
	nRF_TX_MODE;
	nRF_START_TRANSMIT_DATA; //Начало передачи данных
}

//**************************************************************************
//**************************************************************************

//Чтение массива по адресу, SPI2...
void nRF_Read_Buffer(u8 cmd, u8 addr, u8 number_of_elements){

	u8 null = 0;

	nRF_CSN_LOW_LEVEL; //Начало сеанса обмена по шине SPI

		*(volatile uint8_t *)&SPI2->DR = cmd | addr; //Передаём адрес целевого регистра + комманда записи
		while (SPI2->SR & (1<<7)); //Ждем пока SPI2 занят
		null = *(volatile uint8_t *)&SPI2->DR;

			for (unsigned char i = 0; i <= number_of_elements; i++){

				*(volatile uint8_t *)&SPI2->DR = 0xFF;
				while (SPI2->SR & (1<<7)); //Ждем пока SPI2 занят
				nRF_RX_buffer[i] = *(volatile uint8_t *)&SPI2->DR;
			}

	nRF_CSN_HIGH_LEVEL; //Завершение сеанса обмена по шине SPI
}

//**************************************************************************
//**************************************************************************

//Запись массива по адресу, SPI2...
void nRF_Write_Buffer(u8 cmd, u8 addr, u8 *buffer){

	u8 null;

	nRF_CSN_LOW_LEVEL; //Начало сеанса обмена по шине SPI

		*(volatile uint8_t *)&SPI2->DR = cmd | addr;
		while (SPI2->SR & (1<<7)); //Ждем пока SPI2 занят
		null = *(volatile uint8_t *)&SPI2->DR;

			while(*buffer)
				{
					*(volatile uint8_t *)&SPI2->DR = *buffer++;
					while (SPI2->SR & (1<<7)); //Ждем пока SPI2 занят
					null = *(volatile uint8_t *)&SPI2->DR;
				}

	nRF_CSN_HIGH_LEVEL; //Завершение сеанса обмена по шине SPI
}

//**************************************************************************
//**************************************************************************

//Читаем регистры nRF...
void nRF_get_Register_Value(void){

	USART1_Send_String((u8*)"STATUS........");
	USART1_Print_Byte(nRF_Exchange_Word(R_REGISTER,STATUS,NOP), BIN);
	USART1_Send_Byte('\n');

	USART1_Send_String((u8*)"CONFIG........");
	USART1_Print_Byte(nRF_Exchange_Word(R_REGISTER,CONFIG,NOP), BIN);
	USART1_Send_Byte('\n');

	USART1_Send_String((u8*)"EN_AA.........");
	USART1_Print_Byte(nRF_Exchange_Word(R_REGISTER,EN_AA,NOP), BIN);
	USART1_Send_Byte('\n');

	USART1_Send_String((u8*)"EN_RXADDR.....");
	USART1_Print_Byte(nRF_Exchange_Word(R_REGISTER,EN_RXADDR,NOP), BIN);
	USART1_Send_Byte('\n');

	USART1_Send_String((u8*)"SETUP_AW......");
	USART1_Print_Byte(nRF_Exchange_Word(R_REGISTER,SETUP_AW,NOP), BIN);
	USART1_Send_Byte('\n');

	USART1_Send_String((u8*)"SETUP_RETR....");
	USART1_Print_Byte(nRF_Exchange_Word(R_REGISTER,SETUP_RETR,NOP), BIN);
	USART1_Send_Byte('\n');

	USART1_Send_String((u8*)"RF_CH.........");
	USART1_Print_Byte(nRF_Exchange_Word(R_REGISTER,RF_CH,NOP), BIN);
	USART1_Send_Byte('\n');

	USART1_Send_String((u8*)"RF_SETUP......");
	USART1_Print_Byte(nRF_Exchange_Word(R_REGISTER,RF_SETUP,NOP), BIN);
	USART1_Send_Byte('\n');

	USART1_Send_String((u8*)"OBSERVE_TX....");
	USART1_Print_Byte(nRF_Exchange_Word(R_REGISTER,OBSERVE_TX,NOP), BIN);
	USART1_Send_Byte('\n');

	USART1_Send_String((u8*)"RX_PW_P0......");
	USART1_Print_Byte(nRF_Exchange_Word(R_REGISTER,RX_PW_P0,NOP), BIN);
	USART1_Send_Byte('\n');

	USART1_Send_String((u8*)"FIFO_STATUS...");
	USART1_Print_Byte(nRF_Exchange_Word(R_REGISTER,FIFO_STATUS,NOP), BIN);
	USART1_Send_Byte('\n');

	USART1_Send_String((u8*)"DYNPD.........");
	USART1_Print_Byte(nRF_Exchange_Word(R_REGISTER,DYNPD,NOP), BIN);
	USART1_Send_Byte('\n');

	USART1_Send_String((u8*)"FEATURE.......");
	USART1_Print_Byte(nRF_Exchange_Word(R_REGISTER,FEATURE,NOP), BIN);
	USART1_Send_Byte('\n');
	USART1_Send_Byte('\n');
}

//**************************************************************************
//**************************************************************************
