#include "os_public.h"
#include "drv_i2c.h"

#define I2C_SDA GPIOB,GPIO_PIN_7
#define I2C_SCL GPIOB,GPIO_PIN_6
#define I2C_Set1(i2c) GPIO_SetBits(i2c);I2C_Delay(5);
#define I2C_Set0(i2c) GPIO_ResetBits(i2c);I2C_Delay(5);
#define I2C_Get(i2c) GPIO_ReadInputBit(i2c);

static VOID I2C_Delay(IN U32 n)
{
    U8 i;

    while(n--)
    {
        /* 1us */
        for(i = 0; i < 15; i++)
        {
            __asm("NOP");
        }
    }
}

static VOID I2C_SetOutput(IN GPIO_TypeDef* GPIOx, IN uint16_t GPIO_Pin)
{
    GPIO_InitPara GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_OUT_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_50MHZ;
    GPIO_Init(GPIOx, &GPIO_InitStructure);
}

static VOID I2C_SetInput(IN GPIO_TypeDef* GPIOx, IN uint16_t GPIO_Pin)
{
    GPIO_InitPara GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_IN_FLOATING;
    GPIO_Init(GPIOx, &GPIO_InitStructure);
}


VOID DRV_I2C_Start(VOID)
{
    I2C_SetOutput(I2C_SDA);
    I2C_Set1(I2C_SDA);
    I2C_Set1(I2C_SCL);
    I2C_Set0(I2C_SDA);
    I2C_Set0(I2C_SCL);
}

VOID DRV_I2C_Stop(VOID)
{
    I2C_SetOutput(I2C_SDA);
    I2C_Set0(I2C_SDA);
    I2C_Set1(I2C_SCL);
    I2C_Set1(I2C_SDA);
}


U32 DRV_I2C_WriteByte(IN U8 data)
{
    U8 i = 0;
    U8 byte = data;
    U8 sda = 0;
    
    I2C_SetOutput(I2C_SDA);
        
    for (i = 0; i < 8; i++)
    {
        I2C_Set0(I2C_SCL);
        if (byte & 0x80)
        {
            I2C_Set1(I2C_SDA);
        }
        else
        {
            I2C_Set0(I2C_SDA);
        }
        I2C_Set1(I2C_SCL);
        byte <<= 1;
    }
    
    I2C_Set0(I2C_SCL);
    I2C_SetInput(I2C_SDA);
    I2C_Set1(I2C_SCL);

    sda = I2C_Get(I2C_SDA);
    if (sda)
    {
        I2C_Set0(I2C_SCL);
        I2C_SetOutput(I2C_SDA);
        return OS_ERROR;
    }
    
    I2C_Set0(I2C_SCL);
    I2C_SetOutput(I2C_SDA);
    I2C_Set1(I2C_SDA);
    
    return OS_OK;
}

U32 DRV_I2C_ReadByte(OUT U8 *byte)
{
    U8 i = 0;
    U8 bit = 0;
    U8 sda = 0;

    I2C_SetInput(I2C_SDA);
    
    for (i = 0; i < 8; i++)
    {
        I2C_Set1(I2C_SCL);
        sda = I2C_Get(I2C_SDA);
        if (sda)
        {
            bit |= 0x1;
        }
        I2C_Set0(I2C_SCL);
        if (i != 7)
        {
            bit <<= 1;
        }
    }
    *byte = bit;
    return OS_OK;
}

VOID DRV_I2C_NoAck(VOID)
{
    I2C_Set0(I2C_SCL);
    I2C_SetOutput(I2C_SDA);
    I2C_Set1(I2C_SDA);
    I2C_Set1(I2C_SCL);
    I2C_Set0(I2C_SCL);
}

VOID DRV_I2C_Ack(VOID)
{
    I2C_Set0(I2C_SCL);
    I2C_SetOutput(I2C_SDA);
    I2C_Set0(I2C_SDA);
    I2C_Set1(I2C_SCL);
    I2C_Set0(I2C_SCL);
}

VOID DRV_I2C_Init(VOID)
{
    GPIO_InitPara GPIO_InitStructure;

    RCC_APB2PeriphClock_Enable(RCC_APB2PERIPH_GPIOB,ENABLE);

    GPIO_InitStructure.GPIO_Pin =  GPIO_PIN_6 | GPIO_PIN_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_OUT_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_50MHZ;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_SetBits(GPIOB, GPIO_PIN_6);
    GPIO_SetBits(GPIOB, GPIO_PIN_7);
}

