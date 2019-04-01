#include "os_public.h"
#include "drv_iwdg.h"

VOID DRV_IWDG_Init(VOID)
{
    /* Enable write access to IWDG_PSR and IWDG_RLDR registers */
    IWDG_Write_Enable(IWDG_WRITEACCESS_ENABLE);

    /* IWDG counter clock: 40KHz(LSI) / 64 = 0.625 KHz */
    IWDG_SetPrescaler(IWDG_PRESCALER_16);

    /* Set counter reload value to 625 */
    IWDG_SetReloadValue(0x0fff);

    /* Reload IWDG counter */
    IWDG_ReloadCounter();

    /* Enable IWDG (the LSI oscillator will be enabled by hardware) */
    IWDG_Enable();
}
