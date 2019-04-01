#ifndef __DRV_I2C_H__
#define __DRV_I2C_H__

extern VOID DRV_I2C_Init(VOID);
extern VOID DRV_I2C_Start(VOID);
extern VOID DRV_I2C_Stop(VOID);
extern U32 DRV_I2C_WriteByte(IN U8 data);
extern U32 DRV_I2C_ReadByte(OUT U8 *byte);
extern VOID DRV_I2C_NoAck(VOID);
extern VOID DRV_I2C_Ack(VOID);

#endif

