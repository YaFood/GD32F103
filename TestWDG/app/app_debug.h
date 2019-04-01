#ifndef __APP_DEBUG_H__
#define __APP_DEBUG_H__

#include "drv_uart1.h"

#define	APP_ERROR(fmt, ...) do {printf("[ERROR][%s,%d]: " fmt "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__);} while(0)
#define	APP_TRACE(fmt, ...) do {printf("[TRACE][%s,%d]: " fmt "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__);} while(0)
#define	APP_DEBUG(fmt, ...) do {printf("[DEBUG][%s,%d]: " fmt "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__);} while(0)

extern VOID APP_DEBUG_Init(VOID);

#endif

