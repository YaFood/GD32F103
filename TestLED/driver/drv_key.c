#include "os_public.h"
#include "drv_key.h"

VOID DRV_KEY_Init(VOID)
{
    GPIO_InitPara GPIO_InitStructure;
    RCC_APB2PeriphClock_Enable(RCC_APB2PERIPH_GPIOB, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_PIN_14;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_IN_FLOATING;
    GPIO_Init(GPIOB,&GPIO_InitStructure);
}
