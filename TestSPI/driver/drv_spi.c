#include "os_public.h"
#include "drv_spi.h"

#if 1 /* 软件模拟 */
#define SPI_SCK_HIGH GPIO_SetBits(GPIOA, GPIO_PIN_5);
#define SPI_SCK_LOW  GPIO_ResetBits(GPIOA, GPIO_PIN_5);
#define SPI_MOSI_HIGH GPIO_SetBits(GPIOA, GPIO_PIN_7);
#define SPI_MOSI_LOW  GPIO_ResetBits(GPIOA, GPIO_PIN_7);
#define SPI_MISO_READ GPIO_ReadInputBit(GPIOA, GPIO_PIN_6);

static VOID SPI_Delay(IN U32 n)
{
    U8 i;

    while(n--)
    {
        /* 1us */
        for(i = 0; i < 50; i++)
        {
            __asm("NOP");
        }
    }
}

U8 DRV_SPI_SwapByte(IN U8 byte)
{
    U8 i = 0;
    U8 inDate = byte;
    U8 outBit = 0;
    U8 outDate = 0;
    
    /* SCKPL = 0; SCKPH = 0 */
    for (i = 0; i < 8; i++)
    {
        if (inDate & 0x80)
        {
            SPI_MOSI_HIGH;
        }
        else
        {
            SPI_MOSI_LOW;
        }
        
        SPI_Delay(5);
        SPI_SCK_HIGH;
        outBit = SPI_MISO_READ;
        if (outBit)
        {
            outDate |= 0x1;
        }
        SPI_Delay(5);
        SPI_SCK_LOW;
        SPI_Delay(5);
        inDate <<= 1;
        if (i <7)
        {
            outDate <<= 1;
        }
    }

    return outDate;
}

VOID DRV_SPI_Init(VOID)
{
    GPIO_InitPara  GPIO_InitStructure;
    
    RCC_APB2PeriphClock_Enable(RCC_APB2PERIPH_GPIOA , ENABLE); 

    GPIO_InitStructure.GPIO_Pin = GPIO_PIN_5 | GPIO_PIN_7; 
    GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_50MHZ;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_OUT_PP;                      
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_PIN_6 ;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    SPI_SCK_LOW; /* SCKPL = 0; SCKPH = 0 */
}

#else /* 硬件方式 */
U8 DRV_SPI_SwapByte(IN U8 byte)
{
    while (SPI_I2S_GetBitState(SPI1, SPI_FLAG_TBE) == RESET);

    SPI_I2S_SendData(SPI1, byte);

    while (SPI_I2S_GetBitState(SPI1, SPI_FLAG_RBNE) == RESET);

    return SPI_I2S_ReceiveData(SPI1);
}

static VOID SPI_Configuration(VOID)
{
    SPI_InitPara  SPI_InitStructure;
    
    RCC_APB2PeriphClock_Enable(RCC_APB2PERIPH_SPI1, ENABLE);  
    
    SPI_InitStructure.SPI_TransType = SPI_TRANSTYPE_FULLDUPLEX;
    SPI_InitStructure.SPI_Mode = SPI_MODE_MASTER;
    SPI_InitStructure.SPI_FrameFormat = SPI_FRAMEFORMAT_8BIT;
    SPI_InitStructure.SPI_SCKPL = SPI_SCKPL_LOW;
    SPI_InitStructure.SPI_SCKPH = SPI_SCKPH_1EDGE;
    SPI_InitStructure.SPI_SWNSSEN = SPI_SWNSS_SOFT;
    SPI_InitStructure.SPI_PSC = SPI_PSC_32; 
    SPI_InitStructure.SPI_FirstBit = SPI_FIRSTBIT_MSB;
    SPI_InitStructure.SPI_CRCPOL = 7;
    SPI_Init(SPI1, &SPI_InitStructure);
    SPI_Enable(SPI1, ENABLE);      
}

static VOID SPI_GpioConfig(VOID)
{
    GPIO_InitPara  GPIO_InitStructure;
    
    RCC_APB2PeriphClock_Enable(RCC_APB2PERIPH_GPIOA , ENABLE); 
    
    GPIO_InitStructure.GPIO_Pin = GPIO_PIN_5 |GPIO_PIN_7; 
    GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_50MHZ;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_AF_PP;                      
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_PIN_6 ;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

VOID DRV_SPI_Init(VOID)
{
    SPI_GpioConfig();
    SPI_Configuration();
}
#endif
