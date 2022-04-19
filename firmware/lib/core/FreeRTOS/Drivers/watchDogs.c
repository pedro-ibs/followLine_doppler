/**
 *  watchDogs.c
 *
 *  @date Created at:	11/03/2021 09:22:13
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
 *	iwdg_vStartConter( iwdgVC_0 )
 *	[...] código [...]
 *	iwdg_vStopConter( iwdgVC_0 )
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
/* stm includes */
#include <core.h>
#include <stm32f1xx_hal_iwdg.h>

/* FreeRTOS standard includes.  */
#include <FreeRTOS/include/FreeRTOSConfig.h>
#include <FreeRTOS/include/FreeRTOS.h>
#include <FreeRTOS/include/task.h>
#include <FreeRTOS/include/list.h>
#include <FreeRTOS/include/queue.h>
#include <FreeRTOS/include/semphr.h>
#include <FreeRTOS/include/portable.h>
#include <FreeRTOS/include/event_groups.h>

#include <FreeRTOS/Drivers/watchDogs.h>

#include <FreeRTOS/Drivers/rtc.h>




/* Private macro -------------------------------------------------------------*/
#define QUEUE_SIZE		( 5			)
#define START_COUNTER_IWDG	( 0x42U			)
#define STOP_COUNTER_IWDG	( 0x4CU			)

#define CHAR_U16		( 2			)
#define IDX_CHAR_VC		( 0 			)
#define IDX_CHAR_CMD		( 1			)


#define IWDG_TASK_DELAY		( _200MS		)
#define IWDG_PRIORITY		( tskIDLE_PRIORITY	)

/* Private variables ---------------------------------------------------------*/
static QueueHandle_t		xQueueIwdg		= NULL;

static TickType_t		puTicks[iwdgVC_NUM]	= { 0 };
static IWDG_HandleTypeDef 	xIwdg			= { 0 };

static u8 puCommand[CHAR_U16]				= { 0 };

/* Private Functions ---------------------------------------------------------*/
void iwdg_vTask(void * pvParameters);



/**
 * @brief iniciar temporisador watch dogs
 *
 */
void iwdg_vInit(void){
	if((xQueueIwdg == NULL) ){
		BaseType_t xErr = xTaskCreate(
			iwdg_vTask,
			"iwdg",
			configMINIMAL_STACK_SIZE*2,
			NULL,
			IWDG_PRIORITY,
			NULL
		);
		
		if(xErr != pdPASS){
			NVIC_SystemReset();
		} 		
	}
}


/**
 * @brief inicia o contador virtual do iwdg
 * @param cuVirtualConter: indice do contador virtual
 */
void iwdg_vStartConter( cu8 cuVirtualConter ){
	if((xQueueIwdg == NULL) ) return;

	u8 puCmd[CHAR_U16];
	puCmd[IDX_CHAR_CMD]	= START_COUNTER_IWDG;
	puCmd[IDX_CHAR_VC]	= cuVirtualConter;

	xQueueSend(xQueueIwdg, puCmd, portMAX_DELAY);
}


/**
 * 
 * @brief para o contador virtual do iwdg
 * @param cuVirtualConter: indice do contador virtual
 */
void iwdg_vStopConter( cu8 cuVirtualConter ){
	if((xQueueIwdg == NULL) ) return;

	u8 puCmd[CHAR_U16];
	puCmd[IDX_CHAR_CMD]	= STOP_COUNTER_IWDG;
	puCmd[IDX_CHAR_VC]	= cuVirtualConter;

	xQueueSend(xQueueIwdg, puCmd, portMAX_DELAY);
}



/*########################################################################################################################################################*/
/*########################################################################################################################################################*/
/*########################################################################################################################################################*/
/*-------------------------------------------------------------------- Private Functions -----------------------------------------------------------------*/
/*########################################################################################################################################################*/

/**
 * @brief task do temporisador watch dogs
 *
 * 
 */
void iwdg_vTask(void * pvParameters){
	(void) pvParameters;

	xQueueIwdg			= xQueueCreate(QUEUE_SIZE, (sizeof(u8)*2));

	/* iniciar iwdg */
	xIwdg.Instance			= IWDG;
	xIwdg.Init.Prescaler		= IWDG_IWDG_PRESCALER;
	xIwdg.Init.Reload		= IWDG_IWDG_CONTER;
	if (HAL_IWDG_Init(&xIwdg) != HAL_OK) {
		Error_Handler();
	}
	HAL_IWDG_Refresh(&xIwdg);

	while(TRUE){

		/* carregar comando do iwdg */
		if(xQueueReceive(xQueueIwdg, puCommand, IWDG_TASK_DELAY) == pdPASS){
				/* é permitido apenas iwdgVC_NUM tenporisadores */
				if( puCommand[IDX_CHAR_VC] < iwdgVC_NUM ){
					switch (puCommand[IDX_CHAR_CMD]) {
						case START_COUNTER_IWDG:
							puTicks[ puCommand[IDX_CHAR_VC] ] = rtc_xTaskGetSeconds() + IWDG_TIMEOUT_TO_RESET;
							break;
						
						case STOP_COUNTER_IWDG:
							puTicks[ puCommand[IDX_CHAR_VC] ] = 0;
							break;

						default:
							__NOP();
							break;
					}

					/* impar variavel */
					puCommand[IDX_CHAR_VC] = 0x00U;
					puCommand[IDX_CHAR_CMD] = 0x00U;
			}
		}

		/* analisar ticks */
		for (u8 uIdx = 0; uIdx < iwdgVC_NUM; uIdx++){
			if(puTicks[uIdx] > 0){
				if(puTicks[uIdx] < rtc_xTaskGetSeconds()){
					puTicks[uIdx] = 0x00U;
					xTaskResumeAll();
					NVIC_SystemReset();
					while (TRUE) { __NOP(); }
					
				}
			}
		}

		/* reiniciar contador do IWDG */
		HAL_IWDG_Refresh(&xIwdg);
	}
}