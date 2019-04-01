#include "app.h"
#include "drv_power.h"
#include "drv_backup.h"
#include "drv_rtc.h"
#include "app_rtc.h"

static time_t RTC_SetTime(VOID)
{
    U32 year = 0xFF; 
    U32 mon = 0xFF; 
    U32 day = 0xFF;
    U32 hour = 0xFF; 
    U32 min = 0xFF; 
    U32 sec = 0xFF;
    struct tm t;

    memset(&t, 0, sizeof(t));
    printf("Please Set Time:\r\n");
    printf("Please input year:\r\n");
    scanf("%u", &year);
    printf("year:%u\r\n", year);
    
    printf("Please input mon:\r\n");
    scanf("%u", &mon);
    printf("mon:%u\r\n", mon);
    
    printf("Please input day:\r\n");
    scanf("%u", &day);
    printf("day:%u\r\n", day);
    
    printf("Please input hour:\r\n");
    scanf("%u", &hour);
    printf("hour:%u\r\n", hour);
    
    printf("Please input min:\r\n");
    scanf("%u", &min);
    printf("min:%u\r\n", min);
    
    printf("Please input sec:\r\n");
    scanf("%u", &sec);
    printf("sec:%u\r\n", sec);
    
    t.tm_sec = sec;
    t.tm_min = min;
    t.tm_hour = hour;
    t.tm_mday = day;
    t.tm_mon = mon-1; /* 0-11 */
    t.tm_year = year-1900; /* 从1900年开始开始计算的 */

    APP_DEBUG("%u-%u-%u %u:%u:%u", t.tm_year, t.tm_mon, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);
    return(mktime(&t));
}

VOID APP_RTC_Test(VOID)
{
    BOOL firstBootFlag = FALSE; 
    time_t now = 0;

    DRV_POWER_DumpBootReason();
    
    firstBootFlag = DRV_POWER_IsFirstBoot();
    DRV_RTC_Init(firstBootFlag);
    if (TRUE == firstBootFlag)
    {
        DRV_RTC_TimeAdjust(RTC_SetTime());
    }
    
    while(1)
    {
        APP_Delay(1000);
        now = time(NULL);
        printf("%s", ctime(&now));
    }
}
