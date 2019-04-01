#include "os_public.h"
#include "drv_debug.h"
#include "drv_backup.h"
#include "drv_rtc.h"

#define DRV_RTC_FIRSTFLAG_REG BKP_DR1
#define DRV_RTC_FIRSTFLAG_VALUE 0xA5A5

time_t DRV_RTC_GetTime(time_t *timer)
{
    U32 c = 0;
    c = RTC_GetCounter();
    if (timer != NULL)
    {
        *timer = c;
    }

    return c;
}

VOID DRV_RTC_TimeAdjust(IN U32 sec)
{
    RTC_WaitLWOFF();
    RTC_SetCounter(sec);
    RTC_WaitLWOFF();
}

VOID RTC_IRQHandler(VOID)
{
    if (RTC_GetIntBitState(RTC_INT_SF) != RESET)
    {
        /* Clear the RTC Second interrupt */
        RTC_ClearIntBitState(RTC_INT_SF);
    }
}

static RTC_FirstInit(VOID)
{
    DRV_TRACE("First power on need to configure RTC.");
    
    /* 选择晶振LSE，低速外部晶振，即32.768Khz */
    RCC_RTCCLKConfig(RCC_RTCCLKSOURCE_LSE);
    RCC_LSEConfig(RCC_LSE_EN);
    while (RCC_GetBitState(RCC_FLAG_LSESTB) == RESET)
    {
    }
    RCC_RTCCLK_Enable(ENABLE);
    RTC_WaitRSF();
    RTC_WaitLWOFF();

    /* 分频，即1Hz */
    RTC_SetPrescaler(32768-1); /* 1s */
    RTC_WaitLWOFF();
}

static VOID RTC_NotFristInit(VOID)
{
    DRV_TRACE("Just need wait clock synchronized.");
    RTC_WaitRSF();
}

VOID DRV_RTC_Init(IN BOOL firstFlag)
{
    /* 区分是否首次上电 */
    if (firstFlag != FALSE)
    {
        RTC_FirstInit();
    }
    else
    {
        RTC_NotFristInit();
    }
}

