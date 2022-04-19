/**
 * utility.h
 *
 *  @date Created at:	14/07/2021 10:21:32
 *	@author:	Pedro Igor B. S.
 *	@email:		pibscontato@gmail.com
 * 	GitHub:		https://github.com/pedro-ibs
 * 	tabSize:	8
 *
 * ####################################################################
 * 
 *   Copyright (C) Pedro Igor B. S 2019
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
 * 
 * ####################################################################
 *
 * funções auxiliares
 *
 */

/* Includes ---------------------------------------------------------------------------------------------------------------------------------------------*/
#include "textProtocol.h"
#include "FreeRTOS/include/FreeRTOSConfig.h"

#ifndef utility_H_
#define utility_H_

/* macro -------------------------------------------------------------------------------------------------------------------------------------------------*/
/* Definition --------------------------------------------------------------------------------------------------------------------------------------------*/

// _bool bIsBitSet(EventGroupHandle_t xEventGroup, EventBits_t xBit);


void swap_vSetup(void);

char *swap_pcTakeAccessLargeSwap(void);
char *swap_pcGiveAccessLargeSwap(void);

char *swap_pcTakeAccessSmallSwap(void);
char *swap_pcGiveAccessSmallSwap(void);

#endif /* utility_H_ */
