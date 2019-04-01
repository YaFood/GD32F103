#ifndef __DRV_SYSTICK_H__
#define __DRV_SYSTICK_H__

extern S32 DRV_SYSTICK_Init(VOID);
extern VOID DRV_SYSTICK_Delay(IN U32 ms);
extern U64 DRV_SYSTICK_GetTimeMs(VOID);

#define APP_Delay DRV_SYSTICK_Delay
#define APP_TimeMs DRV_SYSTICK_GetTimeMs

#endif

