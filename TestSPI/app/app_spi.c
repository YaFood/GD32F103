#include "app.h"
#include "drv_gd25q40.h"
#include "app_spi.h"

#define  sFLASH_ID              0xC84015            
#define  FLASH_WriteAddress     0x000000
#define  FLASH_ReadAddress      FLASH_WriteAddress

VOID APP_SPI_Test(VOID)
{
    U32 Manufact_ID = 0;
    U8  Tx_Buffer[256];
    U8  Rx_Buffer[256];
    U16 i = 0;
    
    DRV_GD25Q40_Init();
    
    printf("\n\rGD32103C-EVAL-V1.1 SPI Flash: configured...\n\r");
    Manufact_ID = DRV_GD25Q40_ReadID();
    printf("\n\rThe Flash_ID:0x%X\n\r", Manufact_ID);
  
    if (Manufact_ID == sFLASH_ID)   
    {
        printf("\n\rWrite to Tx_Buffer:\n\r");
        for(i=0; i<=255; i++)    
        {
            Tx_Buffer[i] = i;
            printf("0x%02X ",Tx_Buffer[i]);
            if(i%16 == 15)
            {
                printf("\n\r");
            }
        }
        printf("\n\rRead from Rx_Buffer:\n\r");
        DRV_GD25Q40_SectorErase(FLASH_WriteAddress);
        DRV_GD25Q40_BufferWrite(Tx_Buffer,FLASH_WriteAddress, 256);
        APP_Delay(10);
        DRV_GD25Q40_BufferRead(Rx_Buffer,FLASH_ReadAddress, 256);  
    
        for(i=0; i<=255; i++) 
        {	
            printf("0x%02X ", Rx_Buffer[i]);
            if(i%16 == 15)
            {
                printf("\n\r");
            }
        }
        printf("\n\rSPI Flash: Initialize Successfully!\n\r");
    }
    else
    {
        printf("\n\rSPI Flash: Initialize Fail!\n\r");
    }


}
