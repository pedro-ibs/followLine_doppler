/**
 * sysHandler.h
 *
 *  @date Created at:	22/02/2021 12:40:09
 *	@author:	Pedro Igor B. S.
 *	@email:		pibscontato@gmail.com
 * 	GitHub:		https://github.com/pedro-ibs
 * 	tabSize:	8
 *
 * ########################################################
 * opensource.org/licenses/BSD-3-Clause
 * ########################################################
 *
 * Cortex-M3 Processor Interruption and Exception Handlers 
 * denerated form stmCubeMx
 *
 */

/* Includes ------------------------------------------------------------------*/

#ifndef sys_handler_H_
#define sys_handler_H_

/* macro ---------------------------------------------------------------------*/
/* Definition ----------------------------------------------------------------*/

#include <stm32f1xx_hal.h>
#include <stm32f1xx.h>

void NMI_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
void DebugMon_Handler(void);
void Error_Handler(void);

#ifdef  USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line);
#endif /* USE_FULL_ASSERT */

#endif /* sys_handler_H_ */