#include "os_public.h"
#include "drv_systick.h"

static U64 gDrvSystickCount = 0;
static U32 gDrvSystickDelayCount = 0;

S32 DRV_SYSTICK_Init(VOID)
{
    /* 1000Hz,1ms中断一次 */
    if (SysTick_Config(SystemCoreClock / 1000))
    {
        return OS_ERROR;
    }
    
    NVIC_SetPriority(SysTick_IRQn, 0x00);

    return OS_OK;
}

/* 1ms中断一次 */
VOID SysTick_Handler(VOID)
{
    gDrvSystickCount++;
    if (gDrvSystickDelayCount > 0)
    {
        gDrvSystickDelayCount--;
    }
}

VOID DRV_SYSTICK_Delay(IN U32 ms)
{
    gDrvSystickDelayCount = ms;
    while (1)
    {
        if (gDrvSystickDelayCount <= 0)
        {
            break;
        }
    }

    return;
}

U64 DRV_SYSTICK_GetTimeMs(VOID)
{
    return gDrvSystickCount;
}
