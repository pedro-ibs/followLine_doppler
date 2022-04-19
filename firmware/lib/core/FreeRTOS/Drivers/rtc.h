/**
 * rtc.h
 *
 *  @date Created at:	03/03/2021 16:09:39
 *	@author:	Pedro Igor B. S.
 *	@email:		pibscontato@gmail.com
 * 	GitHub:		https://github.com/pedro-ibs
 * 	tabSize:	8
 *
 * ########################################################
 *   Copyright (C) Pedro Igor B. S 2021
 * -------------------------------------------------------------------
 *
 *   Licença: GNU GPL 2
 * -------------------------------------------------------------------
 *   This program is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU General Public License as
 *   published by the Free Software Foundation; version 2 of the
 *   License.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 * -------------------------------------------------------------------
 * ########################################################
 *
 * Controla a periferia de hardware RCT. essa biblioteca
 * pode gerar uma data como 20/7/69-23:56:19
 *
 */

/* Includes ------------------------------------------------------------------*/
#include <core.h>
#include<FreeRTOS/include/FreeRTOSConfig.h>

#ifndef rtc_H_
#define rtc_H_

/* macro ---------------------------------------------------------------------*/
/* Definition ----------------------------------------------------------------*/
void rtc_vInit(void);
void rtc_vUpdate(RTC_DateTypeDef *xDate, RTC_TimeTypeDef *xTime);


TickType_t rtc_xTaskGetSeconds( void );
RTC_TimeTypeDef rtc_xGetTime( void );
RTC_DateTypeDef rtc_xGetDate( void );

CCHR *rtc_pcBuildTimeStamp( const RTC_DateTypeDef xDate, const RTC_TimeTypeDef xTime, char *pcBuffer );
CCHR *rtc_pcGetTimeStamp( char *pcBuffer );

_bool rtc_bTimerOverflow( TickType_t *pxTimeS, const TickType_t cxOverflowS );

#endif /* rtc_H_ */



