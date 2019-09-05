#ifndef _DTYPE_H
#define _DTYPE_H

#define NULL 0

//Определение пользовательских типов данных...
enum logic {HIGH = 1, LOW = 0}; 		//Логические сигналы
enum boolean {TRUE = 1, FALSE = 0}; 	//Логические операторы
enum direction {CW = 1, CCW = 0}; 		//Операторы вращения
enum type {HEX, DEC, OCT, BIN}; 		//Операторы типов данных
enum stat {BUSY, IDLE, EVENT, SLEEP};	//Операторы состояния

enum ptcl								//Операторы протокола
{
	WAITING,				//Ожидание события
	COMMAND_RECEIVED,		//Комманда принята, в ожидании обработки
	WAITING_ACKNOWLEDGE,
	ACKNOWLEDGE_RECEIVED,
	SEND_ACKNOWLEDGE,
	ACKNOWLEDGE_SENT,
	WAITING_COMMAND,
	SEND_COMMAND,
	DATA_LOST
};


#endif //_DTYPE_H
