/**
 * motor.h
 *
 *  @date Created at:	17/04/2022 12:51:18
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
 * Biblioteca para controle de motores por meio de ponte H + PWM
 *
 */




/* Includes ---------------------------------------------------------------------------------------------------------------------------------------------*/
#include "motor.h"
/* Local macro ------------------------------------------------------------------------------------------------------------------------------------------*/
/* Local variables --------------------------------------------------------------------------------------------------------------------------------------*/
/* Local Functions --------------------------------------------------------------------------------------------------------------------------------------*/
/* Functions --------------------------------------------------------------------------------------------------------------------------------------------*/


void motor_vToFront(u16 uPwm, Motor *pxMotor){
	pwm_vSetChannel(uPwm, pxMotor->xP1);
	pwm_vSetChannel(0, pxMotor->xP2);
}

void motor_vToBack(u16 uPwm, Motor *pxMotor){
	pwm_vSetChannel(0, pxMotor->xP1);
	pwm_vSetChannel(uPwm, pxMotor->xP2);
}

void motor_vToStop(u16 uPwm, Motor *pxMotor){
	pwm_vSetChannel(uPwm, pxMotor->xP1);
	pwm_vSetChannel(uPwm, pxMotor->xP2);
}

void motor_vTurnoff(Motor *pxMotor){
	pwm_vSetChannel(0, pxMotor->xP1);
	pwm_vSetChannel(0, pxMotor->xP2);
}

/*########################################################################################################################################################*/
/*########################################################################################################################################################*/
/*########################################################################################################################################################*/
/*-------------------------------------------------------------------- Local Functions -------------------------------------------------------------------*/
/*########################################################################################################################################################*/
