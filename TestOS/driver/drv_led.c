#include "os_public.h"
#include "drv_led.h"

void LED_Init(void)
{
    GPIO_InitPara GPIO_InitStructure;
    RCC_APB2PeriphClock_Enable(RCC_APB2PERIPH_GPIOC |RCC_APB2PERIPH_GPIOE|RCC_APB2PERIPH_AF,ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_PIN_0 | GPIO_PIN_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_OUT_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_50MHZ;
    GPIO_Init(GPIOC,&GPIO_InitStructure);
}

int LED_Light(IN U8 Led, IN BOOL OnOrOff)
{
    if (LED2 == Led)
    {
        if (LED_ON == OnOrOff)
        {
            GPIO_SetBits(GPIOC,GPIO_PIN_0);   
            
        }
        else if (LED_ON == OnOrOff)
        {
            GPIO_ResetBits(GPIOC,GPIO_PIN_0);   
        }
        else
        {
            return OS_ERROR;
        }
        
    }
    else if (LED3 == Led)
    {
        if (OnOrOff == LED_ON)
        {
            GPIO_SetBits(GPIOC,GPIO_PIN_2);   
            
        }
        else
        {
            GPIO_ResetBits(GPIOC,GPIO_PIN_2);   
        }

    }
    else 
    {
        return OS_ERROR;
    }
    
    return OS_OK;
}


