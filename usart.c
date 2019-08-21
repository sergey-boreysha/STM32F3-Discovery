
#include "usart.h"

//����������� ����������...
volatile enum ptcl USART1_Status_Command = WAITING;	//���� ��������� RX ������ - ��������!
u8 USART1_RX_Buffer[16];							//����� ������� USART1

//**************************************************************************

//������������� USART1...
void USART1_Init(void){

	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;	//�������� ������������ USART1

	GPIOA->MODER |= (1<<19|0<<18); 			//PA9, TX, AF7 mode
	GPIOA->AFRH |= (0<<7|1<<6|1<<5|1<<4);	//PA9 in AF7 mode

	GPIOA->MODER |= (1<<21|0<<20);			//RX, PA10 AF7 mode
	GPIOA->AFRH |= (0<<11|1<<10|1<<9|1<<8);	//PA10 in AF7 mode

	USART1->BRR = 0x1D4C;	//Baudrate 9600
	USART1->CR1 |= 1<<0;	//USART1 enable
	USART1->CR1 |= 1<<3;	//Transmitter enable
	USART1->CR1 |= 1<<2;	//Reciver enable

	USART1->CR1 |= 1<<5;	//�������� ���������� ��������
	NVIC_EnableIRQ(USART1_IRQn);

	USART1_Send_Byte('\n'); //�������� �������� ����� � ����� ������

}

//**************************************************************************

//������� ���� �� USART1...
void USART1_Send_Byte(u8 txData) {

	USART1->TDR = txData;			//������� ����
	while(!(USART1->ISR & (1<<7)));	//��� ���� ����������� TDR
}

//**************************************************************************

//������� ������ �� USART1...
void USART1_Send_String(u8 *data){

	while(*data)
		{
			USART1->TDR = *data++;
			while(!(USART1->ISR & (1<<7))); //��� ���� ����������� TDR
		}
}

//**************************************************************************

//�������������� ����� � ����� � ����� � USART1...
void USART1_Print_Byte(u8 data, enum type number_system){

	if (number_system == HEX)
		{

			uint8_t sign1 = 0;
			uint8_t sign0 = 0;

				sign1 = data>>4;
				if (sign1 <= 9) sign1 += '0'; else sign1 += '7';

				sign0 = data & 0b00001111;
				if (sign0 <= 9) sign0 += '0'; else sign0 += '7';

			USART1_Send_Byte(sign1);
			USART1_Send_Byte(sign0);
		}

			if (number_system == BIN)
				{

					USART1_Send_String((u8*)"0b");

					for (u8 i = 0; i <= 7; i++)
						{
							if (data & 0b10000000) USART1_Send_Byte('1'); else USART1_Send_Byte('0');
							data = data << 1;
						}
				}

}

//**************************************************************************

//�������������� ����� � ����� � ����� � USART1...
void USART1_Print_Word(u16 data){

	uint8_t sign3 = 0;
	uint8_t sign2 = 0;
	uint8_t sign1 = 0;
	uint8_t sign0 = 0;

		sign3 = (data>>12) & 0b0000000000001111;
		if (sign3 <= 9) sign3 += 0x30; else sign3 += 0x37;

			sign2 = (data>>8) & 0b0000000000001111;
			if (sign2 <= 9) sign2 += 0x30; else sign2 += 0x37;

			sign1 = (data>>4) & 0b0000000000001111;
			if (sign1 <= 9) sign1 += 0x30; else sign1 += 0x37;

		sign0 = data & 0b0000000000001111;
		if (sign0 <= 9) sign0 += 0x30; else sign0 += 0x37;

	USART1_Send_Byte(sign3);
	USART1_Send_Byte(sign2);
	USART1_Send_Byte(sign1);
	USART1_Send_Byte(sign0);
}

//**************************************************************************

void USART1_IRQHandler(void){

	static u8 index = 0;

	USART1_RX_Buffer[index] = USART1->RDR; //��������� �������� ����

//������� ������� ���������, ������� �������
	if (USART1_RX_Buffer[index] == '&')
		{
			USART1->CR1 &= ~(1<<5);						//��������� ���������� ��������
			USART1_RX_Buffer[index+1] = '\0';			//���������� ������� ��������� ������
			USART1_Status_Command = COMMAND_RECEIVED;	//���� ��������� RX ������ - �������� �������!
			index = 0;
			return;
		}

	if (++index > 15) index = 0; //������ �� ������������
}

//**************************************************************************
//**************************************************************************
