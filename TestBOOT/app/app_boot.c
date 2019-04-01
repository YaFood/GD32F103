#include "app.h"
#include "drv_iwdg.h"
#include "drv_uart1.h"
#include "drv_gd25q40.h"
#include "app_xmodem.h"
#include "app_file.h"
#include "app_upgrade.h"
#include "app_boot.h"

typedef enum{
    BOOT_SMSTATUS_HELP = 0,
    BOOT_SMSTATUS_REBOOT,
    BOOT_SMSTATUS_GETAPP,
    BOOT_SMSTATUS_UPDATE,
    BOOT_SMSTATUS_UPDATEANDREBOOT,
    BOOT_SMSTATUS_DUMPFLASHAPP,
    BOOT_SMSTATUS_DUMPROMAPP,
    BOOT_SMSTATUS_NOP,
    BOOT_SMSTATUS_BUTT
}BootSmStatus_e;

typedef struct{
    BootSmStatus_e smStatus;
    U8 smStatusCount; 
}BootSm_t;
static BootSm_t gBootSm;

#if 0 
UI部分
#endif

static VOID BOOT_UiGuide(VOID)
{
    printf("\n**************************\n");
    printf("  Press Ctrl+c into bootmenu.\n");
}

static VOID BOOT_UiWelcome(VOID)
{
    printf("\n**************************\n");
    printf("  welcome to boot\n");
}

static VOID BOOT_UiHelp(VOID)
{
    printf("**************************\n");
    printf("0. help\n");
    printf("1. reboot\n");
    printf("2. get app by uart\n");
    printf("3. update\n");
    printf("4. update and reboot\n");
    printf("5. dump app in flash\n");
    printf("6. dump app in rom\n");
}

static VOID BOOT_UiReboot(VOID)
{
    printf("**************************\n");
    printf("booting...\n");
}

static VOID BOOT_UiGetAppPrepare(VOID)
{
    printf("**************************\n");
    printf("please wait a moment to get the app.\n");
}

static VOID BOOT_UiGetAppStart(VOID)
{
    printf("**************************\n");
    printf("getting app,press q stop.\n");
}

static VOID BOOT_UiGetAppOk(VOID)
{
    printf("**************************\n");
    printf("get app file sucess.\n");
    printf("press 0 get help.\n");
}

static VOID BOOT_UiGetAppErr(VOID)
{
    printf("**************************\n");
    printf("get app file faild.\n");
    printf("press 2 get app again or press 0 get help.\n");
}

static VOID BOOT_UiUpdate(VOID)
{
    printf("**************************\n");
    printf("updating...\n");
}

static VOID BOOT_UiUpdateOk(VOID)
{
    printf("**************************\n");
    printf("update ok, press 0 get help.\n");
}

static VOID BOOT_UiDumpAppStart(IN BootSmStatus_e smStatus)
{
    printf("**************************\n");
    if (BOOT_SMSTATUS_DUMPFLASHAPP == smStatus)
    {
        printf("app dumpping from flash...\n");
    }
    else
    {
        printf("app dumpping from rom...\n");
    }
}

static VOID BOOT_UiDumpAppOk(VOID)
{
    printf("**************************\n");
    printf("app dumpping ok, press 0 get help.\n");
}

static VOID BOOT_UiDumpAppErr(VOID)
{
    printf("**************************\n");
    printf("app dumpping faild, press 0 get help.\n");
}


#if 0
boot状态机部分
#endif

static VOID BOOT_SmNop(VOID)
{
    U8 ch = 0;
    S32 ret = OS_OK;
    
    if (gBootSm.smStatus != BOOT_SMSTATUS_NOP)
    {
        return;
    }
    
    ret = DRV_UART1_GetChar(0, &ch);
    if (ret != OS_OK)
    {
        APP_ERROR("ret=%d", ret);
        return;
    }

    APP_BOOT_SmInput(ch);
}

static VOID BOOT_SmHelp(VOID)
{
    if (gBootSm.smStatus != BOOT_SMSTATUS_HELP)
    {
        return;
    }
    
    APP_TRACE("");
    BOOT_UiHelp();
    
    gBootSm.smStatus = BOOT_SMSTATUS_NOP;
    gBootSm.smStatusCount = 0;
}

static VOID BOOT_SmReboot(VOID)
{
    if (gBootSm.smStatus != BOOT_SMSTATUS_REBOOT)
    {
        return;
    }
    
    APP_TRACE("");
    BOOT_UiReboot();
    DRV_IWDG_Init(); /* 利用看门狗复位芯片 */
    
    gBootSm.smStatus = BOOT_SMSTATUS_NOP;
    gBootSm.smStatusCount = 0;
}

static VOID BOOT_SmGetApp(VOID)
{
    S32 ret = OS_OK;
    
    if (gBootSm.smStatus != BOOT_SMSTATUS_GETAPP)
    {
        return;
    }
    
    APP_TRACE("");
    if (1 == gBootSm.smStatusCount)
    {
        /* 首次进入打印提示信息 */
        BOOT_UiGetAppPrepare();
        APP_FILE_Open(APP_FILE_FD_APPINFLASH);
        BOOT_UiGetAppStart();
        
    }
    
    gBootSm.smStatusCount++;
    ret = APP_XMODEM_Recive();
    if (ret == OS_OK)
    {
        gBootSm.smStatus = BOOT_SMSTATUS_NOP;
        gBootSm.smStatusCount = 0;
        BOOT_UiGetAppOk();
    }
    else if (ret == OS_ERROR)
    {
        gBootSm.smStatus = BOOT_SMSTATUS_NOP;
        gBootSm.smStatusCount = 0;
        BOOT_UiGetAppErr();
    }
}

static VOID BOOT_SmUpdate(VOID)
{
    if (gBootSm.smStatus != BOOT_SMSTATUS_UPDATE)
    {
        return;
    }
    
    APP_TRACE("");
    BOOT_UiUpdate();
    APP_UPGRADE_Updating();
    BOOT_UiUpdateOk();
    
    gBootSm.smStatus = BOOT_SMSTATUS_NOP;
    gBootSm.smStatusCount = 0;
}

static VOID BOOT_SmUpdateAndReboot(VOID)
{
    if (gBootSm.smStatus != BOOT_SMSTATUS_UPDATEANDREBOOT)
    {
        return;
    }
    
    APP_TRACE("");
    BOOT_UiUpdate();
    APP_UPGRADE_Updating();
    BOOT_UiUpdateOk();
    BOOT_UiReboot();
    DRV_IWDG_Init(); /* 利用看门狗复位芯片 */
    
    gBootSm.smStatus = BOOT_SMSTATUS_NOP;
    gBootSm.smStatusCount = 0;
}

static VOID BOOT_SmDumpApp(VOID)
{
    
    S32 ret = OS_OK;

    if ((gBootSm.smStatus != BOOT_SMSTATUS_DUMPFLASHAPP) && 
        (gBootSm.smStatus != BOOT_SMSTATUS_DUMPROMAPP))
    {
        return ;
    }
    
    APP_TRACE("");
    if (1 == gBootSm.smStatusCount)
    {
        /* 首次进入打印提示信息 */
        BOOT_UiDumpAppStart(gBootSm.smStatus);
        if (BOOT_SMSTATUS_DUMPFLASHAPP == gBootSm.smStatus)
        {
            APP_FILE_SeekStartOfFile(APP_FILE_FD_APPINFLASH);
        }
        else if (BOOT_SMSTATUS_DUMPROMAPP == gBootSm.smStatus)
        {
            APP_FILE_SeekStartOfFile(APP_FILE_FD_APPINROM);
        }
    }
    gBootSm.smStatusCount++;

    if (BOOT_SMSTATUS_DUMPFLASHAPP == gBootSm.smStatus)
    {
        ret = APP_XMODEM_Send(APP_FILE_FD_APPINFLASH);
    }
    else if (BOOT_SMSTATUS_DUMPROMAPP == gBootSm.smStatus)
    {
        ret = APP_XMODEM_Send(APP_FILE_FD_APPINROM);
    }
    if (ret == OS_OK)
    {
        BOOT_UiDumpAppOk();
        gBootSm.smStatus = BOOT_SMSTATUS_NOP;
        gBootSm.smStatusCount = 0;
    }
    else if (ret == OS_ERROR)
    {
        BOOT_UiDumpAppErr();
        gBootSm.smStatus = BOOT_SMSTATUS_NOP;
        gBootSm.smStatusCount = 0;
    }
}

static VOID BOOT_SmLoop(VOID)
{
    BOOT_SmNop();
    BOOT_SmHelp();
    BOOT_SmReboot();
    BOOT_SmGetApp();
    BOOT_SmUpdate();
    BOOT_SmUpdateAndReboot();
    BOOT_SmDumpApp();
}

static VOID BOOT_SmInit(VOID)
{
    memset(&gBootSm, 0, sizeof(gBootSm));
    gBootSm.smStatus = BOOT_SMSTATUS_NOP;
}

VOID APP_BOOT_SmInput(IN U8 cmd)
{
    if ((BOOT_SMSTATUS_NOP != gBootSm.smStatus) && (cmd != 'q'))
    {
        return;
    }

    switch (cmd)
    {
        case 'q':
            gBootSm.smStatus = BOOT_SMSTATUS_HELP;
            gBootSm.smStatusCount = 1;
            break;
        case '0':
            gBootSm.smStatus = BOOT_SMSTATUS_HELP;
            gBootSm.smStatusCount = 1;
            break;
        case '1':
            gBootSm.smStatus = BOOT_SMSTATUS_REBOOT;
            gBootSm.smStatusCount = 1;
            break;
        case '2':
            gBootSm.smStatus = BOOT_SMSTATUS_GETAPP;
            gBootSm.smStatusCount = 1;
            break;
        case '3':
            gBootSm.smStatus = BOOT_SMSTATUS_UPDATE;
            gBootSm.smStatusCount = 1;
            break;
        case '4':
            gBootSm.smStatus = BOOT_SMSTATUS_UPDATEANDREBOOT;
            gBootSm.smStatusCount = 1;
            break;
        case '5':
            gBootSm.smStatus = BOOT_SMSTATUS_DUMPFLASHAPP;
            gBootSm.smStatusCount = 1;
            break;
        case '6':
            gBootSm.smStatus = BOOT_SMSTATUS_DUMPROMAPP;
            gBootSm.smStatusCount = 1;
            break;
        default:
            break;
    }
}

#if 0
初始化和程序入口
#endif
typedef  void (*pAppFunction)(void);
#define BOOT_APP_MAIN_ADDR 0x800E000
static VOID BOOT_GoToApp(VOID)
{
    pAppFunction Jump_To_Application;
    unsigned long jumpAddress;
    
    //跳转至用户代码
    jumpAddress = *(volatile U32*)(BOOT_APP_MAIN_ADDR + 4);
    Jump_To_Application = (pAppFunction)jumpAddress;
    
    //初始化用户程序的堆栈指针
    __set_MSP(*(volatile U32*) BOOT_APP_MAIN_ADDR);
    Jump_To_Application();
}

static VOID BOOT_Init(VOID)
{
    BOOT_SmInit();
    DRV_GD25Q40_Init();
}

VOID APP_BOOT_Test(VOID)
{
    U8 ch = 0;
    U8 count = 0;
    
    BOOT_Init();
    
    BOOT_UiGuide();
    for (count = 0; count < 5; count++)
    {
        (VOID)DRV_UART1_GetChar(3000, &ch);
        if (0x03 == ch) /* Ctrl+c */
        {
            BOOT_UiWelcome(); 
            BOOT_UiHelp();
            while (1)
            {
                BOOT_SmLoop();
            }
        }
        printf(". ");
    }
    printf("\n");

    /* 跳转到APP运行 */
    APP_TRACE("go to APP.");
    BOOT_GoToApp();
}
