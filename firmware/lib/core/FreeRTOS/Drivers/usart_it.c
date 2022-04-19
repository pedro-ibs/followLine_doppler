/**
 * usart_it.c
 *
 *  @date Created at:	23/02/2021 16:49:47
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
 * driver serial para a comunicação serial RS232, esse driver
 * faz uso das interrupções USART_IT_TXE e USART_IT_RXNE para
 * transmitir e ressaber dados. Alem disso há recursos de queue
 * (para transmitir) e mutex para auxiliar no uso do driver
 * 
 * é possível  desativar a interrupção USART_IT_RXNE e transmitir
 * sem o uso da interrupção USART_IT_TXE.
 * 
 * para mais detalhes veja as descrições das funções 
 * 
 */



/* Includes ------------------------------------------------------------------*/
/* stm includes */
#include <core.h>
#include <stm32f1xx_hal_uart.h>
#include <stm32f1xx_hal_usart.h>

/* FreeRTOS standard includes.  */
#include <FreeRTOS/include/FreeRTOSConfig.h>
#include <FreeRTOS/include/FreeRTOS.h>
#include <FreeRTOS/include/task.h>
#include <FreeRTOS/include/list.h>
#include <FreeRTOS/include/queue.h>
#include <FreeRTOS/include/semphr.h>
#include <FreeRTOS/include/portable.h>
#include <FreeRTOS/Drivers/usart.h>

#if(USART_USE == USART_IT)


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
typedef struct {
	UART_HandleTypeDef	xHandle;
	IRQn_Type		xIRQ;
	QueueHandle_t		xTXD;
	char 			pcBufferRXD[SIZE_BUFFER_RXD];
	size_t			uBufferSize;
} xUsartHandleData;


static xUsartHandleData xUsartIT[ttyNUM] = {
	{{0}, USART1_IRQn, NULL, "", 0},
	{{0}, USART2_IRQn, NULL, "", 0},
	{{0}, USART3_IRQn, NULL, "", 0}
};



/* Private Functions ---------------------------------------------------------*/
_bool usart_bCheckError(const TTY xtty);
void usart_vInitVars(const TTY xtty);
void usart_vInitIT(const TTY xtty);
void usart_vIT(const TTY xtty,BaseType_t *const pxHigherPriorityTaskWoken);


/**
 * @brief inicia a uart e variaveis use gpio_vInitAll() antes.
 * @param tty, enumeração ttyUSART1, ttyUSART2 ttyUSART3, caso
 * xtty seja invalido a função não será executada
 * @param BaudRate, velocidade da porta serial.
 */
void usart_vSetup(TTY xtty, const BaudRate cuBaudRate){	
	if(xtty >= ttyNUM) return;

	usart_vInitVars(xtty);
	usart_vInit(&xUsartIT[xtty].xHandle, xtty, cuBaudRate);
	usart_vInitIT(xtty);
}


/**
 * @brief desativa a interrupção por recebimento de dados
 * USART_IT_RXNE
 * @param tty, enumeração ttyUSART1, ttyUSART2 ttyUSART3, caso
 * xtty seja invalido a função não será executada.
 */
void usart_vStopIT_RXD(const TTY xtty){
	if(usart_bCheckError(xtty)) return;
	__HAL_USART_DISABLE_IT(&xUsartIT[xtty].xHandle, USART_IT_RXNE);
}

 
/**
 * @brief reativa a interrupção por recebimento de dados
 * USART_IT_RXNE
 * @param tty, enumeração ttyUSART1, ttyUSART2 ttyUSART3, caso
 * xtty seja invalido a função não será executada.
 */
void usart_vStartIT_RXD(const TTY xtty){
	if(usart_bCheckError(xtty)) return;
	__HAL_USART_ENABLE_IT(&xUsartIT[xtty].xHandle, USART_IT_RXNE);	
}


/**
 * @brief calcula o tamanho ocupado do buffer RXD.
 * @param tty, enumeração ttyUSART1, ttyUSART2 ttyUSART3
 * @return Caso xtty não seja um valor valudo o  valor
 * returnado será -1, do contrario o valor returnado
 * será o tamanho ocupado do buffer RXD .
 */
int usart_iSizeBuffer(const TTY xtty){
	if(usart_bCheckError(xtty)) return -1;
	return xUsartIT[xtty].uBufferSize;
}


/**
 * @brief apaga os dados armazenados no buffer.
 * @param tty, enumeração ttyUSART1, ttyUSART2 ttyUSART3, caso
 * xtty seja invalido a função não será executada.
 */
void usart_vCleanBuffer(const TTY xtty){
	if(usart_bCheckError(xtty)) return;
	for(size_t uIdx = 0; uIdx < SIZE_BUFFER_RXD; uIdx++){
		xUsartIT[xtty].pcBufferRXD[uIdx] = 0x00;
	}
	xUsartIT[xtty].uBufferSize = 0;

}


/**
 * @brief pegar o buffer RXD da usart.
 * 
 * @param tty, enumeração ttyUSART1, ttyUSART2 ttyUSART3, caso
 * xtty seja invalido a função não será executada.
 * 
 * @param pcSwap onde serão salvo os dados da queue
 * 
 * @return CCHR: retorna o ponteiro do buffer sa serial
 */
CCHR *usart_pcGetBuffer(TTY xtty){
	if(usart_bCheckError(xtty)) return NULL;
	return xUsartIT[xtty].pcBufferRXD;
}


/**
 * @brief envia um caractere. Essa função utilizam o periferio uart junto
 * da interrupção USART_FLAG_TXE. Esse envio ocorre por por meio de uma queue,
 * onde por por meio dessa função a queue é populada enquanto a interrupção
 * envia os dado.
 * @param tty, enumeração ttyUSART1, ttyUSART2 ttyUSART3, caso
 * xtty seja invalido a função não será executada
 * @param ccChr, caractere tipo const char
 */
void usart_vSendChr(const TTY xtty, CCHR ccChr){
	if(usart_bCheckError(xtty)) return;

	#if(USART_IT_TXD == USART_IT_TXD_ENABLE)
		/**
		 * se a queue estiver cheia esse while possibilita a troca de
		 * contexto entre tasks, caso a mensagem transmitida seja muito
		 * grandeve a velocidade da cominicassão muito baixa.
		 */
		__HAL_USART_DISABLE_IT(&xUsartIT[xtty].xHandle, USART_IT_RXNE);
		while( xQueueSend( xUsartIT[ xtty ].xTXD, &ccChr, 0 ) != pdPASS ){
			/* casso a queue esteja cheia espere espere esvaziar */
			__HAL_USART_ENABLE_IT(&xUsartIT[xtty].xHandle, UART_IT_TXE);
			vTaskDelay( _10MS );
		}
		__HAL_USART_ENABLE_IT(&xUsartIT[xtty].xHandle, UART_IT_TXE);

	#elif(USART_IT_TXD == USART_IT_TXD_DISABLE)
		usart_vAtomicSendChr(xtty, ccChr);
	#endif
	
};


/**
 * @brief envia uma cadeia de caracteres. Essa função utilizam o periferio uart
 * junto da interrupção USART_FLAG_TXE. Esse envio ocorre por por meio de uma
 * queue, onde por por meio dessa função a queue é populada enquanto a interrupção
 * envia os dado.
 * @param tty, enumeração ttyUSART1, ttyUSART2 ttyUSART3, caso
 * xtty seja invalido a função não será executada
 * @param pcBuffer, buffer a ser enviado 
 * @param cuSize tamanho exato de pcBuffer
 */
void usart_vSendBlk(const TTY xtty, CCHR *pcBuffer, const size_t cuSize){
	if(usart_bCheckError(xtty))	return;
	if(pcBuffer == NULL)		return;

	#if(USART_IT_TXD == USART_IT_TXD_ENABLE)
		for(size_t idx = 0; idx<cuSize; idx++){
			usart_vSendChr(xtty, pcBuffer[idx]);
		}	
	#elif(USART_IT_TXD == USART_IT_TXD_DISABLE)
		usart_vAtomicSendBlk(xtty, pcBuffer, cuSize);
	#endif
}


/**
 * @brief envia uma cadeia de caracteres, ao final do envio de pcBuffer
 * a função irá enviar "\r\n". Essa função utilizam o periferio uart junto
 * da interrupção USART_FLAG_TXE. Esse envio ocorre por por meio de uma queue,
 * onde por por meio dessa função a queue é populada enquanto a interrupção
 * envia os dado.
 * @param tty, enumeração ttyUSART1, ttyUSART2 ttyUSART3, caso
 * xtty seja invalido a função não será executada
 * @param pcBuffer, buffer a ser enviado 
 * @param cuSize tamanho exato de pcBuffer
 */
void usart_vSendBlkLn(const TTY xtty, CCHR *pcBuffer, const size_t cuSize){
	usart_vSendBlk(xtty, pcBuffer, cuSize);
	usart_vSendBlk(xtty, "\r\n", 2);
}


/**
 * @brief envia uma string. Essa função utilizam o periferio uart junto
 * da interrupção USART_FLAG_TXE. Esse envio ocorre por por meio de uma queue,
 * que por meio dessa é populada enquanto a interrupção envia os dado.
 * @param tty, enumeração ttyUSART1, ttyUSART2 ttyUSART3, caso
 * xtty seja invalido a função não será executada
 * @param pcString, string a ser enviada 
 */
void usart_vSendStr(const TTY xtty, CCHR *pcString){
	usart_vSendBlk(xtty, pcString, strlen(pcString));
}

/**
 * @brief envia uma string, ao final do envio de pcString
 * a função irá enviar "\r\n". Essa função utilizam o periferio uart junto
 * da interrupção USART_FLAG_TXE. Esse envio ocorre por por meio de uma queue,
 * que por meio dessa é populada enquanto a interrupção envia os dado.
 * @param tty, enumeração ttyUSART1, ttyUSART2 ttyUSART3, caso
 * xtty seja invalido a função não será executada
 * @param pcString, string a ser enviada 
 */
void usart_vSendStrLn(const TTY xtty, CCHR *pcString){
	usart_vSendBlkLn(xtty, pcString, strlen(pcString));
}


/*########################################################################################################################################################*/
/*########################################################################################################################################################*/
/*########################################################################################################################################################*/
/*-------------------------------------------------------------------- Private Functions -----------------------------------------------------------------*/
/*########################################################################################################################################################*/

/**
 * @brief testa a se as variaves teterente s ttyX foram inicializadas
 * @param tty, enumeração ttyUSART1, ttyUSART2 ttyUSART3
 * @return TRUE: variavel não inicialisadas
 * @return FALSE: variavel inicialisadas
 */
_bool usart_bCheckError(const TTY xtty){
	if(xtty >= ttyNUM){
		return TRUE;
	}

	#if(USART_IT_TXD == USART_IT_TXD_ENABLE)
		if(xUsartIT[xtty].xTXD	== NULL){
			return TRUE;
		}
	#endif

	return FALSE;
}


/**
 * @brief inicialisa as queue para cada USART
 * @param tty, enumeração ttyUSART1, ttyUSART2 ttyUSART3
 */
void usart_vInitVars(const TTY xtty){

	#if(USART_IT_TXD == USART_IT_TXD_ENABLE)
		if(xUsartIT[xtty].xTXD == NULL){
			xUsartIT[xtty].xTXD = xQueueCreate(QUEUE_SIZE_BUFFER_TXD, sizeof(char));
		}
	#endif

	for(size_t uIdx = 0; uIdx < SIZE_BUFFER_RXD; uIdx++){
		xUsartIT[xtty].pcBufferRXD[uIdx] = 0x00;
	}
	xUsartIT[xtty].uBufferSize = 0;

}


/**
 * @brief inicialisa as interrupções para cada USART
 * @param tty, enumeração ttyUSART1, ttyUSART2 ttyUSART3
 */
void usart_vInitIT(const TTY xtty){
	__HAL_USART_CLEAR_FLAG(&xUsartIT[xtty].xHandle,	USART_FLAG_RXNE | USART_FLAG_TXE );
	__HAL_USART_DISABLE_IT(&xUsartIT[xtty].xHandle,	USART_IT_TXE);
	__HAL_USART_ENABLE_IT(&xUsartIT[xtty].xHandle,	USART_IT_RXNE);
	
	HAL_NVIC_SetPriority(xUsartIT[xtty].xIRQ, USART_NVIC_PRIORITY, USART_NVIC_SUBPRIORITY);
	HAL_NVIC_EnableIRQ(xUsartIT[xtty].xIRQ);
}


/**
 * @brief
 * @param tty, enumeração ttyUSART1, ttyUSART2 ttyUSART3
 * @param pxHigherPriorityTaskWoken, recurso do FreeRTOS para o controle
 * de troca de contexto em interruições. 
 */
void usart_vIT(const TTY xtty, BaseType_t *const pxHigherPriorityTaskWoken){

	/**
	 * interrupção por recebimento de um framer
	 */ 
	if(__HAL_UART_GET_FLAG(&xUsartIT[xtty].xHandle, USART_FLAG_RXNE) == SET){
		__HAL_USART_CLEAR_FLAG(&xUsartIT[xtty].xHandle, USART_FLAG_RXNE);
		
		u8 uBuffer = 0x00U;

		if ( (xUsartIT[xtty].xHandle.Init.WordLength == UART_WORDLENGTH_9B) || ((xUsartIT[xtty].xHandle.Init.WordLength == UART_WORDLENGTH_8B) && (xUsartIT[xtty].xHandle.Init.Parity == UART_PARITY_NONE))) {
			uBuffer = ( xUsartIT[xtty].xHandle.Instance->DR & 0x00FF);
		} else {
			uBuffer = ( xUsartIT[xtty].xHandle.Instance->DR & 0x007F);
		}

		if(xUsartIT[xtty].uBufferSize >= SIZE_BUFFER_RXD){
			xUsartIT[xtty].uBufferSize = 0;
		}

		if(uBuffer != 0x00){
			xUsartIT[xtty].pcBufferRXD[xUsartIT[xtty].uBufferSize++] = (char)uBuffer;
		}

	}


	/**
	 * interrupção por envio de um framer
	 */ 
	#if(USART_IT_TXD == USART_IT_TXD_ENABLE)
		if(__HAL_UART_GET_FLAG(&xUsartIT[xtty].xHandle, USART_FLAG_TXE) == SET ){
			__HAL_USART_CLEAR_FLAG(&xUsartIT[xtty].xHandle, USART_FLAG_TXE);


			u8 uBuffer = 0x00U;
			
			if(xQueueReceiveFromISR(xUsartIT[xtty].xTXD, &uBuffer, pxHigherPriorityTaskWoken) == pdPASS ){
				// xUsartIT[xtty].xHandle.Instance->DR = (uBuffer & 0xFFU);
				HAL_UART_Transmit(&xUsartIT[xtty].xHandle, (u8*)&uBuffer, 1, UART_TRANSMIT_TIMEOUT);
			} else {
				// quando a queue estiver vazia a interrupção não deve ser chamada novamente
				// amentos que a queue seja populada novamente
				__HAL_USART_DISABLE_IT(&xUsartIT[xtty].xHandle, USART_IT_TXE);
				__HAL_USART_ENABLE_IT(&xUsartIT[xtty].xHandle, USART_IT_RXNE);


			}
		}
	#endif


	/**
	 * TODO: descobri o por que disso 
	 * ! Por algum motivo tive que deixar isso aqui pois de a serial 
	 * ! receber muitos dados ela trava e para de ativar a interrupção
	 * ! USART_IT_RXNE
	 */
	__HAL_USART_ENABLE_IT(&xUsartIT[xtty].xHandle, USART_IT_RXNE);	
}





/** ################################################################################################ **/
/** ######################################### Interrupções ######################################### **/
/** ################################################################################################ **/

void USART1_IRQHandler(void){
	HAL_UART_IRQHandler(&xUsartIT[ttyUSART1].xHandle);
	long xHigherPriorityTaskWoken = pdFALSE;
	usart_vIT(ttyUSART1, &xHigherPriorityTaskWoken);
	portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
}

void USART2_IRQHandler(void){
	HAL_UART_IRQHandler(&xUsartIT[ttyUSART2].xHandle);
	long xHigherPriorityTaskWoken = pdFALSE;
	usart_vIT(ttyUSART2, &xHigherPriorityTaskWoken);
	portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
}

void USART3_IRQHandler(void){
	HAL_UART_IRQHandler(&xUsartIT[ttyUSART3].xHandle);
	long xHigherPriorityTaskWoken = pdFALSE;
	usart_vIT(ttyUSART3, &xHigherPriorityTaskWoken);
	portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
}

#endif