#include "app.h"
#include "app_timer.h"

int main()
{
    (VOID)DRV_SYSTICK_Init();
    APP_DEBUG_Init();
    APP_TIMER_Test();
}
