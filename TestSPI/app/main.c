#include "app.h"
#include "app_spi.h"

int main()
{
    (VOID)DRV_SYSTICK_Init();
    APP_DEBUG_Init();
    APP_SPI_Test();
}
