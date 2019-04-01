#include "app.h"
#include "app_led.h"
#include "app_key.h"

int main()
{
    (VOID)DRV_SYSTICK_Init();
    APP_DEBUG_Init();
    APP_KEY_Init();
    APP_LED_Init();
    
    while(1)
    {
        APP_KEY_Loop();
        APP_LED_Loop();
    }
}
