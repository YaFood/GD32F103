#include "app.h"
#include "app_led.h"

int main()
{
    (VOID)DRV_SYSTICK_Init();
    APP_DEBUG_Init();
    
    APP_LED_Test();
}
