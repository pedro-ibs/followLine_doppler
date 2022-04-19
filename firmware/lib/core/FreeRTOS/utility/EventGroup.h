/**
 * EventGroup.h
 *
 *  @date Created at:	14/07/2021 13:06:33
 *	@author:	Pedro Igor B. S.
 *	@email:		pibscontato@gmail.com
 * 	GitHub:		https://github.com/pedro-ibs
 * 	tabSize:	8
 * 
 * ###################################################################
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
 * ###################################################################
 *
 * Funções auxiliares do FreeRTOS dedicadas ao Eventos
 *
 */

/* Includes ---------------------------------------------------------------------------------------------------------------------------------------------*/
#include "FreeRTOS/include/FreeRTOSConfig.h"
#include "FreeRTOS/include/FreeRTOS.h"
#include "FreeRTOS/include/event_groups.h"


#ifndef event_group_H_
#define event_group_H_

/* macro ------------------------------------------------------------------------------------------------------------------------------------------------*/
/* Definition --------------------------------------------------------------------------------------------------------------------------------------------*/

_bool evtg_bIsBitSet(EventGroupHandle_t xEventGroup, EventBits_t xBit);

#endif /* event_group_H_ */
