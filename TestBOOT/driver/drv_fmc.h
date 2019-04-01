#ifndef __DRV_FMC_H__
#define __DRV_FMC_H__

extern VOID DRV_FMC_Erase(IN U32 addrMax, IN U32 addrStart);
/* len长度不能超过buf空间长度 */
extern VOID DRV_FMC_ReadBuffer(IN U32 addr, IN U32 len, OUT U8 *buf);
/* buf空间必须是4的倍数，len长度不能超过buf空间长度 */
extern VOID DRV_FMC_WriteBuffer(IN U32 addr, IN U8 *buf, IN U32 len);

#endif

