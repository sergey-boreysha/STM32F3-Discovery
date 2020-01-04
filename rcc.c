
#include "L3GD20.h"

//**************************************************************************
//**************************************************************************

//Инициализация ФАПЧ...
void RCC_init(){

	RCC->CFGR |= (0<<21|1<<20|1<<19|1<<18);	//PLL multiplication factor
											//0111: PLL input clock x 9

	RCC->CFGR |= (1<<16|0<<15);		//PLL entry clock source
									//10: HSE used as PREDIV1 entry

	RCC->CR |= (1<<16);				//HSE clock enable
	while(!(RCC->CR & (1<<17)));	//HSE clock ready flag
	USART1_Send_String((u8*)"HSE oscillator is stable!...\n");

	//FLASH->ACR |= FLASH_ACR_PRFTBE;
	FLASH->ACR |= (uint32_t)((uint8_t)0x02);

	RCC->CR |= (1<<24);				//PLL enable
	while(!(RCC->CR & (1<<17)));	//PLL clock ready flag
	USART1_Send_String((u8*)"PLL is locked!...\n");

	RCC->CFGR |= (1<<1|0<<0);		//System clock switch
									//10: PLL selected as system clock
	while(!(RCC->CFGR & (1<<3)));	//System clock switch
	USART1->BRR = 0x1D4C;			//Baudrate 9600, 72MHz
	USART1_Send_String((u8*)"PLL selected as system clock!...\n");

}
