#include "os_public.h"
#include "drv_adc.h"

static VOID ADC_GpioConfig(VOID)
{
    GPIO_InitPara GPIO_InitStructure;
  
    GPIO_InitStructure.GPIO_Pin = GPIO_PIN_3;
    GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_50MHZ;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_AIN;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
}

static VOID ADC_AdcConfig(VOID)
{
    ADC_InitPara ADC_InitStructure;
    
    ADC_InitStructure.ADC_Mode = ADC_MODE_INDEPENDENT;
    ADC_InitStructure.ADC_Mode_Scan = DISABLE;
    ADC_InitStructure.ADC_Mode_Continuous = ENABLE;
    ADC_InitStructure.ADC_Trig_External = ADC_EXTERNAL_TRIGGER_MODE_NONE;
    ADC_InitStructure.ADC_Data_Align = ADC_DATAALIGN_RIGHT;
    ADC_InitStructure.ADC_Channel_Number = 1 ;
    ADC_Init(ADC1, &ADC_InitStructure);
  
    ADC_RegularChannel_Config(ADC1, ADC_CHANNEL_13, 1, ADC_SAMPLETIME_71POINT5);
    ADC_Enable(ADC1, ENABLE);
    ADC_Calibration(ADC1);
    ADC_SoftwareStartConv_Enable(ADC1, ENABLE);
}

VOID DRV_ADC_Init(VOID)
{
    RCC_APB2PeriphClock_Enable(RCC_APB2PERIPH_GPIOC , ENABLE);
    RCC_APB2PeriphClock_Enable(RCC_APB2PERIPH_ADC1, ENABLE);
    RCC_ADCCLKConfig(RCC_ADCCLK_APB2_DIV12); 
    
    ADC_GpioConfig();
    ADC_AdcConfig();
}

U16 DRV_ADC_GetConversionValue(VOID)
{
    return ADC_GetConversionValue(ADC1);
}
