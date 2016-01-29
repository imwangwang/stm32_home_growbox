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
#include "tm_stm32f4_adc.h"

/* Internal functions */
void TM_ADC_INT_Channel_0_Init(ADC_TypeDef* ADCx);
void TM_ADC_INT_Channel_1_Init(ADC_TypeDef* ADCx);
void TM_ADC_INT_Channel_2_Init(ADC_TypeDef* ADCx);
void TM_ADC_INT_Channel_3_Init(ADC_TypeDef* ADCx);
void TM_ADC_INT_Channel_4_Init(ADC_TypeDef* ADCx);
void TM_ADC_INT_Channel_5_Init(ADC_TypeDef* ADCx);
void TM_ADC_INT_Channel_6_Init(ADC_TypeDef* ADCx);
void TM_ADC_INT_Channel_7_Init(ADC_TypeDef* ADCx);
void TM_ADC_INT_Channel_8_Init(ADC_TypeDef* ADCx);
void TM_ADC_INT_Channel_9_Init(ADC_TypeDef* ADCx);
void TM_ADC_INT_Channel_10_Init(ADC_TypeDef* ADCx);
void TM_ADC_INT_Channel_11_Init(ADC_TypeDef* ADCx);
void TM_ADC_INT_Channel_12_Init(ADC_TypeDef* ADCx);
void TM_ADC_INT_Channel_13_Init(ADC_TypeDef* ADCx);
void TM_ADC_INT_Channel_14_Init(ADC_TypeDef* ADCx);
void TM_ADC_INT_Channel_15_Init(ADC_TypeDef* ADCx);
void TM_ADC_INT_InitPin(GPIO_TypeDef* GPIOx, uint16_t PinX);

void TM_ADC_Init(ADC_TypeDef* ADCx, uint8_t channel) {
	if (channel == ADC_Channel_0) {
		TM_ADC_INT_Channel_0_Init(ADCx);
	} else if (channel == ADC_Channel_1) {
		TM_ADC_INT_Channel_1_Init(ADCx);
	} else if (channel == ADC_Channel_2) {
		TM_ADC_INT_Channel_2_Init(ADCx);
	} else if (channel == ADC_Channel_3) {
		TM_ADC_INT_Channel_3_Init(ADCx);
	} else if (channel == ADC_Channel_4) {
		TM_ADC_INT_Channel_4_Init(ADCx);
	} else if (channel == ADC_Channel_5) {
		TM_ADC_INT_Channel_5_Init(ADCx);
	} else if (channel == ADC_Channel_6) {
		TM_ADC_INT_Channel_6_Init(ADCx);
	} else if (channel == ADC_Channel_7) {
		TM_ADC_INT_Channel_7_Init(ADCx);
	} else if (channel == ADC_Channel_8) {
		TM_ADC_INT_Channel_8_Init(ADCx);
	} else if (channel == ADC_Channel_9) {
		TM_ADC_INT_Channel_9_Init(ADCx);
	} else if (channel == ADC_Channel_10) {
		TM_ADC_INT_Channel_10_Init(ADCx);
	} else if (channel == ADC_Channel_11) {
		TM_ADC_INT_Channel_11_Init(ADCx);
	} else if (channel == ADC_Channel_12) {
		TM_ADC_INT_Channel_12_Init(ADCx);
	} else if (channel == ADC_Channel_13) {
		TM_ADC_INT_Channel_13_Init(ADCx);
	} else if (channel == ADC_Channel_14) {
		TM_ADC_INT_Channel_14_Init(ADCx);
	} else if (channel == ADC_Channel_15) {
		TM_ADC_INT_Channel_15_Init(ADCx);
	}
	
	/* Init ADC */
	TM_ADC_InitADC(ADCx, channel);
}

void TM_ADC_InitADC(ADC_TypeDef* ADCx, uint8_t channel) {
	ADC_InitTypeDef ADC_InitStructure;

	/* Init ADC settings */
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 1;

	
	/* Enable clock and fill resolution settings */
	if (ADCx == ADC1) {
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
		ADC_RegularChannelConfig(ADC1, channel, 1, ADC_SampleTime_13Cycles5);
	} else if (ADCx == ADC2) {
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE);
		ADC_RegularChannelConfig(ADC2, channel, 1, ADC_SampleTime_13Cycles5);
	} else if (ADCx == ADC3) {
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, ENABLE);
		ADC_RegularChannelConfig(ADC3, channel, 1, ADC_SampleTime_13Cycles5);
	}

	/* Initialize ADC */
	ADC_Init(ADCx, &ADC_InitStructure);	
	/* Enable ADC */
	ADC_Cmd(ADCx, ENABLE);
	/* Enable ADC1 reset calibaration register */   
	ADC_ResetCalibration(ADCx);
	/* Check the end of ADC1 reset calibration register */
	while(ADC_GetResetCalibrationStatus(ADCx));
	/* Start ADC1 calibaration */
	ADC_StartCalibration(ADCx);
	/* Check the end of ADC1 calibration */
	while(ADC_GetCalibrationStatus(ADCx));

}

uint16_t TM_ADC_Read(ADC_TypeDef* ADCx, uint8_t channel) {
	ADC_RegularChannelConfig(ADCx, channel, 1, ADC_SampleTime_13Cycles5);
	/* Start ADC1 Software Conversion */ 
	ADC_SoftwareStartConvCmd(ADCx, ENABLE);
	
	/* Wait till done */
	while (ADC_GetFlagStatus(ADCx, ADC_FLAG_EOC) == RESET);
	
	/* Return result */
	return ADC_GetConversionValue(ADCx);
}

void TM_ADC_EnableVbat(void) {
	/* TBD */
}

void TM_ADC_DisableVbat(void) {
	/* TBD */
}

uint16_t TM_ADC_ReadVbat(ADC_TypeDef* ADCx) {
	uint32_t result;
	/* TBD */	
	/* Return value in mV */
	return (uint16_t) result;
}

/* Private functions */
void TM_ADC_INT_Channel_0_Init(ADC_TypeDef* ADCx) {
	TM_ADC_INT_InitPin(GPIOA, GPIO_Pin_0);
}
void TM_ADC_INT_Channel_1_Init(ADC_TypeDef* ADCx) {
	TM_ADC_INT_InitPin(GPIOA, GPIO_Pin_1);
}
void TM_ADC_INT_Channel_2_Init(ADC_TypeDef* ADCx) {
	TM_ADC_INT_InitPin(GPIOA, GPIO_Pin_2);
}
void TM_ADC_INT_Channel_3_Init(ADC_TypeDef* ADCx) {
	TM_ADC_INT_InitPin(GPIOA, GPIO_Pin_3);
}
void TM_ADC_INT_Channel_4_Init(ADC_TypeDef* ADCx) {
	if (ADCx == ADC1 || ADCx == ADC2) {
		TM_ADC_INT_InitPin(GPIOA, GPIO_Pin_4);
	} else if (ADCx == ADC3) {
		TM_ADC_INT_InitPin(GPIOF, GPIO_Pin_6);
	}
}
void TM_ADC_INT_Channel_5_Init(ADC_TypeDef* ADCx) {
	if (ADCx == ADC1 || ADCx == ADC2) {
		TM_ADC_INT_InitPin(GPIOA, GPIO_Pin_5);
	} else if (ADCx == ADC3) {
		TM_ADC_INT_InitPin(GPIOF, GPIO_Pin_7);
	}
}
void TM_ADC_INT_Channel_6_Init(ADC_TypeDef* ADCx) {
	if (ADCx == ADC1 || ADCx == ADC2) {
		TM_ADC_INT_InitPin(GPIOA, GPIO_Pin_6);
	} else if (ADCx == ADC3) {
		TM_ADC_INT_InitPin(GPIOF, GPIO_Pin_8);
	}
}
void TM_ADC_INT_Channel_7_Init(ADC_TypeDef* ADCx) {
	if (ADCx == ADC1 || ADCx == ADC2) {
		TM_ADC_INT_InitPin(GPIOA, GPIO_Pin_7);
	} else if (ADCx == ADC3) {
		TM_ADC_INT_InitPin(GPIOF, GPIO_Pin_9);
	}
}
void TM_ADC_INT_Channel_8_Init(ADC_TypeDef* ADCx) {
	if (ADCx == ADC1 || ADCx == ADC2) {
		TM_ADC_INT_InitPin(GPIOB, GPIO_Pin_0);
	} else if (ADCx == ADC3) {
		TM_ADC_INT_InitPin(GPIOF, GPIO_Pin_10);
	}
}
void TM_ADC_INT_Channel_9_Init(ADC_TypeDef* ADCx) {
	if (ADCx == ADC1 || ADCx == ADC2) {
		TM_ADC_INT_InitPin(GPIOB, GPIO_Pin_1);
	} else if (ADCx == ADC3) {
		TM_ADC_INT_InitPin(GPIOF, GPIO_Pin_3);
	}
}
void TM_ADC_INT_Channel_10_Init(ADC_TypeDef* ADCx) {
	TM_ADC_INT_InitPin(GPIOC, GPIO_Pin_0);
}
void TM_ADC_INT_Channel_11_Init(ADC_TypeDef* ADCx) {
	TM_ADC_INT_InitPin(GPIOC, GPIO_Pin_1);
}
void TM_ADC_INT_Channel_12_Init(ADC_TypeDef* ADCx) {
	TM_ADC_INT_InitPin(GPIOC, GPIO_Pin_2);
}
void TM_ADC_INT_Channel_13_Init(ADC_TypeDef* ADCx) {
	TM_ADC_INT_InitPin(GPIOC, GPIO_Pin_3);
}
void TM_ADC_INT_Channel_14_Init(ADC_TypeDef* ADCx) {
	if (ADCx == ADC1 || ADCx == ADC2) {
		TM_ADC_INT_InitPin(GPIOC, GPIO_Pin_3);
	} else if (ADCx == ADC3) {
		TM_ADC_INT_InitPin(GPIOF, GPIO_Pin_4);
	}
}
void TM_ADC_INT_Channel_15_Init(ADC_TypeDef* ADCx) {
	if (ADCx == ADC1 || ADCx == ADC2) {
		TM_ADC_INT_InitPin(GPIOC, GPIO_Pin_5);
	} else if (ADCx == ADC3) {
		TM_ADC_INT_InitPin(GPIOF, GPIO_Pin_5);
	}
}

void TM_ADC_INT_InitPin(GPIO_TypeDef* GPIOx, uint16_t PinX) {
	/* Enable GPIO */
	TM_GPIO_Init(GPIOx, PinX, GPIO_Mode_AIN, GPIO_Speed_2MHz);
}
