/**
 * rtc.c.c
 *
 *  @date Created at:	03/03/2021 16:10:56
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
 * Controla a periferia de hardware RCT. essa biblioteca
 * pode gerar uma data como 20/7/69-23:56:19
 *
 */



/* Includes ------------------------------------------------------------------*/
/* stm includes */
#include <core.h>
#include <stm32f1xx_hal_rtc.h>
#include <stm32f1xx_hal_rtc_ex.h>


/* FreeRTOS standard includes.  */
#include <FreeRTOS/include/FreeRTOSConfig.h>
#include <FreeRTOS/include/FreeRTOS.h>
#include <FreeRTOS/include/task.h>
#include <FreeRTOS/include/list.h>
#include <FreeRTOS/include/queue.h>
#include <FreeRTOS/include/semphr.h>
#include <FreeRTOS/include/portable.h>

/* include driver */
#include <FreeRTOS/Drivers/rtc.h>


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static RTC_HandleTypeDef xRtc			= { 0 };
static SemaphoreHandle_t xSemaphoreRTC		= NULL;

/* Private Functions ---------------------------------------------------------*/
char *__pcZeroLeft(char *pcStr, const size_t cuSizeWithZeros);


/**
 * @brief inicializa o periferico RTC com clock interno 
 * A data inicial é 20 de Julio de 1969, as 23:56:19
 */
void rtc_vInit(void){

	if(xSemaphoreRTC == NULL){
		xSemaphoreRTC = xSemaphoreCreateMutex();
	}

	xSemaphoreTake(xSemaphoreRTC, portMAX_DELAY);

	HAL_PWR_EnableBkUpAccess();
	__HAL_RCC_BKP_CLK_ENABLE();
	__HAL_RCC_RTC_ENABLE();	

	/** Inicar RTC **/
	xRtc.Instance			= RTC;
	xRtc.Init.AsynchPrediv		= RTC_AUTO_1_SECOND;
	xRtc.Init.OutPut 		= RTC_OUTPUTSOURCE_NONE;
	if (HAL_RTC_Init(&xRtc) != HAL_OK) {
		Error_Handler();
	}

	/** Iniciar RTC e setar o tempo e data **/
	RTC_TimeTypeDef xTime		= { 0 };
	xTime.Hours			= 23;
	xTime.Minutes			= 56;
	xTime.Seconds			= 19;
	if (HAL_RTC_SetTime(&xRtc, &xTime, RTC_FORMAT_BIN) != HAL_OK) {
		Error_Handler();
	}

	RTC_DateTypeDef xDateToUpdate	= { 0 };
	xDateToUpdate.WeekDay		= RTC_WEEKDAY_SUNDAY;
	xDateToUpdate.Month		= RTC_MONTH_JULY;
	xDateToUpdate.Date		= 20;
	xDateToUpdate.Year		= 69;
	if (HAL_RTC_SetDate(&xRtc, &xDateToUpdate, RTC_FORMAT_BIN) != HAL_OK) {
		Error_Handler();
	}


	xSemaphoreGive(xSemaphoreRTC);

}

/**
 * @brief atualiza a data do driver RTC
 * 
 * @param xDate: RTC_DateTypeDef, data como dia, dia da semana, mes e ano
 * 
 * @param xTime: RTC_TimeTypeDef, tempo como tempo, horas, minutos e segundos
 */
void rtc_vUpdate(RTC_DateTypeDef *xDate, RTC_TimeTypeDef *xTime){
	xSemaphoreTake(xSemaphoreRTC, portMAX_DELAY);

	if (HAL_RTC_SetTime(&xRtc, xTime, RTC_FORMAT_BIN) != HAL_OK) {
		Error_Handler();
	}

	if (HAL_RTC_SetDate(&xRtc, xDate, RTC_FORMAT_BIN) != HAL_OK) {
		Error_Handler();
	}

	xSemaphoreGive(xSemaphoreRTC);
}


/**
 * @brief lé a quantidade de segunsdos que uma task está ativa.
 * @note utiliza xTaskGetTickCount, que lê o valor do cotador
 * de ticks de uma task. como o FreeRTOS está configurado para
 * contar um tick acada 1 ms essa função pega a milezima parte
 * disso.
 *.
 * @return segundos, [TickType_t]
 * 
 */
TickType_t rtc_xTaskGetSeconds( void ){
	return xTaskGetTickCount() / 1000;
}


/**
 * @brief ler tempo, horas, minutos e segundos
 *
 * @return RTC_TimeTypeDef
 */
RTC_TimeTypeDef rtc_xGetTime(void){
	xSemaphoreTake(xSemaphoreRTC, portMAX_DELAY);
	RTC_TimeTypeDef xTime = { 0 };
	HAL_RTC_GetTime(&xRtc, &xTime, RTC_FORMAT_BIN);
	xSemaphoreGive(xSemaphoreRTC);
	return xTime;
}


/**
 * @brief ler dia, dia da semana, mes e ano 
 *
 * @return RTC_DateTypeDef
 */
RTC_DateTypeDef rtc_xGetDate(void){
	xSemaphoreTake(xSemaphoreRTC, portMAX_DELAY);
	RTC_DateTypeDef xDate = { 0 };
	HAL_RTC_GetDate(&xRtc, &xDate, RTC_FORMAT_BIN);
	xSemaphoreGive(xSemaphoreRTC);
	return xDate;
}



/**
 * @brief monta um time stamp em formato de uma string como 20/07/69-20:56:19
 * @param xDate: RTC_DateTypeDef, data como dia, dia da semana, mes e ano
 * @param xTime: RTC_TimeTypeDef, tempo como tempo, horas, minutos e segundos
 * @param pcBuffer: vetor ondo o time stamp será montado
 * @return pcBuffer com a data montada
 */
CCHR *rtc_pcBuildTimeStamp(const RTC_DateTypeDef xDate, const RTC_TimeTypeDef xTime, char *pcBuffer){	
	xSemaphoreTake(xSemaphoreRTC, portMAX_DELAY);

	char pcSwap[4];

	// dia
	itoa(xDate.Date, pcSwap, DEC);
	strcat(pcBuffer, __pcZeroLeft(pcSwap, 2));
	strcat(pcBuffer, "/");

	// mes
	itoa(xDate.Month, pcSwap, DEC);
	strcat(pcBuffer, __pcZeroLeft(pcSwap, 2));
	strcat(pcBuffer, "/");

	// ano
	itoa(xDate.Year, pcSwap, DEC);
	strcat(pcBuffer, __pcZeroLeft(pcSwap, 2));
	strcat(pcBuffer, "-");

	// horas
	itoa(xTime.Hours, pcSwap, DEC);
	strcat(pcBuffer, __pcZeroLeft(pcSwap, 2));
	strcat(pcBuffer, ":");

	// minutos
	itoa(xTime.Minutes, pcSwap, DEC);
	strcat(pcBuffer, __pcZeroLeft(pcSwap, 2));
	strcat(pcBuffer, ":");

	//segundos
	itoa(xTime.Seconds, pcSwap, DEC);
	strcat(pcBuffer, __pcZeroLeft(pcSwap, 2));
	strcat(pcBuffer, "");

	xSemaphoreGive(xSemaphoreRTC);

	return (CCHR*)pcBuffer;
}


/**
 * @brief monta o time stamp em formato de uma string como 20/07/69-20:56:19
 * de acordo com a tata configurada no sistema.
 * @param pcBuffer: vetor ondo o time stamp será montado
 * @return pcBuffer com a data montada
 */
CCHR *rtc_pcGetTimeStamp(char *pcBuffer){
	 return rtc_pcBuildTimeStamp( rtc_xGetDate(), rtc_xGetTime(), pcBuffer);
}


/**
 * @brief testa se ouve um estouro de um contador de tempo
 * 
 * @param pxTimeS contador que armazana o temp em segundos
 * 
 * @param cxOverflowS tempo de estouro em segundos
 * 
 * @note essa função faz uso do driver rtc, portato o temo 
 * é com relação a existência da task onde a mesma é chamada
 * 
 * @note apos o estouro pxTimeS NÃO é limpo
 * 
 * @return TRUE:  houve um estouro
 * 
 * @return FALSE: não houve um estouro
 */
_bool rtc_bTimerOverflow( TickType_t *pxTimeS, const TickType_t cxOverflowS ){
	if(*pxTimeS == 0){
		*pxTimeS = cxOverflowS + rtc_xTaskGetSeconds();
	} else{
		if(*pxTimeS < rtc_xTaskGetSeconds()){
			return TRUE;
		}
	}
	return FALSE;
}

/*########################################################################################################################################################*/
/*########################################################################################################################################################*/
/*########################################################################################################################################################*/
/*-------------------------------------------------------------------- Private Functions -----------------------------------------------------------------*/
/*########################################################################################################################################################*/




/**
 * @brief adiciona zero a esquerda.
 * @param pcStr string que será editada
 * @param cuSizeWithZeros quantidade de zeros a esqurda 
 * @return char*: string com os zeros a esquerda inseridos
 * 
 * @note copiado da lib textProtocol:
 * https://github.com/pedro-ibs/textProtocol
 * 
 */
char *__pcZeroLeft(char *pcStr, const size_t cuSizeWithZeros){
	while (cuSizeWithZeros > strlen(pcStr)){
		size_t idx = strlen(pcStr);
		do{
			pcStr[idx+1] = pcStr[idx];
			if(idx-- == 0) break;
		}while(TRUE);
		pcStr[0] = '0';
	}

	return pcStr;
}