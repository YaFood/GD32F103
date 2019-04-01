#include "os_public.h"
#include "drv_fmc.h"

#define DRV_FMC_PAGE_SIZE  0x400 /* 1K */ 

typedef union
{
	unsigned int i_data;
	unsigned char c_data[4];
}DrvFmc_u;


VOID DRV_FMC_Erase(IN U32 addrMax, IN U32 addrStart)
{
    volatile U32 NbrOfPage = 0x00;
    volatile U32 EraseCounter = 0x00;
    
    /* Unlock the Flash Bank1 Program Erase controller */
    FMC_Unlock();
    
    /* Define the number of page to be erased */
    NbrOfPage = (addrMax) / DRV_FMC_PAGE_SIZE;
    
    /* Clear All pending flags */
    FMC_ClearBitState(FMC_FLAG_EOP | FMC_FLAG_WERR | FMC_FLAG_PERR );
    
    /* Erase the FLASH pages */
    for(EraseCounter = 0; EraseCounter < NbrOfPage; EraseCounter++)
    {
        (VOID)FMC_ErasePage(addrStart + (DRV_FMC_PAGE_SIZE * EraseCounter));
        FMC_ClearBitState(FMC_FLAG_EOP | FMC_FLAG_WERR | FMC_FLAG_PERR );
    }

    FMC_Lock();

    return;
}

/* len长度不能超过buf空间长度 */
VOID DRV_FMC_ReadBuffer(IN U32 addr, IN U32 len, OUT U8 *buf)
{   
    U32 i = 0;
    
    for (i = 0; i < len; i++)
    {   
        buf[i] = *(U8 *)(addr+i);
    }
    
    return;
}

/* buf空间必须是4的倍数，len长度不能超过buf空间长度 */
VOID DRV_FMC_WriteBuffer(IN U32 addr, IN U8 *buf, IN U32 len)
{
    U32 i = 0;
    U32 offset = 0;
    DrvFmc_u data;
    
    /* Unlock the Flash Bank1 Program Erase controller */
    FMC_Unlock();

    /* Clear All pending flags */
    FMC_ClearBitState(FMC_FLAG_EOP | FMC_FLAG_WERR | FMC_FLAG_PERR );    
    for (i = 0; i < len; i += 4)
    {
        memcpy(data.c_data, buf + offset, 4);
        (VOID)FMC_ProgramWord(addr + offset, data.i_data);
        FMC_ClearBitState(FMC_FLAG_EOP | FMC_FLAG_WERR | FMC_FLAG_PERR );    
        offset += 4;
    }
    FMC_Lock();
    
    return;
}

