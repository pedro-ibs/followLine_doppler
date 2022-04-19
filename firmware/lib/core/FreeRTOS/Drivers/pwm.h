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
 * Gera sinais PWM, Cada Timer tem acesso a um grupo de pinos por exemplo
 * com o TIM1 será possível gerar PWM nos pinos
 * 	PA8  T1C1	PWM_CH1
 * 	PA9  T1C2	PWM_CH2
 * 	PA10 T1C3	PWM_CH3
 * 	PA11 T1C4	PWM_CH4
 * 
 * 1) utilise pwm_vSetGpio() para configurar os pinos GPIOs que serão 
 * utilisados;
 * 
 * 2 ) com pwm_vSetup(); é configurado qual timer será utilizado e 
 * frequência de trabalho com "pwm_vSetup(TIN1, 72-1, 1000-1)" o TIM! irá
 * gerar um PWM de 1kHz.
 * 
 * 3 ) Para controlar o pulso PWM use "pwm_vSetChannel()", o valor do pulso
 * deve estar coerente com o periodo configurado no timer.
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
