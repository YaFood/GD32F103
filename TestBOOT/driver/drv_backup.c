#include "os_public.h"
#include "drv_backup.h"


VOID DRV_BACKUP_Init(VOID)
{
    RCC_APB1PeriphClock_Enable(RCC_APB1PERIPH_PWR | RCC_APB1PERIPH_BKP, ENABLE);
    PWR_BackupAccess_Enable(ENABLE);
    BKP_DeInit();
}

