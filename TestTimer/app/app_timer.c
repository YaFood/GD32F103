#include "app.h"
#include "drv_led.h"
#include "drv_timer.h"
#include "app_timer.h"

VOID APP_TIMER_Test(VOID)
{
    DRV_LED_Init();
    //DRV_TIMER_Timer2Init();
    DRV_TIMER_Timer2PwmInit();
    while(1);
}
