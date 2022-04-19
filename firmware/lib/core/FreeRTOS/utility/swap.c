/**
 * swap.c.c
 *
 *  @date Created at:	14/07/2021 10:28:40
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
 * afim de otimizar o uso de memoria do microcontrolador, foi 
 * criado um recurso de swap, um bloco de memoria que pode 
 * ser usado para troca de dados. para utilizar a swap deve ser
 * seguido as seguintes etapas.
 * 
 * * pegar o acesso da swap 
 * 
 * * utilizar algumas das funções de transferência de dados
 * * ou o ponteiro da swap, esse ponteiro deve ser temporario
 * * para evitar acesso indevido na memoria swap
 * 
 * * devolver o acesso da swap
 * 
 * ! a swap só pode ser usada por apenas uma task por vez
 * ! portanto evite usar delays enquanto estiver utilizando
 * ! a swap 
 * 
 * 
 * char pcSwap = swap_pcTakeAccessLargeSwap();
 * 
 * 	[... código ...]
 * 
 * pcSwap = swap_pcGiveAccessLargeSwap();
 *
 */


/* Includes ---------------------------------------------------------------------------------------------------------------------------------------------*/
#include "swap.h"
#include "FreeRTOS/include/FreeRTOSConfig.h"
#include "FreeRTOS/includes.h"
/* Private macro ----------------------------------------------------------------------------------------------------------------------------------------*/
/* Private variables ------------------------------------------------------------------------------------------------------------------------------------*/
static char pcLargeSwap[configSIZE_LargeSwap]		= "";
static char pcSmallSwap[configSIZE_SmallSwap]		= "";
static SemaphoreHandle_t xLargeSwapMutex		= NULL;
static SemaphoreHandle_t xSmallSwapMutex		= NULL;
/* Private Functions ------------------------------------------------------------------------------------------------------------------------------------*/
/* Functions --------------------------------------------------------------------------------------------------------------------------------------------*/


/**
 * @brief inicia as variaves de controle da swap
 */
void swap_vSetup(void){
	
	if(xLargeSwapMutex == NULL){
		xLargeSwapMutex = xSemaphoreCreateMutex();
	}

	if(xSmallSwapMutex == NULL){
		xSmallSwapMutex = xSemaphoreCreateMutex();
	}

	textp_puCleanBlk((u8*)pcLargeSwap, configSIZE_LargeSwap);
	textp_puCleanBlk((u8*)pcLargeSwap, configSIZE_SmallSwap);
}


/**
 * @brief pega o acesso da swap
 * 
 * @return CHAR: ponteiro da swap
 */
char *swap_pcTakeAccessLargeSwap(void){
	xSemaphoreTake(xLargeSwapMutex, portMAX_DELAY);
	textp_puCleanBlk((u8*)pcLargeSwap, configSIZE_LargeSwap);

	return pcLargeSwap;
}


/**
 * @brief devolve o acesso da swap
 * 
 * @return CHAR: NULL
 */
char *swap_pcGiveAccessLargeSwap(void){
	textp_puCleanBlk((u8*)pcLargeSwap, configSIZE_LargeSwap);	
	xSemaphoreGive(xLargeSwapMutex);
	return NULL;
}


/**
 * @brief pega o acesso da swap
 * 
 * @return CHAR: ponteiro da swap
 */
char *swap_pcTakeAccessSmallSwap(void){
	xSemaphoreTake(xSmallSwapMutex, portMAX_DELAY);
	textp_puCleanBlk((u8*)pcSmallSwap, configSIZE_SmallSwap);
	return pcSmallSwap;
}


/**
 * @brief devolve o acesso da swap
 * 
 * @return CHAR: NULL
 */
char *swap_pcGiveAccessSmallSwap(void){
	textp_puCleanBlk((u8*)pcSmallSwap, configSIZE_SmallSwap);	
	xSemaphoreGive(xSmallSwapMutex);
	return NULL;
}




/*########################################################################################################################################################*/
/*########################################################################################################################################################*/
/*########################################################################################################################################################*/
/*-------------------------------------------------------------------- Private Functions -----------------------------------------------------------------*/
/*########################################################################################################################################################*/
