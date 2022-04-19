/**
 * gpio.c
 *
 *  @date Created at:	23/02/2021 09:07:00
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
 * Controle digital dos pinos GPIO, utilise as macros GPIOXX
 * para trabalhar com os pinos, por exemplo GPIOC, GPIO_PIN_13
 * é equivalente ao GPIOC13
 * 
 * esse driver pode ler escrever e trocar o estados de todos
 * os pinos. Além disso é possível desativar a comunicação SWJ.
 * 
 * TODO: Está em desenvolvimente o tratamento de interrupções
 * externas.
 *
 */



/* Includes ------------------------------------------------------------------*/
#include <FreeRTOS/Drivers/gpio.h>
#include <stm32f103xb.h>
#include <stm32f1xx_ll_gpio.h>

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
typedef struct{
	GPIO_TypeDef *pxGPIOx;		// GPIO PORT
	u32 uPINx;			// GPIO PIN
}xGpioPin;


static const xGpioPin xGpio[GPIO_NUM] = {
	{GPIOA,   GPIO_PIN_0},	
	{GPIOA,   GPIO_PIN_1},	
	{GPIOA,   GPIO_PIN_2},	
	{GPIOA,   GPIO_PIN_3},	
	{GPIOA,   GPIO_PIN_4},	
	{GPIOA,   GPIO_PIN_5},	
	{GPIOA,   GPIO_PIN_6},	
	{GPIOA,   GPIO_PIN_7},	
	{GPIOA,   GPIO_PIN_8},	
	{GPIOA,   GPIO_PIN_9},	
	{GPIOA,   GPIO_PIN_10},
	{GPIOA,   GPIO_PIN_11},	
	{GPIOA,   GPIO_PIN_12},	
	{GPIOA,   GPIO_PIN_13},	
	{GPIOA,   GPIO_PIN_14},	
	{GPIOA,   GPIO_PIN_15},

	{GPIOB,   GPIO_PIN_0},
	{GPIOB,   GPIO_PIN_1},
	{GPIOB,   GPIO_PIN_2},
	{GPIOB,   GPIO_PIN_3},
	{GPIOB,   GPIO_PIN_4},
	{GPIOB,   GPIO_PIN_5},
	{GPIOB,   GPIO_PIN_6},
	{GPIOB,   GPIO_PIN_7},
	{GPIOB,   GPIO_PIN_8},
	{GPIOB,   GPIO_PIN_9},
	{GPIOB,   GPIO_PIN_10},
	{GPIOB,   GPIO_PIN_11},
	{GPIOB,   GPIO_PIN_12},
	{GPIOB,   GPIO_PIN_13},
	{GPIOB,   GPIO_PIN_14},
	{GPIOB,   GPIO_PIN_15},

	{GPIOC,   GPIO_PIN_13},
	{GPIOC,   GPIO_PIN_14},
	{GPIOC,   GPIO_PIN_15},
	
	{GPIOD,   GPIO_PIN_0},
	{GPIOD,   GPIO_PIN_1},

};



/* Private Functions ---------------------------------------------------------*/

/**
 * @brief habilitar clock das portas GPIO
 *
 * 
 */
void gpio_vInitAll( void ) {

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOA_CLK_DISABLE();
	__HAL_RCC_GPIOB_CLK_DISABLE();
	__HAL_RCC_GPIOC_CLK_DISABLE();
	__HAL_RCC_GPIOD_CLK_DISABLE();

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
}

/**
 * @brief desabilita a comunicação JTAG. para poder 
 * usar livremente os pinos GPIO3, GPIOB4 e GPIOA15.
 */
void gpio_vDisableDebug (void) {
	__HAL_RCC_AFIO_CLK_ENABLE();	
	__HAL_AFIO_REMAP_SWJ_NOJTAG();	// desativar JTAG
}

/**
 * @brief configura o momo de operação do pino GPIO
 * @note configurações fixas: GPIO_SPEED_FREQ_LOW
 * @param cxGPIOxx, pino a ser configurado
 * @param cuMode, modo de operação:
 *	GPIO_MODE_INPUT		Entrada alta impedância ( flutuasão ) 
 *	GPIO_MODE_OUTPUT_PP	Saida "Push Pull";
 *	GPIO_MODE_OUTPUT_OD	Saida "Open Drain";
 *	GPIO_MODE_AF_PP		Função Alternativa "Push Pull";
 *	GPIO_MODE_AF_OD		Função Alternativa "Open Drain";
 *	GPIO_MODE_AF_INPUT	Função Alternativa Entrada alta impedância ( flutuasão );
 * @param cuPull, estado padrão do pino:
 *	GPIO_NOPULL  		"Pull-up" ou "Pull-down" desativados, alta impedância ( flutuasão );
 *	GPIO_PULLUP  		"Pull-up"	ativado;             
 *	GPIO_PULLDOWN		"Pull-down" 	ativado ;               
 */
void gpio_vMode (const GpioLabel cxGPIOxx, cu32 cuMode, cu32 cuPull) {
	
	HAL_GPIO_WritePin(xGpio[cxGPIOxx].pxGPIOx, xGpio[cxGPIOxx].uPINx, GPIO_PIN_RESET);
	
	GPIO_InitTypeDef GPIO_InitStruct 	= {0};
	
	GPIO_InitStruct.Pin			= xGpio[cxGPIOxx].uPINx;
	GPIO_InitStruct.Mode			= cuMode;
	GPIO_InitStruct.Pull			= cuPull;
	GPIO_InitStruct.Speed			= GPIO_SPEED_FREQ_LOW;
	
	HAL_GPIO_Init(xGpio[cxGPIOxx].pxGPIOx, &GPIO_InitStruct);
}


/**
 * @brief configura o momo de operação do pino GPIO
 * @note configurações fixas: GPIO_MODE_ANALOG, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH
 * @param cxGPIOxx, pino a ser configurado
 */
void gpio_vAnalogMode (const GpioLabel cxGPIOxx) {
	
	GPIO_InitTypeDef GPIO_InitStruct	= {0};

	GPIO_InitStruct.Pin			= xGpio[cxGPIOxx].uPINx;
	GPIO_InitStruct.Mode			= GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull			= GPIO_NOPULL;
	GPIO_InitStruct.Speed			= GPIO_SPEED_FREQ_HIGH;
	
	HAL_GPIO_Init(xGpio[cxGPIOxx].pxGPIOx, &GPIO_InitStruct);
}


/**
 * @brief desativa o pino GPIO
 * @param cxGPIOxx, pino a ser configurado
 */
void gpio_vDeinit(const GpioLabel cxGPIOxx){
	HAL_GPIO_DeInit(xGpio[cxGPIOxx].pxGPIOx, xGpio[cxGPIOxx].uPINx);
}



/**
 * @brief lê o estado de um pino
 * @param cxGPIOxx pino a ser lido
 * @return FALSE:	estado 0/LOW/GPIO_PIN_RESET
 * @return TRUE:	estado 1/HIGH/GPIO_PIN_SET
 * 
 */
_bool gpio_bRead(const GpioLabel cxGPIOxx) {
	if(HAL_GPIO_ReadPin(xGpio[cxGPIOxx].pxGPIOx, xGpio[cxGPIOxx].uPINx) == GPIO_PIN_RESET) {
		return FALSE;
	}
	return TRUE;
}

/**
 * @brief escreve um valor em um pino
 * @param bValue TRUE:	para estado 1/HIGH/GPIO_PIN_SET
 * @param bValue FALSE:	para estado 0/LOW/GPIO_PIN_RESET
 */
void gpio_vWrite(const GpioLabel cxGPIOxx, _bool bValue) {
	HAL_GPIO_WritePin(xGpio[cxGPIOxx].pxGPIOx, xGpio[cxGPIOxx].uPINx, (bValue)?(GPIO_PIN_SET):(GPIO_PIN_RESET));
}

/**
 * @brief troca o estado do pino
 * @param cxGPIOxx, pino que trocará de estado
 */
void gpio_vToggle(const GpioLabel cxGPIOxx) {
	HAL_GPIO_TogglePin(xGpio[cxGPIOxx].pxGPIOx, xGpio[cxGPIOxx].uPINx);


	
}

/*########################################################################################################################################################*/
/*########################################################################################################################################################*/
/*########################################################################################################################################################*/
/*-------------------------------------------------------------------- Private Functions -----------------------------------------------------------------*/
/*########################################################################################################################################################*/