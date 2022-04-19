/**
 *  EEPROMemulation.c
 *
 *  @date Created at:	19/03/2021 12:21:35
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
 * Esse driver é utilizado para escrever dados na memoria flash do micro
 * no caso de não haver um chip de memoria EEPROM externo ou interno
 * 
 * apos inicializar as variaveis do sistema com flash_vInit()
 * 
 * é preciso popular o buffer, para isso utilize  flash_vWriteByteToBuffer();
 * ou flash_vWriteBlockToBuffer();
 * 
 * o buffer está alocado na memoria RAM do microcontrolador portanto ainda é
 * volatil. para tornar os dado nem informaçõe não volateis utilise:
 * flash_vBufferToFlash().
 * 
 * toda vez que o micocontrolador inicializar é preciso iniciar a variaves de
 * sistemas flash e em seguida carregar os dados da flash para o buffer com:
 * flash_vFlashToBuffer().  dessa maneira é possível trabalhar com o buffer
 * escrevendo e lendo diversas informações  sem que a flash seja muito exisida.
 * 
 * 
 * utilise a macro FLASH_PAGE para definir qual pagina será utilizada, verifique
 * o manual do dispositivo para saber quantas paginas ele tem.
 * 
 * a macro FLASH_BASE_ADDRESS é o endereço fisico da pagina para acessar use:
 * (const u8*)FLASH_BASE_ADDRESS
 * 
 * 
 * fodas as funções desse driver não utilizam controle de acesso, nesse caso ao 
 * utilizar uma ou um grupo de funções deste driver deve ser usado as funções 
 * de controle de acesso:
 * 
 * flash_vTakeAccesses();
 * ...	[code]	...
 * flash_vGiveAccesses();
 *
 */



/* Includes ------------------------------------------------------------------*/
/* stm includes */
#include <core.h>
#include <stdbool.h>
#include <stm32f1xx_hal.h>


/* FreeRTOS standard includes.  */
#include <FreeRTOS/include/FreeRTOSConfig.h>
#include <FreeRTOS/include/FreeRTOS.h>
#include <FreeRTOS/include/task.h>
#include <FreeRTOS/include/list.h>
#include <FreeRTOS/include/queue.h>
#include <FreeRTOS/include/semphr.h>
#include <FreeRTOS/include/portable.h>

/* include driver */
#include <FreeRTOS/Drivers/flash.h>

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static u8 puBuffer[PAGESIZE]	= { 0 };
static SemaphoreHandle_t xFlash	= NULL;

/* Private Functions ---------------------------------------------------------*/


/**
 * @brief inicialisa o mutex, limpa o buffer de transferência
 * @note: deve ser iinicialisado dentro de uma task
 *
 * 
 */
void flash_vInit( void ){
	if(xFlash == NULL){
		xFlash = xSemaphoreCreateMutex();
	}


	for (size_t uIdx = 0; uIdx < PAGESIZE; uIdx++) {
		puBuffer[uIdx] = 0xFF;
	}

	HAL_FLASH_Unlock();
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPERR);
	HAL_FLASH_Lock();
}


/**
 * @brief pega o acesso da flash
 */
void flash_vTakeAccesses( void ){
	xSemaphoreTake(xFlash, portMAX_DELAY);
}

/**
 * @brief devolve o acesso da flash
 */
void flash_vGiveAccesses( void ){
	xSemaphoreGive(xFlash);
}

/**
 * @brief escreve um byte no buffer 
 * @note: O buffer tem o tamanho em byte correspondente a uma pagina
 * da memoria flash do micocontrolador no caso do stm32fi03Cx é 1024
 * 
 * @param cuIdx: indice do buffer
 * @param cuData: byte a ser salvo
 */
void flash_vWriteByteToBuffer( cu16 cuIdx, cu8 cuData ){
	puBuffer[cuIdx] = cuData;
}

/**
 * @brief escreve um bloco de memoria no buffer
 * @note  O buffer tem o tamanho em byte correspondente a uma pagina
 * da memoria flash do micocontrolador no caso do stm32fi03Cx é 1024
 * 
 * @param pcuData: ponteiro do bloco de memoria
 * @param cuSizeBlock: tamanho do bloco de memoria
 *  
 */
void flash_vWriteBlockToBuffer(const u8 *pcuData, const size_t cuSizeBlock){	
	memcpy(puBuffer, pcuData, cuSizeBlock);
}


/**
 * @brief copia o buffer para outro bloco de memoria
 * @note O bloco deve ter o mesmo tamanho que o buffer da flash ou
 * menor. sendo que o buffer tem o tamanho em byte correspondente a
 * uma pagina damemoria flash do micocontrolador no caso do stm32fi03Cx
 * é 1024
 * 
 * @param puCopyTo: ponteiro do bloco de memoria
 * 
 * @param cuSizeBlock tamanho do bloco a ser copiado
 * 
 * @return ponteiro puCopyTo
 * 
 */
u8 *flash_puCopyBuffer( u8* puCopyTo, const size_t cuSizeBlock){
	memcpy(puCopyTo, puBuffer, cuSizeBlock);
	return puCopyTo;
}



/**
 * @brief pega o ponteiro do buffer de memoria.
 * @note com o ponteiro você tem acesso diteco ao buffer 
 * sempre utiliso o controle de acesso da flash quando estiver
 * manipulando este ponteiro
 * 
 *
 */
u8 *flash_puGetBuffer(void){
	return puBuffer;
}


/**
 * @brief escreve o que tem no buffer na flash
 * 
 *
 */
void flash_vBufferToFlash(void){
	u64 uSwap = 0;
	HAL_FLASH_Unlock();
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPERR);
	for(size_t uIdx=0; uIdx<PAGESIZE/sizeof(u64); uIdx++){
		uSwap = *(u64*)&puBuffer[uIdx*sizeof(u64)];		
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, (FLASH_BASE_ADDRESS+uIdx*sizeof(u64)), uSwap);
	}
	HAL_FLASH_Lock();
}

/**
 * @brief escreve o que tem nq flash no buffer
 * 
 *
 */
void flash_vFlashToBuffer(void){
	HAL_FLASH_Unlock();
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPERR);
	memcpy(puBuffer, (void*)FLASH_BASE_ADDRESS, PAGESIZE);
	HAL_FLASH_Lock();

}


/**
 * @brief apaga a pagina destinada a salvar dados não volateis
 * 
 *
 */
_bool flash_vErase( void ){
	u32 uErr = 0;
	FLASH_EraseInitTypeDef xFlash = { 0 };

	HAL_FLASH_Unlock();
	
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPERR);


	/**
	 * forma como a pagina será apagada, ápenas uma pagina com FLASH_TYPEERASE_PAGES
	 * ou de forma massiva FLASH_TYPEERASE_MASSERASE
	 */
	xFlash.TypeErase	= FLASH_TYPEERASE_PAGES;

	xFlash.PageAddress	= FLASH_BASE_ADDRESS;

	/**
	 * cada FLASH_BANK comtem var varias paginas alguns dispositivos podem ter mais 
	 * de um BANK  
	 */
	xFlash.Banks		= FLASH_BANK_1;

	xFlash.NbPages		= 1;

	if(HAL_FLASHEx_Erase(&xFlash, &uErr) != HAL_OK){
		uErr = 0;
	}

	HAL_FLASH_Lock();

	return (uErr != 0xFFFFFFFF)?(FALSE):(TRUE);
}



/*########################################################################################################################################################*/
/*########################################################################################################################################################*/
/*########################################################################################################################################################*/
/*-------------------------------------------------------------------- Private Functions -----------------------------------------------------------------*/
/*########################################################################################################################################################*/