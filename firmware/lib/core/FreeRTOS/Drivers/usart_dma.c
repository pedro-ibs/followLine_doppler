/**
 * usart_dma.c
 *
 *  @date Created at:	16/03/2021 11:28:05
 *	@author:	Pedro Igor B. S.
 *	@email:		pibscontato@gmail.com
 * 	GitHub:		https://github.com/pedro-ibs
 * 	tabSize:	8
 *
 * ########################################################
 * TODO: Licence
 * ########################################################
 *
 * TODO: documentation or resume or Abstract
 *
 */



/* Includes ------------------------------------------------------------------*/
/* stm includes */
#include <core.h>
#include <stm32f1xx_hal_uart.h>
#include <stm32f1xx_hal_usart.h>
#include <stm32f1xx_hal_dma.h>

/* FreeRTOS standard includes.  */
#include <FreeRTOS/include/FreeRTOSConfig.h>
#include <FreeRTOS/include/FreeRTOS.h>
#include <FreeRTOS/include/task.h>
#include <FreeRTOS/include/list.h>
#include <FreeRTOS/include/queue.h>
#include <FreeRTOS/include/semphr.h>
#include <FreeRTOS/include/portable.h>
#include <FreeRTOS/Drivers/usart.h>


#if(USART_USE == USART_DMA)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
typedef struct {
	UART_HandleTypeDef		xHandle;
	DMA_HandleTypeDef		xDMA_RXD;
	DMA_HandleTypeDef		xDMA_TXD;
	// DMA_Channel_TypeDef		*pxCH_TXD;
	// DMA_Channel_TypeDef		*pxCH_RXD;
	// char				pcRXD[SIZE_BUFFER_TXD];
	// char 				pcTXD[SIZE_BUFFER_RXD];	
} xUsartHandleData;


typedef struct {
	DMA_Channel_TypeDef		*pxCH_TXD;
	DMA_Channel_TypeDef		*pxCH_RXD;
	IRQn_Type			xIRQ_CH_TXD;
	IRQn_Type			xIRQ_CH_RXD;
	IRQn_Type			xIRQ_USART;
}xUsartSetting;

static xUsartHandleData xUsartDMA[ttyNUM] = {
	{ {0}, {0}, {0} },
	{ {0}, {0}, {0} },
	{ {0}, {0}, {0} }
 };

static const xUsartSetting xUsartSet[ttyNUM] = {
	{DMA1_Channel4, DMA1_Channel5, DMA1_Channel4_IRQn, DMA1_Channel5_IRQn, USART1_IRQn},
	{DMA1_Channel7, DMA1_Channel6, DMA1_Channel7_IRQn, DMA1_Channel6_IRQn, USART2_IRQn},
	{DMA1_Channel2, DMA1_Channel3, DMA1_Channel2_IRQn, DMA1_Channel3_IRQn, USART3_IRQn}
};

/* Private Functions ---------------------------------------------------------*/
void usart_vInitDMA(const TTY xtty);



/**
 * @brief inicia a uart e variaveis use gpio_vInitAll() antes.
 * @param tty, enumeração ttyUSART1, ttyUSART2 ttyUSART3, caso
 * xtty seja invalido a função não será executada
 * @param BaudRate, velocidade da porta serial.
 */
void usart_vSetup(const TTY xtty, const BaudRate cuBaudRate){
	usart_vInit(&xUsartDMA[xtty].xHandle, xtty, cuBaudRate);
	usart_vInitDMA(xtty);
}


/**
 * @brief calcula o tamanho ocupado do buffer RXD.
 * @param tty, enumeração ttyUSART1, ttyUSART2 ttyUSART3
 * @return Caso xtty não seja um valor valudo o  valor
 * returnado será -1, do contrario o valor returnado
 * será o tamanho ocupado do buffer RXD .
 */
int usart_iSizeBuffer(const TTY xtty){
	//TODO: func
	return 0;
}


/**
 * @brief apaga os dados armazenados no buffer.
 * @param tty, enumeração ttyUSART1, ttyUSART2 ttyUSART3, caso
 * xtty seja invalido a função não será executada.
 */
void usart_vCleanBuffer(const TTY xtty){
	//TODO: func
}


/**
 * @brief pegar o buffer RXD da usart.
 * @param tty, enumeração ttyUSART1, ttyUSART2 ttyUSART3, caso
 * xtty seja invalido a função não será executada.
 * @return CCHR: retorna o ponteiro do tipo CCHR (const char *)
 * do buffer pcRXD. caso xtty seja invalido será retornado NULL
 */
CCHR *usart_pcGetBuffer(TTY xtty){
	//TODO: func
	return NULL;
}


/**
 * @brief envia um caractere. Essa função utilizam o periferio uart junto
 * da interrupção USART_FLAG_TXE. Esse envio ocorre por por meio de uma queue,
 * onde por por meio dessa função a queue é populada enquanto a interrupção
 * envia os dado.
 * @param tty, enumeração ttyUSART1, ttyUSART2 ttyUSART3, caso
 * xtty seja invalido a função não será executada
 * @param ccChr, caractere tipo const char
 */
void usart_vSendChr(const TTY xtty, CCHR ccChr){
	while ( HAL_UART_GetState(&xUsartDMA[xtty].xHandle) != HAL_UART_STATE_READY);
	HAL_UART_Transmit_DMA(&xUsartDMA[xtty].xHandle, (u8*)&ccChr, 1);
};


/**
 * @brief envia uma cadeia de caracteres. Essa função utilizam o periferio uart
 * junto da interrupção USART_FLAG_TXE. Esse envio ocorre por por meio de uma
 * queue, onde por por meio dessa função a queue é populada enquanto a interrupção
 * envia os dado.
 * @param tty, enumeração ttyUSART1, ttyUSART2 ttyUSART3, caso
 * xtty seja invalido a função não será executada
 * @param pcBuffer, buffer a ser enviado 
 * @param cuSize tamanho exato de pcBuffer
 */
void usart_vSendBlk(const TTY xtty, CCHR *pcBuffer, const size_t cuSize){
	while ( HAL_UART_GetState(&xUsartDMA[xtty].xHandle) != HAL_UART_STATE_READY);
	HAL_UART_Transmit_DMA(&xUsartDMA[xtty].xHandle, (u8*)pcBuffer, cuSize);
}


/**
 * @brief envia uma cadeia de caracteres, ao final do envio de pcBuffer
 * a função irá enviar "\r\n". Essa função utilizam o periferio uart junto
 * da interrupção USART_FLAG_TXE. Esse envio ocorre por por meio de uma queue,
 * onde por por meio dessa função a queue é populada enquanto a interrupção
 * envia os dado.
 * @param tty, enumeração ttyUSART1, ttyUSART2 ttyUSART3, caso
 * xtty seja invalido a função não será executada
 * @param pcBuffer, buffer a ser enviado 
 * @param cuSize tamanho exato de pcBuffer
 */
void usart_vSendBlkLn(const TTY xtty, CCHR *pcBuffer, const size_t cuSize){
	usart_vSendBlk(xtty, pcBuffer, cuSize);
	usart_vSendBlk(xtty, "\r\n", 2);
}


/**
 * @brief envia uma string. Essa função utilizam o periferio uart junto
 * da interrupção USART_FLAG_TXE. Esse envio ocorre por por meio de uma queue,
 * que por meio dessa é populada enquanto a interrupção envia os dado.
 * @param tty, enumeração ttyUSART1, ttyUSART2 ttyUSART3, caso
 * xtty seja invalido a função não será executada
 * @param pcString, string a ser enviada 
 */
void usart_vSendStr(const TTY xtty, CCHR *pcString){
	usart_vSendBlk(xtty, pcString, strlen(pcString));
}

/**
 * @brief envia uma string, ao final do envio de pcString
 * a função irá enviar "\r\n". Essa função utilizam o periferio uart junto
 * da interrupção USART_FLAG_TXE. Esse envio ocorre por por meio de uma queue,
 * que por meio dessa é populada enquanto a interrupção envia os dado.
 * @param tty, enumeração ttyUSART1, ttyUSART2 ttyUSART3, caso
 * xtty seja invalido a função não será executada
 * @param pcString, string a ser enviada 
 */
void usart_vSendStrLn(const TTY xtty, CCHR *pcString){
	usart_vSendBlkLn(xtty, pcString, strlen(pcString));
}





/*########################################################################################################################################################*/
/*########################################################################################################################################################*/
/*########################################################################################################################################################*/
/*-------------------------------------------------------------------- Private Functions -----------------------------------------------------------------*/
/*########################################################################################################################################################*/

void usart_vInitDMA(const TTY xtty) {
	
	__HAL_RCC_DMA1_CLK_ENABLE();
	
	/* USARTX_RX Init */
	xUsartDMA[xtty].xDMA_RXD.Instance			= xUsartSet[xtty].pxCH_RXD;
	xUsartDMA[xtty].xDMA_RXD.Init.Direction			= DMA_PERIPH_TO_MEMORY;
	xUsartDMA[xtty].xDMA_RXD.Init.PeriphInc			= DMA_PINC_DISABLE;
	xUsartDMA[xtty].xDMA_RXD.Init.MemInc			= DMA_MINC_ENABLE;
	xUsartDMA[xtty].xDMA_RXD.Init.PeriphDataAlignment	= DMA_PDATAALIGN_BYTE;
	xUsartDMA[xtty].xDMA_RXD.Init.MemDataAlignment		= DMA_MDATAALIGN_BYTE;
	xUsartDMA[xtty].xDMA_RXD.Init.Mode			= DMA_CIRCULAR;
	xUsartDMA[xtty].xDMA_RXD.Init.Priority			= DMA_PRIORITY_HIGH;
	if (HAL_DMA_Init(&xUsartDMA[xtty].xDMA_RXD) != HAL_OK) {
		Error_Handler();
	}
	__HAL_LINKDMA(&xUsartDMA[xtty].xHandle, hdmarx, xUsartDMA[xtty].xDMA_RXD);


	/* USART1_TX Init */
	xUsartDMA[xtty].xDMA_TXD.Instance			= xUsartSet[xtty].pxCH_TXD;
	xUsartDMA[xtty].xDMA_TXD.Init.Direction			= DMA_MEMORY_TO_PERIPH;
	xUsartDMA[xtty].xDMA_TXD.Init.PeriphInc			= DMA_PINC_DISABLE;
	xUsartDMA[xtty].xDMA_TXD.Init.MemInc			= DMA_MINC_ENABLE;
	xUsartDMA[xtty].xDMA_TXD.Init.PeriphDataAlignment	= DMA_PDATAALIGN_BYTE;
	xUsartDMA[xtty].xDMA_TXD.Init.MemDataAlignment		= DMA_MDATAALIGN_BYTE;
	xUsartDMA[xtty].xDMA_TXD.Init.Mode			= DMA_NORMAL;
	xUsartDMA[xtty].xDMA_TXD.Init.Priority			= DMA_PRIORITY_MEDIUM;
	if (HAL_DMA_Init(&xUsartDMA[xtty].xDMA_TXD) != HAL_OK) {
		Error_Handler();
	}
	__HAL_LINKDMA(&xUsartDMA[xtty].xHandle, hdmatx, xUsartDMA[xtty].xDMA_TXD);


	// HAL_UART_DMA_Tx_Stop()

	HAL_NVIC_SetPriority(DMA1_Channel4_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Channel4_IRQn);
	/* DMA1_Channel5_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(DMA1_Channel5_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Channel5_IRQn);

}



/** ################################################################################################ **/
/** ######################################### Interrupções ######################################### **/
/** ################################################################################################ **/

void DMA1_Channel4_IRQHandler(void) {
	HAL_DMA_IRQHandler(&xUsartDMA[ttyUSART1].xDMA_RXD);
}

void DMA1_Channel5_IRQHandler(void) {
	HAL_DMA_IRQHandler(&xUsartDMA[ttyUSART1].xDMA_TXD);
	HAL_UART_DMAStop(&xUsartDMA[ttyUSART1].xHandle);
}

void DMA1_Channel6_IRQHandler(void) {
	HAL_DMA_IRQHandler(&xUsartDMA[ttyUSART2].xDMA_RXD);
}

void DMA1_Channel7_IRQHandler(void) {
	HAL_DMA_IRQHandler(&xUsartDMA[ttyUSART2].xDMA_TXD);
}

void DMA1_Channel2_IRQHandler(void) {
	HAL_DMA_IRQHandler(&xUsartDMA[ttyUSART3].xDMA_TXD);
}

void DMA1_Channel3_IRQHandler(void) {
	HAL_DMA_IRQHandler(&xUsartDMA[ttyUSART3].xDMA_RXD);
}

#endif /* #if(USART_USE == USART_DMA) */