#include "app.h"

int main()
{
    (VOID)DRV_SYSTICK_Init();

    #if 0
    DRV_UART1_PollInit();
    DRV_UART1_PollTest();
    #endif

    #if 0
    DRV_UART1_InterruptInit();
    DRV_UART1_InterruptTest();
    #endif

    DRV_UART1_DmaInit();
    DRV_UART1_DmaTest();
}
