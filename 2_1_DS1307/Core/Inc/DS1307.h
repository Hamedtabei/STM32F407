/*
 * DS1307.h
 *
 *  Created on: Oct 19, 2024
 *      Author: Y.HamedT
 */

#ifndef DS1307_H_
#define DS1307_H_

#include "main.h"

/**************************** define **********************************/

// Hi2c Pointer to a I2C_HandleTypeDef structure
#define H_I2C				(hi2c1)

// Target device address
#define	DS1307_ADDRESS		(0xD0)

// Day of the week constants
#define MONDAY				(1)
#define TUESDAY				(2)
#define WEDNESDAY			(3)
#define THURSDAY			(4)
#define FRIDAY				(5)
#define SATURDAY			(6)
#define SUNDAY				(7)

// Square Wave Output Rate Constants
#define SQW_RATE_1			(0)
#define SQW_RATE_4K			(1)
#define SQW_RATE_8K			(2)
#define SQW_RATE_32K		(3)

// DS1307 Register Address Constants
#define REG_SEC				(0)
#define REG_MIN				(1)
#define REG_HOUR			(2)
#define REG_DOW				(3)
#define REG_DATE			(4)
#define REG_MON				(5)
#define REG_YEAR			(6)
#define REG_CTRL			(7)

/*************************** function ***********************************/

void 	DS1307_Init();

// Setters
void 	DS1307_SetYear(uint16_t year);
void 	DS1307_SetMonth(uint8_t mon);
void 	DS1307_SetDay(uint8_t day);

void 	DS1307_SetDayOfWeek(uint8_t dow);

void 	DS1307_SetHour(uint8_t hour);
void 	DS1307_SetMinute(uint8_t min);
void 	DS1307_SetSecond(uint8_t sec);

void	DS1307_SetTime(uint8_t hour, uint8_t min, uint8_t sec);
void	DS1307_SetDate(uint16_t year, uint8_t mon, uint8_t date);

// Getters
uint16_t DS1307_GetYear();
uint8_t DS1307_GetMonth();
uint8_t DS1307_GetDay();

uint8_t DS1307_GetDayOfWeek();

uint8_t DS1307_GetHour();
uint8_t DS1307_GetMinute();
uint8_t DS1307_GetSecond();

void 	DS1307_GetDateStr(char divider, char output[]) ;
void 	DS1307_GetDOWStr(char output[]);
void 	DS1307_GetTimeStr(char output[]);

#endif /* DS1307_H_ */
