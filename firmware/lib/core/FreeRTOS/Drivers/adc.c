/**
 * adc.c
 *
 *  @date Created at:	13/07/2021 21:21:35
 *	@author:	Pedro Igor B. S.
 *	@email:		pibscontato@gmail.com
 * 	GitHub:		https://github.com/pedro-ibs
 * 	tabSize:	8
 *
 * ######################################################################
 * 
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
 * 
 * ######################################################################
 *
 *
 */



/* Includes ---------------------------------------------------------------------------------------------------------------------------------------------*/
/* stm includes */
#include <core.h>
#include <stm32f1xx_hal_adc.h>


/* FreeRTOS standard includes.  */
#include <FreeRTOS/include/FreeRTOSConfig.h>
#include <FreeRTOS/include/FreeRTOS.h>
#include <FreeRTOS/include/task.h>
#include <FreeRTOS/include/list.h>
#include <FreeRTOS/include/queue.h>
#include <FreeRTOS/include/semphr.h>
#include <FreeRTOS/include/portable.h>
#include <FreeRTOS/include/event_groups.h>

/* include driver */
#include <FreeRTOS/Drivers/adc.h>
#include <FreeRTOS/Drivers/gpio.h>
#include <FreeRTOS/Drivers/timer.h>
/* Private macro ----------------------------------------------------------------------------------------------------------------------------------------*/
#define ADC_BIT_OneShotMode			( BIT00 )
#define ADC_BIT_GetSampleMode			( BIT01 )


/* Private variables ------------------------------------------------------------------------------------------------------------------------------------*/
static SemaphoreHandle_t xAdc1Mutex		= NULL;
static DMA_HandleTypeDef xDmaAdc1		= { 0 };
static ADC_HandleTypeDef xAdc1			= { 0 };
/* Private Functions ------------------------------------------------------------------------------------------------------------------------------------*/
u32 adc1_uGetChannel(const AdcChannel cxChannel);
void adc1_vDMA1(u16 *puAdc1Buffer, const size_t cuSizeAdc1Buffer);
/* Functions --------------------------------------------------------------------------------------------------------------------------------------------*/



/**
 * @brief Inicializa as variaveis utilizada no driver adc.h
 */
void adc1_vInit(void){
	if(xAdc1Mutex == NULL){
		xAdc1Mutex = xSemaphoreCreateMutex();
	}
}


/**
 * @brief pega o acesso do adc1
 */
void adc1_vTakeAccess( void ){
	xSemaphoreTake(xAdc1Mutex, portMAX_DELAY);	
}


/**
 * @brief devolve o acesso do adc1
 */
void adc1_vGiveAccess(  void  ){
	xSemaphoreGive(xAdc1Mutex);
}

/**
 * @brief pega o acesso do adc1
 */
void adc1_vTakeAccessFromISR( BaseType_t * const pxHigherPriorityTaskWoken ){
	xSemaphoreTakeFromISR( xAdc1Mutex, pxHigherPriorityTaskWoken );
}


/**
 * @brief devolve o acesso do adc1
 */
void adc1_vGiveAccessFromISR( BaseType_t * const pxHigherPriorityTaskWoken ){
	xSemaphoreGiveFromISR( xAdc1Mutex, pxHigherPriorityTaskWoken );
}



/**
 * @brief Este Modo le valores em tempo de execução, 
 * no momento em que a função de leitura é chamada.
 * 
 * @param cuNbrOfConversion quantidade de canais ADC
 * que serão lidos
 * 
 */
void adc1_vSetOneShotMode( cu32 cuNbrOfConversion ){

	adc1_vTakeAccess();

	__HAL_RCC_ADC1_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	xAdc1.Instance				= ADC1;
	xAdc1.Init.ScanConvMode			= ADC_SCAN_ENABLE;
	xAdc1.Init.ContinuousConvMode		= DISABLE;
	xAdc1.Init.DiscontinuousConvMode	= ENABLE;
	xAdc1.Init.ExternalTrigConv		= ADC_SOFTWARE_START;
	xAdc1.Init.DataAlign			= ADC_DATAALIGN_RIGHT;
	xAdc1.Init.NbrOfConversion 		= cuNbrOfConversion;
	xAdc1.Init.NbrOfDiscConversion 		= 1;


	if (HAL_ADC_Init(&xAdc1) != HAL_OK) {
		Error_Handler();
	}

	adc1_vGiveAccess();

}

/**
 * @brief Configura o processo de aquisição de dados
 * dos canais o adc1
 * @param cuNbrOfConversion quantidade de canais ADC
 * que serão lidos
 */
void adc1_vSetGetSampleMode( cu32 cuNbrOfConversion ){

	adc1_vTakeAccess();

	__HAL_RCC_ADC1_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	xAdc1.Instance				= ADC1;
	xAdc1.Init.ScanConvMode			= ADC_SCAN_ENABLE;
	xAdc1.Init.ContinuousConvMode		= DISABLE;
	xAdc1.Init.DiscontinuousConvMode	= DISABLE;
	xAdc1.Init.ExternalTrigConv		= ADC_EXTERNALTRIGCONV_T3_TRGO;
	xAdc1.Init.DataAlign			= ADC_DATAALIGN_RIGHT;
	xAdc1.Init.NbrOfConversion 		= cuNbrOfConversion;


	if (HAL_ADC_Init(&xAdc1) != HAL_OK) {
		Error_Handler();
	}

	adc1_vGiveAccess();
}


/**
 * @brief iniciar/configura o canal do periferico ADC1
 * @param cuChannel, de ADC1_PA0 até ADC1_PA7. ADC1_PB0
 * e ADC1_PB1
 * 
 * @param cuSamplingTime tempo de amostra do canal ADC
 *	ADC_SAMPLETIME_1CYCLE_5    
 *	ADC_SAMPLETIME_7CYCLES_5   
 *	ADC_SAMPLETIME_13CYCLES_5  
 *	ADC_SAMPLETIME_28CYCLES_5  
 *	ADC_SAMPLETIME_41CYCLES_5  
 *	ADC_SAMPLETIME_55CYCLES_5  
 *	ADC_SAMPLETIME_71CYCLES_5  
 *	ADC_SAMPLETIME_239CYCLES_5 
 *
 * @param cuRank Rank do canal ADC
 * 	ADC_REGULAR_RANK_1  
 * 	ADC_REGULAR_RANK_2  
 * 	ADC_REGULAR_RANK_3  
 * 	ADC_REGULAR_RANK_4  
 * 	ADC_REGULAR_RANK_5  
 * 	ADC_REGULAR_RANK_6  
 * 	ADC_REGULAR_RANK_7  
 * 	ADC_REGULAR_RANK_8  
 * 	ADC_REGULAR_RANK_9  
 * 	ADC_REGULAR_RANK_10 
 * 	ADC_REGULAR_RANK_11 
 * 	ADC_REGULAR_RANK_12 
 * 	ADC_REGULAR_RANK_13 
 * 	ADC_REGULAR_RANK_14 
 * 	ADC_REGULAR_RANK_15 
 * 	ADC_REGULAR_RANK_16 
 */
void adc1_vSetChannel( const AdcChannel cxChannel, cu32 cuSamplingTime, cu32 cuRank ){
	gpio_vAnalogMode(cxChannel);

	ADC_ChannelConfTypeDef xConfig		= {0};

	xConfig.Channel				= adc1_uGetChannel(cxChannel);
	xConfig.Rank				= cuRank;
	xConfig.SamplingTime			= cuSamplingTime;
	
	if (HAL_ADC_ConfigChannel(&xAdc1, &xConfig) != HAL_OK) {
		Error_Handler();
	}
}


/**
 * @brief Le os canais ADC de acordo com o RANK configurado
 * esta função não faz uso do mutex.
 */
u16 adc1_uReadNextRank( void ){
	HAL_ADC_Start(&xAdc1);
	HAL_ADC_PollForConversion(&xAdc1, 100);
	
	if(HAL_IS_BIT_SET(HAL_ADC_GetState(&xAdc1),HAL_ADC_STATE_REG_EOC)){
		return HAL_ADC_GetValue(&xAdc1);
	}
	return 0;
}

/**
 * @brief inicializa o processo de aquisição de dados
 * dos canais o adc1
 * 
 * @param uPrescaler, divisor do contador.
 * @param uPeriod, contador do Timer 3
 */
void adc1_vStartGetSampleMode(u16 *puAdc1Buffer, const size_t cuSizeAdc1Buffer, cu32 uPrescaler, cu32 uPeriod ){
	adc1_vTakeAccess();
	adc1_vDMA1(puAdc1Buffer, cuSizeAdc1Buffer);
	adc1_vGiveAccess();

	tim3_vStartAdc1Trigger( uPrescaler, uPeriod );		
}


/**
 * @brief Desativa o canal do periferico ADC1
 * @param cuChannel, de ADC1_PA0 até ADC1_PA7. ADC1_PB0
 * e ADC1_PB1
 */
void adc1_vDeInitChannel(const AdcChannel cuChannel){
	gpio_vDeinit(cuChannel);	
}



/**
 * @brief desativa acoleta de dados
 * @note desativa o ADC1 e canaisrespectivos DMA e TIMER
 *
 */
void adc1_vDeInit(void){
	
	tim3_vDeinit();

	adc1_vTakeAccess();
	__HAL_RCC_ADC1_CLK_DISABLE();
	HAL_DMA_DeInit(xAdc1.DMA_Handle);
	adc1_vGiveAccess();
}


/*########################################################################################################################################################*/
/*########################################################################################################################################################*/
/*########################################################################################################################################################*/
/*-------------------------------------------------------------------- Private Functions -----------------------------------------------------------------*/
/*########################################################################################################################################################*/

u32 adc1_uGetChannel(const AdcChannel cxChannel){

	switch (cxChannel) {
		case ADC1_PA0:{ return ADC_CHANNEL_0; } break;
		case ADC1_PA1:{ return ADC_CHANNEL_1; } break;
		case ADC1_PA2:{ return ADC_CHANNEL_2; } break;
		case ADC1_PA3:{ return ADC_CHANNEL_3; } break;
		case ADC1_PA4:{ return ADC_CHANNEL_4; } break;
		case ADC1_PA5:{ return ADC_CHANNEL_5; } break;
		case ADC1_PA6:{ return ADC_CHANNEL_6; } break;
		case ADC1_PA7:{ return ADC_CHANNEL_7; } break;
		case ADC1_PB0:{ return ADC_CHANNEL_8; } break;
		case ADC1_PB1:{ return ADC_CHANNEL_9; } break;
		
		default: {
			Error_Handler();
		} break;
	}

	return 0;
}


/**
 * @brief Configura e inicializa do DMA
 *
 */
void adc1_vDMA1(u16 *puAdc1Buffer, const size_t cuSizeAdc1Buffer){

	__HAL_RCC_DMA1_CLK_ENABLE();

	xDmaAdc1.Instance			= DMA1_Channel1;
	xDmaAdc1.Init.Direction			= DMA_PERIPH_TO_MEMORY;
	xDmaAdc1.Init.PeriphInc			= DMA_PINC_DISABLE;
	xDmaAdc1.Init.MemInc			= DMA_MINC_ENABLE;
	xDmaAdc1.Init.PeriphDataAlignment	= DMA_PDATAALIGN_HALFWORD;
	xDmaAdc1.Init.MemDataAlignment		= DMA_MDATAALIGN_HALFWORD;
	xDmaAdc1.Init.Mode			= DMA_CIRCULAR;
	xDmaAdc1.Init.Priority			= DMA_PRIORITY_VERY_HIGH;
	
	if (HAL_DMA_Init(&xDmaAdc1) != HAL_OK) {
		Error_Handler();
	}

	__HAL_LINKDMA(&xAdc1, DMA_Handle, xDmaAdc1);

	HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, ADC1_DMA1_NVIC_PRIORITY, ADC1_DMA1_NVIC_SUBPRIORITY);
	HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);


	HAL_ADC_Start_DMA(&xAdc1, (uint32_t*)puAdc1Buffer, cuSizeAdc1Buffer);
}




/** ################################################################################################ **/
/** ######################################### Interrupções ######################################### **/
/** ################################################################################################ **/


void DMA1_Channel1_IRQHandler(void) {
	HAL_DMA_IRQHandler(&xDmaAdc1);
	long xHigherPriorityTaskWoken = pdFALSE;
	adc1_vDMA1Ch1Handler(&xHigherPriorityTaskWoken);
	portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
}


void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
	long xHigherPriorityTaskWoken = pdFALSE;
	
	acd1_vBufferDoneHandler(&xHigherPriorityTaskWoken);

	portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
}