#include "app.h"
#include "app_boot.h"

int main()
{
    (VOID)DRV_SYSTICK_Init();
    APP_DEBUG_Init();
    
    APP_BOOT_Test();
}
