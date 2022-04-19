/**
 * motor.h
 *
 *  @date Created at:	17/04/2022 12:51:07
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
 * Biblioteca generica para controle de motores por meio de uma PONTE H
 * 
 * 	   	   ______
 * 	PWM_CH1 --|	|-- M1A1
 * 	PWM_CH2 --|	|-- M1A2
 *		  |	|
 *	PWM_CH3 --|	|-- M2A1
 *	PWM_CH4 --|_____|-- M2A2
 */

/* Includes ---------------------------------------------------------------------------------------------------------------------------------------------*/
#include <core.h>
#include <FreeRTOS/includes.h>

#ifndef motor_H_
#define motor_H_

/* macro ------------------------------------------------------------------------------------------------------------------------------------------------*/
/* Definition --------------------------------------------------------------------------------------------------------------------------------------------*/

typedef struct {
	PwmChenel xP1;
	PwmChenel xP2;
} Motor;


void motor_vToFront(u16 uPwm, Motor *pxMotor);
void motor_vToBack(u16 uPwm, Motor *pxMotor);
void motor_vToStop(u16 uPwm, Motor *pxMotor);
void motor_vTurnoff(Motor *pxMotor);

#endif /* motor_H_ */
