#ifndef __APP_FILE_H__
#define __APP_FILE_H__

#define APP_FILE_FD_APPINFLASH 1
#define APP_FILE_FD_APPINROM 2

extern VOID APP_FILE_Open(IN U8 fd);
extern S32 APP_FILE_Write(IN U8 fd, IN U8 *buffer, IN U16 numByteToWrite);
extern S32 APP_FILE_Read(IN U8 fd, U16 numByteToRead, OUT U8 *buffer);
extern VOID APP_FILE_Seek(IN U8 fd, IN U32 offset);
extern VOID APP_FILE_SeekStartOfFile(IN U8 fd);

#endif

