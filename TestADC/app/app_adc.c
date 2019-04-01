#include "app.h"
#include "drv_adc.h"
#include "app_adc.h"

VOID APP_ADC_Test(VOID)
{
    U16 adc = 0;
    
    DRV_ADC_Init();
    APP_Delay(50);
    while (1)
    {
        adc = DRV_ADC_GetConversionValue();
        APP_DEBUG("adc=%4.3f", (10000 * adc/4096 * 3.299) / 10000);
        APP_Delay(2000); 
    }
}

