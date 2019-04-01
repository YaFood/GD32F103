#include "os_public.h"
#include "drv_i2c.h"
#include "drv_at24c02c.h"

VOID DRV_AT24C02C_WriteByte(IN U8 slaveAddr, IN U8 byteAddr, IN U8 data)
{
    DRV_I2C_Start();
    DRV_I2C_WriteByte(slaveAddr);
    DRV_I2C_WriteByte(byteAddr);
    DRV_I2C_WriteByte(data);
    DRV_I2C_Stop();
}

VOID DRV_AT24C02C_WritePage(IN U8 slaveAddr, IN U8 byteAddr, IN U8 data[], IN U8 len)
{
    U8 i = 0; 
    
    DRV_I2C_Start();
    DRV_I2C_WriteByte(slaveAddr);
    DRV_I2C_WriteByte(byteAddr);
    for (i = 0; i < len; i++)
    {
        DRV_I2C_WriteByte(data[i]);
    }
    DRV_I2C_Stop();
}

VOID DRV_AT24C02C_ReadByte(IN U8 slaveAddr, IN U8 byteAddr, OUT U8 *data)
{
    U8 tmp = 0; 

    DRV_I2C_Start();
    DRV_I2C_WriteByte(slaveAddr);
    DRV_I2C_WriteByte(byteAddr);
    DRV_I2C_Start();
    DRV_I2C_WriteByte(slaveAddr+1);
    DRV_I2C_ReadByte(&tmp);
    DRV_I2C_NoAck();    
    DRV_I2C_Stop(); 
    *data = tmp;
}

VOID DRV_AT24C02C_ReadPage(IN U8 slaveAddr, IN U8 byteAddr, OUT U8 data[], IN U8 len)
{
    U8 tmp = 0; 
    U8 i = 0;

    DRV_I2C_Start();
    DRV_I2C_WriteByte(slaveAddr);
    DRV_I2C_WriteByte(byteAddr);
    DRV_I2C_Start();
    DRV_I2C_WriteByte(slaveAddr+1);
    for (i = 0; i < len-1; i++)
    {
        DRV_I2C_ReadByte(&tmp);
        DRV_I2C_Ack();
        data[i] = tmp;
    }
    
    DRV_I2C_ReadByte(&tmp);
    DRV_I2C_NoAck();    
    data[i] = tmp;
    DRV_I2C_Stop();    
}

VOID DRV_AT24C02C_Init(VOID)
{
    DRV_I2C_Init();
}
