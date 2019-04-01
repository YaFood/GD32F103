#ifndef __DRV_KEY_H__
#define __DRV_KEY_H__

#define DRV_KEY3 GPIOB, GPIO_PIN_14
#define DRV_KEY_GetStatus(key) GPIO_ReadInputBit(key)
#define DRV_KEY_DOWN 0
#define DRV_KEY_UP 1

extern VOID DRV_KEY_Init(VOID);

#endif

