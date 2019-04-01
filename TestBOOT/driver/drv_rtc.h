#ifndef __DRV_RTC_H__
#define __DRV_RTC_H__

#define time DRV_RTC_GetTime

extern time_t DRV_RTC_GetTime(time_t *timer);
extern VOID DRV_RTC_Init(IN BOOL firstFlag);
extern VOID DRV_RTC_TimeAdjust(IN U32 sec);

#endif

