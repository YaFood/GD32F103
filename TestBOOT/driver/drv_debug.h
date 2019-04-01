#ifndef __APP_DEBUG_H__
#define __APP_DEBUG_H__

#include "drv_uart1.h"

#define	DRV_ERROR(fmt, ...) do {printf("[DRV ERROR][%s,%d]: " fmt "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__);} while(0)
#define	DRV_TRACE(fmt, ...) do {printf("[DRV TRACE][%s,%d]: " fmt "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__);} while(0)
#define	DRV_DEBUG(fmt, ...) do {printf("[DRV DEBUG][%s,%d]: " fmt "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__);} while(0)

#endif

