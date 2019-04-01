#include "app.h"
#include "drv_gd25q40.h"
#include "app_file.h"
#include "app_xmodem.h"

#define XMODEM_SOH  0x01  /* Xmodem数据头 */
#define XMODEM_STX  0x02  /* 1K-Xmodem数据头 */
#define XMODEM_EOT  0x04  /* 发送结束 */
#define XMODEM_ACK  0x06  /* 认可响应 */
#define XMODEM_NAK  0x15  /* 不认可响应 */
#define XMODEM_CAN  0x18  /* 撤销传送 */
#define XMODEM_CTRLZ 0x1A /* 填充数据包 */

#define XMODEM_TIMEOUT 2000
#define XMODEM_MAXPKTLEN 133
#define XMODEM_PKTBUFLEN 128

static U16 XMODEM_Crc16(IN const U8 *buf, IN U8 len)
{
    U8 i = 0;
    U16 crc = 0;

    while (len--) 
    {
        crc ^= *buf++ << 8;

        for (i = 0; i < 8; ++i) 
        {
            if( crc & 0x8000 )
            {
                crc = (crc << 1) ^ 0x1021;
            }         
            else
            {
                crc = crc << 1;
            }
        }
    }
    return crc;
}

static S32 XMODEM_Check(IN BOOL isCrc, IN const U8 *buf, U8 sz)
{
    U16 crc = 0;
    U16 tcrc = 0;
    U8 i = 0;
    U8 cks = 0;
    
    if (TRUE == isCrc) 
    {
        crc = XMODEM_Crc16(buf, sz);
        tcrc = (buf[sz]<<8)+buf[sz+1];
        if (crc != tcrc)
        {
            APP_ERROR("%u, %u", crc, tcrc);
            return OS_ERROR;
        }
    }
    else 
    {
        for (i = 0; i < sz; ++i) 
        {
            cks += buf[i];
        }
        if (cks != buf[sz])
        {
            APP_ERROR("%u, %u", cks, buf[sz]);
            return OS_ERROR;
        }
    }

	return OS_OK;
}

static S32 XMODEM_GetOnePkt(IN U8 pktNum)
{
    U8 ch = 0;
    S32 ret = OS_OK;
    U8 i = 0;
    U8 xbuff[XMODEM_MAXPKTLEN] = {0};

    for (i = 1; i < XMODEM_MAXPKTLEN; i++)
    {
        ret = DRV_UART1_GetChar(XMODEM_TIMEOUT, &ch);
        if (ret != OS_OK)
        {
            APP_ERROR("ret=%d", ret);
            return OS_ERROR;
        }
        xbuff[i-1] = ch;
    }

    if (xbuff[0] != (U8)(~xbuff[1]))
    {
        APP_ERROR("%u,%u", xbuff[0], xbuff[1]);
        return OS_ERROR;
    }

    ret = XMODEM_Check(TRUE, &xbuff[2], XMODEM_PKTBUFLEN);
    if (ret != OS_OK)
    {
        APP_ERROR("ret=%d", ret);
        return OS_ERROR;
    }

    if (pktNum != xbuff[0])
    {
        (VOID)APP_FILE_Write(APP_FILE_FD_APPINFLASH, &xbuff[2], XMODEM_PKTBUFLEN);
    }
    
    return OS_OK;
}

S32 APP_XMODEM_Recive(VOID)
{
    U8 ch = 0;
    S32 ret = OS_OK;
    U8 pktNum = 0;
    
    ret = DRV_UART1_GetChar(XMODEM_TIMEOUT, &ch);
    if (ret != OS_OK)
    {
        APP_ERROR("ret=%d", ret);
        return OS_CONTINUE;
    }

    switch (ch)
    {
        case XMODEM_SOH:
            ret = XMODEM_GetOnePkt(pktNum);
            if (ret != OS_OK)
            {
                DRV_UART1_PutChar(XMODEM_NAK);
                break;
            }
            pktNum++;
            DRV_UART1_PutChar(XMODEM_ACK);
            break;
        case XMODEM_EOT:
            DRV_UART1_PutChar(XMODEM_ACK);
            return OS_OK;
        case XMODEM_CAN:
            DRV_UART1_PutChar(XMODEM_ACK);
            return OS_ERROR;
        case 'q':
            return OS_ERROR;
    }
    
    return OS_CONTINUE;
}

static VOID XMODEM_FillPkt(IN U8 cmd , IN U8 index, IN U8 *buffer)
{
    U16 crc = 0;
    
    buffer[0] = cmd;
    buffer[1] = index;
    buffer[2] = (U8)(~index);
    crc = XMODEM_Crc16(&buffer[3], XMODEM_PKTBUFLEN);
    buffer[XMODEM_PKTBUFLEN+3] = (crc >> 8);
    buffer[XMODEM_PKTBUFLEN+4] = crc;
    
    return;
}

static S32 XMODEM_SendSohPkt(IN U8 fd)
{
    BOOL resend = FALSE;
    U8 index = 1;
    U8 ch = 0;
    S32 ret = OS_OK;
    U8 buffer[XMODEM_MAXPKTLEN] = {0};
    U8 retryCount = 0;

    for (;;)
    {
        if (FALSE == resend)
        {
            retryCount = 0;
            memset(buffer, 0, XMODEM_MAXPKTLEN);
            ret = APP_FILE_Read(fd, XMODEM_PKTBUFLEN, &buffer[3]);
            if (ret != OS_OK)
            {
                return OS_OK;
            }
            XMODEM_FillPkt(XMODEM_SOH, index, buffer);
        }

        DRV_UART1_SendBuf(buffer, XMODEM_MAXPKTLEN);

        ret = DRV_UART1_GetChar(XMODEM_TIMEOUT, &ch);
        if (ret != OS_OK)
        {
            resend = TRUE;
            retryCount++;
        }
        switch (ch)
        {
            case XMODEM_ACK:
                    index++;
                    resend = FALSE;
                break;
            case XMODEM_NAK:
                resend = TRUE;
                retryCount++;
                break;
            case XMODEM_CAN:
                return OS_ERROR;
        }

        if (retryCount > 16)
        {
            return OS_ERROR;
        }
    }
}

static VOID XMODEM_SendEotPkt(VOID)
{
    U8 retryCount = 0;
    S32 ret = OS_OK;
    U8 buffer[XMODEM_MAXPKTLEN] = {0};
    U8 ch = 0;
    
    XMODEM_FillPkt(XMODEM_EOT, 1, buffer);
    DRV_UART1_SendBuf(buffer, XMODEM_MAXPKTLEN);

    while (1)
    {
        retryCount++;
        ret = DRV_UART1_GetChar(XMODEM_TIMEOUT, &ch);
        if (ret != OS_OK)
        {
            DRV_UART1_SendBuf(buffer, XMODEM_MAXPKTLEN);
        }
        else
        {
            switch (ch)
            {
                case XMODEM_ACK:
                    return;
                case XMODEM_NAK:
                    DRV_UART1_SendBuf(buffer, XMODEM_MAXPKTLEN);
                    break;
                case XMODEM_CAN:
                    return;
            }
        }
        if (retryCount > 16)
        {
            return;
        }
    }
}

static VOID XMODEM_SendCanPkt(VOID)
{
    U8 buffer[XMODEM_MAXPKTLEN] = {0};
    
    XMODEM_FillPkt(XMODEM_CAN, 1, buffer);
    DRV_UART1_SendBuf(buffer, XMODEM_MAXPKTLEN);
}

S32 APP_XMODEM_Send(IN U8 fd)
{
    S32 ret = OS_OK;
    
    ret = XMODEM_SendSohPkt(fd);
    if (OS_OK == ret)
    {
        XMODEM_SendEotPkt();
        return OS_OK;
    }
    else if (OS_ERROR == ret)
    {
        XMODEM_SendCanPkt();
        return OS_ERROR;
    }
    
    return OS_OK;
}

