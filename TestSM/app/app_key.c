#include "app.h"
#include "drv_key.h"
#include "app_led.h"
#include "app_key.h"

#define APP_KEY_JITTERTIME 50 /* 50ms */
#define APP_KEY_LONGPRESSTIME 3000 /* 3s */

typedef enum
{
    KEY_SMSTATUS_UP = 0,
    KEY_SMSTATUS_UPING,
    KEY_SMSTATUS_DOWN,
    KEY_SMSTATUS_DOWNING,
    KEY_SMSTATUS_BUTT
}KeySmStatus_e;

typedef struct
{ 
    KeySmStatus_e smStatus;     /* up-->downing-->down-->uping-->up */
    U64 downingMoment;  
    U64 jitterTimeBegin;
}KeySm_t;

static KeySm_t gKeySm;

static VOID KEY_SmStatusUp(VOID)
{
    U8 keyStatus = 0;

    if (KEY_SMSTATUS_UP != gKeySm.smStatus)
    {
        return;
    }
    
    keyStatus = DRV_KEY_GetStatus(DRV_KEY3);
    if (DRV_KEY_DOWN == keyStatus)
    {
        gKeySm.smStatus = KEY_SMSTATUS_DOWNING;
        gKeySm.jitterTimeBegin = APP_TimeMs();
        APP_TRACE("up --> downing");
    }
}

static VOID KEY_SmStatusDowning(VOID)
{
    U64 currentTime = 0;
    U8 keyStatus = 0;

    if (KEY_SMSTATUS_DOWNING != gKeySm.smStatus)
    {
        return;
    }
    
    currentTime = APP_TimeMs();
    if (currentTime < (gKeySm.jitterTimeBegin + APP_KEY_JITTERTIME))
    {
        return;
    }
    
    keyStatus = DRV_KEY_GetStatus(DRV_KEY3);
    if (DRV_KEY_DOWN == keyStatus)
    {
        gKeySm.smStatus = KEY_SMSTATUS_DOWN;
        gKeySm.downingMoment = APP_TimeMs();
        APP_TRACE("downing --> down");
    }
    else if (DRV_KEY_UP == keyStatus)
    {
        gKeySm.smStatus = KEY_SMSTATUS_UP;
        APP_TRACE("downing --> up");
    }
    else
    {
        APP_ERROR("");
    }
}

static VOID KEY_SmStatusDown(VOID)
{
    U8 keyStatus = 0;
    
    if (KEY_SMSTATUS_DOWN != gKeySm.smStatus)
    {
        return;
    }
    
    keyStatus = DRV_KEY_GetStatus(DRV_KEY3);
    if (DRV_KEY_UP == keyStatus)
    {
        gKeySm.smStatus = KEY_SMSTATUS_UPING;
        gKeySm.jitterTimeBegin = APP_TimeMs();
        APP_TRACE("down --> uping");
    }
}

static VOID KEY_SmStatusUping(VOID)
{
    U64 currentTime = 0;
    U8 keyStatus = 0;
    
    if (KEY_SMSTATUS_UPING != gKeySm.smStatus)
    {
        return;
    }

    currentTime = APP_TimeMs();
    if (currentTime < (gKeySm.jitterTimeBegin + APP_KEY_JITTERTIME))
    {
        return;
    }

    keyStatus = DRV_KEY_GetStatus(DRV_KEY3);
    if (DRV_KEY_DOWN == keyStatus)
    {
        gKeySm.smStatus = KEY_SMSTATUS_DOWN;
        APP_TRACE("uping --> down");
    }
    else if (DRV_KEY_UP == keyStatus)
    {
        gKeySm.smStatus = KEY_SMSTATUS_UP;
        currentTime = APP_TimeMs();
        if (currentTime >= (gKeySm.downingMoment + APP_KEY_LONGPRESSTIME))
        {
            APP_DEBUG("long press.");
            APP_LED_SmSwitch4LongPress();
        }
        else
        {
            APP_DEBUG("short press.");
            APP_LED_SmSwitch4ShortPress();
        }
        
        APP_TRACE("uping --> up");
    }
    else
    {
        APP_ERROR("");
    }
}

VOID APP_KEY_Init(VOID)
{
    memset(&gKeySm, 0, sizeof(gKeySm));
    gKeySm.smStatus = KEY_SMSTATUS_UP;
    
    DRV_KEY_Init();
}

VOID APP_KEY_Loop(VOID)
{
    KEY_SmStatusUp();
    KEY_SmStatusDowning();
    KEY_SmStatusDown();
    KEY_SmStatusUping();
}
