#include "os_public.h"
#include "drv_spi.h"
#include "drv_gd25q40.h"

#define  GD25Q40_PageSize    0x100

#define WRITE      0x02  /* Write to Memory instruction */
#define WRSR       0x01  /* Write Status Register instruction */
#define WREN       0x06  /* Write enable instruction */

#define READ       0x03  /* Read from Memory instruction */
#define RDSR       0x05  /* Read Status Register instruction  */
#define RDID       0x9F  /* Read identification */
#define SE         0x20  /* Sector Erase instruction */
#define BE         0xC7  /* Bulk Erase instruction */        

#define WIP_Flag   0x01  /* Write In Progress (WIP) flag */
#define Dummy_Byte 0xA5

#define  GD25Q40_CS_LOW()    GPIO_ResetBits(GPIOE, GPIO_PIN_3);
#define  GD25Q40_CS_HIGH()   GPIO_SetBits(GPIOE, GPIO_PIN_3);

static VOID GD25Q40_WriteEnable(VOID)
{
    GD25Q40_CS_LOW();
    DRV_SPI_SwapByte(WREN);
    GD25Q40_CS_HIGH();
}

static VOID GD25Q40_WaitForWriteEnd(VOID)
{
    U8 FLASH_Status = 0;

    GD25Q40_CS_LOW();
    DRV_SPI_SwapByte(RDSR);

    do
    {
        FLASH_Status = DRV_SPI_SwapByte(Dummy_Byte);
    }
    while ((FLASH_Status & WIP_Flag) == SET); /* Write in progress */

    GD25Q40_CS_HIGH();
}

VOID DRV_GD25Q40_SectorErase(U32 SectorAddr)
{
    GD25Q40_WriteEnable();

    GD25Q40_CS_LOW();
    DRV_SPI_SwapByte(SE);
    DRV_SPI_SwapByte((SectorAddr & 0xFF0000) >> 16);
    DRV_SPI_SwapByte((SectorAddr & 0xFF00) >> 8);
    DRV_SPI_SwapByte(SectorAddr & 0xFF);
    GD25Q40_CS_HIGH();

    GD25Q40_WaitForWriteEnd();
}

VOID DRV_GD25Q40_BulkErase(VOID)
{
    GD25Q40_WriteEnable();

    GD25Q40_CS_LOW();
    DRV_SPI_SwapByte(BE);
    GD25Q40_CS_HIGH();

    GD25Q40_WaitForWriteEnd();
}

VOID DRV_GD25Q40_BufferRead(U8* pBuffer, U32 ReadAddr, U16 NumByteToRead)
{
    GD25Q40_CS_LOW();
    DRV_SPI_SwapByte(READ);
    DRV_SPI_SwapByte((ReadAddr & 0xFF0000) >> 16);
    DRV_SPI_SwapByte((ReadAddr& 0xFF00) >> 8);
    DRV_SPI_SwapByte(ReadAddr & 0xFF);
    while (NumByteToRead--) 
    {
        *pBuffer = DRV_SPI_SwapByte(Dummy_Byte);
        pBuffer++;
    }
    GD25Q40_CS_HIGH();
}

VOID DRV_GD25Q40_PageWrite(U8* pBuffer, U32 WriteAddr, U16 NumByteToWrite)
{
    GD25Q40_WriteEnable();

    GD25Q40_CS_LOW();
    DRV_SPI_SwapByte(WRITE);
    DRV_SPI_SwapByte((WriteAddr & 0xFF0000) >> 16);
    DRV_SPI_SwapByte((WriteAddr & 0xFF00) >> 8);
    DRV_SPI_SwapByte(WriteAddr & 0xFF);

    while (NumByteToWrite--)
    {
        DRV_SPI_SwapByte(*pBuffer);
        pBuffer++;
    }
    GD25Q40_CS_HIGH();

    GD25Q40_WaitForWriteEnd();
}

VOID DRV_GD25Q40_BufferWrite(U8 *pBuffer, U32 WriteAddr, U16 NumByteToWrite)
{
    U8 NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;

    Addr = WriteAddr % GD25Q40_PageSize;
    count = GD25Q40_PageSize - Addr;
    NumOfPage =  NumByteToWrite / GD25Q40_PageSize;
    NumOfSingle = NumByteToWrite % GD25Q40_PageSize;
     /* WriteAddr is GD25Q40_PageSize aligned  */
    if (Addr == 0)
    {   
        /* NumByteToWrite < GD25Q40_PageSize */
        if (NumOfPage == 0) 
        {
            DRV_GD25Q40_PageWrite(pBuffer, WriteAddr, NumByteToWrite);
        }
        else /* NumByteToWrite > GD25Q40_PageSize */
        {
            while (NumOfPage--)
            {
                DRV_GD25Q40_PageWrite(pBuffer, WriteAddr, GD25Q40_PageSize);
                WriteAddr +=  GD25Q40_PageSize;
                pBuffer += GD25Q40_PageSize;
            }
            DRV_GD25Q40_PageWrite(pBuffer, WriteAddr, NumOfSingle);
        }
    }
    else /* WriteAddr is not GD25Q40_PageSize aligned  */
    {
        if (NumOfPage == 0)
        {
            /* (NumByteToWrite + WriteAddr) > GD25Q40_PageSize */
            if (NumOfSingle > count) 
            {
                temp = NumOfSingle - count;
                DRV_GD25Q40_PageWrite(pBuffer, WriteAddr, count);
                WriteAddr +=  count;
                pBuffer += count;
                DRV_GD25Q40_PageWrite(pBuffer, WriteAddr, temp);
            }
            else
            {
                DRV_GD25Q40_PageWrite(pBuffer, WriteAddr, NumByteToWrite);
            }
        }
        else /* NumByteToWrite > GD25Q40_PageSize */
        {
            NumByteToWrite -= count;
            NumOfPage =  NumByteToWrite / GD25Q40_PageSize;
            NumOfSingle = NumByteToWrite % GD25Q40_PageSize;

            DRV_GD25Q40_PageWrite(pBuffer, WriteAddr, count);
            WriteAddr +=  count;
            pBuffer += count;

            while (NumOfPage--)
            {
                DRV_GD25Q40_PageWrite(pBuffer, WriteAddr, GD25Q40_PageSize);
                WriteAddr +=  GD25Q40_PageSize;
                pBuffer += GD25Q40_PageSize;
            }

            if (NumOfSingle != 0)
            {
                DRV_GD25Q40_PageWrite(pBuffer, WriteAddr, NumOfSingle);
            }
        }
    }
}

U32 DRV_GD25Q40_ReadID(VOID)
{
    U32 Temp = 0, Temp0 = 0, Temp1 = 0, Temp2 = 0;

    GD25Q40_CS_LOW();
    DRV_SPI_SwapByte(0x9F);
    Temp0 = DRV_SPI_SwapByte(Dummy_Byte);
    Temp1 = DRV_SPI_SwapByte(Dummy_Byte);
    Temp2 = DRV_SPI_SwapByte(Dummy_Byte);
    GD25Q40_CS_HIGH();

    Temp = (Temp0 << 16) | (Temp1 << 8) | Temp2;
    return Temp;
}

VOID DRV_GD25Q40_Init(VOID)
{
    GPIO_InitPara  GPIO_InitStructure;
    
    DRV_SPI_Init();
    RCC_APB2PeriphClock_Enable(RCC_APB2PERIPH_GPIOE , ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_PIN_3;                              
    GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_50MHZ;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_OUT_PP;
    GPIO_Init(GPIOE, &GPIO_InitStructure);
}

