/**
 * usart.c
 *
 *  @date Created at:	18/03/2021 10:17:28
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
 * driver base de inicialização da usart
 *
 */



/* Includes ------------------------------------------------------------------*/
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


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
typedef struct usart {
	USART_TypeDef *pxPort;
	UART_HandleTypeDef *pxHandle;
	SemaphoreHandle_t xSemaphore;

} xUsartHandle;


static xUsartHandle xUH[ttyNUM] ={
	{USART1, NULL, NULL},
	{USART2, NULL, NULL},
	{USART3, NULL, NULL},
};



/* Private Functions ---------------------------------------------------------*/
void usart_vSetupGPIO(const TTY xtty);


/**
 * @brief pega o acesso ao periferio.
 * @param tty, enumeração ttyUSART1, ttyUSART2 ttyUSART3, caso
 * xtty seja invalido a função não será executada.
 */
void usart_vTakeAccess(const TTY xtty){
	if(xUH[xtty].xSemaphore == NULL) return;
	xSemaphoreTake(xUH[xtty].xSemaphore, portMAX_DELAY );
}


/**
 * @brief devolve o acesso ao periferio.
 * @param tty, enumeração ttyUSART1, ttyUSART2 ttyUSART3, caso
 * xtty seja invalido a função não será executada.
 */
void usart_vGiveAccess(const TTY xtty){
	if(xUH[xtty].xSemaphore == NULL) return;
	xSemaphoreGive(xUH[xtty].xSemaphore);
}


/**
 * @brief inicia a uart e variaveis use gpio_vInitAll() antes.
 * @param xHandle: handle de configuração da usart da camada HAL 
 * @param tty, enumeração ttyUSART1, ttyUSART2 ttyUSART3, caso
 * xtty seja invalido a função não será executada
 * @param BaudRate, velocidade da porta serial.
 */
void usart_vInit(UART_HandleTypeDef *xHandle, const TTY xtty,  BaudRate cuBaudRate){
	if(xUH[xtty].xSemaphore == NULL){
		xUH[xtty].xSemaphore = xSemaphoreCreateMutex();		
	}

	usart_vSetupGPIO(xtty);
	xUH[xtty].pxHandle			= xHandle;
	xUH[xtty].pxHandle->Instance		= xUH[xtty].pxPort;
	xUH[xtty].pxHandle->Init.BaudRate	= cuBaudRate;
	xUH[xtty].pxHandle->Init.WordLength	= UART_WORDLENGTH_8B;
	xUH[xtty].pxHandle->Init.StopBits	= UART_STOPBITS_1;
	xUH[xtty].pxHandle->Init.Parity		= UART_PARITY_NONE;
	xUH[xtty].pxHandle->Init.Mode		= UART_MODE_TX_RX;
	xUH[xtty].pxHandle->Init.HwFlowCtl	= UART_HWCONTROL_NONE;
	xUH[xtty].pxHandle->Init.OverSampling	= UART_OVERSAMPLING_16;

	if (HAL_UART_DeInit(xUH[xtty].pxHandle) != HAL_OK) {
		Error_Handler();
	}

	if (HAL_UART_Init(xUH[xtty].pxHandle) != HAL_OK) {
		Error_Handler();
	}
}



/**
 * @brief envia um caractere. As funções usart_vAtomicX
 * @param tty, enumeração ttyUSART1, ttyUSART2 ttyUSART3, caso
 * xtty seja invalido a função não será executada
 * @param ccChr, caractere tipo const char
 */
void usart_vAtomicSendChr(const TTY xtty, CCHR ccChr){
	if(xUH[xtty].pxHandle == NULL)	return;
	while ( HAL_UART_GetState(xUH[xtty].pxHandle) != HAL_UART_STATE_READY);
	HAL_UART_Transmit(xUH[xtty].pxHandle, (u8*)&ccChr, 1, UART_TRANSMIT_TIMEOUT);
};


/**
 * @brief envia uma cadeia de caracteres.
 * @param tty, enumeração ttyUSART1, ttyUSART2 ttyUSART3, caso
 * xtty seja invalido a função não será executada
 * @param pcBuffer, buffer a ser enviado 
 * @param cuSize tamanho exato de pcBuffer
 */
void usart_vAtomicSendBlk(const TTY xtty, CCHR *pcBuffer, const size_t cuSize){
	if(xUH[xtty].pxHandle == NULL)	return;
	if(pcBuffer == NULL)		return;
	while ( HAL_UART_GetState(xUH[xtty].pxHandle) != HAL_UART_STATE_READY);
	HAL_UART_Transmit(xUH[xtty].pxHandle, (u8*)pcBuffer, cuSize, UART_TRANSMIT_TIMEOUT);
}


/**
 * @brief envia uma cadeia de caracteres, ao final do envio de pcBuffer
 * a função irá enviar "\r\n".
 * @param tty, enumeração ttyUSART1, ttyUSART2 ttyUSART3, caso
 * xtty seja invalido a função não será executada
 * @param pcBuffer, buffer a ser enviado 
 * @param cuSize tamanho exato de pcBuffer
 */
void usart_vAtomicSendBlkLn(const TTY xtty, CCHR *pcBuffer, const size_t cuSize){
	usart_vAtomicSendBlk(xtty, pcBuffer, cuSize);
	usart_vAtomicSendBlk(xtty, "\r\n", 2);
}

/**
 * @brief envia uma sting 
 * @param tty, enumeração ttyUSART1, ttyUSART2 ttyUSART3, caso
 * xtty seja invalido a função não será executada
 * @param pcString, buffer a ser enviado 
 */
void usart_vAtomicSendStr(const TTY xtty, CCHR *pcString){
	usart_vAtomicSendBlk(xtty, pcString, strlen(pcString));
}

/**
 * @brief envia uma sting + "\r\n"
 * @param tty, enumeração ttyUSART1, ttyUSART2 ttyUSART3, caso
 * xtty seja invalido a função não será executada
 * @param pcString, buffer a ser enviado 
 */
void usart_vAtomicSendStrLn(const TTY xtty, CCHR *pcString){
	usart_vAtomicSendBlkLn(xtty, pcString, strlen(pcString));
}



/*########################################################################################################################################################*/
/*########################################################################################################################################################*/
/*########################################################################################################################################################*/
/*-------------------------------------------------------------------- Private Functions -----------------------------------------------------------------*/
/*########################################################################################################################################################*/



/**
 * @brief inicialisa os GPIO para cada USART
 * @param tty, enumeração ttyUSART1, ttyUSART2 ttyUSART3
 */
void usart_vSetupGPIO(const TTY xtty){
	GPIO_InitTypeDef GPIO_InitStruct = {0};
    	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

	switch (xtty) {
		case ttyUSART1:
			/**
			 * * habilitando uart1 e trocando so pinos padroes para PB6 e PB7
			 */			
			__HAL_RCC_GPIOB_CLK_ENABLE();
			__HAL_RCC_AFIO_CLK_ENABLE();
			__HAL_RCC_USART1_CLK_ENABLE();
			__HAL_AFIO_REMAP_I2C1_DISABLE();
			__HAL_AFIO_REMAP_USART1_ENABLE();

			// PB6 USART1_TX
			GPIO_InitStruct.Pin	= GPIO_PIN_6;
			GPIO_InitStruct.Mode	= GPIO_MODE_AF_PP;
			HAL_GPIO_DeInit(GPIOB,	GPIO_InitStruct.Pin);
			HAL_GPIO_Init(GPIOB,	&GPIO_InitStruct);

			// PB7 USART1_RX
			GPIO_InitStruct.Pin	= GPIO_PIN_7;
			GPIO_InitStruct.Mode	= GPIO_MODE_INPUT;
			GPIO_InitStruct.Pull	= GPIO_NOPULL;
			HAL_GPIO_DeInit(GPIOB,	GPIO_InitStruct.Pin);
			HAL_GPIO_Init(GPIOB,	&GPIO_InitStruct);
			break;
		
		case ttyUSART2:
			__HAL_RCC_GPIOA_CLK_ENABLE();
			__HAL_RCC_USART2_CLK_ENABLE();
			
			// PA2 USART1_TX
			GPIO_InitStruct.Pin	= GPIO_PIN_2;
			GPIO_InitStruct.Mode	= GPIO_MODE_AF_PP;
			HAL_GPIO_DeInit(GPIOA,	GPIO_InitStruct.Pin);
			HAL_GPIO_Init(GPIOA,	&GPIO_InitStruct);

			// PA3 USART1_RX
			GPIO_InitStruct.Pin	= GPIO_PIN_3;
			GPIO_InitStruct.Mode	= GPIO_MODE_INPUT;
			GPIO_InitStruct.Pull	= GPIO_NOPULL;
			HAL_GPIO_DeInit(GPIOA,	GPIO_InitStruct.Pin);
			HAL_GPIO_Init(GPIOA, 	&GPIO_InitStruct);
			break;

		case ttyUSART3:
		default:
			__HAL_RCC_GPIOB_CLK_ENABLE();
			__HAL_RCC_USART3_CLK_ENABLE();

			// PB10 USART1_TX
			GPIO_InitStruct.Pin	= GPIO_PIN_10;
			GPIO_InitStruct.Mode	= GPIO_MODE_AF_PP;
			HAL_GPIO_DeInit(GPIOB,	GPIO_InitStruct.Pin);
			HAL_GPIO_Init(GPIOB,	&GPIO_InitStruct);

			// PB11 USART1_RX
			GPIO_InitStruct.Pin	= GPIO_PIN_11;
			GPIO_InitStruct.Mode	= GPIO_MODE_INPUT;
			GPIO_InitStruct.Pull	= GPIO_NOPULL;
			HAL_GPIO_DeInit(GPIOB,	GPIO_InitStruct.Pin);
			HAL_GPIO_Init(GPIOB,	&GPIO_InitStruct);
			break;
	}
}
