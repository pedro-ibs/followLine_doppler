/**
 * main_app.h
 *
 *  @date Created at:	13/04/2022 19:30:36
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
 * Esse é um programa dedicado ao controle do robô seguidor de linha doppler
 * utilizado na competição "RMS CHALLENGE" 04/21-23 de 2022
 * 
 * A aplicação utilisa o FreeRTOS e driveres para ter acesso aos recursos
 * da CPU
 * 
 * Para a litura dos sensore é utilizado o DMA
 * Para a execução do controle PID e tuasão dos motores é utilizado a
 * interrupção do DMA
 * 
 * Em config.h e hardware.h estão as definições de hardware e configurações
 * da aplicação 
 * 
 */



/* Includes ---------------------------------------------------------------------------------------------------------------------------------------------*/
#include <core.h>
#include <FreeRTOS/includes.h>
#include <config.h>
#include <hardware.h>
#include <pid.h>
#include <motor.h>
/* Local macro ------------------------------------------------------------------------------------------------------------------------------------------*/
/* Local variables --------------------------------------------------------------------------------------------------------------------------------------*/
typedef struct {
	Motor xMotor;
	float fPwm;
}MotorTask;

static const u16 scpuLineDetected[SAMPLES]	= {
	LINE_DETECTED_0,
	LINE_DETECTED_1,
	LINE_DETECTED_2,
	LINE_DETECTED_3,
	LINE_DETECTED_4,
	LINE_DETECTED_5,
	LINE_DETECTED_6,
	LINE_DETECTED_7
};

static const short int scpuLineValue[SAMPLES]	= {
	LINE_VALUE_0,
	LINE_VALUE_1,
	LINE_VALUE_2,
	LINE_VALUE_3,
	LINE_VALUE_4,
	LINE_VALUE_5,
	LINE_VALUE_6,
	LINE_VALUE_7
};

static u16 spuAdc1Buffer[BUFFER_SIZE]		= { 0 };
static u16 spuAverage[CHANNELS]			= { 0 };
static MotorTask sxMa 				= { { MOTOR_M1A1_PWM, MOTOR_M1A2_PWM }, PWM_MIN };
static MotorTask sxMb 				= { { MOTOR_M2A1_PWM, MOTOR_M2A2_PWM }, PWM_MIN };
static Pid sxPid				= { 0 };
static u16 uPosition				= 0;
static _bool sbStop				= FALSE;
static TickType_t sxIgnoreStop			= 0;
/* Local Functions --------------------------------------------------------------------------------------------------------------------------------------*/
void motor_vTask(void * pvParameters);
void app_vSignalTreatment(void);
void app_vCleanValues( void );
void app_vProcess( void );
/* Functions --------------------------------------------------------------------------------------------------------------------------------------------*/


/*########################################################################################################################################################*/
/*########################################################################################################################################################*/
/*########################################################################################################################################################*/
/*-------------------------------------------------------------------- Local Functions -------------------------------------------------------------------*/
/*########################################################################################################################################################*/

/**
 * Apos as configurações de hardware e valores de variaveis, Essa só irá  task existir
 * para realizar o "debug", caso APP_DEBUG não for definido ela será desativada (deletada).
 * 
 * veja em platformio.ini
 */
void main_vApp(void * pvParameters){
	(void) pvParameters;

	vTaskDelay(_5S);

	sxPid.fSetPoint = PID_SETPOINT;
	sxPid.fKp 	= PID_KP;
	sxPid.fKi 	= PID_KI;
	sxPid.fKd 	= PID_KD;

	gpio_vInitAll(  );
	gpio_vDisableDebug ( );

	gpio_vMode(LED,		GPIO_MODE_OUTPUT_OD, GPIO_PULLDOWN);
	gpio_vMode(LINE_STOP,	GPIO_MODE_INPUT, GPIO_PULLDOWN);
	gpio_vMode(LINE_CURVE,	GPIO_MODE_INPUT, GPIO_PULLDOWN);

	pwm_vSetGpio( MOTOR_M1A1 );
	pwm_vSetGpio( MOTOR_M1A2 );
	pwm_vSetGpio( MOTOR_M2A1 );
	pwm_vSetGpio( MOTOR_M2A2 );

	
	pwm_vSetup(TIM1, PWM_PRESCALER, PWM_PERIOD);
	motor_vToStop(PWM_MAX, &sxMa.xMotor);
	motor_vToStop(PWM_MAX, &sxMb.xMotor);

	adc1_vInit();
	adc1_vSetGetSampleMode( CHANNELS );
	adc1_vSetChannel( LINE_0, ADC_SAMPLETIME, ADC_REGULAR_RANK_1	);
	adc1_vSetChannel( LINE_1, ADC_SAMPLETIME, ADC_REGULAR_RANK_2	);
	adc1_vSetChannel( LINE_2, ADC_SAMPLETIME, ADC_REGULAR_RANK_3	);
	adc1_vSetChannel( LINE_3, ADC_SAMPLETIME, ADC_REGULAR_RANK_4	);
	adc1_vSetChannel( LINE_4, ADC_SAMPLETIME, ADC_REGULAR_RANK_5	);
	adc1_vSetChannel( LINE_5, ADC_SAMPLETIME, ADC_REGULAR_RANK_6	);
	adc1_vSetChannel( LINE_6, ADC_SAMPLETIME, ADC_REGULAR_RANK_7	);
	adc1_vSetChannel( LINE_7, ADC_SAMPLETIME, ADC_REGULAR_RANK_8	);

	/**
	 * A frequência de aquisição é de 10khz, onde os
	 * canais são lidos 10 vezes antes de chamar a
	 * interrupção do DAM dessa forma ela irá ocorrer
	 * a cada 1ms.
	 */
	adc1_vStartGetSampleMode( spuAdc1Buffer, BUFFER_SIZE, PRESCALER, PERIOD );


	#ifdef APP_DEBUG
		char pcString[10] = "0";
	#endif

	while (TRUE) {
		#ifdef APP_DEBUG
			usart_vAtomicSendStr(ttyUSART1, "MA: ");
			usart_vAtomicSendStr(ttyUSART1, textp_pcCharLeft(itoa(sxPid.fSetPoint,	pcString, DEC), 4, ' '));
			usart_vAtomicSendStr(ttyUSART1, ", ");
			usart_vAtomicSendStr(ttyUSART1, textp_pcCharLeft(itoa(uPosition,	pcString, DEC), 4, ' '));
			usart_vAtomicSendStr(ttyUSART1, ", ");
			usart_vAtomicSendStr(ttyUSART1, textp_pcCharLeft(itoa(sxPid.fError,	pcString, DEC), 4, ' '));
			usart_vAtomicSendStr(ttyUSART1, ", ");
			usart_vAtomicSendStr(ttyUSART1, textp_pcCharLeft(itoa(sxPid.fOutput,	pcString, DEC), 4, ' '));
			usart_vAtomicSendStr(ttyUSART1, ", ");
			usart_vAtomicSendStr(ttyUSART1, textp_pcCharLeft(itoa(sxMa.fPwm,	pcString, DEC), 4, ' '));
			usart_vAtomicSendStr(ttyUSART1, ", ");
			usart_vAtomicSendStr(ttyUSART1, textp_pcCharLeft(itoa(sxMb.fPwm,	pcString, DEC), 4, ' '));
			usart_vAtomicSendStr(ttyUSART1, "\r\n" );
		#else
			break;
		#endif

		vTaskDelay(_20MS);
	}

	vTaskDelete(NULL);
}


void app_vCleanValues( void ){
	for (u8 uChannel = 0; uChannel < CHANNELS; uChannel++) {
		spuAverage[uChannel] = 0;
	}
	uPosition = 0;
}

/**
 * 1 ) após o DMA concluir o preenchimento do buffer, os dado
 * são acumulados (somados) para cada canal respectivo.
 * iniciando o processo para calcular do valor médio lido por
 * cada senor.
 * 
 * 2 ) Termino do processo do calculo de media.
 * 
 * 3 ) Trazforma os valores lidos pelos sensores em posicionamento
 * onde variam de 0 a 7000 e gradações como mostrado abaixo:
 * 
 * 
 * S0	S1	S2	S3	S4	S5	S6	S7
 * 0	1000	2000	3000	4000	5000	6000	7000
 * 			LLLLLLLLLLLL
 * 			    3500
 * 
 * S0	S1	S2	S3	S4	S5	S6	S7
 * 0	1000	2000	3000	4000	5000	6000	7000
 * LLLLLLLLL
 *   500
 * 
 * S0	S1	S2	S3	S4	S5	S6	S7
 * 0	1000	2000	3000	4000	5000	6000	7000
 * 						LLLLLLLLLLLL
 * 						    6500
 * 
 * S0	S1	S2	S3	S4	S5	S6	S7
 * 0	1000	2000	3000	4000	5000	6000	7000
 *		LLLL
 *		2000
 */
void app_vSignalTreatment( void ){
	for (u8 uSample = 0; uSample < SAMPLES; uSample++) {
		for (u8 uChannel = 0; uChannel < CHANNELS; uChannel++) {
			spuAverage[uChannel] += spuAdc1Buffer[ ( uSample * CHANNELS ) + uChannel ];
		}
	}

	for (u8 uChannel = 0; uChannel < CHANNELS; uChannel++) {
		spuAverage[uChannel] = (spuAverage[uChannel] / SAMPLES );
	}

	u8 uContDetection = 0;
	for (u8 uChannel = 0; uChannel < CHANNELS; uChannel++) {

		if(spuAverage[uChannel] < scpuLineDetected[uChannel]){
			uContDetection++;

			/**
			 * Manter Valor maximo detectado
			 */
			uPosition = ( scpuLineValue[uChannel]  );

			/**
			 * Criar gradação intermediaria
			 * 
			 */
			if(uContDetection > 1){
				uPosition -= (LINE_VALUE_1 / uContDetection);
			}
		};
	}
}


void app_vProcess( void ){
	if(sbStop == TRUE){
		/**
		 * O overchute do controle pode fazer com que o robô
		 * trave em cima de um crusamento portanto deve 
		 * ser verificado se a faixa está por baixo dos 
		 * sensores para mitigar esse bug
		 * 
		 */
		if( ( uPosition >= LINE_FULL_DETECTED ) == TRUE ){
			sbStop = FALSE;
		} else {
			return; 
		}
	} 


	/**
	 * Se todos os sensores estão ativos o robô está
	 * em um cusamento portanto o stop deve ser ignorado
	 * por um tempo, e o robô deve continuar indo para
	 * frente.
	 */
	if( ( uPosition >= LINE_FULL_DETECTED ) == TRUE ){
		sxMa.fPwm = PWM_MAX;
		sxMb.fPwm = PWM_MAX;	

		motor_vToFront(sxMa.fPwm, &sxMa.xMotor);
		motor_vToFront(sxMb.fPwm, &sxMb.xMotor);

		sxIgnoreStop = xTaskGetTickCountFromISR() + 1000;
	}
	
	/**
	 * Quando faixa de chagada for detectada o robô
	 * iár travar os motores e "desabilitar" o PID
	 */
	if(sxIgnoreStop < xTaskGetTickCountFromISR()){
		if(gpio_bRead(LINE_STOP) == FALSE){
			sbStop = TRUE;

			sxMa.fPwm = PWM_MIN;
			sxMb.fPwm = PWM_MIN;	

			motor_vToStop(1000, &sxMa.xMotor);
			motor_vToStop(1000, &sxMb.xMotor);

			return;
		}
	}

	sxPid.fInput = uPosition;
	sxPid.fOutput =  pid_fRunFromISR(&sxPid);

	/**
	 * Saída do PID
	 */
	if(uPosition < PID_SETPOINT){
		sxMa.fPwm = PWM_BASE + abs(sxPid.fOutput);
		sxMb.fPwm = PWM_BASE - abs(sxPid.fOutput);
	} else if(uPosition > PID_SETPOINT){
		sxMa.fPwm = PWM_BASE - abs(sxPid.fOutput);
		sxMb.fPwm = PWM_BASE + abs(sxPid.fOutput);
	} else {
		sxMa.fPwm = PWM_BASE;
		sxMb.fPwm = PWM_BASE;
	}


	/**
	 * Determinar valores minimos e maximos para garantir 
	 * o funcionamento corredo robô.
	 */
	if(sxMa.fPwm > PWM_MAX ){
		sxMa.fPwm = PWM_MAX;	
	}

	if(sxMa.fPwm < PWM_MIN ){
		sxMa.fPwm = PWM_MIN;	
	}

	if(sxMb.fPwm > PWM_MAX ){
		sxMb.fPwm = PWM_MAX;	
	}

	if(sxMb.fPwm < PWM_MIN ){
		sxMb.fPwm = PWM_MIN;	
	}

	motor_vToFront(sxMa.fPwm, &sxMa.xMotor);
	motor_vToFront(sxMb.fPwm, &sxMb.xMotor);
}

void acd1_vBufferDoneHandler(BaseType_t *const pxHigherPriorityTaskWoken){
	(void) pxHigherPriorityTaskWoken;

	app_vCleanValues();

	app_vSignalTreatment();

	app_vProcess();

	gpio_vToggle(LED);
}


void adc1_vDMA1Ch1Handler(BaseType_t *const pxHigherPriorityTaskWoken){
	(void) pxHigherPriorityTaskWoken;
}

void tim3_vHandler(BaseType_t *const pxHigherPriorityTaskWoken){
	(void) pxHigherPriorityTaskWoken;

}
