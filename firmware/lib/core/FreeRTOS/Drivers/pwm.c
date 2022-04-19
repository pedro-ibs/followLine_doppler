/**
 * pwm.h
 *
 *  @date Created at:	16/04/2022 14:30:16
 *	@author:	Pedro Igor B. S.
 *	@email:		pedro.igor.ifsp@gmail.com
 * 	GitHub:		https://github.com/pedro-ibs
 * 	tabSize:	8
 *
 * #######################################################################
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
 * #######################################################################
 *
 *
 * TODO: documentation or resume or Abstract
 *
 */



/* Includes ---------------------------------------------------------------------------------------------------------------------------------------------*/
#include <FreeRTOS/Drivers/pwm.h>
#include <stm32f103xb.h>
#include <stm32f1xx_ll_gpio.h>
#include <stm32f1xx_ll_tim.h>
#include <stm32f1xx_hal_tim.h>
/* Local macro ------------------------------------------------------------------------------------------------------------------------------------------*/
/* Local variables --------------------------------------------------------------------------------------------------------------------------------------*/
static TIM_HandleTypeDef sxTim1Handle;

static const u32 scpuTimChannel[ PWM_NUM ] = {
	TIM_CHANNEL_1,
	TIM_CHANNEL_2,
	TIM_CHANNEL_3,
	TIM_CHANNEL_4
};

/* Local Functions --------------------------------------------------------------------------------------------------------------------------------------*/
/* Functions --------------------------------------------------------------------------------------------------------------------------------------------*/

void pwm_vSetGpio(GpioLabel xGpio){
	gpio_vMode(xGpio, GPIO_MODE_AF_PP, GPIO_NOPULL);
}

void pwm_vSetup(TIM_TypeDef *pxTimer, u32 uPrescaler, u32 uPeriod ){

	__HAL_RCC_TIM1_CLK_ENABLE();

	sxTim1Handle.Instance			= pxTimer;
	sxTim1Handle.Init.Prescaler		= uPrescaler;
	sxTim1Handle.Init.CounterMode 		= TIM_COUNTERMODE_UP;
	sxTim1Handle.Init.Period		= uPeriod;
	sxTim1Handle.Init.ClockDivision 	= TIM_CLOCKDIVISION_DIV1;
	if (HAL_TIM_Base_Init(&sxTim1Handle) != HAL_OK) {
		Error_Handler();
	}

	TIM_ClockConfigTypeDef sClockSourceConfig = { 0 };
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&sxTim1Handle, &sClockSourceConfig) != HAL_OK) {
		Error_Handler();
	}

	if (HAL_TIM_PWM_Init(&sxTim1Handle) != HAL_OK) {
		Error_Handler();
	}

	TIM_MasterConfigTypeDef sMasterConfig = {0};
	sMasterConfig.MasterOutputTrigger	= TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode		= TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&sxTim1Handle, &sMasterConfig) != HAL_OK) {
		Error_Handler();
	}

	HAL_TIM_Base_Start(&sxTim1Handle);
}

void pwm_vSetChannel(u32 uPulse, u8 uChannel) {

	TIM_OC_InitTypeDef sConfigOC = {0};

	sConfigOC.OCMode	= TIM_OCMODE_PWM1;
	sConfigOC.Pulse		= uPulse;
	sConfigOC.OCPolarity	= TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode	= TIM_OCFAST_DISABLE;


	HAL_TIM_PWM_Stop(&sxTim1Handle, scpuTimChannel[uChannel]);
	HAL_TIM_PWM_ConfigChannel(&sxTim1Handle, &sConfigOC, scpuTimChannel[uChannel]);
	HAL_TIM_PWM_Start(&sxTim1Handle, scpuTimChannel[uChannel]);
}

/*########################################################################################################################################################*/
/*########################################################################################################################################################*/
/*########################################################################################################################################################*/
/*-------------------------------------------------------------------- Local Functions -------------------------------------------------------------------*/
/*########################################################################################################################################################*/


