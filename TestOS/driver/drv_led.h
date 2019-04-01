#ifndef __DRV_LED_H__
#define __DRV_LED_H__

#define LED2 2
#define LED3 3

#define LED_ON TRUE
#define LED_OFF FALSE

extern void LED_Init(void);
extern int LED_Light(IN U8 Led, IN BOOL OnOrOff);


#endif

