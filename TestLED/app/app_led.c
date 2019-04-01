#include "app.h"
#include "drv_led.h"
#include "drv_key.h"
#include "app_led.h"

static VOID LED_Init(VOID)
{
    DRV_LED_Init();
    DRV_KEY_Init();
}

static VOID LED_SetLedStatus(IN U8 status)
{
    if (DRV_KEY_DOWN == status)
    {
        DRV_LED_On(DRV_LED1);
        DRV_LED_On(DRV_LED2);
        DRV_LED_On(DRV_LED3);
        DRV_LED_On(DRV_LED4);
    }
    else if (DRV_KEY_UP == status)
    {
        DRV_LED_Off(DRV_LED1);
        DRV_LED_Off(DRV_LED2);
        DRV_LED_Off(DRV_LED3);
        DRV_LED_Off(DRV_LED4);
    }
    else
    {
        ;
    }
}

static VOID LED_CheckKeyStatus(VOID)
{
    U8 keyStatus = 0;
    
    keyStatus = DRV_KEY_GetStatus(DRV_KEY3);
    if (DRV_KEY_DOWN == keyStatus)
    {
        APP_Delay(50); /* 50ms»•∂∂∂Ø */
        keyStatus = DRV_KEY_GetStatus(DRV_KEY3);
        if (DRV_KEY_DOWN == keyStatus)
        {
            LED_SetLedStatus(DRV_KEY_DOWN);
        }
    }
    else
    {
        LED_SetLedStatus(DRV_KEY_UP);
    }
}

VOID APP_LED_Test(VOID)
{
    LED_Init();
    LED_SetLedStatus(DRV_KEY_UP);
    
    while (1)
    {
        LED_CheckKeyStatus();
    }
}

