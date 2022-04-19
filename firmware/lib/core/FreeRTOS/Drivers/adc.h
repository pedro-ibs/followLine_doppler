/**
 * adc.h
 *
 *  @date Created at:	26/02/2021 10:53:47
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
 * * ADC ONE SHOT MODE -------------------------------------------------
 * Esse modo realiza uma medida por vez, com a função adc1_uReadNextRank()
 * com esse mode é possível ler varios canais do ADC em sequânica em tempo
 * de execução
 * 
 * * Configurando Tres Canais ADC em ONE SHOT MODE ---------------------- 
 * 
 * 1) Iniciar as variaveis do driver:
 * 	adc1_vInit();
 * 
 * 2) Configurar o modo ONE SHOT MODE, cuNbrOfConversion é o numero de
 * canais ADC que vão ser utilizado:
 * 	adc1_vSetOneShotMode( 3 );
 * 
 * 3) Configurar os canais que serão utilizados, cuRank determina quem será
 * lido, não deve ser repetido:
 * 	adc1_vSetChannel(ADC1_PA4, ADC_SAMPLETIME_1CYCLE_5, ADC_REGULAR_RANK_1)
 *	adc1_vSetChannel(ADC1_PA7, ADC_SAMPLETIME_1CYCLE_5, ADC_REGULAR_RANK_1)
 *	adc1_vSetChannel(ADC1_PB0, ADC_SAMPLETIME_1CYCLE_5, ADC_REGULAR_RANK_1)
 *
 * 
 * 4) Ler os valores de todos os canais do ADC configurados
 *	u16 ADC1_PA4_value = adc1_uReadNextRank()
 *	u16 ADC1_PA7_value = adc1_uReadNextRank()
 *	u16 ADC1_PB0_value = adc1_uReadNextRank()
 * 
 * * Desativando o ONE SHOT MODE ------------------------------------------
 * 
 * 1) Desativo o periferico
 * 	adc1_vDeInitChannel()
 * 
 * 2) Desative os canais utilizados
 * 	adc1_vDeInitChannel( ADC1_PA4 )
 * 	adc1_vDeInitChannel( ADC1_PA7 )
 * 	adc1_vDeInitChannel( ADC1_PB0 )
 * 
 */

/* Includes ------------------------------------------------------------------*/

#include <core.h>
#include <FreeRTOS/Drivers/gpio.h>

#ifndef adc_H_
#define adc_H_

/* macro ---------------------------------------------------------------------*/

/**
 * prioridades das interrupções do DMA1
 */
#define ADC1_DMA1_NVIC_PRIORITY		( configADC1_DMA1_NVIC_PRIORITY )
#define ADC1_DMA1_NVIC_SUBPRIORITY	( configADC1_DMA1_NVIC_SUBPRIORITY )
/* Definition ----------------------------------------------------------------*/
typedef enum {
	ADC1_PA0 = GPIOA0,
	ADC1_PA1 = GPIOA1,
	ADC1_PA2 = GPIOA2,
	ADC1_PA3 = GPIOA3,
	ADC1_PA4 = GPIOA4,
	ADC1_PA5 = GPIOA5,
	ADC1_PA6 = GPIOA6,
	ADC1_PA7 = GPIOA7,
	ADC1_PB0 = GPIOB0,
	ADC1_PB1 = GPIOB1,
} AdcChannel;

void adc1_vInit(void);

void adc1_vTakeAccess( void );
void adc1_vGiveAccess(  void  );
void adc1_vTakeAccessFromISR( BaseType_t * const pxHigherPriorityTaskWoken );
void adc1_vGiveAccessFromISR(  BaseType_t * const pxHigherPriorityTaskWoken  );

void adc1_vSetOneShotMode( cu32 cuNbrOfConversion );
void adc1_vSetGetSampleMode( cu32 cuNbrOfConversion );
void adc1_vSetChannel( const AdcChannel cxChannel, cu32 cuSamplingTime, cu32 cuRank );

u16 adc1_uReadNextRank( void );
void adc1_vStartGetSampleMode(u16 *puAdc1Buffer, const size_t cuSizeAdc1Buffer, cu32 uPrescaler, cu32 uPeriod );

void adc1_vDeInitChannel(const AdcChannel cuChannel);
void adc1_vDeInit(void);


/**
 * @brief Função dentro da interrupção DMA1_Channel1_IRQHandler
 * @note não trada nenhuma flag de interrupção, porem
 * pxHigherPriorityTaskWoken é tratado no termino da interrupção  
 * @param pxHigherPriorityTaskWoken, recurso do FreeRTOS
 * para o controle de troca de contexto em interrupções.
 */
extern void adc1_vDMA1Ch1Handler(BaseType_t *const pxHigherPriorityTaskWoken);

/**
 * @brief Função dentro da HAL_ADC_ConvCpltCallback.
 * @note O DMA1 é esposável de por copiar os dados das leituras para
 * o buffer. Toda vez que esta função for chamada significa que o buffer
 * fou completamente populado. e está pronto para ser lido.
 * 
 * A HAL_ADC_ConvCpltCallback interrupção também é usara atualizar os dados
 * na swap usada na função adc1_iGetValue. ambas fazem uso de um mutex.
 * 
 * pxHigherPriorityTaskWoken é tratado no termino da interrupção
 * 
 * @param pxHigherPriorityTaskWoken, recurso do FreeRTOS para o controle
 * de troca de contexto em interrupções.
 */
extern void acd1_vBufferDoneHandler(BaseType_t *const pxHigherPriorityTaskWoken);


#endif /* adc_H_ */