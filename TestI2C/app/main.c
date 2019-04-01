#include "app.h"
#include "app_i2c.h"

int main()
{
    (VOID)DRV_SYSTICK_Init();
    APP_DEBUG_Init();
    APP_I2C_Test();
}
