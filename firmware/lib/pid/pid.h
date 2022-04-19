/**
 * pid.h
 *
 *  @date Created at:	16/04/2022 21:24:45
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
 * Biblioteca básica para controle PID adapitada para o FreeRTOS
 *
 */

/* Includes ---------------------------------------------------------------------------------------------------------------------------------------------*/
#include <core.h>
#include <FreeRTOS/includes.h>



#ifndef pid_H_
#define pid_H_

/* macro ------------------------------------------------------------------------------------------------------------------------------------------------*/
/* Definition --------------------------------------------------------------------------------------------------------------------------------------------*/

typedef struct {
	float fKp;
	float fKi;
	float fKd;

	float fP;
	float fI;
	float fD;

	float fSetPoint;
	float fInput;
	float fLastInput;
	float fOutput;
	float fError;

	TickType_t xLastTime;


} Pid;

float pid_fRun( Pid *pxPid );
float pid_fRunFromISR( Pid *pxPid );


#endif /* pid_H_ */
