/**
 * pwm.h
 *
 *  @date Created at:	16/04/2022 14:29:46
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
#include <core.h>
#include "gpio.h"

#ifndef pwm_H_
#define pwm_H_

/* macro ------------------------------------------------------------------------------------------------------------------------------------------------*/
/* Definition --------------------------------------------------------------------------------------------------------------------------------------------*/

typedef enum{
	PWM_CH1 = 0,
	PWM_CH2	,
	PWM_CH3,
	PWM_CH4,
	PWM_NUM
} PwmChenel;

void pwm_vSetGpio(GpioLabel xGpio);
void pwm_vSetup(TIM_TypeDef *pxTimer, u32 uPrescaler, u32 uPeriod );
void pwm_vSetChannel(u32 uPulse, u8 uChannel);

#endif /* pwm_H_ */
