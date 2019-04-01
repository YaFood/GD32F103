#ifndef __DRV_LED_H__
#define __DRV_LED_H__

#define DRV_LED1 GPIOC,GPIO_PIN_0
#define DRV_LED2 GPIOC,GPIO_PIN_2
#define DRV_LED3 GPIOE,GPIO_PIN_0
#define DRV_LED4 GPIOE,GPIO_PIN_1

#define DRV_LED_On(led) GPIO_SetBits(led);
#define DRV_LED_Off(led) GPIO_ResetBits(led);

extern void DRV_LED_Init(void);

#endif

