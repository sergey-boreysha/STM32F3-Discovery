
#ifndef _ADC_H
#define _ADC_H

//Макроопределения...
#define ADC1_START_REGULAR_CONVERSION ADC1->CR |= (1<<2); //ADSTART:1, ADC start of regular conversion

//Прототипы функций...
void ADC1_Init(void);	//Инициализация ADC

#endif
