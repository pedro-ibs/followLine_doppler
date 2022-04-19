/**
 * pid.h
 *
 *  @date Created at:	16/04/2022 21:25:21
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
 * 1 ) Defina os valores de Kp, Ki e Kd de acordo com o processo a ser 
 * controlado.
 * 
 * 2 ) Defina o valor so SetPoint
 * 
 * 3 ) Salve o valor da entrada
 * 
 * 4 ) calculo
 * 
 * 5 ) saía
 * 
 * Pid xPid = { 0 }
 * 
 * 	sxPid.fKp 	= 0.1;
 * 	sxPid.fKi 	= 0.2;
 * 	sxPid.fKd 	= 0.03;
 *
 * 
 * 	while(TRUE) {
 *	  	sxPid.fSetPoint = 500;
 *		sxPid.fInput = VALOR_DA_AQUISIÇÃO
 *  		pid_fRun(&sxPid);
 * 
 *		sxPid.fOutput 	// SAÍDA  
 * 
 * 	}
 */



/* Includes ---------------------------------------------------------------------------------------------------------------------------------------------*/
#include "pid.h"
#include "FreeRTOS/include/task.h"
/* Local macro ------------------------------------------------------------------------------------------------------------------------------------------*/
#define TO_SECONDS(TICKS) ( ( TICKS ) / 1000.0)

/* Local variables --------------------------------------------------------------------------------------------------------------------------------------*/
/* Local Functions --------------------------------------------------------------------------------------------------------------------------------------*/
float pid_fCore( Pid *pxPid, float xDeltaTime );
/* Functions --------------------------------------------------------------------------------------------------------------------------------------------*/


float pid_fRun( Pid *pxPid ){
	float xDeltaTime = TO_SECONDS( xTaskGetTickCount() - pxPid->xLastTime );
	pxPid->xLastTime = xTaskGetTickCount();
	return pid_fCore( pxPid, xDeltaTime );
}

float pid_fRunFromISR( Pid *pxPid ){
	float xDeltaTime = TO_SECONDS( xTaskGetTickCountFromISR() - pxPid->xLastTime );
	pxPid->xLastTime = xTaskGetTickCountFromISR();
	return pid_fCore( pxPid, xDeltaTime );
}



/*########################################################################################################################################################*/
/*########################################################################################################################################################*/
/*########################################################################################################################################################*/
/*-------------------------------------------------------------------- Local Functions -------------------------------------------------------------------*/
/*########################################################################################################################################################*/

float pid_fCore( Pid *pxPid, float xDeltaTime ){
	pxPid->fError = pxPid->fSetPoint - pxPid->fInput;

	// proportional
	pxPid->fP = pxPid->fError * pxPid->fKp;

	// integra
	pxPid->fI += (pxPid->fError * pxPid->fKi) * xDeltaTime;

	// derived
	pxPid->fD = (pxPid->fLastInput - pxPid->fInput)  * pxPid->fKd * xDeltaTime;
	pxPid->fLastInput = pxPid->fInput;

	pxPid->fOutput = pxPid->fP + pxPid->fI + pxPid->fD;

	return pxPid->fOutput;
}
