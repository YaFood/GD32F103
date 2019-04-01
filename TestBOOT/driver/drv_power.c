#include "os_public.h"
#include "drv_debug.h"
#include "drv_power.h"

#define POWER_FIRSTFLAG_REG BKP_DR1
#define POWER_FIRSTFLAG_VALUE 0xA5A5

VOID DRV_POWER_DumpBootReason(VOID)
{
    if (RCC_GetBitState(RCC_FLAG_IWDGRST) != RESET)
    {
        DRV_DEBUG("Independent Watchdog reset occurred.");
    }
    if (RCC_GetBitState(RCC_FLAG_WWDGRST) != RESET)
    {
        DRV_DEBUG("Window Watchdog reset occurred.");
    }
    if (RCC_GetBitState(RCC_FLAG_LPRRST) != RESET)
    {
        DRV_DEBUG("Low Power reset occurred.");
    }
    if (RCC_GetBitState(RCC_FLAG_EPRST) != RESET)
    {
        DRV_DEBUG("External Pin reset reset occurred.");
    }
    if (RCC_GetBitState(RCC_FLAG_POPDRST) != RESET)
    {
        DRV_DEBUG("Power On/Off reset occurred.");
    }
    if (RCC_GetBitState(RCC_FLAG_SWRRST) != RESET)
    {
        DRV_DEBUG("Software reset occurred.");
    }
}

/* TRUE 第一次启动 */
BOOL DRV_POWER_IsFirstBoot(VOID)
{
    if (POWER_FIRSTFLAG_VALUE != BKP_ReadBackupRegister(POWER_FIRSTFLAG_REG))
    {
        RCC_APB1PeriphClock_Enable(RCC_APB1PERIPH_PWR | RCC_APB1PERIPH_BKP, ENABLE);
        PWR_BackupAccess_Enable(ENABLE);
        BKP_DeInit();
        BKP_WriteBackupRegister(POWER_FIRSTFLAG_REG, POWER_FIRSTFLAG_VALUE);
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

