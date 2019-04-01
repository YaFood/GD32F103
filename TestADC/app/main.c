#include "app.h"
#include "app_adc.h"

int main()
{
    (VOID)DRV_SYSTICK_Init();
    APP_DEBUG_Init();
    APP_ADC_Test();
}
