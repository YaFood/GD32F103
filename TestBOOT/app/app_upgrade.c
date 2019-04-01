#include "app.h"
#include "app_file.h"
#include "app_upgrade.h"

#define APP_UPGRADE_APP_MAX 0x32000
#define APP_UPGRADE_PAGE 128

VOID APP_UPGRADE_Updating(VOID)
{
    U32 i = 0;
    U8 Buf[APP_UPGRADE_PAGE] = {0};

    APP_FILE_Open(APP_FILE_FD_APPINROM);
    
    for (i = 0; i < APP_UPGRADE_APP_MAX; i++)
    {
        APP_FILE_Read(APP_FILE_FD_APPINFLASH, APP_UPGRADE_PAGE, Buf);
        APP_FILE_Write(APP_FILE_FD_APPINROM, Buf, APP_UPGRADE_PAGE);
    }
}

