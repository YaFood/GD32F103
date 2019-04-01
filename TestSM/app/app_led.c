#include "app.h"
#include "drv_led.h"
#include "app_led.h"

#define APP_LED_HALFLIGHT_TIME 1    /* ms */
#define APP_LED_WATERFALL_TIME 1000 /* ms */

typedef enum
{
    LED_SMSTATUS_OFF = 0,
    LED_SMSTATUS_ON,
    LED_SMSTATUS_HALFBRIGHT,
    LED_SMSTATUS_WATERFALL,
    LED_SMSTATUS_BUTT
}LedSmStatus_e;

typedef struct
{
    LedSmStatus_e smStatus;
    LedSmStatus_e currentStatus;
}LedSm_t;

static LedSm_t gLedSm;
static U64 gLightOnMoment = 0;
static U8 gLightCount = 0;

static VOID LED_LightOn(VOID)
{
    if ((LED_SMSTATUS_ON != gLedSm.smStatus) ||
        (LED_SMSTATUS_ON == gLedSm.currentStatus))
    {
        return;
    }

    APP_TRACE("light on.");
    DRV_LED_On(DRV_LED1);
    DRV_LED_On(DRV_LED2);
    DRV_LED_On(DRV_LED3);
    DRV_LED_On(DRV_LED4);
    
    gLedSm.currentStatus = LED_SMSTATUS_ON;
}

static VOID LED_DoHalfBrightInit(VOID)
{
    gLightOnMoment = APP_TimeMs();
    gLightCount = 0;
}

static VOID LED_DoHalfBright(VOID)
{
    U64 time = 0;

    time = APP_TimeMs();
    if (time > gLightOnMoment + APP_LED_HALFLIGHT_TIME)
    {
        gLightOnMoment = APP_TimeMs();
        gLightCount++;
    }
    else
    {
        return;
    }
    
    if (1 == gLightCount)
    {
        DRV_LED_On(DRV_LED1);
        DRV_LED_On(DRV_LED2);
        DRV_LED_On(DRV_LED3);
        DRV_LED_On(DRV_LED4);
    }
    else
    {
        DRV_LED_Off(DRV_LED1);
        DRV_LED_Off(DRV_LED2);
        DRV_LED_Off(DRV_LED3);
        DRV_LED_Off(DRV_LED4);
    }

    /* µ÷½ÚÁÁ¶È */
    if (3 == gLightCount)
    {
        gLightCount = 0;
    }
}

static VOID LED_HalfBright(VOID)
{
    if (LED_SMSTATUS_HALFBRIGHT != gLedSm.smStatus)
    {
        return;
    }

    APP_TRACE("light half.");
    LED_DoHalfBright();
    gLedSm.currentStatus = LED_SMSTATUS_HALFBRIGHT;
}

static VOID LED_DoWaterfallBrightInit(VOID)
{
    gLightOnMoment = APP_TimeMs();
    gLightCount = 0;
}

static VOID LED_DoWaterfallBright(VOID)
{
    U64 time = 0;

    time = APP_TimeMs();
    if (time > gLightOnMoment + APP_LED_WATERFALL_TIME)
    {
        gLightOnMoment = APP_TimeMs();
        gLightCount++;
    }
    else
    {
        return;
    }

    if (1 == gLightCount)
    {
        DRV_LED_On(DRV_LED1);
        DRV_LED_Off(DRV_LED2);
        DRV_LED_Off(DRV_LED3);
        DRV_LED_Off(DRV_LED4);
    }
    
    if (2 == gLightCount)
    {
        DRV_LED_Off(DRV_LED1);
        DRV_LED_On(DRV_LED2);
        DRV_LED_Off(DRV_LED3);
        DRV_LED_Off(DRV_LED4);
    }
    
    if (3 == gLightCount)
    {
        DRV_LED_Off(DRV_LED1);
        DRV_LED_Off(DRV_LED2);
        DRV_LED_On(DRV_LED3);
        DRV_LED_Off(DRV_LED4);
    }
    
    if (4 == gLightCount)
    {
        DRV_LED_Off(DRV_LED1);
        DRV_LED_Off(DRV_LED2);
        DRV_LED_Off(DRV_LED3);
        DRV_LED_On(DRV_LED4);
        gLightCount = 0;
    }
}

static VOID LED_WaterfallBright(VOID)
{
    if (LED_SMSTATUS_WATERFALL != gLedSm.smStatus)
    {
        return;
    }

    APP_TRACE("light waterfall.");
    LED_DoWaterfallBright();
    gLedSm.currentStatus = LED_SMSTATUS_WATERFALL;
}

static VOID LED_LightOff(VOID)
{
    if ((LED_SMSTATUS_OFF != gLedSm.smStatus) ||
        (LED_SMSTATUS_OFF == gLedSm.currentStatus))
    {
        return;
    }

    APP_TRACE("light off.");
    DRV_LED_Off(DRV_LED1);
    DRV_LED_Off(DRV_LED2);
    DRV_LED_Off(DRV_LED3);
    DRV_LED_Off(DRV_LED4);
    
    gLedSm.currentStatus = LED_SMSTATUS_OFF;
}

VOID APP_LED_Init(VOID)
{    
    memset(&gLedSm, 0, sizeof(gLedSm));
    gLedSm.smStatus = LED_SMSTATUS_OFF;
    
    DRV_LED_Init();
}

VOID APP_LED_SmSwitch4LongPress(VOID)
{
    if (LED_SMSTATUS_OFF != gLedSm.smStatus)
    {
        gLedSm.smStatus = LED_SMSTATUS_OFF;
    }
}

VOID APP_LED_SmSwitch4ShortPress(VOID)
{
    switch (gLedSm.smStatus)
    {
        case LED_SMSTATUS_OFF:
            gLedSm.smStatus = LED_SMSTATUS_ON;
            break;
        case LED_SMSTATUS_ON:
            LED_DoHalfBrightInit();
            gLedSm.smStatus = LED_SMSTATUS_HALFBRIGHT;
            break;
        case LED_SMSTATUS_HALFBRIGHT:
            LED_DoWaterfallBrightInit();
            gLedSm.smStatus = LED_SMSTATUS_WATERFALL;
            break;
        case LED_SMSTATUS_WATERFALL:
            gLedSm.smStatus = LED_SMSTATUS_ON;
            break;
        default:
            APP_ERROR("error sm status.");
            break;
    }
}

VOID APP_LED_Loop(VOID)
{
    LED_LightOn();
    LED_HalfBright();
    LED_WaterfallBright();
    LED_LightOff();
}
