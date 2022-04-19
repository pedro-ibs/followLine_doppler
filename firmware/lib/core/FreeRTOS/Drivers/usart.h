/**
 * usart.h
 *
 *  @date Created at:	16/03/2021 11:34:54
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
 * 
 * ------------------------ USART_IT ------------------------
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
 * 
 * ------------------------ USART_DMA ------------------------
 *
 */

/* Includes ------------------------------------------------------------------*/
#include <core.h>
#include <FreeRTOS/include/FreeRTOSConfig.h>

#ifndef usart_it_or_dma_H_
#define usart_it_or_dma_H_

/* macro ---------------------------------------------------------------------*/

/**
 * @brief Configure de acordo com a forma que deseje que a usart funcione
 */
#define USART_IT				( configUSART_IT		)
#define USART_DMA				( configUSART_DMA		)
#define USART_USE				( configUSART_MODE		)


/**
 * @brief ativar ou desativer transmissão por interruisão 
 * para isso a USART_USE deve estar em USART_IT
 * 
 * @note caso seja desativado as funções usart_vSendXXX vão
 * utilizar a usart_vAtomicSendXX
 */
#define USART_IT_TXD_ENABLE			( configUSART_IT_TXD_ENABLE	)
#define USART_IT_TXD_DISABLE			( configUSART_IT_TXD_DISABLE	)
#define USART_IT_TXD				( configUSART_TI_TXD_USE	)


/**
 * @brief tamanho dos de transmissão e recepção e transmissão da serial  
 * 
 * @param QUEUE_SIZE_BUFFER_TXD	queue de transmissão, não usado quando
 * USART_IT_TXD está desativado
 * 
 * @param SIZE_BUFFER_RXD buffer de recepção, esse buffer é circular
 */
#define QUEUE_SIZE_BUFFER_TXD			( configUART_SIZE_BUFFER_TXD	)
#define SIZE_BUFFER_RXD				( configUART_SIZE_BUFFER_RXD	)


/**
 * tempo limite para enviar dados com a função 
 * HAL_UART_Transmit
 * 
 */
#define UART_TRANSMIT_TIMEOUT			( configUART_TRANSMIT_TIMEOUT	)


/**
 * prioridades das interruições de todos 
 * os perifericos usart
 */
#define USART_NVIC_PRIORITY			( configUSART_NVIC_PRIORITY	)
#define USART_NVIC_SUBPRIORITY			( configUSART_NVIC_SUBPRIORITY	)



/* Definition ----------------------------------------------------------------*/
typedef enum{
	ttyUSART1 = 0,
	ttyUSART2,
	ttyUSART3,
	ttyNUM
}TTY;


typedef enum{
	usart_0k3bps	= 300,
	usart_0k6bps	= 600,
	usart_1k2bps	= 1200,
	usart_2k4bps	= 2400,
	usart_4k8bps	= 4800,
	usart_9k6bps	= 9600,
	usart_14k4bps	= 14400,
	usart_19k2bps	= 19200,
	usart_38k4bps	= 38400,
	usart_57k6bps	= 57600,
	usart_115k2bps	= 115200,
	usart_230k4bps	= 230400,
	usart_250k0bps	= 250000,
	usart_460k8bps	= 460800,
	usart_500k0bps	= 500000,
	usart_1M0bps	= 1000000,
}BaudRate;


void usart_vInit(UART_HandleTypeDef *xHandle, const TTY xtty,  BaudRate cuBaudRate);

void usart_vSetup(const TTY xtty, const BaudRate cuBaudRate);

void usart_vTakeAccess(const TTY xtty);
void usart_vGiveAccess(const TTY xtty);

void usart_vCleanBuffer(const TTY xtty);
int usart_iSizeBuffer(const TTY xtty);
CCHR *usart_pcGetBuffer(TTY xtty);

void usart_vAtomicSendChr(const TTY xtty, CCHR ccChr);
void usart_vAtomicSendBlk(const TTY xtty, CCHR *pcBuffer, const size_t cuSize);
void usart_vAtomicSendBlkLn(const TTY xtty, CCHR *pcBuffer, const size_t cuSize);
void usart_vAtomicSendStr(const TTY xtty, CCHR *pcString);
void usart_vAtomicSendStrLn(const TTY xtty, CCHR *pcString);


void usart_vSendChr(const TTY xtty, CCHR ccChr);
void usart_vSendBlk(const TTY xtty, CCHR *pcBuffer, const size_t cuSize);
void usart_vSendBlkLn(const TTY xtty, CCHR *pcBuffer, const size_t cuSize);
void usart_vSendStr(const TTY xtty, CCHR *pcString);
void usart_vSendStrLn(const TTY xtty, CCHR *pcString);



#if(USART_USE == USART_IT)
	void usart_vStopIT_RXD(const TTY xtty);
	void usart_vStartIT_RXD(const TTY xtty);
#elif(USART_USE == USART_DMA)

#endif


#endif /* usart_it_or_dma_H_ */