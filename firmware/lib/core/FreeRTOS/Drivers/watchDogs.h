/**
 *  watchDogs.h
 *
 *  @date Created at:	11/03/2021 09:20:57
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
 * Esse driver utiliza o periferio IWDG, o estouro do contado
 * pode ser configurado pela macro IWDG_CONTER e IWDG_PRESCALER
 * 
 * 
 * Esse drive é uma task que fica contantemente reiniciando constatemente
 * o contador do iwdg. ela por padrão tem prioridade de tskIDLE_PRIORITY
 * a prioridade baixa é utilizada nesse caso para evitar problemas como
 * starvation.
 * 
 * 
 * A frequência que contador do IWDG é reiniciado pode ser configurado
 * na macro IWDG_TASK_DELAY. Esse delay é gerado pela função xQueueReceive
 * que suspende a task enquanto não houver um elemento novo na queue ou
 * enquanto não estourar o tempo IWDG_TASK_DELAY. Protando ao enviar
 * um elemteno para queue o contador é reiniciado em sequencia pela task 
 * do iwdg.
 * 
 * Para evitar o travamente ou a demora de outras tasks de forma que prejudique
 * o determinismo do sistema é utilizado duas funções: 
 * 
 *	iwdg_vStartConter( 0 )
 *	[...] código [...]
 *	iwdg_vStopConter( 0 )
 * 
 * Essas funções populam uma queue com um comando sinalizando que a contagem
 * comessou ou que terminou. Se essa contagem passar do tempo definido pela 
 * macro TIMEOUT_TO_RESET a task do IWDG força o reinicio so microcontrolador.
 * Essa que aceita apenas um elemento por vez. 
 *
 * O tipo de dados IwdgVirtualConter é o que determinam o numero de contadores
 * virtuais desse driver. cada contador deve ser dedicado para apenas uma task
 * ele é do tipo TickType_t e não o cupa o heap das tasks
 * 
 */

/* Includes ------------------------------------------------------------------*/
#include <core.h>
#include <FreeRTOS/include/FreeRTOSConfig.h>

#ifndef watch_dogs_H_
#define watch_dogs_H_

/* macro ---------------------------------------------------------------------*/
#define IWDG_TIMEOUT_TO_RESET		( configIWDG_TIMEPUT_TO_RESET	)
#define IWDG_IWDG_CONTER 		( configIWDG_CONTER		)
#define IWDG_IWDG_PRESCALER		( configIWDG_PRESCALER		)
/* Definition ----------------------------------------------------------------*/
typedef enum {
	iwdgVC_0 = 0,
	iwdgVC_1,
	iwdgVC_2,
	iwdgVC_3,
	iwdgVC_4,
	iwdgVC_5,
	iwdgVC_6,
	iwdgVC_7,
	iwdgVC_8,
	iwdgVC_9,
	iwdgVC_10,
	iwdgVC_11,
	iwdgVC_12,
	iwdgVC_13,
	iwdgVC_14,
	iwdgVC_15,
	iwdgVC_16,
	iwdgVC_17,
	iwdgVC_18,
	iwdgVC_19,
	iwdgVC_20,
	iwdgVC_21,
	iwdgVC_22,
	iwdgVC_23,
	iwdgVC_NUM
} IwdgVirtualConter;


void iwdg_vInit(void);
void iwdg_vStartConter( cu8 cuVirtualConter );
void iwdg_vStopConter( cu8 cuVirtualConter );



#endif /* watch_dogs_H_ */