#include "os_public.h"
#include "drv_uart1.h"

static VOID UART1_GpioInit(VOID)
{
    GPIO_InitPara GPIO_InitStructure;
    
    RCC_APB2PeriphClock_Enable(RCC_APB2PERIPH_GPIOA , ENABLE);
    
    GPIO_InitStructure.GPIO_Pin     = GPIO_PIN_9 ;
    GPIO_InitStructure.GPIO_Mode    = GPIO_MODE_AF_PP;
    GPIO_InitStructure.GPIO_Speed   = GPIO_SPEED_50MHZ;
    GPIO_Init( GPIOA , &GPIO_InitStructure); 
    GPIO_InitStructure.GPIO_Pin     = GPIO_PIN_10;
    GPIO_InitStructure.GPIO_Mode    = GPIO_MODE_IN_FLOATING;;
    GPIO_Init( GPIOA , &GPIO_InitStructure); 
}

static VOID UART1_Config(VOID)
{
    USART_InitPara USART_InitStructure;
    
    RCC_APB2PeriphClock_Enable(RCC_APB2PERIPH_USART1 , ENABLE);
    
    USART_DeInit( USART1 );
    USART_InitStructure.USART_BRR = 115200;  /* 波特率 */
    USART_InitStructure.USART_WL = USART_WL_8B; /* 数据位 */
    USART_InitStructure.USART_STBits = USART_STBITS_1; /* 停止位 */
    USART_InitStructure.USART_Parity = USART_PARITY_RESET; /* 校验位 */
    USART_InitStructure.USART_HardwareFlowControl = USART_HARDWAREFLOWCONTROL_NONE; /* 流控 */
    USART_InitStructure.USART_RxorTx = USART_RXORTX_RX | USART_RXORTX_TX; /* 收发使能 */
    USART_Init(USART1, &USART_InitStructure);
}

VOID DRV_UART1_Init(VOID)
{
    UART1_GpioInit();
    UART1_Config();
    USART_Enable(USART1, ENABLE);
}

PUTCHAR_PROTOTYPE
{
    /* 等待发送完成 */
    while (USART_GetBitState(USART1 , USART_FLAG_TBE) == RESET)
    {
    }
    
    USART_DataSend(USART1 , (U8)ch);
    
    while (USART_GetBitState(USART1 , USART_FLAG_TC) == RESET)
    {
    }
    return ch;
}

GETCHAR_PROTOTYPE
{
    while(USART_GetBitState(USART1 , USART_FLAG_RBNE) == RESET)
    {
    }
    return USART_DataReceive(USART1);
}

VOID DRV_UART1_PutChar(IN U8 ch)
{
    /* 等待发送完成 */
    while (USART_GetBitState(USART1 , USART_FLAG_TBE) == RESET)
    {
    }
    
    USART_DataSend(USART1 , (U8)ch);
    
    while (USART_GetBitState(USART1 , USART_FLAG_TC) == RESET)
    {
    }
    return;
}

S32 DRV_UART1_GetChar(IN U32 timeout, OUT U8 *ch)
{
    U32 timeoutTicks = timeout*1000;
    
    while (USART_GetBitState(USART1 , USART_FLAG_RBNE) == RESET)
    {
       if (timeout != 0)
       {
           __asm("NOP");
           timeoutTicks--;
           if (timeoutTicks <= 0)
           {
               *ch = (U8)USART_DataReceive(USART1);
               return OS_TIMEOUT;
           }
       }
    }
    
    *ch = (U8)USART_DataReceive(USART1);

    return OS_OK;
}

VOID DRV_UART1_SendBuf(IN U8 *buf, IN U32 len)
{
    U32 i = 0;
    
    for (i = 0; i < len; i++)
    {
        DRV_UART1_PutChar(buf[i]);
    }
}

