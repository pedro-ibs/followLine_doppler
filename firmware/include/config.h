/**
 * config.h
 *
 *  @date Created at:	13/04/2022 19:27:34
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
 *
 * configurações da aplicação
 *
 */

/* Includes ---------------------------------------------------------------------------------------------------------------------------------------------*/
#include "hardware.h"
#include "FreeRTOS/include/FreeRTOSConfig.h"

#ifndef config_H_
#define config_H_

/* macro -------------------------------------------------------------------------------------------------------------------------------------------------*/
/* Definition --------------------------------------------------------------------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------------------------------------------------------------------------------*/
/* Configuração para aquisição e tratamento dos sinais analógicos											  */
/*--------------------------------------------------------------------------------------------------------------------------------------------------------*/
#define CHANNELS			( 8							)
#define SAMPLES				( 10 							)
#define BUFFER_SIZE			( CHANNELS * SAMPLES 					)


#define FREQUENCY			( 1000							)
#define PRESCALER			( 1							)
#define PERIOD				( FREG_TO_COUNTER( FREQUENCY*SAMPLES, PRESCALER )	)
#define ADC_SAMPLETIME			( ADC_SAMPLETIME_71CYCLES_5			)

#define LINE_DETECTED_0			( 3500							)
#define LINE_DETECTED_1			( 3500							)
#define LINE_DETECTED_2			( 3500							)
#define LINE_DETECTED_3			( 3500							)
#define LINE_DETECTED_4			( 3500							)
#define LINE_DETECTED_5			( 3500							)
#define LINE_DETECTED_6			( 3500							)
#define LINE_DETECTED_7			( 3500							)
#define LINE_FULL_DETECTED		( LINE_VALUE_7 - ( LINE_VALUE_1 / CHANNELS )		)

#define LINE_VALUE_0			( 1000 * 0						)
#define LINE_VALUE_1			( 1000 * 1						)
#define LINE_VALUE_2			( 1000 * 2						)
#define LINE_VALUE_3			( 1000 * 3						)
#define LINE_VALUE_4			( 1000 * 4						)
#define LINE_VALUE_5			( 1000 * 5						)
#define LINE_VALUE_6			( 1000 * 6						)
#define LINE_VALUE_7			( 1000 * 7						)



/*--------------------------------------------------------------------------------------------------------------------------------------------------------*/
/* Configuração para controle de valocidade														  */
/*--------------------------------------------------------------------------------------------------------------------------------------------------------*/
#define PWM_PRESCALER			( 72-1							)
#define PWM_PERIOD			( 1000-1						)
#define PWM_MAX				( 550							)
#define PWM_MIN				( 0							)
#define PWM_BASE			( PWM_MAX * 0.75					)


#define PID_KP				( 0.75							)
#define PID_KI				( 0.3							)
#define PID_KD				( 0.075							)
#define PID_SETPOINT			( LINE_VALUE_4 - ( LINE_VALUE_1 / 2 )			)

#endif /* config_H_ */
