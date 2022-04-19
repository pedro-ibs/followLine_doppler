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
	siLine = 0;

	for (u8 uChannel = 0; uChannel < CHANNELS; uChannel++) {
		if(spuAverage[uChannel] < scpuLineDetected[uChannel]){
			uContDetection++;

			/**
			 * Manter Valor maximo detectado
			 */
			siLine = ( scpuLineValue[uChannel]  );

			/**
			 * Criar gradação intermediaria 
			 */
			if(uContDetection > 1){
				siLine += (1000 / uContDetection);
			}
		};
	}
}

void motor_vTask(void * pvParameters){
	MotorTask  *pxMotorTask = pvParameters;
	
	while (TRUE) {
		app_vMotor(pxMotorTask);
		vTaskDelay(_10MS);
	}
}


void app_vMotor(MotorTask *pxMotorTask){
	pxMotorTask->xPid.fSetPoint = ( PWM_MAX / ( pxMotorTask->iLine ) );
	if( pxMotorTask->xPid.fSetPoint < PID_SETPOINT_MIN ){
		pxMotorTask->xPid.fSetPoint = PID_SETPOINT_MIN;
	}

	pxMotorTask->xPid.fInput = pxMotorTask->fPwm;

	#ifdef MC_TASK
		pxMotorTask->fPwm += pid_fRun(&pxMotorTask->xPid);
	#endif

	#ifdef MC_ISR
		pxMotorTask->fPwm += pid_fRunFromISR(&pxMotorTask->xPid);
	#endif


	if(pxMotorTask->fPwm  > 1000)	pxMotorTask->fPwm = 1000;
	if(pxMotorTask->fPwm  < 0)	pxMotorTask->fPwm = 0;
	motor_vToFront((u16)pxMotorTask->fPwm, &pxMotorTask->xMotor);
}