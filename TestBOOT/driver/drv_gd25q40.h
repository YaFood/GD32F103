#ifndef __DRV_GD25Q40_H__
#define __DRV_GD25Q40_H__

extern VOID DRV_GD25Q40_Init(VOID);
extern VOID DRV_GD25Q40_PageWrite(U8* pBuffer, U32 WriteAddr, U16 NumByteToWrite);
extern VOID DRV_GD25Q40_BufferWrite(U8* pBuffer, U32 WriteAddr, U16 NumByteToWrite);
extern VOID DRV_GD25Q40_BufferRead(U8* pBuffer, U32 ReadAddr, U16 NumByteToRead);
extern VOID DRV_GD25Q40_BulkErase(VOID);
extern VOID DRV_GD25Q40_SectorErase(U32 SectorAddr);
extern U32 DRV_GD25Q40_ReadID(VOID);

#endif

