
#include "main.h"

//**************************************************************************

//Инициализация SPI1...
void ADC1_Init(void){

	RCC->AHBENR |= 1<<19; //ADC1 and ADC2 enable

	GPIOC->MODER |= 1<<1|1<<0; //Analog mode PC0
	GPIOC->AFRL |= 1<<0; //Alternate function for pin PC0 - AF1

	RCC->AHBENR |= 1<<28; //ADC1 and ADC2 enable
	ADC1_2->CCR |= 1<<17|1<<16; //CKMODE[1:0]:11, ADC clock mode

	//ADC voltage regulator enable sequence
	ADC1->CR &= ~(1<<29|1<<28); //ADVREGEN[1:0]:00, Intermediate stateп»ї
	ADC1->CR |=  (0<<29|1<<28); //ADVREGEN[1:0]:01, ADC voltage regulator enable
	_delay_us(100);

	//ADC1->CR &=  ~(1<<30); //ADCALDIF:0, Calibration in Single-ended inputs Mode
	//ADC1->CR |=  (1<<30); //ADCALDIF:1, Calibration in Differential inputs Mode

	ADC1->CR |= (1<<31); //ADCAL:1, ADC calibration
	while(ADC1->CR & (1<<31)); //ADCAL:0, Calibration complete

	//ADC1->SMPR1 |= (0<<5|0<<4|0<<3); //SMPx[2:0]:000, Sampling time for channel

	ADC1->CFGR |=  (1<<4|0<<3); //RES[1:0]:10, Data resolution 8-bit

	ADC1->SQR1 |= (0<<10|0<<9|1<<8|1<<7|0<<6); //SQ1[4:0]:00110, 1st conversion in regular sequence
	//ADC1->SQR1 |= (0<<3|0<<2|0<<1|0<<0); //L[3:0]:0000, One conversion

	ADC1->CFGR |= (1<<13); //CONT:1, Continuous conversion mode


	//ADC1->CR |= (1<<1); //Disable the ADC
	ADC1->CR |= (1<<0); //ADEN:1, Enable the ADC
	while(!(ADC1->ISR & 1<<0)); //Wait until ADRDY=1
}
