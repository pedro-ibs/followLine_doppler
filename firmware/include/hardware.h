/**
 * hardware.h
 *
 *  @date Created at:	13/04/2022 19:26:11
 *	@author:	Pedro Igor B. S.
 *	@email:		pedro.igor.ifsp@gmail.com
 * 	GitHub:		https://github.com/pedro-ibs
 * 	tabSize:	8
 *
 * #######################################################################
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
 * #######################################################################
 *
 * definições do hardware utilizado 
 *
 */

/* Includes ---------------------------------------------------------------------------------------------------------------------------------------------*/
#include <FreeRTOS/Drivers/all.h>



#ifndef hardware_H_
#define hardware_H_

/* macro ------------------------------------------------------------------------------------------------------------------------------------------------*/


#define LINE_0		ADC1_PA0
#define LINE_1		ADC1_PA1
#define LINE_2		ADC1_PA2
#define LINE_3		ADC1_PA3
#define LINE_4		ADC1_PA4
#define LINE_5		ADC1_PA5
#define LINE_6		ADC1_PA6
#define LINE_7		ADC1_PA7

#define LINE_STOP	GPIOB3
#define LINE_CURVE	GPIOA15

#define MOTOR_M1A1	GPIOA10
#define MOTOR_M1A1_PWM	PWM_CH3
#define MOTOR_M1A2	GPIOA11
#define MOTOR_M1A2_PWM	PWM_CH4

#define MOTOR_M2A1	GPIOA8
#define MOTOR_M2A1_PWM	PWM_CH1
#define MOTOR_M2A2	GPIOA9
#define MOTOR_M2A2_PWM	PWM_CH2

#define LED		GPIOC13

/* Definition -------------------------------------------------------------------------------------------------------------------------------------------*/


#endif /* hardware_H_ */
