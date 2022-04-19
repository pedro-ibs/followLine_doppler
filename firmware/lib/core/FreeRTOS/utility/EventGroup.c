/**
 * EventGroup.c
 *
 *  @date Created at:	14/07/2021 13:06:46
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
#include "EventGroup.h"
/* Private macro ----------------------------------------------------------------------------------------------------------------------------------------*/
/* Private variables ------------------------------------------------------------------------------------------------------------------------------------*/
/* Private Functions ------------------------------------------------------------------------------------------------------------------------------------*/
/* Functions --------------------------------------------------------------------------------------------------------------------------------------------*/



/**
 * @brief testa se o bit está de um eventgroup está setado
 */
_bool evtg_bIsBitSet(EventGroupHandle_t xEventGroup, EventBits_t xBit){
	if(xEventGroup == NULL) return FALSE;
	EventBits_t xBits;
	xBits = xEventGroupGetBits(xEventGroup);
	return ( ( xBits & xBit ) != FALSE );
}


/*########################################################################################################################################################*/
/*########################################################################################################################################################*/
/*########################################################################################################################################################*/
/*-------------------------------------------------------------------- Private Functions -----------------------------------------------------------------*/
/*########################################################################################################################################################*/
