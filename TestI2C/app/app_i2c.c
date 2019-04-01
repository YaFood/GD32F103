#include "app.h"
#include "drv_at24c02c.h"
#include "app_i2c.h"

#define I2C_AT24C02C_ADDR 0xA0

static VOID I2C_Dump(IN U8 *buf, IN U8 len)
{
    U8 i = 0;
    for (i = 0; i < len; i++)
    {
        printf("0x%x ", buf[i]);
    }
    printf("\r\n");
}

VOID APP_I2C_Test(VOID)
{
    U8 len = 255;
    U8 databufIn[5] = {0};
    U8 databufOut[255] = {0};
    U8 dataOut = 0;
    U8 byteAddr = 0x00;

    DRV_AT24C02C_Init();
    
    dataOut = 0;
    DRV_AT24C02C_ReadByte(I2C_AT24C02C_ADDR, byteAddr, &dataOut);
    APP_DEBUG("read=0x%x", dataOut);
    DRV_AT24C02C_WriteByte(I2C_AT24C02C_ADDR, byteAddr, 0x11);
    APP_Delay(10);
    DRV_AT24C02C_ReadByte(I2C_AT24C02C_ADDR, byteAddr, &dataOut);
    APP_DEBUG("read=0x%x", dataOut);
    
    DRV_AT24C02C_WriteByte(I2C_AT24C02C_ADDR, byteAddr, 0x2);
    APP_Delay(10);
    DRV_AT24C02C_ReadByte(I2C_AT24C02C_ADDR, byteAddr, &dataOut);
    APP_DEBUG("read=0x%x", dataOut);
    
    DRV_AT24C02C_WriteByte(I2C_AT24C02C_ADDR, byteAddr, 0xff);
    APP_Delay(10);
    DRV_AT24C02C_ReadByte(I2C_AT24C02C_ADDR, byteAddr, &dataOut);
    APP_DEBUG("read=0x%x", dataOut);

    DRV_AT24C02C_ReadPage(I2C_AT24C02C_ADDR, byteAddr, databufOut, len);
    I2C_Dump(databufOut, len);
    DRV_AT24C02C_WritePage(I2C_AT24C02C_ADDR, byteAddr, databufIn, 5);
    APP_Delay(10);
    DRV_AT24C02C_ReadPage(I2C_AT24C02C_ADDR, byteAddr, databufOut, len);
    I2C_Dump(databufOut, len);
    
    while(1);
}
