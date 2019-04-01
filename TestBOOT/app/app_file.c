#include "app.h"
#include "drv_gd25q40.h"
#include "drv_fmc.h"
#include "app_file.h"

#define APP_FILE_INFLASH_MAX 0x400000 /* 4M */
static U32 gFileInFlashAddrWrite = 0;
static U32 gFileInFlashAddrRead = 0;

#define APP_FILE_INROM_MAX  0x32000 /* 200 */
#define APP_FILE_INROM_START 0x800E000 /* 56KÎ»ÖÃ¿ªÊ¼*/
static U32 gFileInRomAddrWrite = 0;
static U32 gFileInRomAddrRead = 0;

VOID APP_FILE_Open(IN U8 fd)
{
    if (APP_FILE_FD_APPINFLASH == fd)
    {
        DRV_GD25Q40_BulkErase();
        gFileInFlashAddrWrite = 0;
        gFileInFlashAddrRead = 0;
    }
    else if (APP_FILE_FD_APPINROM == fd)
    {
        DRV_FMC_Erase(APP_FILE_INROM_MAX, APP_FILE_INROM_START);
        gFileInRomAddrWrite = APP_FILE_INROM_START;
        gFileInRomAddrRead = APP_FILE_INROM_START;
    }
}


S32 APP_FILE_Write(IN U8 fd, IN U8 *buffer, IN U16 numByteToWrite)
{
    if (APP_FILE_FD_APPINFLASH == fd)
    {
        if (APP_FILE_INFLASH_MAX <= (gFileInFlashAddrWrite + numByteToWrite))
        {
            return OS_ERROR;
        }
        DRV_GD25Q40_BufferWrite(buffer, gFileInFlashAddrWrite, numByteToWrite);
        gFileInFlashAddrWrite += numByteToWrite;
    }
    else if (APP_FILE_FD_APPINROM == fd)
    {
        if ((APP_FILE_INROM_MAX+APP_FILE_INROM_START) <= (gFileInRomAddrWrite + numByteToWrite))
        {
            return OS_ERROR;
        }
        DRV_FMC_WriteBuffer(gFileInRomAddrWrite, buffer, numByteToWrite);
        gFileInRomAddrWrite += numByteToWrite;
    }
    return OS_OK;
}

S32 APP_FILE_Read(IN U8 fd, U16 numByteToRead, OUT U8 *buffer)
{
    if (APP_FILE_FD_APPINFLASH == fd)
    {
        if (APP_FILE_INFLASH_MAX <= (gFileInFlashAddrRead + numByteToRead))
        {
            return OS_ERROR;
        }
        DRV_GD25Q40_BufferRead(buffer, gFileInFlashAddrRead, numByteToRead);
        gFileInFlashAddrRead += numByteToRead;
    }
    else if (APP_FILE_FD_APPINROM == fd)
    {
        if ((APP_FILE_INROM_MAX+APP_FILE_INROM_START) <= (gFileInRomAddrRead + numByteToRead))
        {
            return OS_ERROR;
        }
        DRV_FMC_ReadBuffer(gFileInRomAddrRead, numByteToRead, buffer);
        gFileInRomAddrRead += numByteToRead;
    }
    
    return OS_OK;
}

VOID APP_FILE_Seek(IN U8 fd, IN U32 offset)
{
    if (APP_FILE_FD_APPINFLASH == fd)
    {
        gFileInFlashAddrRead = offset;
    }
    else if (APP_FILE_FD_APPINROM == fd)
    {
        gFileInRomAddrRead = offset;
    }
}

VOID APP_FILE_SeekStartOfFile(IN U8 fd)
{
    if (APP_FILE_FD_APPINFLASH == fd)
    {
        gFileInFlashAddrRead = 0;
    }
    else if (APP_FILE_FD_APPINROM == fd)
    {
        gFileInRomAddrRead = APP_FILE_INROM_START;
    }
}
