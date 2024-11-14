/*
 * DS1307.c
 *
 *  Created on: Oct 19, 2024
 *      Author: Y.HamedT
 */

#include "DS1307.h"

/************************************************/
static uint8_t _DS1307_Decode(uint8_t value) {
	uint8_t decoded = value & 127;
	decoded = (decoded & 15) + 10 * ((decoded & (15 << 4)) >> 4);
	return decoded;
}

/************************************************/
static uint8_t _DS1307_DecodeH(uint8_t value) {
	if (value & 128) // 12 hours
		// for 12 hours but AM is wrong. //TODO
		value = (value & 15) + (12 * ((value & 32) >> 5));
	else // 24 hours
	    value = (value & 15) + (10 * ((value & 48) >> 4));
	return value;
}

/************************************************/
static uint8_t _DS1307_DecodeY(uint8_t value) {
	uint8_t decoded = (value & 15) + 10 * ((value & (15 << 4)) >> 4);
	return decoded;
}

/************************************************/
static uint8_t _DS1307_Encode(uint8_t value) {
	uint8_t encoded = ((value / 10) << 4) + (value % 10);
	return encoded;
}

/************************************************/
static uint8_t _DS1307_ReadReg(uint8_t reg) {
	uint8_t RX_Buffer[1];
	HAL_I2C_Mem_Read(&H_I2C, DS1307_ADDRESS, reg, 1, RX_Buffer, 1, 1000);
	return RX_Buffer[0];
}

/************************************************/
static void _DS1307_WriteReg(uint8_t reg, uint8_t value) {
	uint8_t TX_Buffer[1];
	TX_Buffer[0] = value;
	HAL_I2C_Mem_Write(&H_I2C, DS1307_ADDRESS, reg, 1, TX_Buffer, 1, 1000);
}

/************************************************/
static void _DS1307_EnableModule() {
	uint8_t buffer[1];
	buffer[0] = _DS1307_ReadReg(REG_SEC);
	buffer[0] &= 0x7f;
	_DS1307_WriteReg(REG_SEC, buffer[0]);
}

/************************************************/
static void _DS1307_EnableSQW(bool enable) {
	uint8_t _reg = _DS1307_ReadReg(REG_CTRL);
	_reg &= ~(1 << 4);
	_reg |= (enable << 4);
	_DS1307_WriteReg(REG_CTRL, _reg);
}

/************************************************/
static void _DS1307_SetSQWRate(int rate) {
	uint8_t _reg = _DS1307_ReadReg(REG_CTRL);
	_reg &= ~(3);
    _reg |= (rate);
    _DS1307_WriteReg(REG_CTRL, _reg);
}

/************************************************/
void DS1307_Init(){
	_DS1307_EnableModule();
	_DS1307_EnableSQW(1);
	_DS1307_SetSQWRate(SQW_RATE_4K);
}

/************************************************/
void DS1307_SetYear(uint16_t year) {
	if((year >= 2000) && (year < 3000)){
		year -= 2000;
		_DS1307_WriteReg(REG_YEAR,  _DS1307_Encode(year));
	}
}

/************************************************/
void DS1307_SetMonth(uint8_t mon) {
	if((mon > 0) && (mon <= 12))
		_DS1307_WriteReg(REG_MON,  _DS1307_Encode(mon));
}

/************************************************/
void DS1307_SetDay(uint8_t day) {
	if((day > 0) && (day <= 31))
		_DS1307_WriteReg(REG_DATE,   _DS1307_Encode(day));
}

/************************************************/
void DS1307_SetDayOfWeek(uint8_t dow) {
	if ((dow > 0) && (dow < 8))
		_DS1307_WriteReg(REG_DOW,  dow);
}

/************************************************/
void DS1307_SetHour(uint8_t hour) {
	uint8_t current_h = _DS1307_ReadReg(REG_HOUR);
	uint8_t twlv_hour = current_h & 0x20;//if 0, 24 hour. else 12 hour.

	if(twlv_hour){ // 12 hour , 6th bit = 1
		if ((hour > 0) && (hour <= 12))
				_DS1307_WriteReg(REG_HOUR, (_DS1307_Encode(hour) || twlv_hour));
	}
	else{ // 24 hour
		if ((hour >= 0) && (hour < 24))
				_DS1307_WriteReg(REG_HOUR, _DS1307_Encode(hour));
	}
}

/************************************************/
void DS1307_SetMinute(uint8_t min) {
	if ((min >= 0) && (min < 60))
		_DS1307_WriteReg(REG_MIN,  _DS1307_Encode(min));
}

/************************************************/
void DS1307_SetSecond(uint8_t sec) {
	if((sec >= 0) && (sec < 60))
		_DS1307_WriteReg(REG_SEC,  _DS1307_Encode(sec));
}

/************************************************/
void DS1307_SetTime(uint8_t hour, uint8_t min, uint8_t sec) {
	DS1307_SetHour(hour);
	DS1307_SetMinute(min);
	DS1307_SetSecond(sec);
}

/************************************************/
void DS1307_SetDate(uint16_t year, uint8_t mon, uint8_t date) {
	DS1307_SetYear(year);
	DS1307_SetMonth(mon);
	DS1307_SetDay(date);
}

/************************************************/
uint16_t DS1307_GetYear() {
	 return _DS1307_DecodeY(_DS1307_ReadReg(REG_YEAR)) + 2000;
}

/************************************************/
uint8_t DS1307_GetMonth() {
	return _DS1307_Decode(_DS1307_ReadReg(REG_MON));
}

/************************************************/
uint8_t DS1307_GetDay() {
	return _DS1307_Decode(_DS1307_ReadReg(REG_DATE));
}

/************************************************/
uint8_t DS1307_GetDayOfWeek() {
	return _DS1307_ReadReg(REG_DOW);
}

/************************************************/
uint8_t DS1307_GetHour() {
	return _DS1307_DecodeH(_DS1307_ReadReg(REG_HOUR));
}

/************************************************/
uint8_t DS1307_GetMinute() {
	return _DS1307_Decode(_DS1307_ReadReg(REG_MIN));
}

/************************************************/
uint8_t DS1307_GetSecond() {
	return _DS1307_Decode(_DS1307_ReadReg(REG_SEC));
}

/************************************************/
void DS1307_GetDateStr(char divider, char output[]) {

	uint16_t year = DS1307_GetYear();
	uint8_t mon   = DS1307_GetMonth();
	uint8_t date  = DS1307_GetDay();

	sprintf(output, "%04d%c%02d%c%02d", year, divider, mon, divider, date);
}

/************************************************/
void DS1307_GetDOWStr(char output[])
{
	uint8_t dow = DS1307_GetDayOfWeek();

	switch (dow)
	{
		case MONDAY:
			sprintf(output, "Monday");
			break;
		case TUESDAY:
			sprintf(output, "Tuesday");
			break;
		case WEDNESDAY:
			sprintf(output, "Wednesday");
			break;
		case THURSDAY:
			sprintf(output, "Thursday");
			break;
		case FRIDAY:
			sprintf(output, "Friday");
			break;
		case SATURDAY:
			sprintf(output, "Saturday");
			break;
		case SUNDAY:
			sprintf(output, "Sunday");
			break;
	}
}

/************************************************/
void DS1307_GetTimeStr(char output[]){

	uint8_t hour = DS1307_GetHour();
	uint8_t min  = DS1307_GetMinute();
	uint8_t sec  = DS1307_GetSecond();

	sprintf(output, "%02d:%02d:%02d", hour, min, sec);
}

/************************************************/
