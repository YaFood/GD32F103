#ifndef __DRV_AT24C03C_H__
#define __DRV_AT24C03C_H__

extern VOID DRV_AT24C02C_Init(VOID);
extern VOID DRV_AT24C02C_WriteByte(IN U8 slaveAddr, IN U8 byteAddr, IN U8 data);
extern VOID DRV_AT24C02C_WritePage(IN U8 slaveAddr, IN U8 byteAddr, IN U8 data[], IN U8 len);
extern VOID DRV_AT24C02C_ReadByte(IN U8 slaveAddr, IN U8 byteAddr, OUT U8 *data);
extern VOID DRV_AT24C02C_ReadPage(IN U8 slaveAddr, IN U8 byteAddr, OUT U8 data[], IN U8 len);

#endif

