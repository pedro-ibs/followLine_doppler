/**
 *  flash.h
 *
 *  @date Created at:	19/03/2021 12:21:07
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
 *
 */

/* Includes ------------------------------------------------------------------*/
#include <core.h>


#ifndef flash_H_
#define flash_H_

/* macro ---------------------------------------------------------------------*/

#define FLASH_PAGE			( configFLASH_PAGE )
#define FLASH_BASE_ADDRESS 		( ( (u32)0x08000000 ) + (PAGESIZE * FLASH_PAGE)  )

void flash_vInit( void );
void flash_vTakeAccesses( void );
void flash_vGiveAccesses( void );

void flash_vWriteByteToBuffer( cu16 cuIdx, cu8 cuData );
void flash_vWriteBlockToBuffer(const u8 *pcuData, const size_t cuSizeBlock);
u8 *flash_puCopyBuffer( u8* puCopyTo, const size_t cuSizeBlock);
u8 *flash_puGetBuffer(void);

void flash_vBufferToFlash(void);
void flash_vFlashToBuffer(void);

_bool flash_vErase( void  );


/* Definition ----------------------------------------------------------------*/


#endif /* flash_H_ */