/**	
 * |----------------------------------------------------------------------
 * | Copyright (C) Tilen Majerle, 2014
 * | 
 * | This program is free software: you can redistribute it and/or modify
 * | it under the terms of the GNU General Public License as published by
 * | the Free Software Foundation, either version 3 of the License, or
 * | any later version.
 * |  
 * | This program is distributed in the hope that it will be useful,
 * | but WITHOUT ANY WARRANTY; without even the implied warranty of
 * | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * | GNU General Public License for more details.
 * | 
 * | You should have received a copy of the GNU General Public License
 * | along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * |----------------------------------------------------------------------
 */
#include "tm_stm32f4_usart.h"

/**
 * Internal USART struct
 */
typedef struct {
	uint8_t *Buffer;
	uint16_t Size;
	uint16_t Num;
	uint16_t In;
	uint16_t Out;
	uint8_t Initialized;
} TM_USART_t;

/* Set variables for buffers */
#ifdef TM_USE_USART1
uint8_t TM_USART1_Buffer[TM_USART1_BUFFER_SIZE];
#endif
#ifdef TM_USE_USART2
uint8_t TM_USART2_Buffer[TM_USART2_BUFFER_SIZE];
#endif
#ifdef TM_USE_USART3
uint8_t TM_USART3_Buffer[TM_USART3_BUFFER_SIZE];
#endif
#ifdef TM_USE_UART4
uint8_t TM_UART4_Buffer[TM_UART4_BUFFER_SIZE];
#endif
#ifdef TM_USE_UART5
uint8_t TM_UART5_Buffer[TM_UART5_BUFFER_SIZE];
#endif

#ifdef TM_USE_USART1
TM_USART_t TM_USART1 = {TM_USART1_Buffer, TM_USART1_BUFFER_SIZE, 0, 0, 0, 0};
#endif
#ifdef TM_USE_USART2
TM_USART_t TM_USART2 = {TM_USART2_Buffer, TM_USART2_BUFFER_SIZE, 0, 0, 0, 0};
#endif
#ifdef TM_USE_USART3
TM_USART_t TM_USART3 = {TM_USART3_Buffer, TM_USART3_BUFFER_SIZE, 0, 0, 0, 0};
#endif
#ifdef TM_USE_UART4
TM_USART_t TM_UART4 = {TM_UART4_Buffer, TM_UART4_BUFFER_SIZE, 0, 0, 0, 0};
#endif
#ifdef TM_USE_UART5
TM_USART_t TM_UART5 = {TM_UART5_Buffer, TM_UART5_BUFFER_SIZE, 0, 0, 0, 0};
#endif

void TM_USART1_InitPins(TM_USART_PinsPack_t pinspack);
void TM_USART2_InitPins(TM_USART_PinsPack_t pinspack);
void TM_USART3_InitPins(TM_USART_PinsPack_t pinspack);
void TM_UART4_InitPins(TM_USART_PinsPack_t pinspack);
void TM_UART5_InitPins(TM_USART_PinsPack_t pinspack);
void TM_USART_INT_InsertToBuffer(TM_USART_t* u, uint8_t c);
TM_USART_t* TM_USART_INT_GetUsart(USART_TypeDef* USARTx);
uint8_t TM_USART_INT_GetSubPriority(USART_TypeDef* USARTx);
uint8_t TM_USART_BufferFull(USART_TypeDef* USARTx);

void TM_USART_Init(USART_TypeDef* USARTx, TM_USART_PinsPack_t pinspack, uint32_t baudrate) {
	TM_USART_t* u = TM_USART_INT_GetUsart(USARTx);
	
	/**
	 * Initialization structures declared
	 *
	 * USART: Universal Synchronous/Asynchronous Receiver/Transmitter
	 * NVIC: Nested Vector Interrupt Controller
	 */
	USART_InitTypeDef 	USART_InitStruct;
	NVIC_InitTypeDef	NVIC_InitStruct;
	
	/**
	 * Default settings:
	 * 
	 * Set Baudrate to value you pass to function
	 * Disable Hardware Flow control
	 * Set Mode To TX and RX, so USART will work in full-duplex mode
	 * Disable parity bit
	 * Set 1 stop bit
	 * Set Data bits to 8
	 */
	USART_InitStruct.USART_BaudRate = baudrate;
	
	/*
	 * Initialize USARTx pins
	 * Set channel for USARTx NVIC
	 */
#ifdef TM_USE_USART1
	if (USARTx == USART1) {
		TM_USART1_InitPins(pinspack);
		NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
		
		USART_InitStruct.USART_HardwareFlowControl = TM_USART1_HARDWARE_FLOW_CONTROL;
		USART_InitStruct.USART_Mode = TM_USART1_MODE;
		USART_InitStruct.USART_Parity = TM_USART1_PARITY;
		USART_InitStruct.USART_StopBits = TM_USART1_STOP_BITS;
		USART_InitStruct.USART_WordLength = TM_USART1_WORD_LENGTH;
	}
#endif
#ifdef TM_USE_USART2
	if (USARTx == USART2) {
		TM_USART2_InitPins(pinspack);
		NVIC_InitStruct.NVIC_IRQChannel = USART2_IRQn;
		
		USART_InitStruct.USART_HardwareFlowControl = TM_USART2_HARDWARE_FLOW_CONTROL;
		USART_InitStruct.USART_Mode = TM_USART2_MODE;
		USART_InitStruct.USART_Parity = TM_USART2_PARITY;
		USART_InitStruct.USART_StopBits = TM_USART2_STOP_BITS;
		USART_InitStruct.USART_WordLength = TM_USART2_WORD_LENGTH;
	}
#endif
#ifdef TM_USE_USART3
	if (USARTx == USART3) {
		TM_USART3_InitPins(pinspack);
		NVIC_InitStruct.NVIC_IRQChannel = USART3_IRQn;
		
		USART_InitStruct.USART_HardwareFlowControl = TM_USART3_HARDWARE_FLOW_CONTROL;
		USART_InitStruct.USART_Mode = TM_USART3_MODE;
		USART_InitStruct.USART_Parity = TM_USART3_PARITY;
		USART_InitStruct.USART_StopBits = TM_USART3_STOP_BITS;
		USART_InitStruct.USART_WordLength = TM_USART3_WORD_LENGTH;
	}
#endif
#ifdef TM_USE_UART4
	if (USARTx == UART4) {
		TM_UART4_InitPins(pinspack);
		NVIC_InitStruct.NVIC_IRQChannel = UART4_IRQn;
		
		USART_InitStruct.USART_HardwareFlowControl = TM_UART4_HARDWARE_FLOW_CONTROL;
		USART_InitStruct.USART_Mode = TM_UART4_MODE;
		USART_InitStruct.USART_Parity = TM_UART4_PARITY;
		USART_InitStruct.USART_StopBits = TM_UART4_STOP_BITS;
		USART_InitStruct.USART_WordLength = TM_UART4_WORD_LENGTH;
	}
#endif
#ifdef TM_USE_UART5
	if (USARTx == UART5) {
		TM_UART5_InitPins(pinspack);
		NVIC_InitStruct.NVIC_IRQChannel = UART5_IRQn;
		
		USART_InitStruct.USART_HardwareFlowControl = TM_UART5_HARDWARE_FLOW_CONTROL;
		USART_InitStruct.USART_Mode = TM_UART5_MODE;
		USART_InitStruct.USART_Parity = TM_UART5_PARITY;
		USART_InitStruct.USART_StopBits = TM_UART5_STOP_BITS;
		USART_InitStruct.USART_WordLength = TM_UART5_WORD_LENGTH;
	}
#endif

	/* We are initialized */
	u->Initialized = 1;
	
	/* Disable */
	USART_Cmd(USARTx, DISABLE);
	/* Deinit first */
	USART_DeInit(USARTx);
	
	/* Init */
	USART_Init(USARTx, &USART_InitStruct);
	/* Enable */
	USART_Cmd(USARTx, ENABLE);
	
	/* Enable RX interrupt */
	USART_ITConfig(USARTx, USART_IT_RXNE, ENABLE);

	/**
	 * Set Channel Cmd to enable. That will enable USARTx channel in NVIC
	 * Set NVIC_IRQChannelPreemptionPriority. This means high priority for USARTs
	 * USARTx with lower x has high subpriority
	 *
	 * Initialize NVIC
	 */
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = TM_USART_NVIC_PRIORITY;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = TM_USART_INT_GetSubPriority(USARTx);
	NVIC_Init(&NVIC_InitStruct);
}

uint8_t TM_USART_Getc(USART_TypeDef* USARTx) {
	int8_t c = 0;
	TM_USART_t* u = TM_USART_INT_GetUsart(USARTx);
	
	/* Check if we have any data in buffer */
	if (u->Num > 0) {
		if (u->Out == u->Size) {
			u->Out = 0;
		}
		c = *(u->Buffer + u->Out);
		u->Out++;
		u->Num--;
	}
	return c;
}

uint16_t TM_USART_Gets(USART_TypeDef* USARTx, char* buffer, uint16_t bufsize) {
	uint16_t i = 0;
	
	/* Check for any data on USART */
	if (TM_USART_BufferEmpty(USARTx) || (!TM_USART_FindCharacter(USARTx, '\n') && !TM_USART_BufferFull(USARTx))) {
		return 0;
	}
	
	/* If available buffer size is more than 0 characters */
	while (i < (bufsize - 1)) {
		/* We have available data */
		buffer[i] = (char) TM_USART_Getc(USARTx);
		/* Check for end of string */
		if (buffer[i] == '\n') {
			i++;
			/* Done */
			break;
		} else {
			i++;
		}
	}
	
	/* Add zero to the end of string */
	buffer[i] = 0;               

	return (i);
}

uint8_t TM_USART_BufferEmpty(USART_TypeDef* USARTx) {
	TM_USART_t* u = TM_USART_INT_GetUsart(USARTx);
	return (u->Num == 0);
}

uint8_t TM_USART_BufferFull(USART_TypeDef* USARTx) {
	TM_USART_t* u = TM_USART_INT_GetUsart(USARTx);
	return (u->Num == u->Size);
}

void TM_USART_ClearBuffer(USART_TypeDef* USARTx) {
	TM_USART_t* u = TM_USART_INT_GetUsart(USARTx);
	
	u->Num = 0;
	u->In = 0;
	u->Out = 0;
}

uint8_t TM_USART_FindCharacter(USART_TypeDef* USARTx, uint8_t c) {
	uint16_t num, out;
	TM_USART_t* u = TM_USART_INT_GetUsart(USARTx);
	
	/* Temp variables */
	num = u->Num;
	out = u->Out;
	
	while (num > 0) {
		/* Check overflow */
		if (out == u->Size) {
			out = 0;
		}
		if (*(u->Buffer + out) == (uint8_t)c) {
			/* Character found */
			return 1;
		}
		out++;
		num--;
	}
	
	/* Character is not in buffer */
	return 0;
}

void TM_USART_Puts(USART_TypeDef* USARTx, char* str) {
	TM_USART_t* u = TM_USART_INT_GetUsart(USARTx);
	/* If we are not initialized */
	if (u->Initialized == 0) {
		return;
	}
	
	while (*str) {
		/* Send char by char */
		TM_USART_Putc(USARTx, *str++);
	}
}

void TM_USART_Putc(USART_TypeDef* USARTx, volatile char c) {
	TM_USART_t* u = TM_USART_INT_GetUsart(USARTx);
	/* If we are not initialized */
	if (u->Initialized == 0) {
		return;
	}
	
	/* Wait to be ready, buffer empty */
	while (!(USARTx->SR & USART_FLAG_TXE));
	/* Send data */
	USARTx->DR = (uint16_t)(c & 0x01FF);
}

/* Private functions */
void TM_USART_INT_InsertToBuffer(TM_USART_t* u, uint8_t c) {
	/* Still available space in buffer */
	if (u->Num < u->Size) {
		/* Check overflow */
		if (u->In == u->Size) {
			u->In = 0;
		}
		/* Add to buffer */
		u->Buffer[u->In] = c;
		u->In++;
		u->Num++;
	}
}

TM_USART_t* TM_USART_INT_GetUsart(USART_TypeDef* USARTx) {
	TM_USART_t* u;
	
#ifdef TM_USE_USART1
	if (USARTx == USART1) {
		u = &TM_USART1;
	}
#endif
#ifdef TM_USE_USART2
	if (USARTx == USART2) {
		u = &TM_USART2;
	}
#endif
#ifdef TM_USE_USART3
	if (USARTx == USART3) {
		u = &TM_USART3;
	}
#endif
#ifdef TM_USE_UART4
	if (USARTx == UART4) {
		u = &TM_UART4;
	}
#endif
#ifdef TM_USE_UART5
	if (USARTx == UART5) {
		u = &TM_UART5;
	}
#endif

	return u;
}

uint8_t TM_USART_INT_GetSubPriority(USART_TypeDef* USARTx) {
	uint8_t u;
	
#ifdef TM_USE_USART1
	if (USARTx == USART1) {
		u = 0;
	}
#endif
#ifdef TM_USE_USART2
	if (USARTx == USART2) {
		u = 1;
	}
#endif
#ifdef TM_USE_USART3
	if (USARTx == USART3) {
		u = 2;
	}
#endif
#ifdef TM_USE_UART4
	if (USARTx == UART4) {
		u = 4;
	}
#endif
#ifdef TM_USE_UART5
	if (USARTx == UART5) {
		u = 5;
	}
#endif
	
	return u;
}

#ifdef TM_USE_USART1
void TM_USART1_InitPins(TM_USART_PinsPack_t pinspack) {	
	/* Enable clock for USART1 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	/* Init pins */
	if (pinspack == TM_USART_PinsPack_1) {
		TM_GPIO_Init(GPIOA, GPIO_Pin_9, GPIO_Mode_AF_PP, GPIO_Speed_50MHz);
		TM_GPIO_Init(GPIOA, GPIO_Pin_10, GPIO_Mode_IN_FLOATING, GPIO_Speed_50MHz);
	} else if (pinspack == TM_USART_PinsPack_2) {
		TM_GPIO_Init(GPIOB, GPIO_Pin_6 | GPIO_Pin_7, GPIO_Mode_AF_PP, GPIO_Speed_50MHz);
	}
}
#endif

#ifdef TM_USE_USART2
void TM_USART2_InitPins(TM_USART_PinsPack_t pinspack) {
	/* Enable clock for USART2 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	/* Init pins */
	if (pinspack == TM_USART_PinsPack_1) {
		TM_GPIO_Init(GPIOA, GPIO_Pin_2 | GPIO_Pin_3, GPIO_Mode_AF_PP, GPIO_Speed_50MHz);
	} else if (pinspack == TM_USART_PinsPack_2) {
		TM_GPIO_Init(GPIOD, GPIO_Pin_5 | GPIO_Pin_6, GPIO_Mode_AF_PP, GPIO_Speed_50MHz);
	}
}
#endif

#ifdef TM_USE_USART3
void TM_USART3_InitPins(TM_USART_PinsPack_t pinspack) {
	/* Enable clock for USART3 */
	/* TBD */
}
#endif

#ifdef TM_USE_UART4
void TM_UART4_InitPins(TM_USART_PinsPack_t pinspack) {
	/* TBD */
}
#endif

#ifdef TM_USE_UART5
void TM_UART5_InitPins(TM_USART_PinsPack_t pinspack) {
	/* TBD */
}
#endif


#ifdef TM_USE_USART1
void USART1_IRQHandler(void) {
	//Check if interrupt was because data is received
	if (USART_GetITStatus(USART1, USART_IT_RXNE)) {
		#ifdef TM_USART1_USE_CUSTOM_IRQ
			//Call user function
			TM_USART1_ReceiveHandler(USART1->DR);
		#else
			//Put received data into internal buffer
			TM_USART_INT_InsertToBuffer(&TM_USART1, USART1->DR);
		#endif
	}
}
#endif

#ifdef TM_USE_USART2
void USART2_IRQHandler(void) {
	//Check if interrupt was because data is received
	if (USART_GetITStatus(USART2, USART_IT_RXNE)) {
		#ifdef TM_USART2_USE_CUSTOM_IRQ
			//Call user function
			TM_USART2_ReceiveHandler(USART2->DR);
		#else 
			//Put received data into internal buffer
			TM_USART_INT_InsertToBuffer(&TM_USART2, USART2->DR);
		#endif
	}
}
#endif

#ifdef TM_USE_USART3
void USART3_IRQHandler(void) {
	/* TBD */
}
#endif

#ifdef TM_USE_UART4
void UART4_IRQHandler(void) {
	/* TBD */}
#endif

#ifdef TM_USE_UART5
void UART5_IRQHandler(void) {
	/* TBD */
}
#endif

